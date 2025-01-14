//
// Created by konrad_guest on 14/01/2025.
//

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <vector>
#include <string>
#include <chrono>
#include "instance_generator.h"

class TestFramework {
private:
    static const std::chrono::hours MAX_EXECUTION_TIME;
    InstanceGenerator& generator;
    
    // Helper methods
    bool verifyInputSize(const std::vector<int>& distances, int expectedSize);
    bool verifyInputValues(const std::vector<int>& distances);
    bool checkCutsPossibility(int multisetSize);
    int calculateRequiredCuts(int multisetSize);
    
    // Timing methods
    std::chrono::milliseconds measureExecutionTime(const std::vector<int>& distances);
    bool checkTimeLimit(const std::chrono::milliseconds& duration);
    
    // Data loading methods
    std::vector<int> loadPDPInstance(const std::string& filename);
    std::vector<std::vector<int>> generateSortedVariants(const std::vector<int>& original);

public:
    explicit TestFramework(InstanceGenerator& gen);
    
    // Main testing methods
    bool testRandomInstances(int numTests, int minCuts, int maxCuts);
    bool testPDPInstances(const std::string& dataDirectory);
    bool analyzeSortingImpact(const std::string& instanceFile);
    
    // Input verification methods
    struct VerificationResult {
        bool isValid;
        std::string message;
    };
    
    VerificationResult verifyInstance(const std::vector<int>& distances);
};

#endif //TEST_FRAMEWORK_H
