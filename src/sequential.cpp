#include <limits>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>

#include "Dataset.h"

using namespace std;

struct Cluster;

struct MultiDimPoint {
    double* dimensions;
    int n_dimensions;
    Cluster* cluster;

    MultiDimPoint(): dimensions(new double[2]), n_dimensions(2) {}
    MultiDimPoint(double* dimensions, int n_dimensions): dimensions(new double[n_dimensions]), n_dimensions(n_dimensions) {
        for (int i = 0; i < n_dimensions; i++) {
            this->dimensions[i] = dimensions[i];
        }
    }

    double distance(MultiDimPoint* p) {
        if (n_dimensions != p->n_dimensions) {
            throw invalid_argument("Cannot compute distance between points with different number of dimensions");
        }

        double accumulator = 0;
        for (int i = 0; i < n_dimensions; i++) {
            accumulator += pow(dimensions[i] - p->dimensions[i], 2);
        }
        return sqrt(accumulator);
    }

    void add(MultiDimPoint* p, double* sum) {
        if (n_dimensions != p->n_dimensions) {
            throw invalid_argument("Cannot add points with different number of dimensions");
        }

        for (int i = 0; i < n_dimensions; i++) {
            sum[i] = dimensions[i] + p->dimensions[i];
        }
    }

    void addToAccumulator(double* accumulator) {
        for (int i = 0; i < n_dimensions; i++) {
            accumulator[i] += dimensions[i];
        }
    }

    void updateDimensions(double* dimensions_sums, int& n_points) {
        for (int i = 0; i < n_dimensions; i++) {
            dimensions[i] = dimensions_sums[i] / n_points;
        }
    }

    void assingCluster(Cluster* c);

    void print() {
        cout << "Point: ";
        for (int d = 0; d < n_dimensions; d++) {
            cout << "dimension " << d << ": " << dimensions[d] << ", ";
        }
        cout << endl;
    }
};

struct Cluster {
    int id = 0;
    MultiDimPoint centroid;
    double* dimensions_sums;
    int n_points = 0;

    Cluster(): id(0), n_points(0) {}
    Cluster(int id): id(id), n_points(0) {}

    void updateCentroid() {
        centroid.updateDimensions(dimensions_sums, n_points);
    }
};

void MultiDimPoint::assingCluster(Cluster* c) {
    cluster = c;
    addToAccumulator(c->dimensions_sums);
    (c->n_points)++;
}

MultiDimPoint* getPointsFromCsv(Dataset* dataset, int& n_points) {
    MultiDimPoint* points = new MultiDimPoint[n_points];
    string line;
    ifstream file(dataset->filename);
    int i = 0;
    bool skipFirst = dataset->skipFirstLine;

    while (getline(file, line)) {
        if (i >= n_points) break;
        if (skipFirst) {
            skipFirst = false;
            continue;
        };
        stringstream lineStream(line);
        string bit;
        double dimension;
        double dimensions[dataset->points_dimensions];
        int d = 0;
        while(getline(lineStream, bit, ',')){
            if (d >= dataset->points_dimensions) break;
            dimension = stof(bit);
            dimensions[d] = dimension;
            d++;
        }
        points[i] = MultiDimPoint(dimensions, dataset->points_dimensions);
        i++;
    }
    return points;
}

void initClusters(Cluster* clusters, int& n_clusters) {
    for (int c = 0; c < n_clusters; c++) { // Iterate through clusters
        clusters[c] = Cluster(c);
    }
}

void assignPointsToCentroids(MultiDimPoint* points, Cluster* clusters, int& n_points, int& n_clusters) {
    for (int p = 0; p < n_points; p++) { // Iterate through points
        double min_distance = __DBL_MAX__; // Initialize min distance to double max value

        for (int c = 0; c < n_clusters; c++) { // Iterate through clusters
            double distance = points[p].distance(&clusters[c].centroid); // Compute distance between point and centroid
            if (distance < min_distance) {
                min_distance = distance;
                points[p].assingCluster(&clusters[c]); // Assign cluster to point
            }
        }
    }
}

void updateCentroids(Cluster* clusters, int& n_clusters) {
    for (int c = 0; c < n_clusters; c++) { // Iterate through clusters
        clusters[c].updateCentroid();
    }
}

void outputResultsToFile(string filename, MultiDimPoint* points, Cluster* clusters, int& n_points, int& n_clusters, int& points_dimensions) {
    ofstream myfile;
    myfile.open(filename);
    for (int d = 0; d < points_dimensions - 1; d++) {
        myfile << "dimension" << d << ",";
    }
    myfile << "dimension" << points_dimensions - 1 << endl;

    for (int p = 0; p < n_points; p++) {
        MultiDimPoint point = points[p];
        for (int d = 0; d < point.n_dimensions; d++) {
            myfile << point.dimensions[d] << ",";
        }
        myfile << point.cluster->id << endl;
    }
    myfile.close();
}

/*
    in_seed: if a seed is given (other than 0) it is used as a seed for the number generator, otherwise a time-based one is used. This is usefull for reproducibility
*/
void kMeans(MultiDimPoint* points, int& n_points, int& n_clusters, int max_iters, unsigned int in_seed, int& points_dimensions, string output_name) {
    // Init clusters
    srand(in_seed ? in_seed : time(0));
    Cluster clusters[n_clusters];
    initClusters(clusters, n_clusters);
    for (int i = 0; i < n_clusters; i++) {
        clusters[i].centroid = points[rand() % n_points];
        clusters[i].dimensions_sums = new double[clusters[i].centroid.n_dimensions] {0};
    }

    for (int i = 0; i < max_iters; i++) {
        assignPointsToCentroids(points, clusters, n_points, n_clusters);
        updateCentroids(clusters, n_clusters);
    }

    outputResultsToFile(output_name, points, clusters, n_points, n_clusters, points_dimensions);
}

void kMeans(Dataset* dataset, int& n_points, int& n_clusters, int max_iters, unsigned int in_seed, string output_name) {
    MultiDimPoint* points = getPointsFromCsv(dataset, n_points);
    /* for (int p = 0; p < n_points; p++) {
        points[p].print();
    } */
    kMeans(points, n_points, n_clusters, max_iters, in_seed, dataset->points_dimensions, output_name);
}

// Tests
void testGetPointsFromCsv(Dataset* dataset, int& n_points) {
    string line;
    ifstream file(dataset->filename);
    int i = 0;
    bool skipFirst = dataset->skipFirstLine;

    while (getline(file, line)) {
        if (i >= n_points) break;
        if (skipFirst) {
            skipFirst = false;
            continue;
        };
        stringstream lineStream(line);
        string bit;
        double dimension;
        int d = 0;
        cout << "Line: " << line << endl;
        while(getline(lineStream, bit, ',')){
            if (d >= dataset->points_dimensions) break;
            dimension = stof(bit);
            cout << "dimension" << d << ": " << dimension << ", ";
            d++;
        }
        cout << endl;
        i++;
    }
}