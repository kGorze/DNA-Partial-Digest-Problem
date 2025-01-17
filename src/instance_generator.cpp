#include "../include/instance_generator.h"
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

InstanceGenerator::InstanceGenerator() {
    // By default, store instances in GlobalPaths::INSTANCES_DIR
    outputDirectory = GlobalPaths::INSTANCES_DIR;
}

void InstanceGenerator::setOutputDirectory(const std::string& dir) {
    outputDirectory = fs::path(dir);
    fs::create_directories(outputDirectory);
}

std::string InstanceGenerator::getOutputDirectory() const {
    return outputDirectory.string();
}

std::string InstanceGenerator::getFullPath(const std::string& filename) const {
    fs::path filePath(filename);
    if (filePath.is_absolute() ||
        filename.rfind("./", 0) == 0 ||
        filename.rfind("../", 0) == 0) {
        return filePath.string();
    }
    return (outputDirectory / filePath).string();
}

bool InstanceGenerator::generateInstance(int cuts, const std::string& filename, SortOrder order) {
    RestrictionMap newMap(cuts);
    if (!newMap.generateMap(cuts)) {
        return false;
    }
    std::vector<int> distances = newMap.generateDistances();
    switch (order) {
        case SortOrder::ASCENDING:
            std::sort(distances.begin(), distances.end());
            break;
        case SortOrder::DESCENDING:
            std::sort(distances.begin(), distances.end(), std::greater<>());
            break;
        case SortOrder::SHUFFLED:
        default: {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::shuffle(distances.begin(), distances.end(), gen);
        }
    }

    // Make sure the outputDirectory is valid
    fs::create_directories(outputDirectory);

    // Write the instance file
    std::string fullPath = getFullPath(filename);
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << fullPath << "\n";
        return false;
    }
    for (size_t i = 0; i < distances.size(); i++) {
        file << distances[i];
        if (i < distances.size() - 1) file << " ";
    }
    file.close();

    // Write a verification file
    std::string verifyPath = getFullPath(filename + ".verify");
    std::ofstream mapFile(verifyPath);
    if (!mapFile.is_open()) {
        std::cerr << "Cannot open verification file: " << verifyPath << "\n";
        return false;
    }
    mapFile << "Cuts: " << cuts << "\n";
    mapFile << "Sites: ";
    const auto& sites = newMap.getSites();
    for (size_t i = 0; i < sites.size(); i++) {
        mapFile << sites[i];
        if (i < sites.size() - 1) mapFile << " ";
    }
    mapFile << "\nDistances: ";
    for (size_t i = 0; i < distances.size(); i++) {
        mapFile << distances[i];
        if (i < distances.size() - 1) mapFile << " ";
    }
    mapFile.close();

    savedMaps[filename] = std::move(newMap);
    return true;
}

std::vector<int> InstanceGenerator::loadInstance(const std::string& filename) {
    std::vector<int> distances;
    std::string fullPath = getFullPath(filename);
    std::ifstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << fullPath << "\n";
        return distances;
    }
    std::string line;
    std::getline(file, line);
    file.close();

    std::istringstream iss(line);
    int val;
    while (iss >> val) {
        distances.push_back(val);
    }
    if (!distances.empty()) {
        std::cout << "Loaded " << distances.size() << " distances from " << fullPath << "\n";
    }
    return distances;
}

bool InstanceGenerator::verifyInstance(const std::string& filename) {
    auto it = savedMaps.find(filename);
    if (it == savedMaps.end()) {
        std::cout << "Map not found for instance " << filename << ".\n";
        return false;
    }
    std::vector<int> distances = loadInstance(filename);
    return it->second.verifyDistances(distances);
}
