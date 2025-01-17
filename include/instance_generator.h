#ifndef INSTANCE_GENERATOR_H
#define INSTANCE_GENERATOR_H

#include "restriction_map.h"
#include "global_paths.h"
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
 * Class that generates PDE instances and writes them to files.
 */
class InstanceGenerator {
private:
    std::map<std::string, RestrictionMap> savedMaps;
    std::filesystem::path outputDirectory;

    std::string getFullPath(const std::string& filename) const;

public:
    InstanceGenerator();

    bool generateInstance(int cuts, const std::string& filename, SortOrder order = SortOrder::SHUFFLED);
    std::vector<int> loadInstance(const std::string& filename);
    bool verifyInstance(const std::string& filename);

    void setOutputDirectory(const std::string& dir);
    std::string getOutputDirectory() const;
};

#endif // INSTANCE_GENERATOR_H
