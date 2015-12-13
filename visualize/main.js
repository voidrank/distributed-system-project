var Canvas = require('canvas'),
    Image = Canvas.Image,
    canvas = new Canvas(1000, 1000),
    ctx = canvas.getContext('2d'),
    mapdb = require("./mapdb/build/Release/mapdb");
  

function drawMap() {
    var maxLon = mapdb.getMaxLon(),
        minLon = mapdb.getMinLon(),
        maxLat = mapdb.getMaxLat(),
        minLat = mapdb.getMinLat();
    var width = maxLon - minLon,
        height = maxLat - minLat,
        widthRatio = 990 / width,
        heightRatio = 990 / height,
        ratio = Math.min(widthRatio, heightRatio);


    var clusterColor = ["#000000", "#00ff00", "#0000ff", "#ff0000", "#660033",
    "#00FF66", "#3399FF", "#66FF33", "#66FFFF", "#66FFFF", "#CC9999", "#FF00FF", 
    "#FFCC00"];

    //console.log(maxLon, minLon, maxLat, minLat);
    //console.log(width, height);

    var widthOffset = 5,
        heightOffset = 5;

    var n = mapdb.getN(),
        m = mapdb.getM();


    function trans(node) {
        node.lon = node.lon - minLon;
        node.lat = node.lat - minLat;
        node.lat = height - node.lat;
        node.lon = node.lon * ratio;
        node.lat = node.lat * ratio;
        return node;
    }

    for (var i = 0; i < n; i += 1) {
        var node = mapdb.getNode(i);

        node = trans(node);
        
        ctx.fillStyle = clusterColor[mapdb.getCluster(i)];
        ctx.fillRect(node.lon, node.lat, 1, 1);
    }

    var count = 0;
    for (var i = 0; i < m; i += 1) {
        var edge = mapdb.getEdge(i);
        //console.log(edge.a);
        var node_a = mapdb.getNodeById(edge.a),
            node_b = mapdb.getNodeById(edge.b);
        node_a = trans(node_a);
        node_b = trans(node_b);
        var color_a = mapdb.getClusterById(edge.a),
            color_b = mapdb.getClusterById(edge.b);
        if (color_a == color_b) {
            // console.log(color_a);
            ctx.strokeStyle = clusterColor[color_a];
            ctx.beginPath();
            ctx.moveTo(node_a.lon, node_a.lat);
            ctx.lineTo(node_b.lon, node_b.lat);
            ctx.stroke();
        }
        else {
            //console.log(node_a.lon, node_a.lat, "  ", node_b.lon, node_b.lat);
            ++count;
        }
    }

}

mapdb.loadMap("map.in");
for (var i = 799; i < 800; i += 1) {
    mapdb.loadCluster("../distributed/v1/test" + i + ".out");
    drawMap();
    console.log('<img src="' + canvas.toDataURL() + '" />');
}
