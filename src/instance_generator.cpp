//
// Created by konrad_guest on 14/01/2025.
//


#include "../include/instance_generator.h"

#include <fstream>
#include <sstream>
#include <iostream>
#include <random>
#include <ctime>
#include <filesystem>
#include <iomanip>

namespace fs = std::filesystem;

InstanceGenerator::InstanceGenerator(int totalLength) : map(totalLength) {
    createRunDirectory();
}

void InstanceGenerator::createRunDirectory() {
    // Pobierz aktualny czas
    auto now = std::time(nullptr);
    auto local_time = *std::localtime(&now);
    
    // Utwórz nazwę katalogu w formacie run_HHMM
    std::ostringstream oss;
    oss << "run_" 
        << std::setfill('0') << std::setw(2) << local_time.tm_hour
        << std::setfill('0') << std::setw(2) << local_time.tm_min;
    
    runDirectory = oss.str();
    
    // Utwórz katalog
    fs::create_directory(runDirectory);
    std::cout << "Utworzono katalog: " << runDirectory << "\n";
}

std::string InstanceGenerator::getFullPath(const std::string& filename) const {
    return (fs::path(runDirectory) / filename).string();
}

bool InstanceGenerator::generateInstance(int cuts, const std::string& filename) {
    if(!map.generateMap(cuts)) {
        return false;
    }
    
    std::vector<int> distances = map.generateDistances();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);
    
    // Użyj pełnej ścieżki do pliku
    std::string fullPath = getFullPath(filename);
    std::ofstream file(fullPath);
    if(!file.is_open()) {
        std::cout << "Nie można otworzyć pliku: " << fullPath << "\n";
        return false;
    }
    
    for(size_t i = 0; i < distances.size(); i++) {
        file << distances[i];
        if(i < distances.size() - 1) file << " ";
    }
    file.close();
    
    // Zapisz plik weryfikacyjny również w nowym katalogu
    std::ofstream mapFile(getFullPath(filename + ".verify"));
    if(!mapFile.is_open()) {
        return false;
    }
    
    mapFile << "Cuts: " << cuts << "\n";
    mapFile << "Sites: ";
    const auto& sites = map.getSites();
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
    
    savedMaps[filename] = map;
    
    return true;
}

std::vector<int> InstanceGenerator::loadInstance(const std::string& filename) {
    std::vector<int> distances;
    std::ifstream file(getFullPath(filename));
    
    if(file.is_open()) {
        std::string line;
        std::getline(file, line);
        std::istringstream iss(line);
        int distance;
        
        while(iss >> distance) {
            distances.push_back(distance);
        }
        
        file.close();
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