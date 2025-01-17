#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <vector>
#include <string>
#include <chrono>
#include <optional>
#include <filesystem>

#include "instance_generator.h"
#include "map_solver.h"
#include "benchmark.h"
#include "debug_map_solver.h"
#include "algorithms/bbb_algorithm.h"
#include "algorithms/bbb2_algorithm.h"
#include "algorithms/bbd_algorithm.h"
#include "data_arrangement_benchmark.h"
#include "data_arrangement_analysis.h"

class TestFramework {
private:
    static const std::chrono::hours MAX_EXECUTION_TIME;
    static constexpr int MIN_CUTS = 3;
    static constexpr int DEFAULT_RANDOM_COUNT = 10;

    InstanceGenerator& generator;
    BBbAlgorithm bbbSolver;
    Benchmark benchmark;

    struct VerificationResult {
        bool isValid;
        std::string message;
        std::optional<std::vector<int>> solution;
        double verificationTimeMs;
    };

    bool generateInstance(int cuts, const std::string& filename, SortOrder order);
    SortOrder getSortOrderFromUser();
    bool isValidNumberOfCuts(int cuts) const;
    VerificationResult verifyInstanceFile(const std::string& filepath);
    bool verifyInputSize(const std::vector<int>& distances, int expectedSize);
    bool verifyInputValues(const std::vector<int>& distances);
    bool checkCutsPossibility(int multisetSize);
    int calculateRequiredCuts(int multisetSize);
    void testAllInstances(int algorithmChoice);
    bool validateSolution(const std::vector<int>& solution, const std::vector<int>& distances);
    std::vector<int> generateDistancesFromSolution(const std::vector<int>& solution);
    bool areMultisetsEqual(std::vector<int> a, std::vector<int> b);
    std::filesystem::path getFullPath(const std::string& filename) const;
    void listAvailableInstances();
    void displayVerificationResult(const std::string& filename, const VerificationResult& result);
    bool runDebugSolver(const std::string& filename);
    void displayDebugStatistics(const DebugMapSolver::Statistics& stats);

public:
    explicit TestFramework(InstanceGenerator& gen);
    void runInteractiveMode();
    bool generateRandomInstances(int count = DEFAULT_RANDOM_COUNT, SortOrder order = SortOrder::SHUFFLED);
    bool generateInstancesRange(int maxCuts, SortOrder order = SortOrder::SHUFFLED);
    bool verifyAllInstances();
    bool solveSpecificInstance(const std::string& filename);
};

#endif // TEST_FRAMEWORK_H
