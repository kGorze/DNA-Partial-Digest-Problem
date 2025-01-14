//
// Created by konrad_guest on 14/01/2025.
//

#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <vector>
#include <chrono>

#include <vector>
#include <chrono>

class MapSolver {
private:
    std::vector<int> distances;    // Input multiset of distances
    std::vector<int> currentMap;   // Currently constructed map
    int totalLength;               // Total map length
    int maxind;                    // Maximum index (number of cuts + 2)
    
    // Progress tracking
    uint64_t totalPaths;          // Total number of possible paths
    uint64_t processedPaths;      // Number of processed paths
    std::chrono::steady_clock::time_point startTime;
    
    // Statistics
    struct Statistics {
        uint64_t totalPaths;
        uint64_t processedPaths;
        double searchTimeMs;
        std::vector<int> solution;
        std::vector<int> inputDistances;
        bool solutionFound;
    };
    Statistics stats;
    
    // Private methods
    /**
     * Zmieniona sygnatura: teraz przyjmujemy liczbę faktycznie przypisanych indeksów.
     */
    bool isValidPartialSolution(int assignedCount) const;
    
    void szukaj(int ind, bool* jest);
    uint64_t calculateTotalPaths() const;
    void updateProgress();
    void displayProgress() const;

public:
    // Constructor
    MapSolver(const std::vector<int>& inputDistances, int length);
    
    // Main solving method
    bool solve();
    
    // Getters
    const std::vector<int>& getSolution() const { return stats.solution; }
    const Statistics& getStatistics() const { return stats; }
    
    int getNumberOfCuts() const { return maxind - 2; }
    int getTotalLength() const { return totalLength; }
};

#endif //MAP_SOLVER_H
