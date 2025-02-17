#ifndef BENCHMARK_H
#define BENCHMARK_H

#include "global_paths.h"
#include "instance_generator.h"
#include "map_solver.h"
#include "algorithms/bbd_algorithm.h"
#include "algorithms/bbb_algorithm.h"
#include "algorithms/bbb2_algorithm.h"

#include <vector>
#include <string>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <cmath>
#include <random>
#include <optional>
#include <sstream>

class Benchmark {
public:
    enum class Algorithm {
        BASIC_MAP,
        BBD,
        BBB,
        BBB2
    };

    enum class BenchmarkMode {
        ALL_ALGORITHMS,
        FAST_ALGORITHMS_ONLY
    };

    enum class TestType {
        STANDARD,
        DUPLICATES,
        PATTERNS,
        EXTREME
    };

    struct BenchmarkResult {
        int instanceSize{};
        double executionTimeMs{};
        Algorithm algorithmType;
        TestType testType;
        std::string description;
        bool successful{};
    };

    struct BenchmarkConfig {
        std::vector<int> standardSizes;
        std::vector<int> specialCaseSizes;
        int repeatCount{};
        int specialCaseRepetitions{};
        BenchmarkMode mode;
    };

    struct BenchmarkSolution {
        std::vector<int> solution;
        bool found{};
        double executionTimeMs{};

        BenchmarkSolution(std::vector<int> sol = {}, bool f = false, double time = 0.0)
            : solution(std::move(sol)), found(f), executionTimeMs(time) {}
    };

    struct ValidatedResult {
        BenchmarkResult result;
        bool isValid;
        std::vector<int> solution;

        ValidatedResult(const BenchmarkResult& res, bool valid, const std::vector<int>& sol = {})
            : result(res), isValid(valid), solution(sol) {}
    };

    Benchmark();
    ~Benchmark();

    void setBenchmarkConfig(const BenchmarkConfig& config);
    void runBenchmark();
    void runSingleAlgorithmBenchmark(Algorithm algo);
    void saveResults(const std::string& filename);
    void runTestTypeBenchmark(TestType type, const std::vector<int>& sizes = {});
    void runComprehensiveBenchmark();
    void runFastAlgorithmsBenchmark();

private:
    BenchmarkConfig config;
    std::vector<BenchmarkResult> results;
    InstanceGenerator instanceGenerator;

    static constexpr int DEFAULT_REPEAT_COUNT = 5;
    std::string tempFileName{"temp_instance.txt"};

    void cleanupTempFiles();
    void createBenchmarkDirectory();
    double measureAlgorithmTime(Algorithm algo, int sizeVal);
    std::string getAlgorithmName(Algorithm algo) const;
    void prepareInstance(int sizeVal, TestType type = TestType::STANDARD);
    void createDirectoryStructure();

    std::vector<int> generateDuplicatesInstance(int sizeVal);
    std::vector<int> generatePatternsInstance(int sizeVal);
    std::vector<int> generateExtremeInstance(int sizeVal);
    std::string getTestTypeName(TestType type) const;
    std::vector<Algorithm> getAlgorithmsForMode(BenchmarkMode mode) const;

    void runBasicMapSolver(const std::vector<int>& distances, int totalLength);
    void runBBdAlgorithm(const std::vector<int>& distances);
    void runBBbAlgorithm(const std::vector<int>& distances);
    void runBBb2Algorithm(const std::vector<int>& distances);

    std::vector<ValidatedResult> validatedResults;
    std::map<std::string, std::vector<int>> referenceResults;

    BenchmarkSolution runAlgorithmWithValidation(Algorithm algo, const std::vector<int>& distances);
    bool validateSolution(const std::vector<int>& solution, const std::vector<int>& distances);
    void loadReferenceResults(const std::string& directory);
    bool compareSolutions(const std::vector<int>& sol1, const std::vector<int>& sol2);
    std::string generateInstanceHash(const std::vector<int>& distances);
};

#endif // BENCHMARK_H
