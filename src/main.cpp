#include "Dataset.h"
#include "sequential.cpp"
#include <iostream>

int main(int argc, char** argv) {
    // Iris dataset: 4 dimension points, no header line
    Dataset iris;
    iris.filename = "../datasets/iris.data.csv";
    iris.points_dimensions = 4;
    iris.skipFirstLine = false;

    // Wine quality dataset: 12 dimension points, one header line at the top
    Dataset wineQuality;
    wineQuality.filename = "../datasets/WineQualityReplicated.csv";
    wineQuality.points_dimensions = 12;
    wineQuality.skipFirstLine = true;

    // Parameters
    int n_points = 5100; // number of points to take from csv
    int n_clusters = 5; // wanted number of clusters
    unsigned int seed = 1111; // seed for cluster initialization
    int max_iters = 2000; // max number of iterations

    //kMeans(&iris, n_points, n_clusters, 300, seed, "../outputs/results_sequential_iris_" + std::to_string(seed) + ".csv");
    kMeans(&wineQuality, n_points, n_clusters, max_iters, seed, "../outputs/results_sequential_wine_replicated_" + std::to_string(seed) + "_" + std::to_string(max_iters) + ".csv");

    return 0;
}
