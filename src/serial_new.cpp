#include <limits>
#include <vector>
#include <cmath>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <omp.h>

#include "Dataset.h"

using namespace std;

const int MAX_POINTS = 10000;
const int MAX_DIMENSIONS = 50;
const int MAX_CLUSTERS = 50;

void kMeans(Dataset* dataset, int& n_points, int& n_clusters, int max_iters, unsigned int in_seed, string output_name) {
    double points[MAX_POINTS][MAX_DIMENSIONS+1];
    double centroids[MAX_CLUSTERS][MAX_DIMENSIONS];
    double dimensions_sums[MAX_CLUSTERS][MAX_DIMENSIONS];
    int cluster_size[MAX_CLUSTERS];

    double start_time = omp_get_wtime();
    std::cout << "Beginning" << std::endl;
    
    // Get points from csv file
    string line;
    ifstream file(dataset->filename);
    int i = 0;
    bool skipFirst = dataset->skipFirstLine;

    while (getline(file, line)) {
        if (i >= n_points || i >= MAX_POINTS) break;
        if (skipFirst) {
            skipFirst = false;
            continue;
        };
        stringstream lineStream(line);
        string bit;
        double dimension;
        int d = 0;
        while(getline(lineStream, bit, ',')){
            if (d >= dataset->points_dimensions) break;
            points[i][d] = stof(bit);
            d++;
        }
        points[i][dataset->points_dimensions] = -1; // Initialize cluster assignment to -1
        i++;
    }

    double got_points_from_csv = omp_get_wtime();

    // Init clusters
    srand(in_seed ? in_seed : time(0));
    for (int c = 0; c < n_clusters && c < MAX_CLUSTERS; c++) {
        int random_point_index = rand() % n_points;
        for (int d = 0; d < dataset->points_dimensions && d < MAX_DIMENSIONS; d++) {
            centroids[c][d] = points[random_point_index][d];
            dimensions_sums[c][d] = 0; 
            cluster_size[c] = 0;
        }
    }

    double min_distance = __DBL_MAX__, distance = 0;

    double pre_iterations = omp_get_wtime();

    for(int iter = 0; iter < max_iters; iter++) {
        // Assign points to nearest centroid
        for (int p = 0; p < n_points; p++) {
            min_distance = __DBL_MAX__;
            int assigned_cluster = -1;
            for (int c = 0; c < n_clusters && c < MAX_CLUSTERS; c++) {
                distance = 0;
                for (int d = 0; d < dataset->points_dimensions && d < MAX_DIMENSIONS; d++) {
                    distance += pow(points[p][d] - centroids[c][d], 2);
                }
                distance = sqrt(distance);
                if (distance < min_distance) {
                    min_distance = distance;
                    points[p][dataset->points_dimensions] = c; // Assign cluster index to point
                    assigned_cluster = c;
                }
            }

            cluster_size[assigned_cluster]++;
            for (int d = 0; d < dataset->points_dimensions && d < MAX_DIMENSIONS; d++) {
                dimensions_sums[assigned_cluster][d] += points[p][d];
            }
        }

        // Update centroids
        for (int c = 0; c < n_clusters && c < MAX_CLUSTERS; c++) {
            for (int d = 0; d < dataset->points_dimensions && d < MAX_DIMENSIONS; d++) {
                if (cluster_size[c] > 0) {
                    centroids[c][d] = dimensions_sums[c][d] / cluster_size[c];
                }
                dimensions_sums[c][d] = 0; // Reset for next iteration
            }
            cluster_size[c] = 0; // Reset for next iteration
        }
    }

    double after_iterations = omp_get_wtime();

    // Output results to csv file
    ofstream output_file(output_name);
    for (int d = 0; d < dataset->points_dimensions; d++) {
        output_file << "dimension" << d << ",";
    }
    output_file << "cluster" << endl;
    for (int p = 0; p < n_points; p++) {
        for (int d = 0; d < dataset->points_dimensions; d++) {
            output_file << points[p][d] << ",";
        }
        output_file << points[p][dataset->points_dimensions] << endl;
    }
    
    double end_time = omp_get_wtime();
    std::cout << "Finished" << std::endl;
    std::cout << "Total elapsed time: " << end_time - start_time << " seconds" << std::endl;
    std::cout << "Time to get points from csv: " << got_points_from_csv - start_time << " seconds" << std::endl;
    std::cout << "Time for iterations: " << after_iterations - pre_iterations << " seconds" << std::endl;
    std::cout << "Time to output results: " << end_time - after_iterations << " seconds" << std::endl;
}
