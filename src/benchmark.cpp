//
// Created by konrad_guest on 14/01/2025.
//
#include "../include/benchmark.h"
#include <iostream>
#include <algorithm>
#include <numeric>

const std::string Benchmark::BENCHMARK_DIR = "benchmark";
const std::vector<int> Benchmark::INSTANCE_SIZES = {10, 20, 50, 100, 200, 500, 1000};

Benchmark::Benchmark() {
    createBenchmarkDirectory();
}

void Benchmark::createBenchmarkDirectory() {
    namespace fs = std::filesystem;
    if (!fs::exists(BENCHMARK_DIR)) {
        fs::create_directory(BENCHMARK_DIR);
    }
}

void Benchmark::runBenchmark() {
    std::cout << "\nWybierz algorytm do przetestowania:\n"
              << "1. Podstawowy algorytm\n"
              << "2. Algorytm zoptymalizowany\n"
              << "3. Algorytm równoległy\n"
              << "4. Algorytm hybrydowy\n"
              << "5. Wszystkie algorytmy\n";

    int choice;
    std::cin >> choice;

    if (choice >= 1 && choice <= 4) {
        runSingleAlgorithmBenchmark(static_cast<Algorithm>(choice - 1));
    } else if (choice == 5) {
        for (int i = 0; i < 4; ++i) {
            runSingleAlgorithmBenchmark(static_cast<Algorithm>(i));
        }
    } else {
        std::cout << "Nieprawidłowy wybór\n";
        return;
    }

    // Generowanie nazwy pliku z timestampem
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::system_clock::to_time_t(now);
    std::string filename = "benchmark_results_" + std::to_string(timestamp) + ".csv";
    
    saveResults(filename);
}

void Benchmark::runSingleAlgorithmBenchmark(Algorithm algo) {
    std::cout << "\nUruchamiam benchmark dla algorytmu: " 
              << getAlgorithmName(algo) << "\n";

    for (int size : INSTANCE_SIZES) {
        std::cout << "Testowanie dla rozmiaru n = " << size << "... ";
        double avgTime = 0.0;

        // Wykonaj kilka pomiarów i uśrednij wyniki
        for (int i = 0; i < REPEAT_COUNT; ++i) {
            avgTime += measureAlgorithmTime(algo, size);
        }
        avgTime /= REPEAT_COUNT;

        results.push_back({size, avgTime, algo});
        std::cout << "średni czas: " << avgTime << "ms\n";
    }
}

double Benchmark::measureAlgorithmTime(Algorithm algo, int size) {
    prepareInstance(size);  // Przygotuj dane testowe

    auto start = std::chrono::high_resolution_clock::now();
    
    // Wywołaj odpowiedni algorytm
    switch (algo) {
        case Algorithm::BASIC:
            runBasicAlgorithm(size);
            break;
        case Algorithm::OPTIMIZED:
            runOptimizedAlgorithm(size);
            break;
        case Algorithm::PARALLEL:
            runParallelAlgorithm(size);
            break;
        case Algorithm::HYBRID:
            runHybridAlgorithm(size);
            break;
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    
    return duration.count();
}

void Benchmark::saveResults(const std::string& filename) {
    std::string fullPath = BENCHMARK_DIR + "/" + filename;
    std::ofstream file(fullPath);
    
    if (!file.is_open()) {
        std::cerr << "Nie można otworzyć pliku: " << fullPath << "\n";
        return;
    }

    // Zapisz nagłówek CSV
    file << "algorithm,size,time_ms\n";

    // Zapisz wyniki
    for (const auto& result : results) {
        file << getAlgorithmName(result.algorithmType) << ","
             << result.instanceSize << ","
             << result.executionTimeMs << "\n";
    }

    std::cout << "\nWyniki zapisano do pliku: " << fullPath << "\n";
}

std::string Benchmark::getAlgorithmName(Algorithm algo) const {
    switch (algo) {
        case Algorithm::BASIC:     return "basic";
        case Algorithm::OPTIMIZED: return "optimized";
        case Algorithm::PARALLEL:  return "parallel";
        case Algorithm::HYBRID:    return "hybrid";
        default:                   return "unknown";
    }
}

// Metody do implementacji później
void Benchmark::prepareInstance(int size) {
    // Tu będzie przygotowanie danych testowych
}

void Benchmark::runBasicAlgorithm(int size) {
    // Placeholder na podstawowy algorytm
}

void Benchmark::runOptimizedAlgorithm(int size) {
    BBdAlgorithm algorithm;
    std::vector<int> testData(size);

    for (int i = 0; i < size; ++i) {
        testData[i] = i + 1;
    }
    
    auto result = algorithm.solve(testData);
}

void Benchmark::runParallelAlgorithm(int size) {
    // Placeholder na algorytm równoległy
}

void Benchmark::runHybridAlgorithm(int size) {
    // Placeholder na algorytm hybrydowy
}

//PREEXISTING ALGORITHM
BBdAlgorithm::Node::Node(const std::vector<int>& d, const std::vector<int>& x, int lvl)
    : D(d), X(x), level(lvl) {}

// Implementacja metody solve
std::optional<std::vector<int>> BBdAlgorithm::solve(std::vector<int> D) {
    if (D.empty()) return std::nullopt;

    // Znajdź szerokość (maksymalny element)
    int width = *std::max_element(D.begin(), D.end());
    
    // Inicjalizuj X jako {0, width}
    std::vector<int> initialX = {0, width};
    
    // Usuń width z D
    auto it = std::find(D.begin(), D.end(), width);
    if (it != D.end()) {
        D.erase(it);
    }
    
    // Utwórz węzeł początkowy
    Node root(D, initialX, 0);
    
    return place(root);
}

// Obliczenie różnic między nowym punktem y a wszystkimi punktami w X
std::vector<int> BBdAlgorithm::calculateDelta(int y, const std::vector<int>& X) {
    std::vector<int> delta;
    for (int x : X) {
        delta.push_back(std::abs(y - x));
    }
    return delta;
}

// Sprawdzenie, czy wszystkie elementy podzbioru znajdują się w zbiorze
bool BBdAlgorithm::isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
    std::vector<int> temp = set;
    for (int element : subset) {
        auto it = std::find(temp.begin(), temp.end(), element);
        if (it == temp.end()) return false;
        temp.erase(it);
    }
    return true;
}

// Usunięcie elementów podzbioru ze zbioru
std::vector<int> BBdAlgorithm::removeSubset(const std::vector<int>& set, const std::vector<int>& subset) {
    std::vector<int> result = set;
    for (int element : subset) {
        auto it = std::find(result.begin(), result.end(), element);
        if (it != result.end()) {
            result.erase(it);
        }
    }
    return result;
}

std::optional<std::vector<int>> BBdAlgorithm::place(Node& current) {
    if (current.D.empty()) {
        // Znaleziono rozwiązanie
        return current.X;
    }

    // Pobierz maksymalny element z D
    int y = *std::max_element(current.D.begin(), current.D.end());
    
    // Spróbuj dodać y do X
    std::vector<int> deltaY = calculateDelta(y, current.X);
    if (isSubset(deltaY, current.D)) {
        std::vector<int> newX = current.X;
        newX.push_back(y);
        std::vector<int> newD = removeSubset(current.D, deltaY);
        
        Node next(newD, newX, current.level + 1);
        auto result = place(next);
        if (result) return result;
    }

    // Spróbuj width - y
    int width = current.X.back();  // width to ostatni element w X
    int complementY = width - y;
    if (complementY != y) {  // Unikaj powielania tej samej wartości
        std::vector<int> deltaComplement = calculateDelta(complementY, current.X);
        if (isSubset(deltaComplement, current.D)) {
            std::vector<int> newX = current.X;
            newX.push_back(complementY);
            std::vector<int> newD = removeSubset(current.D, deltaComplement);
            
            Node next(newD, newX, current.level + 1);
            auto result = place(next);
            if (result) return result;
        }
    }

    return std::nullopt;
}