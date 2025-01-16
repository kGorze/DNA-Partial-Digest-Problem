//
// Created by konrad_guest on 14/01/2025.
//
#include "../include/test_framework.h"
#include "../include/map_solver.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <iostream>
#include <limits>

namespace fs = std::filesystem;

const std::chrono::hours TestFramework::MAX_EXECUTION_TIME(1);
const std::string TestFramework::INSTANCES_DIR = "instances";
const std::string TestFramework::DATA_DIR = "data";
const int TestFramework::RANDOM_INSTANCES_COUNT = 10;

static void displaySolution(const std::vector<int>& solution) {
    if(solution.empty()) {
        std::cout << "No solution found.\n";
        return;
    }
    std::vector<int> sol = solution;
    std::sort(sol.begin(), sol.end());
    std::cout << "Solution found!\n";
    std::cout << "Points positions: ";
    for (int site : sol) {
        std::cout << site << " ";
    }
    std::cout << "\nDistances between consecutive points: ";
    for (size_t i = 1; i < sol.size(); ++i) {
        std::cout << (sol[i] - sol[i-1]) << " ";
    }
    std::cout << "\n";
}

TestFramework::TestFramework(InstanceGenerator& gen) : generator(gen) {}

bool TestFramework::verifyInputSize(const std::vector<int>& distances, int expectedSize) {
    return (int)distances.size() == expectedSize;
}

bool TestFramework::verifyInputValues(const std::vector<int>& distances) {
    return std::all_of(distances.begin(), distances.end(), 
        [](int d) { return d > 0 && d <= 512; });
}

int TestFramework::calculateRequiredCuts(int multisetSize) {
    double discriminant = 1.0 + 8.0 * multisetSize;
    double n = ( -1.0 + std::sqrt(discriminant) ) / 2.0;
    return static_cast<int>(std::round(n - 1.0));
}

bool TestFramework::checkCutsPossibility(int multisetSize) {
    int requiredCuts = calculateRequiredCuts(multisetSize);
    int actualSize = (requiredCuts + 2) * (requiredCuts + 1) / 2;
    return actualSize == multisetSize;
}

std::chrono::milliseconds TestFramework::measureExecutionTime(const std::vector<int>& distances) {
    auto start = std::chrono::high_resolution_clock::now();
    // Tu można wstawić testowy algorytm (ale w tym frameworku i tak używamy Benchmarka).
    auto end = std::chrono::high_resolution_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
}

bool TestFramework::checkTimeLimit(const std::chrono::milliseconds& duration) {
    return duration < MAX_EXECUTION_TIME;
}

std::vector<int> TestFramework::loadPDPInstance(const std::string& filename) {
    std::vector<int> distances;
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Unable to open file: " + filename);
    }
    int value;
    while (file >> value) {
        distances.push_back(value);
    }
    return distances;
}

std::vector<std::vector<int>> TestFramework::generateSortedVariants(const std::vector<int>& original) {
    std::vector<std::vector<int>> variants;
    variants.push_back(original);
    
    auto ascending = original;
    std::sort(ascending.begin(), ascending.end());
    variants.push_back(ascending);
    
    auto descending = original;
    std::sort(descending.begin(), descending.end(), std::greater<>());
    variants.push_back(descending);
    
    return variants;
}

void TestFramework::clearInstancesDirectory() {
    if (fs::exists(INSTANCES_DIR)) {
        for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
            fs::remove(entry.path());
        }
    } else {
        fs::create_directory(INSTANCES_DIR);
    }
}

void TestFramework::generateRandomInstances() {
    clearInstancesDirectory();
    
    for (int i = 0; i < RANDOM_INSTANCES_COUNT; i++) {
        int cuts = 3 + (i % 5);
        std::string filename = "instance_" + std::to_string(i + 1) + ".txt";
        std::string fullPath = getFullPath(INSTANCES_DIR, filename);
        
        if (!generator.generateInstance(cuts, fullPath)) {
            std::cout << "Failed to generate instance " << (i + 1) << std::endl;
        }
    }
    std::cout << "Generated " << RANDOM_INSTANCES_COUNT << " random instances in 'instances' directory.\n";
}

std::string TestFramework::getFullPath(const std::string& directory, const std::string& filename) {
    return (fs::path(directory) / filename).string();
}

void TestFramework::listAvailableInstances(const std::string& directory) {
    std::cout << "Available instances in " << directory << ":\n";
    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".txt") {
            std::cout << "  " << entry.path().filename().string() << std::endl;
        }
    }
}

bool TestFramework::testRandomInstances() {
    if (!fs::exists(INSTANCES_DIR) || fs::is_empty(INSTANCES_DIR)) {
        std::cout << "No instances found in the 'instances' directory." << std::endl;
        return false;
    }
    
    bool allTestsPassed = true;
    for (const auto& entry : fs::directory_iterator(INSTANCES_DIR)) {
        if (entry.path().extension() == ".txt") {
            std::vector<int> distances = generator.loadInstance(entry.path().string());
            if (distances.empty()) {
                std::cout << entry.path().filename().string() << ": Failed to load distances" << std::endl;
                allTestsPassed = false;
                continue;
            }
            int totalLength = *std::max_element(distances.begin(), distances.end());
            MapSolver solver(distances, totalLength);
            auto solution = solver.solve();
            if (solution) {
                std::cout << entry.path().filename().string() << ": ";
                displaySolution(*solution);
            } else {
                std::cout << entry.path().filename().string() << ": No solution found\n";
                allTestsPassed = false;
            }
        }
        std::cout << "\n";
    }
    return allTestsPassed;
}

bool TestFramework::testPDPInstances(const std::string& dataDirectory) {
    bool allTestsPassed = true;
    
    if (!fs::exists(dataDirectory)) {
        std::cout << "Error: Directory '" << dataDirectory << "' does not exist." << std::endl;
        return false;
    }
    
    try {
        int totalFiles = 0;
        for (const auto& entry : fs::directory_iterator(dataDirectory)) {
            if (entry.path().extension() == ".txt") {
                totalFiles++;
            }
        }
        
        std::cout << "Found " << totalFiles << " PDP instances to test.\n";
        int currentFile = 0;
        
        for (const auto& entry : fs::directory_iterator(dataDirectory)) {
            if (entry.path().extension() == ".txt") {
                currentFile++;
                std::cout << "Testing PDP instance " << currentFile << "/" << totalFiles 
                          << " (" << entry.path().filename().string() << ")... ";
                
                try {
                    auto distances = loadPDPInstance(entry.path().string());
                    auto result = verifyInstance(distances);
                    
                    if (!result.isValid) {
                        std::cout << "FAILED: " << result.message << std::endl;
                        allTestsPassed = false;
                    } else {
                        std::cout << "PASSED\n";
                    }
                } catch (const std::exception& e) {
                    std::cout << "ERROR: " << e.what() << std::endl;
                    allTestsPassed = false;
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cout << "Filesystem error: " << e.what() << std::endl;
        return false;
    }
    
    return allTestsPassed;
}

bool TestFramework::analyzeSortingImpact(const std::string& instanceFile) {
    try {
        auto original = generator.loadInstance(instanceFile);
        auto variants = generateSortedVariants(original);
        
        const char* sortTypes[] = {"Original", "Ascending", "Descending"};
        
        for (size_t i = 0; i < variants.size(); i++) {
            auto duration = measureExecutionTime(variants[i]);
            std::cout << "  " << sortTypes[i] << " order: " << duration.count() << "ms\n";
            if (!checkTimeLimit(duration)) {
                std::cout << "  WARNING: Execution exceeded time limit!\n";
                return false;
            }
        }
        return true;
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return false;
    }
}

void TestFramework::solveSpecificInstance(const std::string& directory, const std::string& filename) {
    std::string fullPath = getFullPath(directory, filename);
    std::vector<int> distances = generator.loadInstance(fullPath);
    
    if (distances.empty()) {
        std::cout << "Failed to load instance.\n";
        return;
    }
    
    int cuts = calculateRequiredCuts(distances.size());
    int totalLength = *std::max_element(distances.begin(), distances.end());
    
    std::cout << "Solving instance " << filename << " (cuts: " << cuts << ")..." << std::endl;
    MapSolver solver(distances, totalLength);
    
    auto solution = solver.solve();
    if (solution) {
        displaySolution(*solution);
    } else {
        std::cout << "No solution found.\n";
    }
}

void TestFramework::runInteractiveMode() {
    while (true) {
        std::cout << "\nSelect operation mode:\n";
        std::cout << "1. Test all instances from 'instances' directory\n";
        std::cout << "2. Solve specific instance\n";
        std::cout << "3. Generate new random instances\n";
        std::cout << "4. Verify all instances\n";
        std::cout << "5. Generate advanced instances\n";
        std::cout << "6. Run benchmark\n";
        std::cout << "7. Exit\n";
        
        int choice;
        std::cin >> choice;
        
        switch (choice) {
        case 1:
            testRandomInstances();
            break;
        case 2: {
            std::cout << "Select directory:\n";
            std::cout << "1. instances\n";
            std::cout << "2. data\n";
            int dirChoice;
            std::cin >> dirChoice;
            std::string directory = (dirChoice == 1) ? INSTANCES_DIR : DATA_DIR;
            if (fs::exists(directory)) {
                listAvailableInstances(directory);
                std::string filename;
                std::cout << "Enter instance filename: ";
                std::cin >> filename;
                
                std::cout << "\nSelect algorithm to solve instance:\n"
                          << "1. Basic Map Solver\n"
                          << "2. BBd Algorithm\n"
                          << "3. BBb Algorithm\n"
                          << "4. BBb2 Algorithm\n"
                          << "Choice: ";
                int algoChoice;
                std::cin >> algoChoice;
                
                std::vector<int> distances = generator.loadInstance(getFullPath(directory, filename));
                if (distances.empty()) {
                    std::cout << "Failed to load instance.\n";
                    break;
                }
                int totalLength = *std::max_element(distances.begin(), distances.end());

                std::optional<std::vector<int>> solution;
                switch(algoChoice) {
                    case 1: {
                        MapSolver solver(distances, totalLength);
                        solution = solver.solve();
                        break;
                    }
                    case 2: {
                        BBdAlgorithm algorithm;
                        solution = algorithm.solve(distances);
                        break;
                    }
                    case 3: {
                        BBbAlgorithm algorithm;
                        solution = algorithm.solve(distances);
                        break;
                    }
                    case 4: {
                        BBb2Algorithm algorithm;
                        solution = algorithm.solve(distances);
                        break;
                    }
                    default:
                        std::cout << "Invalid algorithm choice.\n";
                        continue;
                }
                if (solution) {
                    displaySolution(*solution);
                } else {
                    std::cout << "No solution found.\n";
                }
            } else {
                std::cout << "Selected directory does not exist." << std::endl;
            }
            break;
        }
        case 3: {
            SortOrder order = getSortOrderFromUser();
            generateRandomInstances(order);
            break;
        }
        case 4:
            verifyAllInstances();
            break;
        case 5: {
            std::cout << "Enter maximum number of cuts (minimum is " << MIN_CUTS << "): ";
            int maxCuts;
            std::cin >> maxCuts;
            if (std::cin.fail()) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please enter a number.\n";
                break;
            }
            SortOrder order = getSortOrderFromUser();
            generateInstancesRange(maxCuts, order);
            break;
        }
        case 6:
            benchmark.runBenchmark();
            break;
        case 7:
            return;
        default:
            std::cout << "Invalid choice.\n";
        }
    }
}

bool TestFramework::verifyInstancesInDirectory(const std::string& directory) {
    if (!fs::exists(directory)) {
        std::cout << "Directory '" << directory << "' does not exist." << std::endl;
        return false;
    }

    bool allValid = true;
    int totalFiles = 0;
    int validFiles = 0;

    for (const auto& entry : fs::directory_iterator(directory)) {
        if (entry.path().extension() == ".txt") {
            totalFiles++;
            std::cout << "Verifying " << entry.path().filename().string() << "... ";
            try {
                std::vector<int> distances = generator.loadInstance(entry.path().string());
                if (generator.verifyInstance(entry.path().filename().string())) {
                    std::cout << "VALID\n";
                    validFiles++;
                } else {
                    std::cout << "INVALID\n";
                    allValid = false;
                }
            } catch (const std::exception& e) {
                std::cout << "ERROR: " << e.what() << std::endl;
                allValid = false;
            }
        }
    }

    std::cout << "\nSummary for " << directory << ":\n";
    std::cout << "Total files: " << totalFiles << "\n";
    std::cout << "Valid files: " << validFiles << "\n";
    std::cout << "Invalid files: " << (totalFiles - validFiles) << "\n";

    return allValid;
}

bool TestFramework::verifyAllInstances() {
    std::cout << "Verifying all instances in both directories...\n\n";
    
    bool instancesValid = verifyInstancesInDirectory(INSTANCES_DIR);
    bool dataValid = verifyInstancesInDirectory(DATA_DIR);
    
    std::cout << "\nOverall verification result: " 
              << (instancesValid && dataValid ? "ALL VALID" : "SOME INVALID") 
              << std::endl;
              
    return instancesValid && dataValid;
}

SortOrder TestFramework::getSortOrderFromUser() {
    std::cout << "\nSelect sorting order:\n";
    std::cout << "1. Shuffled (random order)\n";
    std::cout << "2. Ascending order\n";
    std::cout << "3. Descending order\n";
    int choice;
    std::cin >> choice;
    switch(choice) {
        case 2: return SortOrder::ASCENDING;
        case 3: return SortOrder::DESCENDING;
        default: return SortOrder::SHUFFLED;
    }
}

void TestFramework::generateRandomInstances(SortOrder order) {
    clearInstancesDirectory();
    for (int i = 0; i < RANDOM_INSTANCES_COUNT; i++) {
        int cuts = 3 + (i % 5);
        std::string filename = "instance_" + std::to_string(i + 1) + ".txt";
        std::string fullPath = getFullPath(INSTANCES_DIR, filename);
        if (!generator.generateInstance(cuts, fullPath, order)) {
            std::cout << "Failed to generate instance " << (i + 1) << std::endl;
        }
    }
    std::cout << "Generated " << RANDOM_INSTANCES_COUNT << " random instances in 'instances' directory.\n";
}

void TestFramework::generateInstancesRange(int maxCuts) {
    if (!isValidNumberOfCuts(maxCuts)) {
        std::cout << "Invalid number of cuts. Minimum allowed is " << MIN_CUTS << std::endl;
        return;
    }

    clearInstancesDirectory();
    int generatedCount = 0;

    for (int cuts = MIN_CUTS; cuts <= maxCuts; cuts++) {
        std::string filename = "instance_" + std::to_string(cuts) + ".txt";
        std::string fullPath = getFullPath(INSTANCES_DIR, filename);
        
        std::cout << "Generating instance with " << cuts << " cuts... ";
        if (generator.generateInstance(cuts, fullPath)) {
            std::cout << "SUCCESS\n";
            generatedCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    std::cout << "\nGeneration complete:\n";
    std::cout << "Successfully generated: " << generatedCount << " instances\n";
    std::cout << "Failed generations: " << (maxCuts - MIN_CUTS + 1 - generatedCount) << std::endl;
}

void TestFramework::generateInstancesRange(int maxCuts, SortOrder order) {
    if (!isValidNumberOfCuts(maxCuts)) {
        std::cout << "Invalid number of cuts. Minimum allowed is " << MIN_CUTS << std::endl;
        return;
    }

    clearInstancesDirectory();
    int generatedCount = 0;

    for (int cuts = MIN_CUTS; cuts <= maxCuts; cuts++) {
        std::string filename = "instance_" + std::to_string(cuts) + ".txt";
        std::string fullPath = getFullPath(INSTANCES_DIR, filename);

        std::cout << "Generating instance with " << cuts << " cuts... ";
        if (generator.generateInstance(cuts, fullPath, order)) {
            std::cout << "SUCCESS\n";
            generatedCount++;
        } else {
            std::cout << "FAILED\n";
        }
    }

    std::cout << "\nGeneration complete:\n";
    std::cout << "Successfully generated: " << generatedCount << " instances\n";
    std::cout << "Failed generations: " << (maxCuts - MIN_CUTS + 1 - generatedCount) << std::endl;
}

bool TestFramework::isValidNumberOfCuts(int cuts) const {
    return cuts >= MIN_CUTS;
}

TestFramework::VerificationResult TestFramework::verifyInstance(const std::vector<int>& distances) {
    if (distances.empty()) {
        return {false, "Empty instance"};
    }
    int expectedCuts = calculateRequiredCuts((int)distances.size());
    int expectedSize = (expectedCuts + 2) * (expectedCuts + 1) / 2;
    if (!verifyInputSize(distances, expectedSize)) {
        return {false, "Invalid input size"};
    }
    if (!verifyInputValues(distances)) {
        return {false, "Invalid distance values"};
    }
    if (!checkCutsPossibility((int)distances.size())) {
        return {false, "No valid number of cuts possible for this multiset size"};
    }
    return {true, "Instance verified successfully"};
}
