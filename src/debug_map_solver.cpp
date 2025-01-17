#include "../include/debug_map_solver.h"
#include <algorithm>
#include <cmath>
#include <iomanip>

DebugMapSolver::DebugMapSolver(const std::vector<int>& inputDistances, int length,
                               bool enableFileLog, const std::string& logPath)
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
    maxind = static_cast<int>((1 + std::sqrt(1 + 8.0 * distances.size())) / 2);
    currentMap.resize(maxind, -1);

    for (int d : distances) {
        distanceCounter[d]++;
    }

    if (debugToFile) {
        logFile << "Initial setup:\n";
        logFile << "Total length: " << totalLength << "\n";
        logFile << "Number of positions (maxind): " << maxind << "\n";
        logFile << "Input distances: ";
        for (int d : distances) {
            logFile << d << " ";
        }
        logFile << "\n\n";
    }

    stats.totalPaths       = 0;
    stats.processedPaths   = 0;
    stats.searchTimeMs     = 0.0;
    stats.solutionFound    = false;
    stats.solution         = {};
    stats.inputDistances   = inputDistances;
}

bool DebugMapSolver::isValidPartialSolution(int assignedCount) {
    if (assignedCount <= 1) return true;

    if (debugToFile) {
        std::stringstream ss;
        ss << getIndentation() << "Checking validity for first " << assignedCount << " positions\n";
        logFile << ss.str();
    }

    std::map<int, int> usedDistances;
    for (int i = 0; i < assignedCount; i++) {
        if (currentMap[i] == -1) continue;
        for (int j = 0; j < i; j++) {
            if (currentMap[j] == -1) continue;
            int distance = std::abs(currentMap[i] - currentMap[j]);
            usedDistances[distance]++;
            auto it = distanceCounter.find(distance);
            if (it == distanceCounter.end()) {
                if (debugToFile) {
                    logFile << getIndentation() << "Invalid distance " << distance << "\n";
                }
                return false;
            }
            if (usedDistances[distance] > it->second) {
                if (debugToFile) {
                    logFile << getIndentation() << "Distance " << distance 
                            << " used too many times.\n";
                }
                return false;
            }
        }
    }
    if (debugToFile) {
        logFile << getIndentation() << "Valid partial solution up to position " << assignedCount << "\n";
    }
    return true;
}

void DebugMapSolver::szukaj(int ind, bool* jest) {
    debugDepth++;
    maxReachedDepth = std::max(maxReachedDepth, debugDepth);

    if (*jest) {
        debugDepth--;
        return;
    }
    if (ind == maxind) {
        if (debugToFile) {
            logFile << getIndentation() << "Reached final position. Checking full solution...\n";
        }
        if (isValidPartialSolution(maxind)) {
            *jest = true;
            stats.solution      = currentMap;
            stats.solutionFound = true;
            if (debugToFile) {
                logFile << getIndentation() << "Valid solution found!\n";
                dumpStateToFile("final_solution.txt");
            }
        } else {
            if (debugToFile) {
                logFile << getIndentation() << "Invalid final solution.\n";
            }
        }
        debugDepth--;
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
        startVal = (prevPos == -1) ? 1 : prevPos + 1;
        endVal   = totalLength - (maxind - ind - 1);
    }

    if (debugToFile) {
        logFile << getIndentation() << "Searching position " << ind
                << " (range: " << startVal << " to " << endVal << ")\n";
    }

    for (int pos = startVal; pos <= endVal && !*jest; pos++) {
        currentMap[ind] = pos;
        analyzeSolutionAttempt(ind, pos);

        if (isValidPartialSolution(ind + 1)) {
            szukaj(ind + 1, jest);
        } else {
            analyzeFailure(ind, pos, "Invalid partial solution");
        }
    }
    if (!*jest && debugToFile) {
        logFile << getIndentation() << "Backtracking from position " << ind << "\n";
    }

    currentMap[ind] = -1;
    debugDepth--;
}

std::optional<std::vector<int>> DebugMapSolver::solve() {
    auto startTime = std::chrono::steady_clock::now();
    bool jest      = false;
    currentMap[0]  = 0;
    currentMap[maxind - 1] = totalLength;

    if (debugToFile) {
        logFile << "Starting search with initial map:\n";
        logMapState("Initial state");
    }

    szukaj(1, &jest);

    auto endTime = std::chrono::steady_clock::now();
    stats.searchTimeMs = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count();

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
    return (jest ? std::optional<std::vector<int>>(stats.solution) : std::nullopt);
}

void DebugMapSolver::analyzeSolutionAttempt(int ind, int pos) {
    if (!debugToFile) return;
    logFile << getIndentation() << "Trying position " << pos << " at index " << ind << "\n";
    logMapState("Current map state:");

    std::vector<int> newDistances;
    for (int i = 0; i < ind; i++) {
        if (currentMap[i] != -1) {
            newDistances.push_back(std::abs(pos - currentMap[i]));
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
        logFile << getIndentation() << "Conflicting pairs found:\n";
        for (const auto& pair : conflicts) {
            logFile << getIndentation() << "  Positions " << pair.first
                    << " and " << pair.second << " (values: "
                    << currentMap[pair.first] << ", " << currentMap[pair.second] << ")\n";
        }
    }
    invalidationHistory.push_back({ind, currentMap});
}

std::string DebugMapSolver::getIndentation() const {
    return std::string(debugDepth * 2, ' ');
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
    logFile << getIndentation() << "Current distance usage:\n";
    for (const auto& [distance, count] : distanceCounter) {
        logFile << getIndentation() << "  Distance " << distance 
                << ": available " << count << " times\n";
    }
}

std::vector<std::pair<int, int>> DebugMapSolver::findConflictingPairs() const {
    std::vector<std::pair<int, int>> conflicts;
    for (int i = 0; i < maxind; i++) {
        if (currentMap[i] == -1) continue;
        for (int j = 0; j < i; j++) {
            if (currentMap[j] == -1) continue;
            int dist = std::abs(currentMap[i] - currentMap[j]);
            if (distanceCounter.find(dist) == distanceCounter.end()) {
                conflicts.push_back({j, i});
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
    for (int i = 0; i < maxind; i++) {
        stateFile << i << ": " << currentMap[i] << "\n";
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
    stateFile.close();
}
