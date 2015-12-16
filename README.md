# OSM node 细菌繁殖模拟

## 目的
给OSM分块，预处理出每个块之间的最短路，以及块与块之间的最短路，达到在线地图查询O(1)操作。细菌繁殖模拟过程用来作分块算法。

### 估价函数
f(G) = sigma size_i * log(log_i) + alpha * sigma outpoint_size



## map reduce 框架

scala on spark

### scala语言特点
运行在jvm上的函数式强类型oop语言。支持lambda, traits, 类型推导，泛型编程等优秀特性。写起来要比Java短，但难度大，代码也不是很好看。

### spark框架特点
运行在jvm上的支持内存分布式文件的分布式系统。比起传统hadoop来讲，spark可以把计算的结果存在内存中，而不是map, reduce结束之后都进行写硬盘操作，这给一些大型密集型计算提供了可能行。spark比起hadoop还拥有broadcast, accumulator等全局变量的功能(利用p2p)，比起hadoop来讲，这个全局变量更快操作更灵活。还有GraphX(图操作)、MLib(机器学习库)、K-means(聚类算法)等优秀的第三方库。


## 算法

### Init
利用osm中的道路作为边，将node连起来，开始时随机选取一些node作为初始细菌节点，“放入”10个细菌至这些节点中。



常数: 初始带细菌节点个数、初始细菌个数、最大细菌数  
函数: 细菌增长函数f、细菌迁移函数g


### Mapreduce

 

#### map
状态表示st = (节点编号， 细菌种编号， 细菌个数)

对于每一个有细菌的节点，我们先判断当前节点的细菌的个数有没有超过**最大细菌数**的一半。

1. 如果没有超过，yield (st.\_1, st.\_2, f(st._3))
1. 如果超过，那么 yield (st.\_1, st.\_2, f(st._3)); for (j | [i,j] in E) yield (j, st.\_2, g(st.\_3))

#### reduce

reduceByKey(节点),  
在同一个节点,对于(a,b)，会产生几个情况   
1. a, b同属一个细菌族, yield(a.\_1, a.\_2, a.\_3 + b.\_3)   
2. a, b不同属一个细菌族, yield(a.\_1, a.\_2, a.\_3 - b.\_3)

由于map之后顺序shuffle，可以近似认为第二个reduce是正确的

## DEMO

1-5 上海主城区 
6 上海、浙江、宁波