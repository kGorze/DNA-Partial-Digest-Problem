#include "../include/map_solver.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

MapSolver::MapSolver(const std::vector<int>& inputDistances, int length)
    : distances(inputDistances), totalLength(length), processedPaths(0)
{
    std::sort(distances.begin(), distances.end());
    maxind = static_cast<int>((1 + std::sqrt(1 + 8.0 * distances.size())) / 2);
    currentMap.resize(maxind, -1);

    totalPaths = calculateTotalPaths();

    for (int d : distances) {
        distanceCounter[d]++;
    }

    stats.totalPaths      = totalPaths;
    stats.processedPaths  = 0;
    stats.searchTimeMs    = 0.0;
    stats.solutionFound   = false;
    stats.solution        = {};
    stats.inputDistances  = inputDistances;
}

void MapSolver::initializeRemainingDistances() {
    remainingDistances.clear();
    for (int d : distances) {
        remainingDistances[d]++;
    }
}

uint64_t MapSolver::calculateTotalPaths() const {
    // Simple guess: (endVal - startVal) ^ (maxind - 2) ...
    // For demonstration only
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
    auto elapsedMs   = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    std::cout << "\rPaths processed: " << processedPaths << " | Time: " << elapsedMs << "ms" << std::flush;
}

bool MapSolver::isValidPartialSolution(int assignedCount) const {
    if (assignedCount <= 1) return true;

    std::map<int, int> usedDistances;
    for (int i = 0; i < assignedCount; i++) {
        if (currentMap[i] == -1) continue;
        for (int j = 0; j < i; j++) {
            if (currentMap[j] == -1) continue;
            int distance = std::abs(currentMap[i] - currentMap[j]);
            auto it = distanceCounter.find(distance);
            if (it == distanceCounter.end()) {
                return false;
            }
            usedDistances[distance]++;
            if (usedDistances[distance] > it->second) {
                return false;
            }
        }
    }
    return true;
}

void MapSolver::szukaj(int ind, bool* jest) {
    if (*jest) return;
    processedPaths++;

    if (ind == maxind) {
        if (isValidPartialSolution(maxind)) {
            *jest = true;
            stats.solution      = currentMap;
            stats.solutionFound = true;
        }
        updateProgress();
        return;
    }

    int startVal, endVal;
    if (ind == 0) {
        startVal = endVal = 0;
    } else if (ind == maxind - 1) {
        startVal = endVal = totalLength;
    } else {
        int prevPos = -1;
        for (int i = ind - 1; i >= 0; i--) {
            if (currentMap[i] != -1) {
                prevPos = currentMap[i];
                break;
            }
        }
        if (prevPos == -1) {
            startVal = 1;
        } else {
            startVal = prevPos + 1;
        }
        endVal = totalLength - (maxind - ind - 1);
    }

    for (int pos = startVal; pos <= endVal && !*jest; pos++) {
        currentMap[ind] = pos;
        if (isValidPartialSolution(ind + 1)) {
            szukaj(ind + 1, jest);
        }
    }
    currentMap[ind] = -1; // Backtrack
}

std::optional<std::vector<int>> MapSolver::solve() {
    bool jest = false;
    startTime = std::chrono::steady_clock::now();

    std::fill(currentMap.begin(), currentMap.end(), -1);
    currentMap[0]         = 0;
    currentMap[maxind - 1]= totalLength;

    szukaj(1, &jest);

    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    stats.processedPaths = processedPaths;

    if (jest) {
        return stats.solution;
    }
    return std::nullopt;
}

std::optional<std::vector<int>> MapSolver::solveWithCondition() {
    bool jest = false;
    startTime = std::chrono::steady_clock::now();

    std::fill(currentMap.begin(), currentMap.end(), -1);
    currentMap[0]         = 0;
    currentMap[maxind - 1]= totalLength;

    initializeRemainingDistances();
    if (!updateDistanceUsage(totalLength, false)) {
        return std::nullopt;
    }

    szukajWithCondition(1, &jest);

    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs  = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();
    stats.processedPaths= processedPaths;

    if (jest) {
        return stats.solution;
    }
    return std::nullopt;
}

void MapSolver::szukajWithCondition(int ind, bool* jest) {
    if (*jest) return;
    processedPaths++;

    if (ind == maxind - 1) {
        updateProgress();
        bool allUsed = true;
        for (const auto& [distance, cnt] : remainingDistances) {
            if (cnt != 0) {
                allUsed = false;
                break;
            }
        }
        if (allUsed) {
            *jest = true;
            stats.solution      = currentMap;
            stats.solutionFound = true;
        }
        return;
    }

    int startVal = currentMap[ind - 1] + 1;
    int endVal   = totalLength - (maxind - ind - 1);

    for (int pos = startVal; pos <= endVal; pos++) {
        bool canPlace = true;
        std::vector<int> usedDistances;
        for (int j = 0; j < ind; j++) {
            if (currentMap[j] == -1) continue;
            int dist = std::abs(pos - currentMap[j]);
            if (!updateDistanceUsage(dist, false)) {
                canPlace = false;
                break;
            }
            usedDistances.push_back(dist);
        }
        if (canPlace) {
            int distToEnd = totalLength - pos;
            if (!updateDistanceUsage(distToEnd, false)) {
                canPlace = false;
            } else {
                usedDistances.push_back(distToEnd);
            }
        }

        if (canPlace) {
            currentMap[ind] = pos;
            szukajWithCondition(ind + 1, jest);
            if (*jest) return;
            currentMap[ind] = -1;
        }
        for (int d : usedDistances) {
            updateDistanceUsage(d, true);
        }
    }
}

bool MapSolver::updateDistanceUsage(int distance, bool add) {
    if (add) {
        remainingDistances[distance]++;
        return true;
    }
    auto it = remainingDistances.find(distance);
    if (it == remainingDistances.end() || it->second == 0) {
        return false;
    }
    it->second--;
    return true;
}

bool MapSolver::allDistancesUsedUp() const {
    for (const auto& [distance, count] : remainingDistances) {
        if (count != 0) {
            return false;
        }
    }
    return true;
}

std::vector<int> MapSolver::getUnusedDistances() const {
    std::vector<int> unused;
    for (const auto& [distance, count] : remainingDistances) {
        for (int i = 0; i < count; i++) {
            unused.push_back(distance);
        }
    }
    return unused;
}
