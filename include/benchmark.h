//
// Created by konrad_guest on 14/01/2025.
//

#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <map>
#include <cmath>

class Benchmark {
public:
    // Typy algorytmów do testowania
    enum class Algorithm {
        BASIC,
        OPTIMIZED,
        PARALLEL,
        HYBRID
    };

    // Struktura przechowująca wyniki pojedynczego testu
    struct BenchmarkResult {
        int instanceSize;
        double executionTimeMs;
        Algorithm algorithmType;
    };

    // Konstruktor
    Benchmark();

    // Główne metody
    void runBenchmark();
    void runSingleAlgorithmBenchmark(Algorithm algo);
    void saveResults(const std::string& filename);

private:
    // Stałe konfiguracyjne
    static const std::string BENCHMARK_DIR;
    static const std::vector<int> INSTANCE_SIZES;
    static const int REPEAT_COUNT = 5;  // Ile razy powtarzamy test dla uśrednienia wyników

    // Kontener na wyniki
    std::vector<BenchmarkResult> results;

    // Metody pomocnicze
    void createBenchmarkDirectory();
    double measureAlgorithmTime(Algorithm algo, int size);
    std::string getAlgorithmName(Algorithm algo) const;
    void prepareInstance(int size);
    
    // Placeholder na rzeczywiste algorytmy (do implementacji później)
    void runBasicAlgorithm(int size);
    void runOptimizedAlgorithm(int size);
    void runParallelAlgorithm(int size);
    void runHybridAlgorithm(int size);
};

//PREEXISTING ALGORITHM
#include <vector>
#include <optional>

class BBdAlgorithm {
public:
    struct Node {
        std::vector<int> D;  // Aktualny multizbiór D
        std::vector<int> X;  // Aktualny zbiór rozwiązań X
        int level;           // Poziom w drzewie rozwiązań

        Node(const std::vector<int>& d, const std::vector<int>& x, int lvl);
    };

    BBdAlgorithm() = default;

    std::optional<std::vector<int>> solve(std::vector<int> D);

private:
    std::vector<int> calculateDelta(int y, const std::vector<int>& X);
    bool isSubset(const std::vector<int>& subset, const std::vector<int>& set);
    std::vector<int> removeSubset(const std::vector<int>& set, const std::vector<int>& subset);
    std::optional<std::vector<int>> place(Node& current);
};


#endif //BENCHMARK_H
