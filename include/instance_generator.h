//
// Created by konrad_guest on 14/01/2025.
//

#ifndef INSTANCE_GENERATOR_H
#define INSTANCE_GENERATOR_H

#include "../include/restriction_map.h"
#include <map>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <algorithm>

enum class SortOrder {
    SHUFFLED,
    ASCENDING,
    DESCENDING
};

class InstanceGenerator {
private:
    std::map<std::string, RestrictionMap> savedMaps;
    std::string runDirectory;
    
    void createRunDirectory();
    std::string getFullPath(const std::string& filename) const;

public:
    InstanceGenerator() = default;
    bool generateInstance(int cuts, const std::string& filename, SortOrder order = SortOrder::SHUFFLED);
    std::vector<int> loadInstance(const std::string& filename);
    bool verifyInstance(const std::string& filename);
    std::string getRunDirectory() const { return runDirectory; }
};


#endif //INSTANCE_GENERATOR_H
