#include "../../include/algorithms/bbb_algorithm.h"
#include <queue>
#include <functional>
#include <iostream>

std::optional<std::vector<int>> BBbAlgorithm::solve(std::vector<int> D) {
    if (D.empty()) return std::nullopt;
    int width = *std::max_element(D.begin(), D.end());
    auto it = std::find(D.begin(), D.end(), width);
    if (it != D.end()) {
        D.erase(it);
    }
    std::vector<int> X0 = {0, width};

    std::vector<std::vector<int>> LD = {D};
    std::vector<std::vector<int>> LX = {X0};

    while (!LD.empty()) {
        generateNextLevel(LD, LX);
        for (size_t i = 0; i < LD.size(); ++i) {
            if (LD[i].empty()) {
                return LX[i];
            }
        }
    }
    return std::nullopt;
}

std::optional<std::vector<int>> BBbAlgorithm::solvePartial(const std::vector<int>& partialX,
                                                           std::vector<int> leftoverD)
{
    if (leftoverD.empty()) {
        return partialX;
    }
    int width = partialX.back();
    std::vector<std::vector<int>> LD = { leftoverD };
    std::vector<std::vector<int>> LX = { partialX };

    while (!LD.empty()) {
        generateNextLevelPartial(LD, LX, width);
        for (size_t i = 0; i < LD.size(); ++i) {
            if (LD[i].empty()) {
                return LX[i];
            }
        }
    }
    return std::nullopt;
}

std::vector<int> BBbAlgorithm::calculateDelta(int y, const std::vector<int>& X) {
    std::vector<int> delta;
    delta.reserve(X.size());
    for (int x : X) {
        delta.push_back(std::abs(y - x));
    }
    return delta;
}

bool BBbAlgorithm::isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
    std::vector<int> temp = set;
    for (int element : subset) {
        auto it = std::find(temp.begin(), temp.end(), element);
        if (it == temp.end()) {
            return false;
        }
        temp.erase(it);
    }
    return true;
}

std::vector<int> BBbAlgorithm::removeSubset(const std::vector<int>& set,
                                            const std::vector<int>& subset)
{
    std::vector<int> result = set;
    for (int element : subset) {
        auto it = std::find(result.begin(), result.end(), element);
        if (it != result.end()) {
            result.erase(it);
        }
    }
    return result;
}

void BBbAlgorithm::generateNextLevel(std::vector<std::vector<int>>& LD,
                                     std::vector<std::vector<int>>& LX)
{
    std::vector<std::vector<int>> newLD;
    std::vector<std::vector<int>> newLX;
    std::set<std::vector<int>> uniqueX;

    for (size_t i = 0; i < LD.size(); ++i) {
        if (LD[i].empty()) {
            continue;
        }
        int y = *std::max_element(LD[i].begin(), LD[i].end());
        int width = LX[i].back();

        auto deltaY = calculateDelta(y, LX[i]);
        if (isSubset(deltaY, LD[i])) {
            std::vector<int> newX = LX[i];
            newX.push_back(y);
            std::sort(newX.begin(), newX.end());
            if (uniqueX.find(newX) == uniqueX.end()) {
                uniqueX.insert(newX);
                auto newD = removeSubset(LD[i], deltaY);
                newLD.push_back(newD);
                newLX.push_back(newX);
            }
        }

        int complementY = width - y;
        if (complementY != y) {
            auto deltaC = calculateDelta(complementY, LX[i]);
            if (isSubset(deltaC, LD[i])) {
                std::vector<int> newX2 = LX[i];
                newX2.push_back(complementY);
                std::sort(newX2.begin(), newX2.end());
                if (uniqueX.find(newX2) == uniqueX.end()) {
                    uniqueX.insert(newX2);
                    auto newD2 = removeSubset(LD[i], deltaC);
                    newLD.push_back(newD2);
                    newLX.push_back(newX2);
                }
            }
        }
    }
    LD = std::move(newLD);
    LX = std::move(newLX);
}

void BBbAlgorithm::generateNextLevelPartial(std::vector<std::vector<int>>& LD,
                                            std::vector<std::vector<int>>& LX,
                                            int width)
{
    std::vector<std::vector<int>> newLD;
    std::vector<std::vector<int>> newLX;
    std::set<std::vector<int>> uniqueX;

    for (size_t i = 0; i < LD.size(); i++) {
        if (LD[i].empty()) {
            continue;
        }
        int y = *std::max_element(LD[i].begin(), LD[i].end());

        auto deltaY = calculateDelta(y, LX[i]);
        if (isSubset(deltaY, LD[i])) {
            std::vector<int> newX = LX[i];
            newX.push_back(y);
            std::sort(newX.begin(), newX.end());
            if (uniqueX.find(newX) == uniqueX.end()) {
                uniqueX.insert(newX);
                auto newD = removeSubset(LD[i], deltaY);
                newLD.push_back(newD);
                newLX.push_back(newX);
            }
        }

        int complement = width - y;
        if (complement != y) {
            auto deltaC = calculateDelta(complement, LX[i]);
            if (isSubset(deltaC, LD[i])) {
                std::vector<int> newX2 = LX[i];
                newX2.push_back(complement);
                std::sort(newX2.begin(), newX2.end());
                if (uniqueX.find(newX2) == uniqueX.end()) {
                    uniqueX.insert(newX2);
                    auto newD2 = removeSubset(LD[i], deltaC);
                    newLD.push_back(newD2);
                    newLX.push_back(newX2);
                }
            }
        }
    }
    LD = std::move(newLD);
    LX = std::move(newLX);
}
