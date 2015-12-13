1. shell mode.
	$spark-shell and run the spark
	
2. load file from hdfs  
	```
	val textFile = sc.textFile("hdfs://{localhost}:9000/{filepath}")
	```
	notice that {localhost} must be an ip
	from local filesystem  
	```
	val textFile = sc.textFile("/{filepath}")
	```
	
3. server mode  
 $obt package  
\## notice the file architecture 
 $sh run.sh  