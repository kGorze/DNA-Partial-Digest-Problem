//
// Created by konrad_guest on 14/01/2025.
//

#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <vector>
#include <string>
#include <chrono>
#include "instance_generator.h"
#include "benchmark.h"

/**
 * Klasa TestFramework - tryb interaktywny do generowania i testowania instancji PDP.
 */
class TestFramework {
private:
    static const std::chrono::hours MAX_EXECUTION_TIME;
    static const std::string INSTANCES_DIR;
    static const std::string DATA_DIR;
    static const int RANDOM_INSTANCES_COUNT;
    static const int MIN_CUTS = 3;
    InstanceGenerator& generator;
    Benchmark benchmark;

    bool verifyInputSize(const std::vector<int>& distances, int expectedSize);
    bool verifyInputValues(const std::vector<int>& distances);
    bool checkCutsPossibility(int multisetSize);
    int calculateRequiredCuts(int multisetSize);

    std::chrono::milliseconds measureExecutionTime(const std::vector<int>& distances);
    bool checkTimeLimit(const std::chrono::milliseconds& duration);

    std::vector<int> loadPDPInstance(const std::string& filename);
    std::vector<std::vector<int>> generateSortedVariants(const std::vector<int>& original);
    
    void clearInstancesDirectory();
    void generateRandomInstances();
    std::string getFullPath(const std::string& directory, const std::string& filename);
    void listAvailableInstances(const std::string& directory);
    
    bool verifyInstancesInDirectory(const std::string& directory);
    void generateInstancesRange(int maxCuts);
    bool isValidNumberOfCuts(int cuts) const;

    void generateRandomInstances(SortOrder order);
    void generateInstancesRange(int maxCuts, SortOrder order);
    SortOrder getSortOrderFromUser();

public:
    explicit TestFramework(InstanceGenerator& gen);
    
    bool testRandomInstances();
    bool testPDPInstances(const std::string& dataDirectory);
    bool analyzeSortingImpact(const std::string& instanceFile);
    void solveSpecificInstance(const std::string& directory, const std::string& filename);
    void runInteractiveMode();

    struct VerificationResult {
        bool isValid;
        std::string message;
    };
    
    VerificationResult verifyInstance(const std::vector<int>& distances);
    
    bool verifyAllInstances();
    void generateAdvancedInstances();
};

#endif //TEST_FRAMEWORK_H
