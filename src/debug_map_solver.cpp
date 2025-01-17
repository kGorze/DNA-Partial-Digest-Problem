#include "../include/debug_map_solver.h"
#include <algorithm>
#include <cmath>
#include <iomanip>

DebugMapSolver::DebugMapSolver(
    const std::vector<int>& inputDistances,
    int length,
    bool enableFileLog,
    const std::string& logPath
)
    : distances(inputDistances),
      totalLength(length),
      debugToFile(enableFileLog),
      debugDepth(0),
      maxReachedDepth(0)
{
    if (debugToFile) {
        logFile.open(logPath);
    }
    std::sort(distances.begin(), distances.end());
    maxind = static_cast<int>((1 + std::sqrt(1.0 + 8.0 * distances.size())) / 2);
    currentMap.resize(static_cast<size_t>(maxind), -1);

    for (int d : distances) {
        distanceCounter[d]++;
    }

    stats.totalPaths      = 0;
    stats.processedPaths  = 0;
    stats.searchTimeMs    = 0.0;
    stats.solutionFound   = false;
    stats.inputDistances  = inputDistances;

    if (debugToFile) {
        logFile << "Initial setup:\n";
        logFile << "Total length: " << totalLength << "\n";
        logFile << "Number of positions (maxind): " << maxind << "\n";
        logFile << "Input distances sorted:\n";
        for (int d : distances) {
            logFile << d << " ";
        }
        logFile << "\n\n";
    }
}

std::optional<std::vector<int>> DebugMapSolver::solve() {
    auto startTime = std::chrono::steady_clock::now();
    currentMap[0] = 0;
    if (maxind > 0) {
        currentMap[static_cast<size_t>(maxind) - 1] = totalLength;
    }

    bool foundSolution = false;
    searchSolver(1, foundSolution);

    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = static_cast<double>(
        std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count()
    );
    if (debugToFile) {
        logFile << "\nSearch completed in " << stats.searchTimeMs << "ms\n";
        logFile << "Max depth reached: " << maxReachedDepth << "\n";
        logFile << "Total invalidations: " << invalidationHistory.size() << "\n";
        if (stats.solutionFound) {
            logFile << "Solution found: ";
            for (int val : stats.solution) {
                logFile << val << " ";
            }
            logFile << "\n";
        } else {
            logFile << "No solution found.\n";
        }
    }
    if (stats.solutionFound) {
        return stats.solution;
    }
    return std::nullopt;
}

void DebugMapSolver::searchSolver(int ind, bool& foundSolution) {
    ++debugDepth;
    if (foundSolution) {
        --debugDepth;
        return;
    }
    if (ind == maxind) {
        if (isValidPartialSolution(maxind)) {
            foundSolution = true;
            stats.solution = currentMap;
            stats.solutionFound = true;
            if (debugToFile) {
                logFile << getIndentation() << "Valid final solution found.\n";
                dumpStateToFile("final_solution.txt");
            }
        }
        --debugDepth;
        return;
    }

    int startVal = 1;
    int endVal = totalLength - (maxind - ind - 1);

    if (ind == 0) {
        startVal = 0;
        endVal = 0;
    } else if (ind == maxind - 1) {
        startVal = totalLength;
        endVal = totalLength;
    } else {
        int prevPos = -1;
        for (int i = ind - 1; i >= 0; --i) {
            if (currentMap[static_cast<size_t>(i)] != -1) {
                prevPos = currentMap[static_cast<size_t>(i)];
                break;
            }
        }
        if (prevPos != -1) {
            startVal = prevPos + 1;
        }
    }

    if (startVal < 0) {
        startVal = 0;
    }
    if (endVal < startVal) {
        endVal = startVal;
    }

    for (int pos = startVal; pos <= endVal && !foundSolution; ++pos) {
        currentMap[static_cast<size_t>(ind)] = pos;
        analyzeSolutionAttempt(ind, pos);
        if (isValidPartialSolution(ind + 1)) {
            searchSolver(ind + 1, foundSolution);
        } else {
            analyzeFailure(ind, pos, "Invalid partial solution");
        }
    }
    currentMap[static_cast<size_t>(ind)] = -1;
    --debugDepth;
}

bool DebugMapSolver::isValidPartialSolution(int assignedCount) {
    if (assignedCount <= 1) return true;
    std::map<int, int> usedDistances;
    for (int i = 0; i < assignedCount; ++i) {
        if (currentMap[static_cast<size_t>(i)] == -1) {
            continue;
        }
        for (int j = 0; j < i; ++j) {
            if (currentMap[static_cast<size_t>(j)] == -1) {
                continue;
            }
            int distanceVal = std::abs(currentMap[static_cast<size_t>(i)] -
                                       currentMap[static_cast<size_t>(j)]);
            usedDistances[distanceVal]++;
            auto it = distanceCounter.find(distanceVal);
            if (it == distanceCounter.end() || usedDistances[distanceVal] > it->second) {
                return false;
            }
        }
    }
    return true;
}

void DebugMapSolver::analyzeSolutionAttempt(int ind, int pos) {
    if (!debugToFile) return;
    logFile << getIndentation() << "Trying position " << pos
            << " at index " << ind << "\n";
    logMapState("Current map state:");
    std::vector<int> newDistances;
    newDistances.reserve(static_cast<size_t>(ind));
    for (int i = 0; i < ind; ++i) {
        if (currentMap[static_cast<size_t>(i)] != -1) {
            newDistances.push_back(std::abs(pos - currentMap[static_cast<size_t>(i)]));
        }
    }
    logFile << getIndentation() << "New distances created: ";
    for (int d : newDistances) {
        logFile << d << " ";
    }
    logFile << "\n";
    logDistanceConstraints();
}

void DebugMapSolver::analyzeFailure(int ind, int pos, const std::string& reason) {
    if (!debugToFile) return;
    logFile << getIndentation() << "Failed to place " << pos
            << " at index " << ind << ": " << reason << "\n";
    auto conflicts = findConflictingPairs();
    if (!conflicts.empty()) {
        logFile << getIndentation() << "Conflicting pairs:\n";
        for (const auto& pair : conflicts) {
            logFile << getIndentation() << "  Indices " << pair.first << " and " << pair.second
                    << " (values: " << currentMap[static_cast<size_t>(pair.first)] << ", "
                    << currentMap[static_cast<size_t>(pair.second)] << ")\n";
        }
    }
    invalidationHistory.push_back({ind, currentMap});
}

std::string DebugMapSolver::getIndentation() const {
    return std::string(static_cast<size_t>(debugDepth) * 2, ' ');
}

void DebugMapSolver::logMapState(const std::string& message) {
    if (!debugToFile) return;
    logFile << getIndentation() << message << "\n";
    logFile << getIndentation() << "Map: ";
    for (int val : currentMap) {
        logFile << std::setw(3) << val << " ";
    }
    logFile << "\n";
}

void DebugMapSolver::logDistanceConstraints() {
    if (!debugToFile) return;
    logFile << getIndentation() << "Distance usage:\n";
    for (const auto& [distVal, count] : distanceCounter) {
        logFile << getIndentation() << "  " << distVal << ": " << count << "\n";
    }
}

std::vector<std::pair<int, int>> DebugMapSolver::findConflictingPairs() const {
    std::vector<std::pair<int, int>> conflicts;
    for (int i = 0; i < maxind; ++i) {
        if (currentMap[static_cast<size_t>(i)] == -1) continue;
        for (int j = 0; j < i; ++j) {
            if (currentMap[static_cast<size_t>(j)] == -1) continue;
            int distVal = std::abs(currentMap[static_cast<size_t>(i)] -
                                   currentMap[static_cast<size_t>(j)]);
            if (distanceCounter.find(distVal) == distanceCounter.end()) {
                conflicts.emplace_back(j, i);
            }
        }
    }
    return conflicts;
}

void DebugMapSolver::dumpStateToFile(const std::string& filename) const {
    if (!debugToFile) return;
    std::ofstream stateFile(filename);
    if (!stateFile.is_open()) return;

    stateFile << "Final map state:\n";
    for (int i = 0; i < maxind; ++i) {
        stateFile << i << ": " << currentMap[static_cast<size_t>(i)] << "\n";
    }
    stateFile << "\nDistance usage:\n";
    for (const auto& [distance, cnt] : distanceCounter) {
        stateFile << distance << ": " << cnt << "\n";
    }
    stateFile << "\nInvalidation history:\n";
    for (const auto& [pos, mp] : invalidationHistory) {
        stateFile << "At position " << pos << ": ";
        for (int val : mp) {
            stateFile << val << " ";
        }
        stateFile << "\n";
    }
}
