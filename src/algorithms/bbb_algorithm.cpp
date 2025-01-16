#include "../../include/algorithms/bbb_algorithm.h"
#include <queue>
#include <functional>
#include <iostream>

// ---------------------------------------------------------
// Oryginalna metoda: solve(D), zawsze startująca od X0 = {0, max(D)}
// ---------------------------------------------------------
std::optional<std::vector<int>> BBbAlgorithm::solve(std::vector<int> D) {
    if (D.empty()) return std::nullopt;

    // width = max(D)
    int width = *std::max_element(D.begin(), D.end());

    // X0 = {0, width}
    std::vector<int> X0 = {0, width};

    // Usuwamy width z D (jedno wystąpienie)
    auto it = std::find(D.begin(), D.end(), width);
    if (it != D.end()) {
        D.erase(it);
    }

    // BFS
    std::vector<std::vector<int>> LD = {D};
    std::vector<std::vector<int>> LX = {X0};

    while (!LD.empty()) {
        generateNextLevel(LD, LX);

        // Sprawdzamy, czy któraś ścieżka już zużyła wszystkie odległości
        for (size_t i = 0; i < LD.size(); ++i) {
            if (LD[i].empty()) {
                return LX[i];  // gotowe rozwiązanie
            }
        }
    }
    return std::nullopt; // brak rozwiązania
}

// ---------------------------------------------------------
// NOWA METODA: solvePartial(...) 
// Kontynuacja budowania X z already partialX i leftoverD 
// ---------------------------------------------------------
std::optional<std::vector<int>> BBbAlgorithm::solvePartial(const std::vector<int>& partialX,
                                                           std::vector<int> leftoverD)
{
    // Jeśli leftoverD puste, to partialX jest już pełnym rozwiązaniem
    if (leftoverD.empty()) {
        return partialX;
    }

    // width to partialX.back() (zakładamy, że partialX jest posortowane i
    //  największy element to szerokość)
    int width = partialX.back();

    // Inicjujemy BFS:
    //  - LD: listę multizbiorów odległości, zaczynamy od leftoverD
    //  - LX: listę potencjalnych X, zaczynamy od partialX
    std::vector<std::vector<int>> LD = { leftoverD };
    std::vector<std::vector<int>> LX = { partialX };

    // Analogicznie do solve, ale używamy generateNextLevelPartial
    while (!LD.empty()) {
        generateNextLevelPartial(LD, LX, width);

        for (size_t i = 0; i < LD.size(); ++i) {
            if (LD[i].empty()) {
                return LX[i]; // rozwiązanie
            }
        }
    }

    return std::nullopt;
}

// ---------------------------------------------------------
// Funkcje pomocnicze 
// ---------------------------------------------------------
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
        if (it == temp.end()) return false;
        temp.erase(it);
    }
    return true;
}

std::vector<int> BBbAlgorithm::removeSubset(const std::vector<int>& set, const std::vector<int>& subset) {
    std::vector<int> result = set;
    for (int element : subset) {
        auto it = std::find(result.begin(), result.end(), element);
        if (it != result.end()) {
            result.erase(it);
        }
    }
    return result;
}

// ---------------------------------------------------------
// generateNextLevel - oryginalne dla solve(...)
// ---------------------------------------------------------
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
        int width = LX[i].back(); // zakładamy, że LX[i] jest posortowane, a LX[i].back() = width

        // 1) Spróbuj y
        auto deltaY = calculateDelta(y, LX[i]);
        if (isSubset(deltaY, LD[i])) {
            std::vector<int> newX = LX[i];
            newX.push_back(y);
            std::sort(newX.begin(), newX.end());
            if (uniqueX.find(newX) == uniqueX.end()) {
                uniqueX.insert(newX);
                std::vector<int> newD = removeSubset(LD[i], deltaY);
                newLD.push_back(newD);
                newLX.push_back(newX);
            }
        }

        // 2) Spróbuj (width - y)
        int complementY = width - y;
        if (complementY != y) {
            auto deltaC = calculateDelta(complementY, LX[i]);
            if (isSubset(deltaC, LD[i])) {
                std::vector<int> newX2 = LX[i];
                newX2.push_back(complementY);
                std::sort(newX2.begin(), newX2.end());
                if (uniqueX.find(newX2) == uniqueX.end()) {
                    uniqueX.insert(newX2);
                    std::vector<int> newD2 = removeSubset(LD[i], deltaC);
                    newLD.push_back(newD2);
                    newLX.push_back(newX2);
                }
            }
        }
    }

    LD = std::move(newLD);
    LX = std::move(newLX);
}

// ---------------------------------------------------------
// NOWA: generateNextLevelPartial - BFS z uwzględnieniem partialX i stałego width
// ---------------------------------------------------------
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

        // 1) Wypróbuj y
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

        // 2) Wypróbuj width - y (o ile nie równe y)
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
