//
// Created by konrad_guest on 16/01/2025.
//

#ifndef DEBUG_MAP_SOLVER_H
#define DEBUG_MAP_SOLVER_H

#include <vector>
#include <chrono>
#include <map>
#include <optional>
#include <sstream>
#include <fstream>
#include <iomanip>

class DebugMapSolver {
public:
    struct Statistics {
        uint64_t totalPaths;
        uint64_t processedPaths;
        double searchTimeMs;
        std::vector<int> solution;
        std::vector<int> inputDistances;
        bool solutionFound;
    };

    // Konstruktor z dodatkowymi parametrami do debugowania
    DebugMapSolver(const std::vector<int>& inputDistances, int length, bool enableFileLog = true, 
                   const std::string& logPath = "debug_solver.log");
    
    std::optional<std::vector<int>> solve();
    const Statistics& getStatistics() const { return stats; }

private:
    // Podstawowe pola
    std::vector<int> distances;
    std::vector<int> currentMap;
    int totalLength;
    int maxind;
    Statistics stats;
    std::map<int, int> distanceCounter;

    // Pola do debugowania
    bool debugToFile;
    std::ofstream logFile;
    int debugDepth;
    int maxReachedDepth;
    std::map<int, int> bestDistanceUsage;
    std::vector<std::pair<int, std::vector<int>>> invalidationHistory;
    
    // Debug helpers
    void logMapState(const std::string& message);
    void logBacktrack(int position, const std::string& reason);
    void logDistanceUsage();
    void logInvalidation(int position, const std::string& reason);
    std::string getIndentation() const;
    void logDistanceConstraints();
    
    // Metody analizy
    bool checkDistanceConsistency() const;
    std::vector<int> findMissingDistances() const;
    std::vector<int> findExtraDistances() const;
    bool validatePartialSolution(int depth) const;
    std::vector<std::pair<int, int>> findConflictingPairs() const;
    
    // Oryginalne metody z dodanym debugowaniem
    bool isValidPartialSolution(int assignedCount);
    void szukaj(int ind, bool* jest);
    void analyzeSolutionAttempt(int ind, int pos);
    void analyzeFailure(int ind, int pos, const std::string& reason);
    
    // Nowe metody pomocnicze do debugowania
    std::vector<int> getCurrentDistances() const;
    std::string mapToString() const;
    void dumpStateToFile(const std::string& filename) const;
    void saveSearchTree(const std::string& filename) const;
};

#endif //DEBUG_MAP_SOLVER_H
