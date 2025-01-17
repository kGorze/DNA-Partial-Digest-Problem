#ifndef BBB2_ALGORITHM_H
#define BBB2_ALGORITHM_H

#include <vector>
#include <map>
#include <optional>
#include <algorithm>
#include <queue>
#include <set>
#include <iostream>
#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <unordered_set>

#include "bbb_algorithm.h"

class MultisetD {
private:
    std::map<int,int> counts;
public:
    void add(int value) {
        counts[value]++;
    }
    void remove(int value) {
        auto it = counts.find(value);
        if (it != counts.end()) {
            if (--it->second == 0) {
                counts.erase(it);
            }
        }
    }
    bool contains(int value, int cnt=1) const {
        auto it = counts.find(value);
        return (it != counts.end() && it->second >= cnt);
    }
    bool empty() const {
        return counts.empty();
    }
    std::vector<int> toVector() const {
        std::vector<int> result;
        result.reserve(256);
        for (auto &kv : counts) {
            for(int i = 0; i < kv.second; i++) {
                result.push_back(kv.first);
            }
        }
        return result;
    }
    static MultisetD fromVector(const std::vector<int>& vec){
        MultisetD ms;
        for (int x : vec) {
            ms.add(x);
        }
        return ms;
    }
};

class BBb2Algorithm {
public:
    BBb2Algorithm() = default;
    std::optional<std::vector<int>> solve(std::vector<int> D);

private:
    BBbAlgorithm bbbSolver;
    std::vector<int> originalDistances;

    struct AlphaNode {
        MultisetD D;
        std::vector<int> X;
        AlphaNode(MultisetD d, std::vector<int> x) 
            : D(std::move(d)), X(std::move(x)) {}
    };

    void buildToAlpha(std::vector<AlphaNode>& alphaNodes,
                      const std::vector<int>& initialD,
                      const std::vector<int>& initialX,
                      int alpha);

    std::optional<std::vector<int>> processAlphaNode(const AlphaNode& node);
    bool isValidSolution(const std::vector<int>& X, const std::vector<int>& origD) const;
    bool removeDelta(MultisetD &mD, int y, const std::vector<int>& X);
    int calculateN(int setSize) const;
    int findAlphaM(int N) const;
};

#endif // BBB2_ALGORITHM_H
