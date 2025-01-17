//
// Created by konrad_guest on 17/01/2025.
//

#ifndef DATA_ARRANGEMENT_BENCHMARK_H
#define DATA_ARRANGEMENT_BENCHMARK_H

#include <string>
#include <vector>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <random>
#include <iostream>
#include <iomanip>
#include "algorithms/bbb2_algorithm.h"
#include "algorithms/bbb_algorithm.h"
#include "algorithms/bbd_algorithm.h"

class DataArrangementBenchmark {
public:
    struct ArrangementResult {
        std::string arrangementType;
        double averageTimeMs;
        double minTimeMs;
        double maxTimeMs;
        int successCount;
        int totalTests;
    };

    DataArrangementBenchmark(const std::vector<int>& baseDistances, int repetitions = 10);
    std::vector<ArrangementResult> runArrangementTests();

private:
    std::vector<int> originalDistances;
    int repetitionCount;
    
    ArrangementResult testArrangement(const std::string& name, const std::vector<int>& distances);
    std::vector<int> getSortedDistances(bool ascending);
    std::vector<int> getRandomizedDistances();
    std::vector<int> getClusteredDistances();
};

#endif // DATA_ARRANGEMENT_BENCHMARK_H