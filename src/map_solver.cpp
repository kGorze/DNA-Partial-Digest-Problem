#include "../include/map_solver.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iomanip>

MapSolver::MapSolver(const std::vector<int>& inputDistances, int length)
    : distances(inputDistances), totalLength(length), processedPaths(0)
{
    std::sort(distances.begin(), distances.end());
    double sizeForMaxInd = 1.0 + std::sqrt(1.0 + 8.0 * static_cast<double>(distances.size()));
    maxind = static_cast<int>(sizeForMaxInd / 2);

    currentMap.resize(static_cast<size_t>(maxind), -1);
    totalPaths = calculateTotalPaths();

    for (int d : distances) {
        distanceCounter[d]++;
    }

    stats.totalPaths      = totalPaths;
    stats.processedPaths  = 0;
    stats.searchTimeMs    = 0.0;
    stats.solutionFound   = false;
    stats.solution        = distances;
    stats.inputDistances  = distances;
}

uint64_t MapSolver::calculateTotalPaths() const {
    // Very rough upper-bound calculation
    uint64_t total = 1;
    for (int i = 1; i < maxind - 1; i++) {
        uint64_t range = static_cast<uint64_t>(totalLength > i ? (totalLength - i) : 1);
        total *= range;
        if (total == 0) {
            break;
        }
    }
    return total;
}

bool MapSolver::isValidPartialSolution(int assignedCount) const {
    if (assignedCount <= 1) return true;
    std::map<int, int> usedDistances;
    for (int i = 0; i < assignedCount; ++i) {
        if (currentMap[static_cast<size_t>(i)] < 0) {
            continue;
        }
        for (int j = 0; j < i; ++j) {
            if (currentMap[static_cast<size_t>(j)] < 0) {
                continue;
            }
            int distVal = std::abs(currentMap[static_cast<size_t>(i)] -
                                   currentMap[static_cast<size_t>(j)]);
            auto it = distanceCounter.find(distVal);
            if (it == distanceCounter.end()) {
                return false;
            }
            usedDistances[distVal]++;
            if (usedDistances[distVal] > it->second) {
                return false;
            }
        }
    }
    return true;
}

void MapSolver::searchSolver(int ind, bool& foundSolution) {
    if (foundSolution) {
        return;
    }
    ++processedPaths;
    if (ind == maxind) {
        if (isValidPartialSolution(maxind)) {
            foundSolution = true;
            stats.solution = currentMap;
            stats.solutionFound = true;
        }
        updateProgress();
        return;
    }

    int startVal = (ind == 0) ? 0 : 1;
    int endVal = (ind == maxind - 1) ? totalLength : (totalLength - (maxind - ind - 1));

    if (startVal < 0) {
        startVal = 0;
    }
    if (endVal < startVal) {
        endVal = startVal;
    }

    for (int pos = startVal; pos <= endVal && !foundSolution; ++pos) {
        currentMap[static_cast<size_t>(ind)] = pos;
        if (isValidPartialSolution(ind + 1)) {
            searchSolver(ind + 1, foundSolution);
        }
    }
    currentMap[static_cast<size_t>(ind)] = -1;
}

std::optional<std::vector<int>> MapSolver::solve() {
    bool foundSolution = false;
    startTime = std::chrono::steady_clock::now();
    std::fill(currentMap.begin(), currentMap.end(), -1);
    if (!currentMap.empty()) {
        currentMap.front() = 0;
        currentMap.back() = totalLength;
    }

    searchSolver(1, foundSolution);

    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
    );
    stats.processedPaths = processedPaths;

    if (foundSolution) {
        return stats.solution;
    }
    return std::nullopt;
}

void MapSolver::initializeRemainingDistances() {
    remainingDistances.clear();
    for (int d : distances) {
        remainingDistances[d]++;
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

void MapSolver::updateProgress() {
    if (processedPaths % 1000 == 0) {
        displayProgress();
    }
}

void MapSolver::displayProgress() const {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime).count();
    std::cout << "\rPaths processed: " << processedPaths << " | Time: " << elapsedMs << "ms" << std::flush;
}

std::optional<std::vector<int>> MapSolver::solveWithCondition() {
    bool foundSolution = false;
    startTime = std::chrono::steady_clock::now();
    std::fill(currentMap.begin(), currentMap.end(), -1);
    if (!currentMap.empty()) {
        currentMap.front() = 0;
        currentMap.back() = totalLength;
    }
    initializeRemainingDistances();
    if (!updateDistanceUsage(totalLength, false)) {
        return std::nullopt;
    }
    searchSolverWithCondition(1, foundSolution);
    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
    );
    stats.processedPaths = processedPaths;

    if (foundSolution) {
        return stats.solution;
    }
    return std::nullopt;
}

void MapSolver::searchSolverWithCondition(int ind, bool& foundSolution) {
    if (foundSolution) {
        return;
    }
    ++processedPaths;
    if (ind == maxind - 1) {
        updateProgress();
        bool allUsed = true;
        for (const auto& kv : remainingDistances) {
            if (kv.second != 0) {
                allUsed = false;
                break;
            }
        }
        if (allUsed) {
            foundSolution = true;
            stats.solution = currentMap;
            stats.solutionFound = true;
        }
        return;
    }

    int startVal = 1;
    int endVal = totalLength - (maxind - ind - 1);

    for (int pos = startVal; pos <= endVal; ++pos) {
        bool canPlace = true;
        std::vector<int> usedValues;
        usedValues.reserve(static_cast<size_t>(ind));

        for (int j = 0; j < ind; ++j) {
            if (currentMap[static_cast<size_t>(j)] < 0) {
                continue;
            }
            int distVal = std::abs(pos - currentMap[static_cast<size_t>(j)]);
            if (!updateDistanceUsage(distVal, false)) {
                canPlace = false;
                break;
            }
            usedValues.push_back(distVal);
        }
        int distToEnd = totalLength - pos;
        if (canPlace && !updateDistanceUsage(distToEnd, false)) {
            canPlace = false;
        } else if (canPlace) {
            usedValues.push_back(distToEnd);
        }

        if (canPlace) {
            currentMap[static_cast<size_t>(ind)] = pos;
            searchSolverWithCondition(ind + 1, foundSolution);
            if (!foundSolution) {
                currentMap[static_cast<size_t>(ind)] = -1;
            } else {
                break;
            }
        }
        for (int val : usedValues) {
            updateDistanceUsage(val, true);
        }
    }
}
