#ifndef DEBUG_MAP_SOLVER_H
#define DEBUG_MAP_SOLVER_H

#include <vector>
#include <chrono>
#include <map>
#include <optional>
#include <sstream>
#include <fstream>
#include <iomanip>

/**
 * DebugMapSolver is a variant of MapSolver with detailed logging of each step.
 * It can be used for education or in-depth debugging.
 */
class DebugMapSolver {
public:
    struct Statistics {
        uint64_t totalPaths{};
        uint64_t processedPaths{};
        double searchTimeMs{};
        std::vector<int> solution;
        std::vector<int> inputDistances;
        bool solutionFound{};
    };

    DebugMapSolver(const std::vector<int>& inputDistances,
                   int length,
                   bool enableFileLog = true,
                   const std::string& logPath = "debug_solver.log");

    std::optional<std::vector<int>> solve();
    const Statistics& getStatistics() const { return stats; }

private:
    std::vector<int> distances;
    std::vector<int> currentMap;
    int totalLength{};
    int maxind{};
    Statistics stats;
    std::map<int, int> distanceCounter;

    bool debugToFile;
    std::ofstream logFile;
    int debugDepth{};
    int maxReachedDepth{};
    std::map<int, int> bestDistanceUsage;
    std::vector<std::pair<int, std::vector<int>>> invalidationHistory;

    void logMapState(const std::string& message);
    void logDistanceConstraints();
    std::string getIndentation() const;
    bool isValidPartialSolution(int assignedCount);
    void searchSolver(int ind, bool& foundSolution);
    std::vector<std::pair<int, int>> findConflictingPairs() const;
    void analyzeSolutionAttempt(int ind, int pos);
    void analyzeFailure(int ind, int pos, const std::string& reason);
    void dumpStateToFile(const std::string& filename) const;
};

#endif //DEBUG_MAP_SOLVER_H
