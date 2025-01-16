#include "../include/test_framework.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <set>
#include <chrono>

namespace fs = std::filesystem;

const std::chrono::hours TestFramework::MAX_EXECUTION_TIME(1);
const std::string TestFramework::INSTANCES_DIR = "instances";

TestFramework::TestFramework(InstanceGenerator& gen) : generator(gen) {
    if (!fs::exists(INSTANCES_DIR)) {
        fs::create_directory(INSTANCES_DIR);
    }
    generator.setOutputDirectory(INSTANCES_DIR); // Ustaw katalog wyjściowy na "instances"
}

void TestFramework::clearInstancesDirectory() {
    if (fs::exists(INSTANCES_DIR)) {
        for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
            fs::remove(entry.path());
        }
    }
}

std::string TestFramework::getFullPath(const std::string& filename) const {
    return (fs::path(INSTANCES_DIR) / filename).string();
}

void TestFramework::listAvailableInstances() {
    std::cout << "\nAvailable instances in " << INSTANCES_DIR << ":\n";
    
    std::vector<std::string> instances;
    for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            instances.push_back(entry.path().filename().string());
        }
    }
    
    // Sort instances for consistent numbering
    std::sort(instances.begin(), instances.end());
    
    // Display instances with numbers
    for (size_t i = 0; i < instances.size(); i++) {
        std::cout << "  " << (i + 1) << ". " << instances[i] << "\n";
    }
    std::cout << std::endl;
}

bool TestFramework::generateRandomInstances(int count, SortOrder order) {
    clearInstancesDirectory();
    int successCount = 0;

    for (int i = 0; i < count; i++) {
        int cuts = MIN_CUTS + (i % 5); // Varying number of cuts
        std::string filename = "random_" + std::to_string(i + 1) + ".txt";
        
        std::cout << "Generating random instance " << (i + 1) << "/" << count 
                  << " (cuts: " << cuts << ")... ";
                  
        if (generator.generateInstance(cuts, filename, order)) { // Usunięto getFullPath
            std::cout << "SUCCESS\n";
            successCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    std::cout << "\nRandom instance generation complete:\n"
              << "Successfully generated: " << successCount << "/" << count << " instances\n";
              
    return successCount == count;
}

bool TestFramework::generateInstancesRange(int maxCuts, SortOrder order) {
    if (!isValidNumberOfCuts(maxCuts)) {
        std::cout << "Invalid number of cuts. Minimum allowed is " << MIN_CUTS << std::endl;
        return false;
    }

    clearInstancesDirectory();
    int successCount = 0;
    int totalCount = maxCuts - MIN_CUTS + 1;

    for (int cuts = MIN_CUTS; cuts <= maxCuts; cuts++) {
        std::string filename = "sequential_" + std::to_string(cuts) + ".txt";
        std::cout << "Generating instance with " << cuts << " cuts... ";
        
        // Usuwamy getFullPath stąd, bo InstanceGenerator sam zadba o ścieżkę
        if (generator.generateInstance(cuts, filename, order)) {
            std::cout << "SUCCESS\n";
            successCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    std::cout << "\nSequential instance generation complete:\n"
              << "Successfully generated: " << successCount << "/" << totalCount << " instances\n";
              
    return successCount == totalCount;
}

TestFramework::VerificationResult TestFramework::verifyInstanceFile(const std::string& filepath) {
    std::vector<int> distances = generator.loadInstance(filepath);
    if (distances.empty()) {
        return {false, "Failed to load instance", std::nullopt, 0.0};
    }

    // Basic validation
    if (!verifyInputSize(distances, distances.size())) {
        return {false, "Invalid input size", std::nullopt, 0.0};
    }
    if (!verifyInputValues(distances)) {
        return {false, "Invalid distance values", std::nullopt, 0.0};
    }
    if (!checkCutsPossibility(distances.size())) {
        return {false, "Invalid multiset size for PDP", std::nullopt, 0.0};
    }

    // Try to solve using BBb algorithm
    auto start = std::chrono::high_resolution_clock::now();
    auto solution = bbbSolver.solve(distances);
    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    if (!solution) {
        return {false, "No solution found using BBb algorithm", std::nullopt, timeMs};
    }

    // Validate the solution
    if (!validateSolution(*solution, distances)) {
        return {false, "Invalid solution from BBb algorithm", solution, timeMs};
    }

    return {true, "Instance verified successfully", solution, timeMs};
}

bool TestFramework::verifyAllInstances() {
    if (!fs::exists(INSTANCES_DIR) || fs::is_empty(INSTANCES_DIR)) {
        std::cout << "No instances found in '" << INSTANCES_DIR << "' directory.\n";
        return false;
    }

    int totalFiles = 0;
    int validFiles = 0;
    std::cout << "\nStarting comprehensive instance verification...\n\n";

    for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            totalFiles++;
            auto result = verifyInstanceFile(entry.path().string());
            displayVerificationResult(entry.path().filename().string(), result);
            if (result.isValid) validFiles++;
        }
    }

    std::cout << "\nVerification Summary:\n"
              << "Total instances: " << totalFiles << "\n"
              << "Valid instances: " << validFiles << "\n"
              << "Invalid instances: " << (totalFiles - validFiles) << "\n";

    return validFiles == totalFiles;
}

void TestFramework::displayVerificationResult(const std::string& filename, 
                                            const VerificationResult& result) {
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

bool TestFramework::validateSolution(const std::vector<int>& solution, 
                                   const std::vector<int>& distances) {
    if (solution.empty()) return false;

    // Generate distances from solution
    auto solutionDistances = generateDistancesFromSolution(solution);
    
    // Compare multisets
    return areMultisetsEqual(solutionDistances, distances);
}

std::vector<int> TestFramework::generateDistancesFromSolution(const std::vector<int>& solution) {
    std::vector<int> distances;
    for (size_t i = 0; i < solution.size(); i++) {
        for (size_t j = i + 1; j < solution.size(); j++) {
            distances.push_back(std::abs(solution[j] - solution[i]));
        }
    }
    std::sort(distances.begin(), distances.end());
    return distances;
}

bool TestFramework::areMultisetsEqual(std::vector<int> a, std::vector<int> b) {
    std::sort(a.begin(), a.end());
    std::sort(b.begin(), b.end());
    return a == b;
}

void TestFramework::runInteractiveMode() {
    while (true) {
        std::cout << "\nTest Framework Menu:\n";
        std::cout << "1. Generate random instances\n";
        std::cout << "2. Generate instances range\n";
        std::cout << "3. Verify all instances\n";
        std::cout << "4. Solve specific instance\n";
        std::cout << "5. Run benchmark\n";
        std::cout << "6. Run debug solver on instance\n";  // Nowa opcja
        std::cout << "0. Exit\n";
        std::cout << "Choose option: ";

        int choice;
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
                std::cout << "Enter instance filename: ";
                std::string filename;
                std::cin >> filename;
                solveSpecificInstance(filename);
                break;
            }
            
            case 5:
                benchmark.runBenchmark();
                break;
            
            case 6: {  // Nowa opcja debug solvera
                listAvailableInstances();
                std::cout << "Enter instance filename: ";
                std::string filename;
                std::cin >> filename;
                runDebugSolver(filename);
                break;
            }
            
            case 0:
                return;
                
            default:
                std::cout << "Invalid option!\n";
        }
    }
}

void TestFramework::testAllInstances(int algorithmChoice) {
    if (!fs::exists(INSTANCES_DIR) || fs::is_empty(INSTANCES_DIR)) {
        std::cout << "No instances found in '" << INSTANCES_DIR << "' directory.\n";
        return;
    }

    int totalFiles = 0;
    int validFiles = 0;
    std::cout << "\nStarting comprehensive instance testing...\n\n";

    for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            totalFiles++;
            std::string filename = entry.path().filename().string();
            std::vector<int> distances = generator.loadInstance(filename); // Remove INSTANCES_DIR prefix

            if (distances.empty()) {
                std::cout << "Failed to load instance: " << filename << "\n";
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
            double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

            if (solution) {
                validFiles++;
                std::cout << "Instance " << filename << ": SOLVED (" << timeMs << "ms)\n";
                std::cout << "Solution: ";
                for (int x : *solution) {
                    std::cout << x << " ";
                }
                std::cout << "\n";
            } else {
                std::cout << "Instance " << filename << ": NO SOLUTION FOUND (" << timeMs << "ms)\n";
            }
        }
    }

    std::cout << "\nTesting Summary:\n"
              << "Total instances: " << totalFiles << "\n"
              << "Solved instances: " << validFiles << "\n"
              << "Failed instances: " << (totalFiles - validFiles) << "\n";
}

SortOrder TestFramework::getSortOrderFromUser() {
    std::cout << "\nSelect sorting order:\n"
              << "1. Shuffled (random order)\n"
              << "2. Ascending order\n"
              << "3. Descending order\n"
              << "Choice: ";
              
    int choice;
    std::cin >> choice;
    
    switch(choice) {
        case 2: return SortOrder::ASCENDING;
        case 3: return SortOrder::DESCENDING;
        default: return SortOrder::SHUFFLED;
    }
}

bool TestFramework::isValidNumberOfCuts(int cuts) const {
    return cuts >= MIN_CUTS;
}

bool TestFramework::verifyInputSize(const std::vector<int>& distances, int expectedSize) {
    return static_cast<int>(distances.size()) == expectedSize;
}

bool TestFramework::verifyInputValues(const std::vector<int>& distances) {
    return std::all_of(distances.begin(), distances.end(), 
        [](int d) { return d > 0 && d <= 512; });
}

int TestFramework::calculateRequiredCuts(int multisetSize) {
    double discriminant = 1.0 + 8.0 * multisetSize;
    double n = (-1.0 + std::sqrt(discriminant)) / 2.0;
    return static_cast<int>(std::round(n - 1.0));
}

bool TestFramework::checkCutsPossibility(int multisetSize) {
    int requiredCuts = calculateRequiredCuts(multisetSize);
    int expectedSize = (requiredCuts + 2) * (requiredCuts + 1) / 2;
    return expectedSize == multisetSize;
}

bool TestFramework::solveSpecificInstance(const std::string& input) {
    std::string filename;
    
    // Try to parse input as a number
    try {
        int instanceNum = std::stoi(input);
        // Get list of txt files
        std::vector<std::string> instances;
        for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
            if (entry.path().extension() == ".txt") {
                instances.push_back(entry.path().filename().string());
            }
        }
        
        // Sort instances for consistent numbering
        std::sort(instances.begin(), instances.end());
        
        if (instanceNum <= 0 || instanceNum > static_cast<int>(instances.size())) {
            std::cout << "Invalid instance number. Please choose between 1 and " 
                     << instances.size() << "\n";
            return false;
        }
        
        filename = instances[instanceNum - 1];
    }
    catch (const std::invalid_argument&) {
        // Input is not a number, treat it as filename
        filename = input;
    }

    std::string fullPath = (fs::path(INSTANCES_DIR) / filename).string();
    
    if (!fs::exists(fullPath)) {
        std::cout << "Instance file not found: " << fullPath << "\n";
        return false;
    }

    // Wybór algorytmu
    std::cout << "\nChoose algorithm:\n";
    std::cout << "1. BBb Algorithm\n";
    std::cout << "2. BBb2 Algorithm\n";
    std::cout << "3. BBd Algorithm\n";
    std::cout << "4. Basic Map Solver\n";
    std::cout << "5. Debug Basic Map Solver\n";
    std::cout << "Enter choice (1-5): ";
    
    int algorithmChoice;
    std::cin >> algorithmChoice;

    std::vector<int> distances = generator.loadInstance(filename);
    if (distances.empty()) {
        std::cout << "Failed to load instance\n";
        return false;
    }

    std::optional<std::vector<int>> solution;
    auto start = std::chrono::high_resolution_clock::now();

    // Find total length for Map Solvers
    int totalLength = *std::max_element(distances.begin(), distances.end());

    // Create log filename for debug solver
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
            
            // Wyświetl statystyki debug solvera
            const auto& stats = debugSolver.getStatistics();
            std::cout << "\nDebug Statistics:\n";
            std::cout << "Total paths explored: " << stats.totalPaths << "\n";
            std::cout << "Processed paths: " << stats.processedPaths << "\n";
            std::cout << "Search time: " << stats.searchTimeMs << "ms\n";
            std::cout << "Debug log saved to: " << logFilename << "\n";
            break;
        }
        default:
            std::cout << "Invalid algorithm choice\n";
            return false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::string algorithmName;
    switch (algorithmChoice) {
        case 1: algorithmName = "BBb Algorithm4"; break;
        case 2: algorithmName = "BBb2 Algorithm"; break;
        case 3: algorithmName = "BBd Algorithm"; break;
        case 4: algorithmName = "Basic Map Solver"; break;
        case 5: algorithmName = "Debug Basic Map Solver"; break;
        default: algorithmName = "Unknown Algorithm";
    }

    std::cout << "\nResults for " << algorithmName << ":\n";
    
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

bool TestFramework::runDebugSolver(const std::string& filename) {
    std::vector<int> distances = generator.loadInstance(filename);
    if (distances.empty()) {
        std::cout << "Failed to load instance from file: " << filename << std::endl;
        return false;
    }

    int totalLength = *std::max_element(distances.begin(), distances.end());
    
    // Inicjalizacja debug solvera
    DebugMapSolver solver(distances, totalLength, true, "debug_" + filename + ".log");
    
    std::cout << "Running debug solver for instance " << filename << std::endl;
    std::cout << "Total length: " << totalLength << std::endl;
    std::cout << "Number of distances: " << distances.size() << std::endl;
    
    auto startTime = std::chrono::steady_clock::now();
    auto solution = solver.solve();
    auto endTime = std::chrono::steady_clock::now();
    
    auto timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    
    // Wyświetl wyniki i statystyki
    displayDebugStatistics(solver.getStatistics());
    
    if (solution) {
        std::cout << "\nSolution found!" << std::endl;
        std::cout << "Execution time: " << timeMs << "ms" << std::endl;
        std::cout << "Solution: ";
        for (const auto& pos : *solution) {
            std::cout << pos << " ";
        }
        std::cout << std::endl;
        
        if (validateSolution(*solution, distances)) {
            std::cout << "Solution validation: PASSED" << std::endl;
        } else {
            std::cout << "Solution validation: FAILED" << std::endl;
        }
    } else {
        std::cout << "\nNo solution found!" << std::endl;
        std::cout << "Execution time: " << timeMs << "ms" << std::endl;
    }
    
    return true;
}

void TestFramework::displayDebugStatistics(const DebugMapSolver::Statistics& stats) {
    std::cout << "\nDebug Statistics:" << std::endl;
    std::cout << "Total paths explored: " << stats.totalPaths << std::endl;
    std::cout << "Processed paths: " << stats.processedPaths << std::endl;
    std::cout << "Search time: " << stats.searchTimeMs << "ms" << std::endl;
    std::cout << "Solution found: " << (stats.solutionFound ? "Yes" : "No") << std::endl;
    
    if (!stats.inputDistances.empty()) {
        std::cout << "\nInput distances: ";
        for (const auto& dist : stats.inputDistances) {
            std::cout << dist << " ";
        }
        std::cout << std::endl;
    }
}