#ifndef RESTRICTION_MAP_H
#define RESTRICTION_MAP_H

#include <vector>

class RestrictionMap {
private:
    std::vector<int> sites;
    int totalLength{};
    bool isValidMap() const;
    static int calculateMinimumLength(int cuts);

public:
    RestrictionMap() = default;
    explicit RestrictionMap(int cuts);

    void setTotalLength(int length) { totalLength = length; }
    bool generateMap(int cuts);
    std::vector<int> generateDistances() const;
    const std::vector<int>& getSites() const { return sites; }
    bool verifyDistances(const std::vector<int>& distances) const;
    int getTotalLength() const { return totalLength; }
};

#endif // RESTRICTION_MAP_H
