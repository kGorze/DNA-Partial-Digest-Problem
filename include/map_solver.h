#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <vector>
#include <chrono>
#include <map>

class MapSolver {
public:
    struct Statistics {
        uint64_t totalPaths;
        uint64_t processedPaths;
        double searchTimeMs;
        std::vector<int> solution;
        std::vector<int> inputDistances;
        bool solutionFound;
    };

    /**
     * @brief Constructs a MapSolver with given distances and total length
     * @param inputDistances Vector of distances between points
     * @param length Total length of the map
     */
    MapSolver(const std::vector<int>& inputDistances, int length);

    /**
     * @brief Solves the map problem using original algorithm
     * @return true if solution is found, false otherwise
     */
    bool solve();

    /**
     * @brief Solves the map problem with additional distance constraints
     * @return true if solution is found, false otherwise
     */
    bool solveWithCondition();

    // Getters
    const std::vector<int>& getSolution() const { return stats.solution; }
    const Statistics& getStatistics() const { return stats; }
    int getNumberOfCuts() const { return maxind - 2; }
    int getTotalLength() const { return totalLength; }

private:
    // Core data
    std::vector<int> distances;
    std::vector<int> currentMap;
    int totalLength;
    int maxind;
    
    // Progress tracking
    uint64_t totalPaths;
    uint64_t processedPaths;
    std::chrono::steady_clock::time_point startTime;
    Statistics stats;

    // Distance tracking
    std::map<int, int> remainingDistances;
    std::map<int, int> distanceCounter;

    // Core algorithm methods
    bool isValidPartialSolution(int assignedCount) const;
    void szukaj(int ind, bool* jest);
    void szukajWithCondition(int ind, bool* jest);

    // Distance handling methods
    void initializeRemainingDistances();
    bool updateDistanceUsage(int distance, bool add);
    bool canAddPosition(int pos, int ind, std::vector<int>& usedDistances);
    void revertPosition(const std::vector<int>& usedDistances);
    bool allDistancesUsedUp() const;
    std::vector<int> getUnusedDistances() const;

    // Utility methods
    uint64_t calculateTotalPaths() const;
    void updateProgress();
    void displayProgress() const;
    std::vector<int> calculateDistancesBetweenPoints(const std::vector<int>& points) const;
    void displaySolution() const;
    void printRemainingDistances() const;
};

#endif // MAP_SOLVER_H