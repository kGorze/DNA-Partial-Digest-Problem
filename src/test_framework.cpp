//
// Created by konrad_guest on 14/01/2025.
//

#include "../include/test_framework.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <cmath>
#include <iostream>

namespace fs = std::filesystem;

const std::chrono::hours TestFramework::MAX_EXECUTION_TIME(1);

TestFramework::TestFramework(InstanceGenerator& gen) : generator(gen) {}

bool TestFramework::verifyInputSize(const std::vector<int>& distances, int expectedSize) {
    return distances.size() == expectedSize;
}

bool TestFramework::verifyInputValues(const std::vector<int>& distances) {
    return std::all_of(distances.begin(), distances.end(), 
        [](int d) { return d > 0 && d <= 512; });
}

int TestFramework::calculateRequiredCuts(int multisetSize) {
    double discriminant = 1 + 8 * multisetSize;
    double n = (-1 + std::sqrt(discriminant)) / 2;
    return std::round(n - 1);
}

bool TestFramework::checkCutsPossibility(int multisetSize) {
    int requiredCuts = calculateRequiredCuts(multisetSize);
    int actualSize = (requiredCuts + 2) * (requiredCuts + 1) / 2;
    return actualSize == multisetSize;
}

std::chrono::milliseconds TestFramework::measureExecutionTime(const std::vector<int>& distances) {
    auto start = std::chrono::high_resolution_clock::now();
    // TODO: Add actual algorithm execution here
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

bool TestFramework::testRandomInstances(int numTests, int minCuts, int maxCuts) {
    bool allTestsPassed = true;
    std::cout << "Generating and testing " << numTests << " random instances...\n";
    
    for (int i = 0; i < numTests; i++) {
        int cuts = minCuts + (rand() % (maxCuts - minCuts + 1));
        std::string filename = "random_test_" + std::to_string(i) + ".txt";
        
        std::cout << "Testing instance " << (i + 1) << "/" << numTests 
                 << " (cuts=" << cuts << ")... ";
        
        if (!generator.generateInstance(cuts, filename)) {
            std::cout << "Failed to generate instance " << filename << std::endl;
            allTestsPassed = false;
            continue;
        }
        
        auto distances = generator.loadInstance(filename);
        auto result = verifyInstance(distances);
        
        if (!result.isValid) {
            std::cout << "FAILED: " << result.message << std::endl;
            allTestsPassed = false;
        } else {
            std::cout << "PASSED" << std::endl;
        }
    }
    
    return allTestsPassed;
}

bool TestFramework::testPDPInstances(const std::string& dataDirectory) {
    bool allTestsPassed = true;
    
    // Check if directory exists
    if (!fs::exists(dataDirectory)) {
        std::cout << "Error: Directory '" << dataDirectory << "' does not exist.\n";
        std::cout << "Current path: " << fs::current_path() << "\n";
        std::cout << "Please ensure the 'data' directory containing PDP instances is present.\n";
        return false;
    }
    
    try {
        // Count total files first
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
                        std::cout << "PASSED" << std::endl;
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
        std::cout << "Analyzing sorting impact for " << instanceFile << "... ";
        
        auto original = generator.loadInstance(instanceFile);
        auto variants = generateSortedVariants(original);
        
        const char* sortTypes[] = {"Original", "Ascending", "Descending"};
        
        std::cout << "\n";
        for (size_t i = 0; i < variants.size(); i++) {
            auto duration = measureExecutionTime(variants[i]);
            std::cout << "  " << sortTypes[i] << " order: " << duration.count() << "ms\n";
            
            if (!checkTimeLimit(duration)) {
                std::cout << "  WARNING: Execution exceeded time limit!\n";
                return false;
            }
        }
        std::cout << "Analysis complete.\n";
        return true;
        
    } catch (const std::exception& e) {
        std::cout << "ERROR: " << e.what() << std::endl;
        return false;
    }
}

TestFramework::VerificationResult TestFramework::verifyInstance(const std::vector<int>& distances) {
    if (distances.empty()) {
        return {false, "Empty instance"};
    }
    
    int expectedCuts = calculateRequiredCuts(distances.size());
    if (!verifyInputSize(distances, (expectedCuts + 2) * (expectedCuts + 1) / 2)) {
        return {false, "Invalid input size"};
    }
    
    if (!verifyInputValues(distances)) {
        return {false, "Invalid distance values"};
    }
    
    if (!checkCutsPossibility(distances.size())) {
        return {false, "No valid number of cuts possible for this multiset size"};
    }
    
    return {true, "Instance verified successfully"};
}