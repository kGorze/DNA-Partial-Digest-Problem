//
// Created by konrad_guest on 14/01/2025.
//

#include <algorithm>
#include <random>

#include "../include/restriction_map.h"

RestrictionMap::RestrictionMap(int length) : totalLength(length) {}

bool RestrictionMap::isValidMap() const {
    if(sites.empty() || sites[0] != 0 || sites.back() != totalLength) {
        return false;
    }
    
    for(size_t i = 1; i < sites.size(); i++) {
        if(sites[i] <= sites[i-1]) {
            return false;
        }
    }
    
    return true;
}

bool RestrictionMap::generateMap(int cuts) {
    sites.clear();
    sites.push_back(0);
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(20, totalLength-20);
    
    std::vector<int> tempSites;
    for(int i = 0; i < cuts; i++) {
        int newSite = dis(gen);
        tempSites.push_back(newSite);
    }
    
    std::sort(tempSites.begin(), tempSites.end());
    bool validDistances = true;
    for(size_t i = 1; i < tempSites.size(); i++) {
        if(tempSites[i] - tempSites[i-1] < 10) {
            validDistances = false;
            break;
        }
    }
    
    if(!validDistances) {
        return generateMap(cuts);
    }
    
    sites.insert(sites.end(), tempSites.begin(), tempSites.end());
    sites.push_back(totalLength);
    
    return isValidMap();
}

std::vector<int> RestrictionMap::generateDistances() const {
    std::vector<int> distances;
    
    for(size_t i = 0; i < sites.size(); i++) {
        for(size_t j = i + 1; j < sites.size(); j++) {
            distances.push_back(sites[j] - sites[i]);
        }
    }
    
    return distances;
}

const std::vector<int>& RestrictionMap::getSites() const {
    return sites;
}

bool RestrictionMap::verifyDistances(const std::vector<int>& distances) const {
    std::vector<int> mapDistances = generateDistances();
    
    if(distances.size() != mapDistances.size()) {
        return false;
    }
    
    std::vector<int> sortedDistances = distances;
    std::vector<int> sortedMapDistances = mapDistances;
    std::sort(sortedDistances.begin(), sortedDistances.end());
    std::sort(sortedMapDistances.begin(), sortedMapDistances.end());
    
    return sortedDistances == sortedMapDistances;
}

