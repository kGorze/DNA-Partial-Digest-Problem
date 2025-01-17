#include "../include/benchmark.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <limits>

namespace fs = std::filesystem;

Benchmark::Benchmark() {
    GlobalPaths::createGlobalDirectories();
    config.standardSizes         = {10, 20, 50, 100};
    config.specialCaseSizes      = {100};
    config.repeatCount           = DEFAULT_REPEAT_COUNT;
    config.specialCaseRepetitions= 100;
    config.mode                  = BenchmarkMode::ALL_ALGORITHMS;

    createDirectoryStructure();
}

Benchmark::~Benchmark() {
    cleanupTempFiles();
}

void Benchmark::createDirectoryStructure() {
    if (!fs::exists(GlobalPaths::BENCHMARK_DIR)) {
        fs::create_directories(GlobalPaths::BENCHMARK_DIR);
    }
}

void Benchmark::setBenchmarkConfig(const BenchmarkConfig& newConfig) {
    config = newConfig;
}

void Benchmark::createBenchmarkDirectory() {
    if (!fs::exists(GlobalPaths::BENCHMARK_DIR)) {
        fs::create_directories(GlobalPaths::BENCHMARK_DIR);
    }
}

void Benchmark::cleanupTempFiles() {
    if (fs::exists(GlobalPaths::TEMP_INSTANCE_FILE)) {
        fs::remove(GlobalPaths::TEMP_INSTANCE_FILE);
    }
    if (fs::exists(GlobalPaths::BENCHMARK_DIR)) {
        for (const auto& entry : fs::directory_iterator(GlobalPaths::BENCHMARK_DIR)) {
            const auto name = entry.path().filename().string();
            if (name.rfind("benchmark_instance_", 0) == 0) {
                fs::remove(entry.path());
            }
        }
    }
}

void Benchmark::runBenchmark() {
    std::cout << "\nBenchmark configuration:\n"
              << "1. Change sizes for standard tests\n"
              << "2. Change sizes for special cases\n"
              << "3. Change number of repetitions\n"
              << "4. Select algorithm to test\n"
              << "5. Test fast algorithms only (BBd, BBb, BBb2)\n"
              << "6. Use current configuration\n";

    int choice = 0;
    while (!(std::cin >> choice) || choice < 1 || choice > 6) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Invalid choice. Select a number from 1 to 6: ";
    }

    if (choice == 1) {
        std::cout << "Enter the number of different sizes: ";
        int count;
        while (!(std::cin >> count) || count <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid value. Enter a positive integer: ";
        }
        config.standardSizes.clear();
        std::cout << "Enter " << count << " sizes:\n";
        for (int i = 0; i < count; ++i) {
            int sizeVal;
            while (!(std::cin >> sizeVal) || sizeVal <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid size. Enter a positive integer: ";
            }
            config.standardSizes.push_back(sizeVal);
        }
        runBenchmark();
        return;
    } else if (choice == 2) {
        std::cout << "Enter the number of different sizes for special cases: ";
        int count;
        while (!(std::cin >> count) || count <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid value. Enter a positive integer: ";
        }
        config.specialCaseSizes.clear();
        std::cout << "Enter " << count << " sizes:\n";
        for (int i = 0; i < count; ++i) {
            int sizeVal;
            while (!(std::cin >> sizeVal) || sizeVal <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid size. Enter a positive integer: ";
            }
            config.specialCaseSizes.push_back(sizeVal);
        }
        runBenchmark();
        return;
    } else if (choice == 3) {
        std::cout << "Enter the number of repetitions for standard tests: ";
        while (!(std::cin >> config.repeatCount) || config.repeatCount <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid value. Enter a positive integer: ";
        }
        std::cout << "Enter the number of repetitions for special cases: ";
        while (!(std::cin >> config.specialCaseRepetitions) || config.specialCaseRepetitions <= 0) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid value. Enter a positive integer: ";
        }
        runBenchmark();
        return;
    } else if (choice == 4) {
        std::cout << "\nSelect algorithm to test:\n"
                  << "1. Basic Map Solver (Original)\n"
                  << "2. BBd (Branch and Bound)\n"
                  << "3. BBb (Improved Branch and Bound)\n"
                  << "4. BBb2 (Best Variant)\n"
                  << "5. All algorithms\n";

        int algoChoice = 0;
        while (!(std::cin >> algoChoice) || algoChoice < 1 || algoChoice > 5) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid choice. Select a number from 1 to 5: ";
        }

        config.mode = BenchmarkMode::ALL_ALGORITHMS;
        if (algoChoice >= 1 && algoChoice <= 4) {
            runSingleAlgorithmBenchmark(static_cast<Algorithm>(algoChoice - 1));
        } else if (algoChoice == 5) {
            runComprehensiveBenchmark();
        }
    } else if (choice == 5) {
        config.mode = BenchmarkMode::FAST_ALGORITHMS_ONLY;
        runFastAlgorithmsBenchmark();
    } else if (choice == 6) {
        runComprehensiveBenchmark();
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::string outName = "benchmark_results_" + std::to_string(timestamp) + ".csv";
    saveResults(outName);

    cleanupTempFiles();
}

void Benchmark::runSingleAlgorithmBenchmark(Algorithm algo) {
    std::cout << "\nRunning benchmark for algorithm: " << getAlgorithmName(algo) << "\n";
    for (int sizeVal : config.standardSizes) {
        std::cout << "\nTesting size n=" << sizeVal << "\n";
        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(sizeVal, TestType::STANDARD);
            std::vector<int> distances = instanceGenerator.loadInstance(GlobalPaths::TEMP_INSTANCE_FILE.string());
            auto benchResult = runAlgorithmWithValidation(algo, distances);

            BenchmarkResult result{
                sizeVal,
                benchResult.executionTimeMs,
                algo,
                TestType::STANDARD,
                "Test type: Standard, size: " + std::to_string(sizeVal),
                benchResult.found
            };
            results.push_back(result);

            validatedResults.emplace_back(
                result,
                validateSolution(benchResult.solution, distances),
                benchResult.solution
            );
        }
    }
}

void Benchmark::runFastAlgorithmsBenchmark() {
    std::cout << "Running benchmark for fast algorithms (BBd, BBb, BBb2)...\n";
    auto algorithms = getAlgorithmsForMode(BenchmarkMode::FAST_ALGORITHMS_ONLY);

    for (int sizeVal : config.standardSizes) {
        std::cout << "\nTesting size n=" << sizeVal << "\n";
        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(sizeVal, TestType::STANDARD);
            std::vector<int> distances = instanceGenerator.loadInstance(GlobalPaths::TEMP_INSTANCE_FILE.string());

            for (Algorithm algo : algorithms) {
                auto benchResult = runAlgorithmWithValidation(algo, distances);
                BenchmarkResult result{
                    sizeVal,
                    benchResult.executionTimeMs,
                    algo,
                    TestType::STANDARD,
                    "Test type: Standard, size: " + std::to_string(sizeVal),
                    benchResult.found
                };
                results.push_back(result);

                std::cout << getAlgorithmName(algo) << ": "
                          << benchResult.executionTimeMs << "ms "
                          << (benchResult.found ? "(solution found)" : "(no solution)") << "\n";
            }
        }
    }
}

std::vector<Benchmark::Algorithm> Benchmark::getAlgorithmsForMode(BenchmarkMode mode) const {
    if (mode == BenchmarkMode::ALL_ALGORITHMS) {
        return {Algorithm::BASIC_MAP, Algorithm::BBD, Algorithm::BBB, Algorithm::BBB2};
    } else {
        return {Algorithm::BBD, Algorithm::BBB, Algorithm::BBB2};
    }
}

void Benchmark::runTestTypeBenchmark(TestType type, const std::vector<int>& sizes) {
    std::cout << "\nRunning benchmark for test type: " << getTestTypeName(type) << "\n";
    for (int sizeVal : sizes) {
        std::cout << "\nTesting size n=" << sizeVal << "\n";
        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(sizeVal, type);
            for (int algoType = 0; algoType < 4; ++algoType) {
                Algorithm algo = static_cast<Algorithm>(algoType);
                double time = measureAlgorithmTime(algo, sizeVal);
                results.push_back({
                    sizeVal,
                    time,
                    algo,
                    type,
                    "Test type: " + getTestTypeName(type) + ", size: " + std::to_string(sizeVal),
                    (time >= 0)
                });
            }
        }
    }
}

void Benchmark::runComprehensiveBenchmark() {
    std::cout << "Starting comprehensive benchmark...\n";
    runTestTypeBenchmark(TestType::STANDARD, config.standardSizes);

    int originalRepeatCount = config.repeatCount;
    config.repeatCount = config.specialCaseRepetitions;
    for (int sizeVal : config.specialCaseSizes) {
        runTestTypeBenchmark(TestType::DUPLICATES, {sizeVal});
        runTestTypeBenchmark(TestType::PATTERNS, {sizeVal});
        runTestTypeBenchmark(TestType::EXTREME, {sizeVal});
    }
    config.repeatCount = originalRepeatCount;

    std::cout << "\nBenchmark completed.\n";
}

void Benchmark::saveResults(const std::string& filename) {
    createDirectoryStructure();
    fs::path fullPath = GlobalPaths::BENCHMARK_DIR / filename;
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open file: " << fullPath << "\n";
        return;
    }
    file << "algorithm,test_type,size,time_ms,success,description\n";
    for (const auto& result : results) {
        file << getAlgorithmName(result.algorithmType) << ","
             << getTestTypeName(result.testType) << ","
             << result.instanceSize << ","
             << result.executionTimeMs << ","
             << (result.successful ? "true" : "false") << ","
             << "\"" << result.description << "\"\n";
    }
    file.close();
    std::cout << "\nResults saved to: " << fullPath.string() << "\n";
}

double Benchmark::measureAlgorithmTime(Algorithm algo, int sizeVal) {
    std::vector<int> distances = instanceGenerator.loadInstance(GlobalPaths::TEMP_INSTANCE_FILE.string());
    int totalLength = sizeVal * 2; 
    auto start = std::chrono::high_resolution_clock::now();
    try {
        switch (algo) {
            case Algorithm::BASIC_MAP:
                runBasicMapSolver(distances, totalLength);
                break;
            case Algorithm::BBD:
                runBBdAlgorithm(distances);
                break;
            case Algorithm::BBB:
                runBBbAlgorithm(distances);
                break;
            case Algorithm::BBB2:
                runBBb2Algorithm(distances);
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during algorithm execution: " << e.what() << std::endl;
        return -1.0;
    }
    auto end = std::chrono::high_resolution_clock::now();
    return static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );
}

Benchmark::BenchmarkSolution Benchmark::runAlgorithmWithValidation(
    Algorithm algo,
    const std::vector<int>& distances
) {
    auto start = std::chrono::high_resolution_clock::now();
    std::optional<std::vector<int>> solution;
    try {
        switch (algo) {
            case Algorithm::BASIC_MAP: {
                MapSolver solver(distances, static_cast<int>(distances.size() * 2));
                solution = solver.solve();
                break;
            }
            case Algorithm::BBD:
                solution = BBdAlgorithm().solve(distances);
                break;
            case Algorithm::BBB:
                solution = BBbAlgorithm().solve(distances);
                break;
            case Algorithm::BBB2:
                solution = BBb2Algorithm().solve(distances);
                break;
        }
    } catch (const std::exception& e) {
        std::cerr << "Error during algorithm execution: " << e.what() << std::endl;
        return BenchmarkSolution();
    }
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );
    return BenchmarkSolution(solution.value_or(std::vector<int>()), solution.has_value(), elapsed);
}

std::string Benchmark::getAlgorithmName(Algorithm algo) const {
    switch (algo) {
        case Algorithm::BASIC_MAP: return "BasicMapSolver";
        case Algorithm::BBD:       return "BBd";
        case Algorithm::BBB:       return "BBb";
        case Algorithm::BBB2:      return "BBb2";
        default:                   return "Unknown";
    }
}

void Benchmark::prepareInstance(int sizeVal, TestType type) {
    switch (type) {
        case TestType::STANDARD:
            instanceGenerator.generateInstance(sizeVal, GlobalPaths::TEMP_INSTANCE_FILE.string(), SortOrder::SHUFFLED);
            break;
        case TestType::DUPLICATES: {
            auto duplicates = generateDuplicatesInstance(sizeVal);
            std::ofstream file(GlobalPaths::TEMP_INSTANCE_FILE);
            for (int d : duplicates) {
                file << d << " ";
            }
            break;
        }
        case TestType::PATTERNS: {
            auto patterns = generatePatternsInstance(sizeVal);
            std::ofstream file(GlobalPaths::TEMP_INSTANCE_FILE);
            for (int d : patterns) {
                file << d << " ";
            }
            break;
        }
        case TestType::EXTREME: {
            auto extremes = generateExtremeInstance(sizeVal);
            std::ofstream file(GlobalPaths::TEMP_INSTANCE_FILE);
            for (int d : extremes) {
                file << d << " ";
            }
            break;
        }
    }
}

std::string Benchmark::getTestTypeName(TestType type) const {
    switch (type) {
        case TestType::STANDARD:   return "Standard";
        case TestType::DUPLICATES: return "Duplicates";
        case TestType::PATTERNS:   return "Patterns";
        case TestType::EXTREME:    return "Extreme";
        default:                   return "Unknown";
    }
}

std::vector<int> Benchmark::generateDuplicatesInstance(int sizeVal) {
    std::vector<int> distances;
    distances.reserve(sizeVal * (sizeVal - 1) / 2);
    int baseDistance = 10;
    for (int i = 0; i < sizeVal / 2; ++i) {
        distances.push_back(baseDistance);
        distances.push_back(baseDistance);
        baseDistance += 10;
    }
    while (static_cast<int>(distances.size()) < sizeVal * (sizeVal - 1) / 2) {
        distances.push_back(baseDistance++);
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);
    return distances;
}

std::vector<int> Benchmark::generatePatternsInstance(int sizeVal) {
    std::vector<int> points;
    points.reserve(static_cast<size_t>(sizeVal));
    int spacing = (sizeVal <= 1) ? 1 : (sizeVal / (sizeVal - 1));
    for (int i = 0; i < sizeVal; ++i) {
        points.push_back(i * spacing);
    }
    std::vector<int> distances;
    distances.reserve(sizeVal * (sizeVal - 1) / 2);
    for (int i = 0; i < sizeVal; ++i) {
        for (int j = i + 1; j < sizeVal; ++j) {
            distances.push_back(points[j] - points[i]);
        }
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);
    return distances;
}

std::vector<int> Benchmark::generateExtremeInstance(int sizeVal) {
    std::vector<int> distances;
    distances.reserve(sizeVal * (sizeVal - 1) / 2);
    for (int i = 0; i < sizeVal / 2; ++i) {
        distances.push_back(i + 1);
        distances.push_back(1000 * (i + 1));
    }
    while (static_cast<int>(distances.size()) < sizeVal * (sizeVal - 1) / 2) {
        distances.push_back(100 + static_cast<int>(distances.size()));
    }
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);
    return distances;
}

void Benchmark::runBasicMapSolver(const std::vector<int>& distances, int totalLength) {
    MapSolver solver(distances, totalLength);
    solver.solve();
}

void Benchmark::runBBdAlgorithm(const std::vector<int>& distances) {
    BBdAlgorithm algorithm;
    algorithm.solve(distances);
}

void Benchmark::runBBbAlgorithm(const std::vector<int>& distances) {
    BBbAlgorithm algorithm;
    algorithm.solve(distances);
}

void Benchmark::runBBb2Algorithm(const std::vector<int>& distances) {
    BBb2Algorithm algorithm;
    algorithm.solve(distances);
}

bool Benchmark::validateSolution(const std::vector<int>& solution, const std::vector<int>& distances) {
    if (solution.empty()) return false;
    std::vector<int> generatedDistances;
    generatedDistances.reserve(solution.size() * (solution.size() - 1) / 2);
    for (size_t i = 0; i < solution.size(); ++i) {
        for (size_t j = i + 1; j < solution.size(); ++j) {
            generatedDistances.push_back(std::abs(solution[j] - solution[i]));
        }
    }
    std::sort(generatedDistances.begin(), generatedDistances.end());

    std::vector<int> sortedOriginal = distances;
    std::sort(sortedOriginal.begin(), sortedOriginal.end());
    return (sortedOriginal == generatedDistances);
}

void Benchmark::loadReferenceResults(const std::string& directory) {
    fs::path dirPath(directory);
    if (!fs::exists(dirPath)) return;
    for (const auto& entry : fs::directory_iterator(dirPath)) {
        if (entry.path().extension() == ".sol") {
            std::ifstream file(entry.path());
            std::vector<int> sol;
            int v;
            while (file >> v) {
                sol.push_back(v);
            }
            referenceResults[entry.path().stem().string()] = sol;
        }
    }
}

bool Benchmark::compareSolutions(const std::vector<int>& sol1, const std::vector<int>& sol2) {
    if (sol1.size() != sol2.size()) return false;
    std::vector<int> dist1, dist2;
    dist1.reserve(sol1.size() * (sol1.size() - 1) / 2);
    dist2.reserve(sol2.size() * (sol2.size() - 1) / 2);

    for (size_t i = 0; i < sol1.size(); ++i) {
        for (size_t j = i + 1; j < sol1.size(); ++j) {
            dist1.push_back(std::abs(sol1[j] - sol1[i]));
            dist2.push_back(std::abs(sol2[j] - sol2[i]));
        }
    }
    std::sort(dist1.begin(), dist1.end());
    std::sort(dist2.begin(), dist2.end());
    return (dist1 == dist2);
}

std::string Benchmark::generateInstanceHash(const std::vector<int>& distances) {
    std::stringstream ss;
    for (int d : distances) {
        ss << d << "_";
    }
    return ss.str();
}
