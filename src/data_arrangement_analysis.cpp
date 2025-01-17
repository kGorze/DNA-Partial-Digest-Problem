//
// Created by konrad_guest on 17/01/2025.
//
#include "data_arrangement_analysis.h"
#include "data_arrangement_benchmark.h"
#include "instance_generator.h"
#include <iostream>
#include <iomanip>

void runDataArrangementAnalysis(const std::string& instanceFile, int repetitions) {
    InstanceGenerator generator;
    std::vector<int> distances = generator.loadInstance(instanceFile);
    
    if (distances.empty()) {
        std::cout << "Failed to load instance from: " << instanceFile << "\n";
        return;
    }
    
    std::cout << "\nRunning data arrangement analysis for instance: " << instanceFile << "\n";
    std::cout << "Number of distances: " << distances.size() << "\n";
    std::cout << "Repetitions per arrangement: " << repetitions << "\n\n";
    
    DataArrangementBenchmark benchmark(distances, repetitions);
    auto results = benchmark.runArrangementTests();
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Results:\n";
    std::cout << std::setw(12) << "Arrangement" 
              << std::setw(12) << "Avg Time" 
              << std::setw(12) << "Min Time" 
              << std::setw(12) << "Max Time" 
              << std::setw(12) << "Success" << "\n";
    std::cout << std::string(60, '-') << "\n";
    
    for (const auto& result : results) {
        std::cout << std::setw(12) << result.arrangementType
                  << std::setw(12) << result.averageTimeMs
                  << std::setw(12) << result.minTimeMs
                  << std::setw(12) << result.maxTimeMs
                  << std::setw(12) << result.successCount << "/" << result.totalTests << "\n";
    }
}