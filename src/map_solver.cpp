//
// Created by konrad_guest on 14/01/2025.
//

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../include/map_solver.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>  // Upewnij się, że masz zaincludowane <iomanip> do setprecision

#include "../include/map_solver.h"

MapSolver::MapSolver(const std::vector<int>& inputDistances, int length) 
    : distances(inputDistances), totalLength(length), processedPaths(0) 
{
    maxind = static_cast<int>((1 + std::sqrt(1 + 8.0 * distances.size())) / 2);
    currentMap.resize(maxind, 0);
    std::sort(distances.begin(), distances.end());
    
    // Przywrócenie obliczenia totalPaths
    totalPaths = calculateTotalPaths();
    
    stats = Statistics {
        totalPaths,        // totalPaths
        0,                 // processedPaths
        0.0,               // searchTimeMs
        std::vector<int>(),// solution
        inputDistances,    
        false              
    };
}

uint64_t MapSolver::calculateTotalPaths() const {
    // Proste (i mocno zawyżone) oszacowanie liczby wszystkich ścieżek
    // bez uwzględniania ograniczeń
    uint64_t total = 1;
    for (int i = 1; i < maxind - 1; i++) {
        total *= (totalLength - i);
    }
    return total;
}

void MapSolver::updateProgress() {
    processedPaths++;
    if (processedPaths % 1000 == 0) {  
        displayProgress();
    }
}

void MapSolver::displayProgress() const {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                        currentTime - startTime
                    ).count();
    
    std::cout << "\rPaths processed: " << processedPaths 
              << " | Time: " << elapsedMs << "ms" << std::flush;
}

/**
 * Metoda sprawdzająca, czy część aktualnego rozwiązania (do indeksu assignedCount-1)
 * nie generuje odległości niezgodnych z multisetem distances.
 */
bool MapSolver::isValidPartialSolution(int assignedCount) const {
    std::vector<int> currentDistances;
    currentDistances.reserve((assignedCount * (assignedCount-1)) / 2);
    
    for (int i = 0; i < assignedCount; i++) {
        for (int j = 0; j < i; j++) {
            int distance = currentMap[i] - currentMap[j];
            if (distance < 0 || distance > totalLength) {
                return false;
            }
            currentDistances.push_back(distance);
        }
    }
    
    if (assignedCount <= 1) {
        return true;
    }
    
    std::sort(currentDistances.begin(), currentDistances.end());
    
    std::vector<int> tempDistances = distances;
    for (int dist : currentDistances) {
        auto it = std::find(tempDistances.begin(), tempDistances.end(), dist);
        if (it == tempDistances.end()) {
            return false;
        }
        tempDistances.erase(it);
    }
    
    return true;
}

void MapSolver::szukaj(int ind, bool* jest) {
    processedPaths++;  // Inkrementacja licznika dla każdego wejścia do funkcji
    if (*jest) return;
    
    if (ind == maxind) {
        updateProgress();
        if (isValidPartialSolution(maxind)) {
            *jest = true;
            stats.solution = currentMap;
            stats.solutionFound = true;
            
            std::cout << "\nSolution found!\n";
            for (int site : currentMap) {
                std::cout << site << " ";
            }
            std::cout << "\n";
        }
        return;
    }
    
    int start, end;
    if (ind == 0) {
        start = end = 0;
    } else if (ind == maxind - 1) {
        start = end = totalLength;
    } else {
        start = currentMap[ind - 1] + 1;
        end = totalLength - (maxind - ind - 1);
    }
    
    for (int pos = start; pos <= end; pos++) {
        currentMap[ind] = pos;
        
        if (isValidPartialSolution(ind + 1)) {
            szukaj(ind + 1, jest);
        }
        
        if (*jest) break;
    }
}

bool MapSolver::solve() {
    bool jest = false;
    startTime = std::chrono::steady_clock::now();
    
    szukaj(0, &jest);
    
    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(
                            endTime - startTime
                        ).count();
    stats.processedPaths = processedPaths;
    
    // Obliczenie procentu przeszukanych ścieżek
    double percentage = 0.0;
    if(totalPaths > 0) {
        percentage = (static_cast<double>(processedPaths) / static_cast<double>(totalPaths)) * 100.0;
    }
    
    std::cout << "\n\nSearch completed:\n"
              << "- Time taken: " << stats.searchTimeMs << "ms\n"
              << "- Paths processed: " << stats.processedPaths << "\n"
              << "- Approx. percentage of search space processed: " 
              << std::fixed << std::setprecision(6) << percentage << "%\n"
              << "- Solution " << (jest ? "found" : "not found") << "\n";
    
    return jest;
}