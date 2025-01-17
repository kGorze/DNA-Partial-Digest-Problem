#include "../include/test_framework.h"
#include "../include/global_paths.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <set>
#include <chrono>

namespace fs = std::filesystem;

const std::chrono::hours TestFramework::MAX_EXECUTION_TIME(1);

TestFramework::TestFramework(InstanceGenerator& gen)
    : generator(gen) 
{
    fs::create_directories(GlobalPaths::INSTANCES_DIR);
    generator.setOutputDirectory(GlobalPaths::INSTANCES_DIR.string());
}

bool TestFramework::generateInstance(int cuts, const std::string& filename, SortOrder order) {
    return generator.generateInstance(cuts, filename, order);
}

std::filesystem::path TestFramework::getFullPath(const std::string& filename) const {
    return GlobalPaths::INSTANCES_DIR / filename;
}

void TestFramework::listAvailableInstances() {
    std::cout << "\nAvailable instances in " << GlobalPaths::INSTANCES_DIR.string() << ":\n";
    std::vector<std::string> instances;
    if (fs::exists(GlobalPaths::INSTANCES_DIR)) {
        for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
            if (entry.path().extension() == ".txt") {
                instances.push_back(entry.path().filename().string());
            }
        }
        std::sort(instances.begin(), instances.end());
    }
    for (size_t i = 0; i < instances.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << instances[i] << "\n";
    }
    std::cout << std::endl;
}

bool TestFramework::generateRandomInstances(int count, SortOrder order) {
    if (fs::exists(GlobalPaths::INSTANCES_DIR) && fs::is_directory(GlobalPaths::INSTANCES_DIR)) {
        for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
            fs::remove(entry.path());
        }
    }
    int successCount = 0;
    for (int i = 0; i < count; i++) {
        int cuts = MIN_CUTS + (i % 5);
        std::string filename = "random_" + std::to_string(i + 1) + ".txt";
        std::cout << "Generating random instance " << (i + 1) << "/" << count
                  << " (cuts: " << cuts << ")... ";
        if (generateInstance(cuts, filename, order)) {
            std::cout << "SUCCESS\n";
            successCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }
    std::cout << "\nRandom instance generation complete.\n"
              << "Successfully generated: " << successCount << "/" << count << " instances\n";
    return (successCount == count);
}

bool TestFramework::generateInstancesRange(int maxCuts, SortOrder order) {
    if (!isValidNumberOfCuts(maxCuts)) {
        std::cout << "Invalid number of cuts. Minimum allowed is " << MIN_CUTS << std::endl;
        return false;
    }
    if (fs::exists(GlobalPaths::INSTANCES_DIR) && fs::is_directory(GlobalPaths::INSTANCES_DIR)) {
        for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
            fs::remove(entry.path());
        }
    }
    int successCount = 0;
    int totalCount = maxCuts - MIN_CUTS + 1;
    for (int cuts = MIN_CUTS; cuts <= maxCuts; cuts++) {
        std::string filename = "sequential_" + std::to_string(cuts) + ".txt";
        std::cout << "Generating instance with " << cuts << " cuts... ";
        if (generateInstance(cuts, filename, order)) {
            std::cout << "SUCCESS\n";
            successCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }
    std::cout << "\nSequential instance generation complete.\n"
              << "Successfully generated: " << successCount << "/" << totalCount << " instances\n";
    return (successCount == totalCount);
}

bool TestFramework::isValidNumberOfCuts(int cuts) const {
    return (cuts >= MIN_CUTS);
}

bool TestFramework::verifyAllInstances() {
    if (!fs::exists(GlobalPaths::INSTANCES_DIR) || fs::is_empty(GlobalPaths::INSTANCES_DIR)) {
        std::cout << "No instances found in '"
                  << GlobalPaths::INSTANCES_DIR.string() << "' directory.\n";
        return false;
    }
    int totalFiles = 0;
    int validFiles = 0;
    std::cout << "\nStarting instance verification...\n\n";

    for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            totalFiles++;
            auto result = verifyInstanceFile(entry.path().string());
            displayVerificationResult(entry.path().filename().string(), result);
            if (result.isValid) {
                validFiles++;
            }
        }
    }

    std::cout << "\nVerification Summary:\n"
              << "Total instances: " << totalFiles << "\n"
              << "Valid instances: " << validFiles << "\n"
              << "Invalid instances: " << (totalFiles - validFiles) << "\n";
    return (validFiles == totalFiles);
}

TestFramework::VerificationResult TestFramework::verifyInstanceFile(const std::string& filepath) {
    std::vector<int> distances = generator.loadInstance(filepath);
    if (distances.empty()) {
        return {false, "Failed to load instance", std::nullopt, 0.0};
    }

    if (!verifyInputSize(distances, static_cast<int>(distances.size()))) {
        return {false, "Invalid input size", std::nullopt, 0.0};
    }
    if (!verifyInputValues(distances)) {
        return {false, "Invalid distance values", std::nullopt, 0.0};
    }
    if (!checkCutsPossibility(static_cast<int>(distances.size()))) {
        return {false, "Invalid multiset size for PDE", std::nullopt, 0.0};
    }

    auto start = std::chrono::high_resolution_clock::now();
    auto solution = bbbSolver.solve(distances);
    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );

    if (!solution) {
        return {false, "No solution found using BBb algorithm", std::nullopt, timeMs};
    }
    if (!validateSolution(*solution, distances)) {
        return {false, "Invalid solution from BBb algorithm", solution, timeMs};
    }
    return {true, "Instance verified successfully", solution, timeMs};
}

bool TestFramework::verifyInputSize(const std::vector<int>& distances, int expectedSize) {
    return (static_cast<int>(distances.size()) == expectedSize);
}

bool TestFramework::verifyInputValues(const std::vector<int>& distances) {
    return std::all_of(distances.begin(), distances.end(),
        [](int d){ return d > 0 && d <= 65535; }
    );
}

bool TestFramework::checkCutsPossibility(int multisetSize) {
    int requiredCuts = calculateRequiredCuts(multisetSize);
    int expectedSize = (requiredCuts + 2) * (requiredCuts + 1) / 2;
    return (expectedSize == multisetSize);
}

int TestFramework::calculateRequiredCuts(int multisetSize) {
    double disc = 1.0 + 8.0 * static_cast<double>(multisetSize);
    double val = (-1.0 + std::sqrt(disc)) / 2.0;
    return static_cast<int>(std::round(val - 1.0));
}

bool TestFramework::validateSolution(const std::vector<int>& solution, const std::vector<int>& distances) {
    if (solution.empty()) return false;
    auto solutionDistances = generateDistancesFromSolution(solution);
    return areMultisetsEqual(solutionDistances, distances);
}

std::vector<int> TestFramework::generateDistancesFromSolution(const std::vector<int>& solution) {
    std::vector<int> dist;
    dist.reserve(solution.size() * (solution.size() - 1) / 2);
    for (size_t i = 0; i < solution.size(); i++) {
        for (size_t j = i + 1; j < solution.size(); j++) {
            dist.push_back(std::abs(solution[j] - solution[i]));
        }
    }
    std::sort(dist.begin(), dist.end());
    return dist;
}

bool TestFramework::areMultisetsEqual(std::vector<int> a, std::vector<int> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return (a == b);
}

void TestFramework::displayVerificationResult(const std::string& filename, const VerificationResult& result) {
    std::cout << "Verifying " << filename << "... ";
    if (result.isValid) {
        std::cout << "VALID (solved in " << result.verificationTimeMs << "ms)\n";
        if (result.solution) {
            std::cout << "  Solution found: ";
            for (int x : *result.solution) {
                std::cout << x << " ";
            }
            std::cout << "\n";
        }
    } else {
        std::cout << "INVALID: " << result.message << "\n";
    }
}

bool TestFramework::solveSpecificInstance(const std::string& input) {
    std::string filename;
    try {
        int instanceNum = std::stoi(input);
        std::vector<std::string> instances;
        for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
            if (entry.path().extension() == ".txt") {
                instances.push_back(entry.path().filename().string());
            }
        }
        std::sort(instances.begin(), instances.end());
        if (instanceNum <= 0 || instanceNum > static_cast<int>(instances.size())) {
            std::cout << "Invalid instance number. Choose 1.." << instances.size() << "\n";
            return false;
        }
        filename = instances[static_cast<size_t>(instanceNum) - 1];
    } catch (const std::invalid_argument&) {
        filename = input;
    }

    fs::path fullPath = GlobalPaths::INSTANCES_DIR / filename;
    if (!fs::exists(fullPath)) {
        std::cout << "Instance file not found: " << fullPath.string() << "\n";
        return false;
    }

    std::cout << "\nChoose algorithm:\n";
    std::cout << "1. BBb Algorithm\n";
    std::cout << "2. BBb2 Algorithm\n";
    std::cout << "3. BBd Algorithm\n";
    std::cout << "4. Basic Map Solver\n";
    std::cout << "5. Debug Basic Map Solver\n";
    std::cout << "Enter choice (1-5): ";
    int algorithmChoice = 0;
    std::cin >> algorithmChoice;

    std::vector<int> distances = generator.loadInstance(filename);
    if (distances.empty()) {
        std::cout << "Failed to load instance\n";
        return false;
    }

    std::optional<std::vector<int>> solution;
    auto start = std::chrono::high_resolution_clock::now();
    int totalLength = *std::max_element(distances.begin(), distances.end());
    std::string logFilename = "debug_" + filename + ".log";

    switch (algorithmChoice) {
        case 1:
            solution = bbbSolver.solve(distances);
            break;
        case 2: {
            BBb2Algorithm bbb2Solver;
            solution = bbb2Solver.solve(distances);
            break;
        }
        case 3: {
            BBdAlgorithm bbdSolver;
            solution = bbdSolver.solve(distances);
            break;
        }
        case 4: {
            MapSolver solver(distances, totalLength);
            solution = solver.solve();
            break;
        }
        case 5: {
            DebugMapSolver debugSolver(distances, totalLength, true, logFilename);
            solution = debugSolver.solve();
            const auto& stats = debugSolver.getStatistics();
            displayDebugStatistics(stats);
            std::cout << "Debug log saved to: " << logFilename << "\n";
            break;
        }
        default:
            std::cout << "Invalid algorithm choice\n";
            return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
    );

    std::cout << "\nResults for algorithm choice " << algorithmChoice << ":\n";
    if (solution) {
        std::cout << "Solution found in " << timeMs << "ms!\n";
        std::cout << "Solution: ";
        for (int x : *solution) {
            std::cout << x << " ";
        }
        std::cout << "\n";
        if (validateSolution(*solution, distances)) {
            std::cout << "Solution validation: PASSED\n";
        } else {
            std::cout << "Solution validation: FAILED\n";
        }
    } else {
        std::cout << "No solution found (time: " << timeMs << "ms)\n";
    }
    return solution.has_value();
}

void TestFramework::testAllInstances(int algorithmChoice) {
    if (!fs::exists(GlobalPaths::INSTANCES_DIR) || fs::is_empty(GlobalPaths::INSTANCES_DIR)) {
        std::cout << "No instances found in '" << GlobalPaths::INSTANCES_DIR.string() << "' directory.\n";
        return;
    }
    int totalFiles = 0;
    int validFiles = 0;
    std::cout << "\nStarting instance testing...\n\n";

    for (const auto& entry : fs::directory_iterator(GlobalPaths::INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            totalFiles++;
            std::string fname = entry.path().filename().string();
            std::vector<int> distances = generator.loadInstance(fname);
            if (distances.empty()) {
                std::cout << "Failed to load instance: " << fname << "\n";
                continue;
            }
            std::optional<std::vector<int>> solution;
            auto start = std::chrono::high_resolution_clock::now();

            switch (algorithmChoice) {
                case 1:
                    solution = bbbSolver.solve(distances);
                    break;
                case 2: {
                    BBb2Algorithm bbb2Solver;
                    solution = bbb2Solver.solve(distances);
                    break;
                }
                case 3: {
                    BBdAlgorithm bbdSolver;
                    solution = bbdSolver.solve(distances);
                    break;
                }
                default:
                    std::cout << "Invalid algorithm choice\n";
                    return;
            }

            auto end = std::chrono::high_resolution_clock::now();
            double timeMs = static_cast<double>(
                std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
            );
            if (solution) {
                validFiles++;
                std::cout << "Instance " << fname << ": SOLVED (" << timeMs << "ms)\n";
            } else {
                std::cout << "Instance " << fname << ": NO SOLUTION FOUND (" << timeMs << "ms)\n";
            }
        }
    }

    std::cout << "\nTesting Summary:\n"
              << "Total instances: " << totalFiles << "\n"
              << "Solved instances: " << validFiles << "\n"
              << "Failed instances: " << (totalFiles - validFiles) << "\n";
}

void TestFramework::runInteractiveMode() {
    while (true) {
        std::cout << "\nTest Framework Menu:\n";
        std::cout << "1. Generate random instances\n";
        std::cout << "2. Generate instances range\n";
        std::cout << "3. Verify all instances\n";
        std::cout << "4. Solve specific instance\n";
        std::cout << "5. Run benchmark\n";
        std::cout << "6. Run debug solver on instance\n";
        std::cout << "7. Run data arrangement analysis\n";
        std::cout << "0. Exit\n";
        std::cout << "Choose option: ";

        int choice = 0;
        std::cin >> choice;
        switch (choice) {
            case 1: {
                int count;
                std::cout << "Enter number of instances: ";
                std::cin >> count;
                generateRandomInstances(count, getSortOrderFromUser());
                break;
            }
            case 2: {
                int maxCuts;
                std::cout << "Enter maximum number of cuts: ";
                std::cin >> maxCuts;
                generateInstancesRange(maxCuts, getSortOrderFromUser());
                break;
            }
            case 3:
                verifyAllInstances();
                break;
            case 4: {
                listAvailableInstances();
                std::cout << "Enter instance filename or number: ";
                std::string input;
                std::cin >> input;
                solveSpecificInstance(input);
                break;
            }
            case 5:
                benchmark.runBenchmark();
                break;
            case 6: {
                listAvailableInstances();
                std::cout << "Enter instance filename: ";
                std::string filename;
                std::cin >> filename;
                runDebugSolver(filename);
                break;
            }
            case 7: {
                        listAvailableInstances();
                        std::cout << "Enter instance filename: ";
                        std::string filename;
                        std::cin >> filename;
                        std::cout << "Enter number of repetitions per arrangement: ";
                        int reps;
                        std::cin >> reps;
                        runDataArrangementAnalysis(filename, reps);
                        break;
            }
            case 0:
                return;
            default:
                std::cout << "Invalid option!\n";
        }
    }
}

bool TestFramework::runDebugSolver(const std::string& filename) {
    std::vector<int> distances = generator.loadInstance(filename);
    if (distances.empty()) {
        std::cout << "Failed to load instance from file: " << filename << std::endl;
        return false;
    }
    int totalLength = *std::max_element(distances.begin(), distances.end());

    DebugMapSolver solver(distances, totalLength, true, "debug_" + filename + ".log");
    std::cout << "Running debug solver for instance " << filename << std::endl;
    std::cout << "Total length: " << totalLength << std::endl;
    std::cout << "Number of distances: " << distances.size() << std::endl;

    auto startTime = std::chrono::steady_clock::now();
    auto solution = solver.solve();
    auto endTime = std::chrono::steady_clock::now();
    double timeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
    );

    displayDebugStatistics(solver.getStatistics());
    if (solution) {
        std::cout << "\nSolution found!\n";
        std::cout << "Execution time: " << timeMs << "ms\n";
        std::cout << "Solution: ";
        for (int pos : *solution) {
            std::cout << pos << " ";
        }
        std::cout << "\n";
        if (validateSolution(*solution, distances)) {
            std::cout << "Solution validation: PASSED\n";
        } else {
            std::cout << "Solution validation: FAILED\n";
        }
    } else {
        std::cout << "\nNo solution found!\n";
        std::cout << "Execution time: " << timeMs << "ms\n";
    }
    return true;
}

void TestFramework::displayDebugStatistics(const DebugMapSolver::Statistics& stats) {
    std::cout << "\nDebug Statistics:\n";
    std::cout << "Total paths explored: " << stats.totalPaths << "\n";
    std::cout << "Processed paths: " << stats.processedPaths << "\n";
    std::cout << "Search time: " << stats.searchTimeMs << "ms\n";
    std::cout << "Solution found: " << (stats.solutionFound ? "Yes" : "No") << "\n";
    if (!stats.inputDistances.empty()) {
        std::cout << "\nInput distances: ";
        for (int d : stats.inputDistances) {
            std::cout << d << " ";
        }
        std::cout << "\n";
    }
}

SortOrder TestFramework::getSortOrderFromUser() {
    std::cout << "\nSelect sorting order:\n"
              << "1. Shuffled (random)\n"
              << "2. Ascending\n"
              << "3. Descending\n"
              << "Choice: ";
    int choice = 1;
    std::cin >> choice;
    switch(choice) {
        case 2: return SortOrder::ASCENDING;
        case 3: return SortOrder::DESCENDING;
        default: return SortOrder::SHUFFLED;
    }
}
