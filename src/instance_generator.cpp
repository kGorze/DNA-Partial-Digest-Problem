#include "../include/instance_generator.h"
#include <filesystem>
#include <ctime>
#include <iomanip>

namespace fs = std::filesystem;

std::string InstanceGenerator::getFullPath(const std::string& filename) const {
    if (filename.rfind("./", 0) == 0 || filename.rfind("../", 0) == 0) {
        return filename;
    }
    return (fs::path(outputDirectory) / filename).string();
}


bool InstanceGenerator::generateInstance(int cuts, const std::string& filename, SortOrder order) {
    RestrictionMap newMap(cuts);
    if(!newMap.generateMap(cuts)) {
        return false;
    }
    
    std::vector<int> distances = newMap.generateDistances();
    
    switch(order) {
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
            break;
    }
    }
    
    // Ensure the output directory exists
    if (!fs::exists(outputDirectory)) {
        fs::create_directories(outputDirectory);
    }
    
    std::string fullPath = getFullPath(filename);
    std::ofstream file(fullPath);
    if(!file.is_open()) {
        std::cerr << "Cannot open file: " << fullPath << "\n";
        return false;
    }
    
    for(size_t i = 0; i < distances.size(); i++) {
        file << distances[i];
        if(i < distances.size() - 1) file << " ";
    }
    file.close();
    
    // verification file
    std::ofstream mapFile(getFullPath(filename + ".verify"));
    if(!mapFile.is_open()) {
        return false;
    }
    
    mapFile << "Cuts: " << cuts << "\n";
    mapFile << "Sites: ";
    const auto& sites = newMap.getSites();
    for(size_t i = 0; i < sites.size(); i++) {
        mapFile << sites[i];
        if(i < sites.size() - 1) mapFile << " ";
    }
    mapFile << "\nDistances: ";
    for(size_t i = 0; i < distances.size(); i++) {
        mapFile << distances[i];
        if(i < distances.size() - 1) mapFile << " ";
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
        std::cout << "Cannot open file: " << fullPath << "\n";
        return distances;
    }
    
    std::string line;
    std::getline(file, line);
    std::istringstream iss(line);
    int distance;
    
    while (iss >> distance) {
        distances.push_back(distance);
    }
    
    file.close();
    
    if (!distances.empty()) {
        std::cout << "Loaded " << distances.size() << " distances\n";
    }
    
    return distances;
}

bool InstanceGenerator::verifyInstance(const std::string& filename) {
    auto it = savedMaps.find(filename);
    if(it == savedMaps.end()) {
        std::cout << "Mapa dla instancji " << filename << " nie została znaleziona.\n";
        return false;
    }
    
    std::vector<int> distances = loadInstance(filename);
    return it->second.verifyDistances(distances);
}
