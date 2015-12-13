var mapdb = require("./build/Release/mapdb.node");
mapdb.loadMapFromXML("map.osm");
console.log(mapdb.getN());
console.log(mapdb.getM());
console.log(mapdb.getW());
