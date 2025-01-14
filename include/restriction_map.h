//
// Created by konrad_guest on 14/01/2025.
//

#ifndef RESTRICTION_MAP_H
#define RESTRICTION_MAP_H

#include <vector>
#include <iostream>

class RestrictionMap {
private:
    std::vector<int> sites;
    int totalLength;
    bool isValidMap() const;
    
    static int calculateMinimumLength(int cuts);

public:
    // Dodajemy konstruktor domyślny
    RestrictionMap() : totalLength(0) {}
    // Konstruktor z parametrem
    explicit RestrictionMap(int cuts);
    
    // Setter dla totalLength
    void setTotalLength(int length) { totalLength = length; }
    
    bool generateMap(int cuts);
    std::vector<int> generateDistances() const;
    const std::vector<int>& getSites() const;
    bool verifyDistances(const std::vector<int>& distances) const;
    int getTotalLength() const { return totalLength; }
};

#endif //RESTRICTION_MAP_H
