#ifndef DATASET_H
#define DATASET_H

#include <string>

struct Dataset {
    std::string filename;
    int points_dimensions;
    bool skipFirstLine;
};

#endif // DATASET_H