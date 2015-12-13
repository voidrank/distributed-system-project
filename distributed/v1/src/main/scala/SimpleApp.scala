import scala.collection._
import scala.collection.mutable.Buffer
import scala.util.Random
import scala.math._

import java.util._
import java.io._


import org.apache.spark.SparkContext
import org.apache.spark.SparkContext._
import org.apache.spark.SparkConf


object SimpleApp {


	val conf = new SparkConf().setAppName("bacterium simulation")
	val sc = new SparkContext(conf)
	var (n, nds, nodeIndexMapping, m, edges, edgeList) = before
	val maxBacteria = 20
	val random = new Random(System.currentTimeMillis())


    def before() = {
    	val fileInputStream = new FileInputStream(new File("./test.in"))
    	val in = new Scanner(fileInputStream);
    	val n = in.nextInt()
    	val nds = (for (i <- 0 until n) yield (in.next(), in.nextDouble(), in.nextDouble()))
    	val m = in.nextInt()
    	val nodeIndexMapping = (for (i <- 0 until n) yield (nds(i)._1, i)).toMap
    	val edges = for (i <- 0 until m) yield (nodeIndexMapping(in.next()), nodeIndexMapping(in.next()))
		val edgeList = (0 until n).map(line => Buffer[Int]())
		edges.map(edge => {
			edgeList(edge._1) += edge._2
			edgeList(edge._2) += edge._1
		})
		(0 until n).map(node => {
			for (i <- 0 until 5)
				edgeList(node) += node
		})
    	(n, nds, nodeIndexMapping, m, edges, edgeList)
	}


	def ticker(bacterium: org.apache.spark.rdd.RDD[(Int, (Int, Int))], gEdgeList: org.apache.spark.broadcast.Broadcast[immutable.IndexedSeq[mutable.Buffer[Int]]]) = {

		//val incrementNodeState = for (i <- 0 until n) yield sc.accumulator(0)
		bacterium.flatMap(bacteria => 
			if (bacteria._2._2 * 2 > maxBacteria && random.nextInt % 3 == 0 ) 
				gEdgeList.value(bacteria._1).map(node => (node, (bacteria._2._1, (min(bacteria._2._2*0.2, maxBacteria)).toInt)))
			else
				immutable.IndexedSeq((bacteria._1, (bacteria._2._1, (bacteria._2._2*sqrt(maxBacteria/max(1, bacteria._2._2))).toInt)))
		).reduceByKey((a, b) =>
			if (a._1 != b._1) {
				if (a._2 > b._2)
					(a._1, a._2 - b._2)
				else
					(b._1, a._2 + b._2)
			}
			else 
				(a._1, a._2 + b._2)
		)
	}


	def main(args: Array[String]) {

		val seedCount = 8
		val seeds = random.shuffle(for (i <- 0 until n) yield i).splitAt(seedCount)._1
		val gEdgeList = sc.broadcast(edgeList)

		var bacterium = sc.parallelize((0 until seedCount).map(i => (seeds(i), (i+1, 1))))
		for (i <- 0 until 10000) {
			bacterium = ticker(bacterium, gEdgeList)
			// node_id bacterium_id seedCountt
			if (i % 100 == 0) {
				val out = new PrintStream("./test" + i + ".out")
				for (i <- bacterium.collect)
					out.println((i._1).toString + " " + (i._2._1).toString)
				out.close()
			}
		}


		//after(n, nds, m, edges)
	}


	def after(
			n: Int, 
			nds: immutable.IndexedSeq[(String, Double, Double)], 
			m: Int, 
			edges: immutable.IndexedSeq[(Int, Int)])
	{
		val out = new PrintStream("./test.out")
	}
}
