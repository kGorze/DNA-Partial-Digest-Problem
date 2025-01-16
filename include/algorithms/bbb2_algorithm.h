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

#include "bbb_algorithm.h"

/**
 * Struktura MultisetD - do przechowywania wielokrotnego zbioru liczb (D).
 */
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
        for (auto &kv: counts) {
            for(int i=0; i<kv.second; i++){
                result.push_back(kv.first);
            }
        }
        return result;
    }
    static MultisetD fromVector(const std::vector<int>& vec){
        MultisetD ms;
        for(int x : vec){
            ms.add(x);
        }
        return ms;
    }
};

/**
 * BBb2Algorithm: 
 * 1) Buduje rozwiązania metodą BFS do pewnego poziomu alpha (buildToAlpha)
 * 2) Następnie każdy węzeł z tego poziomu alpha jest przekazywany do "drugiego" 
 *    algorytmu BBb (solvePartial), by dokończyć budowę zbioru X.
 */
class BBb2Algorithm {
public:
    BBb2Algorithm() : debugMode(true) {}

    std::optional<std::vector<int>> solve(std::vector<int> D);
    void setDebugMode(bool enable) { debugMode = enable; }

private:
    bool debugMode;
    BBbAlgorithm bbbSolver;            
    std::vector<int> originalDistances;

    struct AlphaNode {
        MultisetD D;
        std::vector<int> X;
        AlphaNode(MultisetD d, std::vector<int> x) 
            : D(std::move(d)), X(std::move(x)) {}
    };

    // Główna faza BFS do poziomu alpha
    void buildToAlpha(std::vector<AlphaNode>& alphaNodes,
                      const std::vector<int>& initialD,
                      const std::vector<int>& initialX,
                      int alpha);

    // Druga faza - z każdego węzła alpha odpalamy bbbSolver w trybie "Partial"
    std::optional<std::vector<int>> processAlphaNode(const AlphaNode& node);

    // Sprawdza, czy X generuje taki sam multizbiór odległości co originalDistances
    bool isValidSolution(const std::vector<int>& X,
                         const std::vector<int>& origD) const;

    // Usuwa z multizbioru D wartości |y - x| dla x w X (o ile wszystkie się da)
    bool removeDelta(MultisetD &mD, int y, const std::vector<int>& X);

    // Funkcje pomocnicze
    int calculateN(int setSize) const;
    int findAlphaM(int N) const;

    void debugPrint(const std::string& msg) const {
        if(debugMode){
            std::cout << "[BBb2 debug] " << msg << std::endl;
        }
    }
    std::string toStr(const std::vector<int>& vec) const {
        std::stringstream ss;
        ss << "{";
        for(size_t i=0;i<vec.size();i++){
            ss<<vec[i]; 
            if(i+1<vec.size()) ss<<",";
        }
        ss<<"}";
        return ss.str();
    }
    std::string toStr(const MultisetD& mD) const {
        auto dv = mD.toVector();
        std::sort(dv.begin(), dv.end());
        std::stringstream ss;
        ss<<"[";
        for(size_t i=0;i<dv.size();i++){
            ss<<dv[i];
            if(i+1<dv.size()) ss<<",";
        }
        ss<<"]";
        return ss.str();
    }
};

#endif // BBB2_ALGORITHM_H
