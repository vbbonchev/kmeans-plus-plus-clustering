# kmeans++ clustering algorithm
This code takes two dataset of points in 2D space and explores the differences between the kmeans and kmeans++ clusterization (both in the code). It shows that while the kmeans algorithm works on the balanced dataset (normal.txt ) it performs poorly on unbalanced ones (unbalance.txt) due to the random initialization of the cluster positions. Ultimately - picking the initial positions via the kmeans++ algorithm solves that problem.

[unbalanced](kmeans-plus-plus-clustering/unbalancedClusters.png)
[balanced](https://raw.githubusercontent.com/vbbonchev/kmeans-plus-plus-clustering/master/normalClusters.bmp)
