python ./extract/python/extract_node.py ./extract/python/map.osm > ./distributed/v1/test.in
cp ./distributed/v1/test.in ./visualize/map.in
cd ./distributed/v1
sh ./compile.sh
sh ./run.sh
cd ../../visualize
node main.js > 1.html
cd ..
