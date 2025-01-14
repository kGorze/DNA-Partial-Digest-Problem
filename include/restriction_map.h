//
// Created by konrad_guest on 14/01/2025.
//

#ifndef RESTRICTION_MAP_H
#define RESTRICTION_MAP_H

#include <vector>

class RestrictionMap {
private:
    std::vector<int> sites;
    int totalLength;
    bool isValidMap() const;

public:
    RestrictionMap(int length = 512);
    bool generateMap(int cuts);
    std::vector<int> generateDistances() const;
    const std::vector<int>& getSites() const;
    bool verifyDistances(const std::vector<int>& distances) const;
};

#endif //RESTRICTION_MAP_H
