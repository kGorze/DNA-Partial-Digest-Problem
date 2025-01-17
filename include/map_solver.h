#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <vector>
#include <chrono>
#include <map>
#include <optional>

/**
 * MapSolver - a simplified PDE solver using backtracking
 */
class MapSolver {
public:
    struct Statistics {
        uint64_t totalPaths{};
        uint64_t processedPaths{};
        double searchTimeMs{};
        std::vector<int> solution;
        std::vector<int> inputDistances;
        bool solutionFound{};
    };

    MapSolver(const std::vector<int>& inputDistances, int length);
    std::optional<std::vector<int>> solve();
    std::optional<std::vector<int>> solveWithCondition();

    const std::vector<int>& getSolution() const { return stats.solution; }
    const Statistics& getStatistics() const { return stats; }
    int getNumberOfCuts() const { return maxind - 2; }
    int getTotalLength() const { return totalLength; }

private:
    std::vector<int> distances;
    std::vector<int> currentMap;
    int totalLength{};
    int maxind{};

    uint64_t totalPaths{};
    uint64_t processedPaths{};
    std::chrono::steady_clock::time_point startTime;
    Statistics stats;

    std::map<int, int> remainingDistances;
    std::map<int, int> distanceCounter;

    bool isValidPartialSolution(int assignedCount) const;
    void searchSolver(int ind, bool& foundSolution);
    void searchSolverWithCondition(int ind, bool& foundSolution);

    void initializeRemainingDistances();
    bool updateDistanceUsage(int distance, bool add);
    uint64_t calculateTotalPaths() const;
    void updateProgress();
    void displayProgress() const;
};

#endif // MAP_SOLVER_H
