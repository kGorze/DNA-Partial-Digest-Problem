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
#include <filesystem>

enum class SortOrder {
    SHUFFLED,
    ASCENDING,
    DESCENDING
};

/**
 * Klasa odpowiedzialna za generowanie instancji PDP
 * (m.in. odległości do pliku, ładowanie instancji, weryfikację itp.).
 */
class InstanceGenerator {
private:
    std::map<std::string, RestrictionMap> savedMaps;
    std::string outputDirectory;
    
    std::string getFullPath(const std::string& filename) const;

public:
    InstanceGenerator() = default;
    bool generateInstance(int cuts, const std::string& filename, SortOrder order = SortOrder::SHUFFLED);
    std::vector<int> loadInstance(const std::string& filename);
    bool verifyInstance(const std::string& filename);
    void setOutputDirectory(const std::string& dir) { outputDirectory = dir; }
    std::string getOutputDirectory() const { return outputDirectory; }

};

#endif //INSTANCE_GENERATOR_H
