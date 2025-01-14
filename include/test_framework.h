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
    static const std::string INSTANCES_DIR;
    static const std::string DATA_DIR;
    static const int RANDOM_INSTANCES_COUNT;
    static const int MIN_CUTS = 3;  // Minimalna sensowna liczba cięć
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
    
    // Helper methods
    void clearInstancesDirectory();
    void generateRandomInstances();
    std::string getFullPath(const std::string& directory, const std::string& filename);
    void listAvailableInstances(const std::string& directory);
    
    // Nowe metody private
    bool verifyInstancesInDirectory(const std::string& directory);
    void generateInstancesRange(int maxCuts);
    bool isValidNumberOfCuts(int cuts) const;

public:
    explicit TestFramework(InstanceGenerator& gen);
    
    // Main testing methods
    bool testRandomInstances();
    bool testPDPInstances(const std::string& dataDirectory);
    bool analyzeSortingImpact(const std::string& instanceFile);
    void solveSpecificInstance(const std::string& directory, const std::string& filename);
    void runInteractiveMode();
    
    // Input verification methods
    struct VerificationResult {
        bool isValid;
        std::string message;
    };
    
    VerificationResult verifyInstance(const std::vector<int>& distances);
    
    // Nowe metody public
    bool verifyAllInstances();
    void generateAdvancedInstances();
};

#endif //TEST_FRAMEWORK_H
