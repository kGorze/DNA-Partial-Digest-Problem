//
// Created by konrad_guest on 14/01/2025.
//
#include "../include/benchmark.h"
#include <iostream>
#include <algorithm>
#include <numeric>
#include <limits>

const std::string Benchmark::BENCHMARK_DIR = "benchmark";
const std::string Benchmark::TEMP_FILE = "temp_instance.txt";

Benchmark::Benchmark() {
    // Default configuration
    config.standardSizes = {10, 20, 50, 100, 200, 300, 400};
    config.specialCaseSizes = {100};
    config.repeatCount = 5;
    config.specialCaseRepetitions = 100;
    config.mode = BenchmarkMode::ALL_ALGORITHMS;
    createBenchmarkDirectory();
}

Benchmark::~Benchmark() {
    cleanupTempFiles();
}

void Benchmark::setBenchmarkConfig(const BenchmarkConfig& newConfig) {
    config = newConfig;
}

void Benchmark::createBenchmarkDirectory() {
    namespace fs = std::filesystem;
    if (!fs::exists(BENCHMARK_DIR)) {
        fs::create_directory(BENCHMARK_DIR);
    }
}

void Benchmark::cleanupTempFiles() {
    namespace fs = std::filesystem;
    if (fs::exists(TEMP_FILE)) {
        fs::remove(TEMP_FILE);
    }
    // Remove all temporary files in the benchmark directory
    for (const auto& entry : fs::directory_iterator(BENCHMARK_DIR)) {
        if (entry.path().filename().string().starts_with("benchmark_instance_")) {
            fs::remove(entry.path());
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

    int choice;
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
            int size;
            while (!(std::cin >> size) || size <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid value. Enter a positive integer: ";
            }
            config.standardSizes.push_back(size);
        }
        runBenchmark();
        return;
    }
    else if (choice == 2) {
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
            int size;
            while (!(std::cin >> size) || size <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid value. Enter a positive integer: ";
            }
            config.specialCaseSizes.push_back(size);
        }
        runBenchmark();
        return;
    }
    else if (choice == 3) {
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
    }
    else if (choice == 4) {
        std::cout << "\nSelect algorithm to test:\n"
                  << "1. Basic Map Solver (Original)\n"
                  << "2. BBd (Branch and Bound)\n"
                  << "3. BBb (Improved Branch and Bound)\n"
                  << "4. BBb2 (Best Variant)\n"
                  << "5. All algorithms\n";

        int algoChoice;
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
    }
    else if (choice == 5) {
        config.mode = BenchmarkMode::FAST_ALGORITHMS_ONLY;
        runFastAlgorithmsBenchmark();
    }
    else if (choice == 6) {
        runComprehensiveBenchmark();
    }

    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::string filename = "benchmark_results_" + std::to_string(timestamp) + ".csv";
    saveResults(filename);

    cleanupTempFiles();
}

void Benchmark::runFastAlgorithmsBenchmark() {
    std::cout << "Running benchmark for fast algorithms (BBd, BBb, BBb2)...\n";
    
    auto algorithms = getAlgorithmsForMode(BenchmarkMode::FAST_ALGORITHMS_ONLY);
    
    for (int size : config.standardSizes) {
        std::cout << "\nTesting size n=" << size << "\n";
        
        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(size, TestType::STANDARD);
            std::vector<int> distances = instanceGenerator.loadInstance(TEMP_FILE);

            for (Algorithm algo : algorithms) {
                auto benchResult = runAlgorithmWithValidation(algo, distances);

                BenchmarkResult result = {
                    size,
                    benchResult.executionTimeMs,
                    algo,
                    TestType::STANDARD,
                    "Test type: Standard, size: " + std::to_string(size),
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
    std::vector<Algorithm> algorithms;
    if (mode == BenchmarkMode::ALL_ALGORITHMS) {
        algorithms = {Algorithm::BASIC_MAP, Algorithm::BBD, Algorithm::BBB, Algorithm::BBB2};
    } else {
        algorithms = {Algorithm::BBD, Algorithm::BBB, Algorithm::BBB2};
    }
    return algorithms;
}

void Benchmark::runSingleAlgorithmBenchmark(Algorithm algo) {
    std::cout << "\nRunning benchmark for algorithm: " << getAlgorithmName(algo) << "\n";

    for (int size : config.standardSizes) {
        std::cout << "\nTesting size n=" << size << "\n";
        
        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(size, TestType::STANDARD);
            std::vector<int> distances = instanceGenerator.loadInstance(TEMP_FILE);

            auto benchResult = runAlgorithmWithValidation(algo, distances);

            BenchmarkResult result = {
                size,
                benchResult.executionTimeMs,
                algo,
                TestType::STANDARD,
                "Test type: Standard, size: " + std::to_string(size),
                benchResult.found
            };
            results.push_back(result);

            validatedResults.emplace_back(
                result,
                validateSolution(benchResult.solution, distances),
                benchResult.solution
            );

            std::string instanceHash = generateInstanceHash(distances);
            auto refIt = referenceResults.find(instanceHash);
            if (refIt != referenceResults.end() && benchResult.found) {
                bool matchesReference = compareSolutions(benchResult.solution, refIt->second);
                std::cout << "Solution " << (matchesReference ? "matches" : "differs from")
                          << " reference solution\n";
            }
        }
    }
}

void Benchmark::saveResults(const std::string& filename) {
    std::string fullPath = BENCHMARK_DIR + "/" + filename;
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
    
    std::cout << "\nResults saved to file: " << fullPath << "\n";
}

void Benchmark::runTestTypeBenchmark(TestType type, const std::vector<int>& sizes) {
    std::cout << "\nRunning benchmark for test type: " << getTestTypeName(type) << "\n";

    for (int size : sizes) {
        std::cout << "\nTesting size n=" << size << "\n";

        for (int i = 0; i < config.repeatCount; ++i) {
            prepareInstance(size, type);

            for (int algoType = 0; algoType < 4; ++algoType) {
                Algorithm algo = static_cast<Algorithm>(algoType);
                
                double time = measureAlgorithmTime(algo, size);

                results.push_back({
                    size,
                    time,
                    algo,
                    type,
                    "Test type: " + getTestTypeName(type) + ", size: " + std::to_string(size),
                    true
                });
            }
        }
    }
}

double Benchmark::measureAlgorithmTime(Algorithm algo, int size) {
    std::vector<int> distances = instanceGenerator.loadInstance(TEMP_FILE);
    int totalLength = size * 2; // This is just an example assumption

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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return duration.count();
}

void Benchmark::runComprehensiveBenchmark() {
    std::cout << "Starting comprehensive benchmark...\n";

    runTestTypeBenchmark(TestType::STANDARD, config.standardSizes);

    int originalRepeatCount = config.repeatCount;
    config.repeatCount = config.specialCaseRepetitions;

    for (int size : config.specialCaseSizes) {
        runTestTypeBenchmark(TestType::DUPLICATES, {size});
        runTestTypeBenchmark(TestType::PATTERNS, {size});
        runTestTypeBenchmark(TestType::EXTREME, {size});
    }

    config.repeatCount = originalRepeatCount;

    std::cout << "\nBenchmark completed.\n";
}

Benchmark::BenchmarkSolution Benchmark::runAlgorithmWithValidation(Algorithm algo, const std::vector<int>& distances) {
    auto start = std::chrono::high_resolution_clock::now();
    std::optional<std::vector<int>> solution;
    
    try {
        switch (algo) {
        case Algorithm::BASIC_MAP: {
                MapSolver solver(distances, distances.size() * 2);
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
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    return BenchmarkSolution(
        solution.value_or(std::vector<int>()),
        solution.has_value(),
        duration.count()
    );
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

void Benchmark::prepareInstance(int size, TestType type) {
    switch (type) {
        case TestType::STANDARD:
            instanceGenerator.generateInstance(size, TEMP_FILE, SortOrder::SHUFFLED);
            break;
        case TestType::DUPLICATES: {
            auto distances = generateDuplicatesInstance(size);
            std::ofstream file(TEMP_FILE);
            for (int d : distances) {
                file << d << " ";
            }
            file.close();
            break;
        }
        case TestType::PATTERNS: {
            auto distances = generatePatternsInstance(size);
            std::ofstream file(TEMP_FILE);
            for (int d : distances) {
                file << d << " ";
            }
            file.close();
            break;
        }
        case TestType::EXTREME: {
            auto distances = generateExtremeInstance(size);
            std::ofstream file(TEMP_FILE);
            for (int d : distances) {
                file << d << " ";
            }
            file.close();
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

std::vector<int> Benchmark::generateDuplicatesInstance(int size) {
    std::vector<int> distances;
    distances.reserve(size * (size - 1) / 2);

    int baseDistance = 10;
    for (int i = 0; i < size / 2; ++i) {
        distances.push_back(baseDistance);
        distances.push_back(baseDistance);
        baseDistance += 10;
    }

    while ((int)distances.size() < size * (size - 1) / 2) {
        distances.push_back(baseDistance++);
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);

    return distances;
}

std::vector<int> Benchmark::generatePatternsInstance(int size) {
    std::vector<int> points;
    points.reserve(size);

    int spacing = (size == 1) ? 1 : size / (size - 1);
    for (int i = 0; i < size; ++i) {
        points.push_back(i * spacing);
    }

    std::vector<int> distances;
    distances.reserve(size * (size - 1) / 2);

    for (int i = 0; i < size; ++i) {
        for (int j = i + 1; j < size; ++j) {
            distances.push_back(points[j] - points[i]);
        }
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(distances.begin(), distances.end(), gen);

    return distances;
}

std::vector<int> Benchmark::generateExtremeInstance(int size) {
    std::vector<int> distances;
    distances.reserve(size * (size - 1) / 2);

    for (int i = 0; i < size / 2; ++i) {
        distances.push_back(i + 1);
        distances.push_back(1000 * (i + 1));
    }

    while ((int)distances.size() < size * (size - 1) / 2) {
        distances.push_back(100 + (int)distances.size());
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
    for (size_t i = 0; i < solution.size(); ++i) {
        for (size_t j = i + 1; j < solution.size(); ++j) {
            generatedDistances.push_back(std::abs(solution[j] - solution[i]));
        }
    }
    
    std::vector<int> sortedOriginal = distances;
    std::vector<int> sortedGenerated = generatedDistances;
    std::sort(sortedOriginal.begin(), sortedOriginal.end());
    std::sort(sortedGenerated.begin(), sortedGenerated.end());
    
    return (sortedOriginal == sortedGenerated);
}

void Benchmark::loadReferenceResults(const std::string& directory) {
    namespace fs = std::filesystem;
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".sol") {
            std::ifstream file(entry.path());
            std::vector<int> solution;
            int value;
            while (file >> value) {
                solution.push_back(value);
            }
            referenceResults[entry.path().stem().string()] = solution;
        }
    }
}

bool Benchmark::compareSolutions(const std::vector<int>& sol1, const std::vector<int>& sol2) {
    if (sol1.size() != sol2.size()) return false;
    
    std::vector<int> dist1, dist2;
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
