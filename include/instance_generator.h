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

class InstanceGenerator {
private:
    RestrictionMap map;
    std::map<std::string, RestrictionMap> savedMaps;
    std::string runDirectory;  // Dodane pole dla katalogu uruchomienia
    
    // Nowa metoda prywatna do tworzenia katalogu
    void createRunDirectory();
    // Nowa metoda do uzyskania pełnej ścieżki pliku
    std::string getFullPath(const std::string& filename) const;

public:
    InstanceGenerator(int totalLength = 512);
    bool generateInstance(int cuts, const std::string& filename);
    std::vector<int> loadInstance(const std::string& filename);
    bool verifyInstance(const std::string& filename);
    // Dodana metoda do pobrania nazwy katalogu
    std::string getRunDirectory() const { return runDirectory; }
};

#endif //INSTANCE_GENERATOR_H
