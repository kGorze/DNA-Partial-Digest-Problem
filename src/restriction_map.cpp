#include "../include/restriction_map.h"
#include <algorithm>
#include <random>
#include <iostream>
#include <cmath>

int RestrictionMap::calculateMinimumLength(int cuts) {
    // Minimum length is a heuristic: (cuts+1)*10
    return (cuts + 1) * 10;
}

RestrictionMap::RestrictionMap(int cuts) {
    totalLength = calculateMinimumLength(cuts);
}

bool RestrictionMap::isValidMap() const {
    if (sites.empty() || sites.front() != 0 || sites.back() != totalLength) {
        return false;
    }
    for (size_t i = 1; i < sites.size(); i++) {
        if (sites[i] <= sites[i - 1]) {
            return false;
        }
    }
    return true;
}

bool RestrictionMap::generateMap(int cuts) {
    int minLength = calculateMinimumLength(cuts);
    totalLength = static_cast<int>(minLength * 1.5);

    constexpr int MAX_ATTEMPTS = 100000;
    int attempts = 0;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> spacingDist(10.0, 2.0);

    while (attempts < MAX_ATTEMPTS) {
        sites.clear();
        sites.push_back(0);
        bool validConfig = true;
        int currentPos = 0;

        for (int i = 0; i < cuts && validConfig; i++) {
            int spacing = 0;
            do {
                spacing = static_cast<int>(std::round(spacingDist(gen)));
            } while (spacing < 10);

            currentPos += spacing;
            if (currentPos > totalLength - 10 * (cuts - i)) {
                validConfig = false;
                break;
            }
            sites.push_back(currentPos);
        }
        sites.push_back(totalLength);

        if (validConfig && isValidMap()) {
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
        std::cerr << "Fallback used. Random generation failed.\n";
        return true;
    }
    std::cerr << "Failed to generate valid map even with fallback.\n";
    return false;
}

std::vector<int> RestrictionMap::generateDistances() const {
    std::vector<int> distances;
    distances.reserve(sites.size() * (sites.size() - 1) / 2);
    for (size_t i = 0; i < sites.size(); i++) {
        for (size_t j = i + 1; j < sites.size(); j++) {
            distances.push_back(sites[j] - sites[i]);
        }
    }
    return distances;
}

bool RestrictionMap::verifyDistances(const std::vector<int>& distances) const {
    std::vector<int> mapDistances = generateDistances();
    if (distances.size() != mapDistances.size()) {
        return false;
    }
    std::vector<int> sortedDist = distances;
    std::vector<int> sortedMap = mapDistances;
    std::sort(sortedDist.begin(), sortedDist.end());
    std::sort(sortedMap.begin(), sortedMap.end());
    return (sortedDist == sortedMap);
}
