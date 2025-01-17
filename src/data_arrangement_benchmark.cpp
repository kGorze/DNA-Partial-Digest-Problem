//
// Created by konrad_guest on 17/01/2025.
//
#include "data_arrangement_benchmark.h"

DataArrangementBenchmark::DataArrangementBenchmark(const std::vector<int>& baseDistances, int repetitions)
    : originalDistances(baseDistances), repetitionCount(repetitions) {}

std::vector<DataArrangementBenchmark::ArrangementResult> DataArrangementBenchmark::runArrangementTests() {
    std::vector<ArrangementResult> results;
    
    results.push_back(testArrangement("Original", originalDistances));
    results.push_back(testArrangement("Ascending", getSortedDistances(true)));
    results.push_back(testArrangement("Descending", getSortedDistances(false)));
    results.push_back(testArrangement("Random", getRandomizedDistances()));
    results.push_back(testArrangement("Clustered", getClusteredDistances()));
    
    return results;
}

DataArrangementBenchmark::ArrangementResult DataArrangementBenchmark::testArrangement(
    const std::string& name, 
    const std::vector<int>& distances
) {
    std::vector<double> times;
    int successCount = 0;
    
    for (int i = 0; i < repetitionCount; ++i) {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        BBb2Algorithm bbb2;
        BBbAlgorithm bbb;
        BBdAlgorithm bbd;
        
        auto solution = bbb2.solve(distances);
        if (solution) successCount++;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        double timeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
            endTime - startTime
        ).count();
        
        times.push_back(timeMs);
    }
    
    double avgTime = std::accumulate(times.begin(), times.end(), 0.0) / times.size();
    double minTime = *std::min_element(times.begin(), times.end());
    double maxTime = *std::max_element(times.begin(), times.end());
    
    return ArrangementResult{
        name, avgTime, minTime, maxTime, successCount, repetitionCount
    };
}

std::vector<int> DataArrangementBenchmark::getSortedDistances(bool ascending) {
    std::vector<int> sorted = originalDistances;
    if (ascending)
        std::sort(sorted.begin(), sorted.end());
    else
        std::sort(sorted.begin(), sorted.end(), std::greater<>());
    return sorted;
}

std::vector<int> DataArrangementBenchmark::getRandomizedDistances() {
    std::vector<int> randomized = originalDistances;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(randomized.begin(), randomized.end(), gen);
    return randomized;
}

std::vector<int> DataArrangementBenchmark::getClusteredDistances() {
    std::vector<int> clustered = originalDistances;
    std::sort(clustered.begin(), clustered.end());
    
    int groupSize = clustered.size() / 3;
    std::random_device rd;
    std::mt19937 gen(rd());
    
    if (groupSize > 0) {
        std::shuffle(clustered.begin(), clustered.begin() + groupSize, gen);
        std::shuffle(clustered.begin() + groupSize, clustered.begin() + 2 * groupSize, gen);
        std::shuffle(clustered.begin() + 2 * groupSize, clustered.end(), gen);
    }
    
    return clustered;
}