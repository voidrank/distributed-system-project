#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <memory>

#include <node.h>

#define RAPIDXML_NO_EXCEPTIONS 1
#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"


FILE *log_file = fopen("log", "w");


namespace xmlParser {
    
    using namespace std;
    using namespace rapidxml;

    xml_document<> doc;
    char charpool[1024*1024*32];

    void loadMap(const string filename = "map.in") {
        FILE *file = fopen(filename.c_str(), "r");
        //size_t length = ftell(file);
        //fprintf(log_file, "%d\n", length);
        //fclose(log_file);
        memset(charpool, 0, sizeof charpool);
        
        fread(charpool, sizeof charpool[0], sizeof charpool / sizeof charpool[0], file);
        char *p = charpool;
        doc.parse<0>(p);
    }

}


namespace store {

    using namespace std;
    

    struct Point {
        double lat, lon;
        long long id;
    };


    struct Edge {
        long long a, b;
    };

    struct Way {
        int op, ed;
        string name;
    };

    int n, m;
    vector<Point> v;
    vector<Edge> e;
    vector<Way> w;
    vector<int> cluster;
    map<long long, int> mapping;


    void loadMap(const string filename = "map.in") {
        FILE *file = fopen(filename.c_str(), "r");
        fscanf(file, "%d", &n);
        v.clear();
        mapping.clear();
        for (int i = 0; i < n; ++i) {
            long long id;
            double lat, lon;
            fscanf(file, "%lld%lf%lf", &id, &lat, &lon);
            mapping[id] = v.size();
            v.push_back(Point{lat, lon, id});
        }
        fscanf(file, "%d", &m);
        for (int i = 0; i < n; ++i) {
            long long x, y;
            fscanf(file, "%lld%lld", &x, &y);
            e.push_back(Edge{x, y});
        }
    }

    void loadMapFromXML(const string filename = "map.osm") {

        xmlParser::loadMap(filename);
        auto& doc = xmlParser::doc;
        auto *root = doc.first_node();
        v.clear();
        e.clear();


        for (auto* node = root->first_node("node");
                node;
                node = node->next_sibling("node")) {
            mapping[atoll(node->first_attribute("id")->value())]= v.size();
            v.push_back(Point{
                    atof(node->first_attribute("lat")->value()), 
                    atof(node->first_attribute("lon")->value()),
                    atoll(node->first_attribute("id")->value())
                    });
        }
        n = v.size();

        mapping.clear();

        for (auto* way = root->first_node("way");
                way;
                way = way->next_sibling("way")) 
        {
            int op = e.size(), ed = e.size();
            auto *edge = way->first_node("nd");
            for (auto* nedge = edge->next_sibling("nd"); 
                    nedge;
                    edge = nedge, nedge = nedge->next_sibling("nd"))
            {
                ++ed;
                e.push_back(Edge{
                        mapping[atoll(edge->first_attribute("ref")->value())],
                        mapping[atoll(nedge->first_attribute("ref")->value())]
                        });
            }
            string name;
            auto *tag = way->first_node("tag");
            for ( ; tag && strcmp(tag->name(), "name") == 0; tag = tag->next_sibling("tag"));
            w.push_back(Way{op, ed, tag?(tag->value()):"no name"}); 

        }
        m = e.size();


    }

    void loadCluster(const string filename = "test.out") {
        FILE *file = fopen(filename.c_str(), "r");
        cluster.clear();
        cluster.resize(n);
        for (int i = 0; i < n; ++i) {
            int x, y;
            fscanf(file, "%d%d", &x, &y);
            cluster[x] = y;
        }
    }


    int getN() {
        return v.size();
    }

    int getM() {
        return e.size();
    }

    int getW() {
        return w.size();
    }

    Point getPoint(int index) {
        return v[index];
    }

    Point getPointById(long long id) {
        return v[mapping[id]];
    }

    Edge getEdge(int index) {
        return e[index];
    }
    
    Way getWay(int index) {
        return w[index];
    }

    int getCluster(int index) {
        return cluster[index];
    }

    int getClusterById(int index) {
        return cluster[mapping[index]];
    }

    double getMaxLat() {
        double ret = -1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = max(ret, v[i].lat);
        return ret;
    }

    double getMinLat() {
        double ret = 1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = min(ret, v[i].lat);
        return ret;
    }

    double getMaxLon() {
        double ret = -1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = max(ret, v[i].lon);
        return ret;
    }

    double getMinLon() {
        double ret = 1000;
        for (int i = 0; i < (int)v.size(); ++i)
            ret = min(ret, v[i].lon);
        return ret;
    }
             
}



namespace exports {

    using v8::FunctionCallbackInfo;
    using v8::Isolate;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;
    using v8::Number;
    using v8::Exception;

    void Method(const FunctionCallbackInfo<Value>& args) {
    }

    void loadMap(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadMap(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void loadMapFromXML(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadMapFromXML(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void loadCluster(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        if (!args[0]->IsString()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }
        store::loadCluster(*v8::String::Utf8Value(args[0]->ToString()));
    }

    void getN(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> n = Number::New(isolate, store::getN());
        args.GetReturnValue().Set(n);
    }


    void getM(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> m = Number::New(isolate, store::getM());
        args.GetReturnValue().Set(m);
    }


    void getW(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        Local<Number> w = Number::New(isolate, store::getW());
        args.GetReturnValue().Set(w);
    }


    void getNode(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::n) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        auto point = store::getPoint(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "lat"), Number::New(isolate, point.lat));
        obj->Set(String::NewFromUtf8(isolate, "lon"), Number::New(isolate, point.lon));
        obj->Set(String::NewFromUtf8(isolate, "id"), Number::New(isolate, point.id));

        args.GetReturnValue().Set(obj);
    }

    void getNodeById(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || store::mapping.find(args[0]->NumberValue()) == store::mapping.end()) {
            printf("%d\n", args[0]->IsNumber());
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        long long index = (long long)(args[0]->NumberValue());
        auto point = store::getPointById(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "lat"), Number::New(isolate, point.lat));
        obj->Set(String::NewFromUtf8(isolate, "lon"), Number::New(isolate, point.lon));
        obj->Set(String::NewFromUtf8(isolate, "id"), Number::New(isolate, point.id));
        args.GetReturnValue().Set(obj);
    }
    

    void getEdge(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::m) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        auto edge = store::getEdge(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "a"), Number::New(isolate, edge.a));
        obj->Set(String::NewFromUtf8(isolate, "b"), Number::New(isolate, edge.b));

        args.GetReturnValue().Set(obj);
    }

    void getWay(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::m) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index = int(args[0]->NumberValue());
        auto way = store::getWay(index);

        Local<Object> obj = Object::New(isolate);
        obj->Set(String::NewFromUtf8(isolate, "op"), Number::New(isolate, way.op));
        obj->Set(String::NewFromUtf8(isolate, "ed"), Number::New(isolate, way.ed));
        obj->Set(String::NewFromUtf8(isolate, "name"), String::NewFromUtf8(isolate, way.name.c_str()));

        args.GetReturnValue().Set(obj);
    }


    void getCluster(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || args[0]->NumberValue() >= store::n) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        int index= int(args[0]->NumberValue());
        
        args.GetReturnValue().Set(Number::New(isolate, store::getCluster(index)));
    }


    void getClusterById(const FunctionCallbackInfo<Value>& args) {

        Isolate* isolate = args.GetIsolate();

        if (!args[0]->IsNumber() || store::mapping.find(args[0]->NumberValue()) == store::mapping.end()) {
            isolate->ThrowException(Exception::TypeError(
                String::NewFromUtf8(isolate, "Wrong arguments")));
            return;
        }

        long long index = (long long)(args[0]->NumberValue());
        
        args.GetReturnValue().Set(Number::New(isolate, store::getClusterById(index)));
    }


    void getMaxLat(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMaxLat()));
    }

    void getMinLat(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMinLat()));
    }


    void getMaxLon(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMaxLon()));
    }

    void getMinLon(const FunctionCallbackInfo<Value>& args) {
        Isolate* isolate = args.GetIsolate();
        args.GetReturnValue().Set(Number::New(isolate, store::getMinLon()));
    }


    void init(Local<Object> exports) {
        NODE_SET_METHOD(exports, "getN", getN);
        NODE_SET_METHOD(exports, "getM", getM);
        NODE_SET_METHOD(exports, "getW", getW);
        NODE_SET_METHOD(exports, "getNode", getNode);
        NODE_SET_METHOD(exports, "getNodeById", getNodeById);
        NODE_SET_METHOD(exports, "getEdge", getEdge);
        NODE_SET_METHOD(exports, "getWay", getWay);
        NODE_SET_METHOD(exports, "getMaxLon", getMaxLon);
        NODE_SET_METHOD(exports, "getMaxLat", getMaxLat);
        NODE_SET_METHOD(exports, "getMinLon", getMinLon);
        NODE_SET_METHOD(exports, "getMinLat", getMinLat);
        NODE_SET_METHOD(exports, "getCluster", getCluster);
        NODE_SET_METHOD(exports, "getClusterById", getClusterById);
        NODE_SET_METHOD(exports, "loadMap", loadMap);
        NODE_SET_METHOD(exports, "loadMapFromXML", loadMapFromXML);
        NODE_SET_METHOD(exports, "loadCluster", loadCluster);
    }

    NODE_MODULE(addon, init)

}
