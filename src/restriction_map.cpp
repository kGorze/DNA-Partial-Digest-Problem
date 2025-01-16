#include <algorithm>
#include <random>
#include "../include/restriction_map.h"

int RestrictionMap::calculateMinimumLength(int cuts) {
    return (cuts + 1) * 10;
}

RestrictionMap::RestrictionMap(int cuts) {
    totalLength = calculateMinimumLength(cuts);
}

bool RestrictionMap::isValidMap() const {
    if (sites.empty() || sites[0] != 0 || sites.back() != totalLength) {
        return false;
    }
    for (size_t i = 1; i < sites.size(); i++) {
        if (sites[i] <= sites[i-1]) {
            return false;
        }
    }
    return true;
}

bool RestrictionMap::generateMap(int cuts) {
    int minimumLength = calculateMinimumLength(cuts);
    totalLength = static_cast<int>(minimumLength * 1.5);
    
    const int MAX_ATTEMPTS = 100000;
    int attempts = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> spacing_dist(10.0, 2.0);

    while (attempts < MAX_ATTEMPTS) {
        sites.clear();
        sites.push_back(0);
        
        bool validConfiguration = true;
        int currentPos = 0;
        
        for (int i = 0; i < cuts && validConfiguration; i++) {
            int spacing;
            do {
                spacing = static_cast<int>(std::round(spacing_dist(gen)));
            } while (spacing < 10);
            
            currentPos += spacing;
            if (currentPos > totalLength - 10 * (cuts - i)) {
                validConfiguration = false;
                break;
            }
            sites.push_back(currentPos);
        }
        
        sites.push_back(totalLength);

        if (validConfiguration && isValidMap()) {
            return true;
        }
        attempts++;
    }

    // fallback
    sites.clear();
    sites.push_back(0);
    for (int i = 1; i <= cuts; i++) {
        sites.push_back(i * 10);
    }
    sites.push_back(totalLength);

    if (isValidMap()) {
        std::cerr << "Note: a fallback was used, as the map could not be drawn.\n";
        return true;
    }
    
    std::cerr << "Failed to generate valid map even with fallback.\n";
    return false;
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
