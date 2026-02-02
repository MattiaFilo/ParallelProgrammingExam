#include "Dataset.h"
#include "sequential.cpp"
#include <iostream>

int main(int argc, char** argv) {
    Dataset iris;
    iris.filename = "../datasets/iris.data.csv";
    iris.points_dimensions = 4;
    iris.skipFirstLine = false;

    Dataset wineQuality;
    wineQuality.filename = "../datasets/WineQualityReplicated.csv";
    wineQuality.points_dimensions = 12;
    iris.skipFirstLine = true;

    int n_points = 5100;
    int n_clusters = 5;
    unsigned int seed = 1111;
    int max_iters = 2000;

    //kMeans(&iris, n_points, n_clusters, 300, seed, "../outputs/results_sequential_iris_" + std::to_string(seed) + ".csv");
    kMeans(&wineQuality, n_points, n_clusters, max_iters, seed, "../outputs/results_sequential_wine_replicated_" + std::to_string(seed) + "_" + std::to_string(max_iters) + ".csv");

    return 0;
}
