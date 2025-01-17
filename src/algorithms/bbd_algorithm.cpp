#include "../../include/algorithms/bbd_algorithm.h"

std::optional<std::vector<int>> BBdAlgorithm::solve(std::vector<int> D) {
    if (D.empty()) return std::nullopt;

    std::sort(D.begin(), D.end(), std::greater<int>());
    int width = D.front();

    // X0 = {0, width}
    std::vector<int> X0 = {0, width};

    // Remove one occurrence of width
    auto it = std::find(D.begin(), D.end(), width);
    if (it != D.end()) {
        D.erase(it);
    }

    Node root(D, X0, 0);
    return place(root);
}

std::vector<int> BBdAlgorithm::calculateDelta(int y, const std::vector<int>& X) {
    std::vector<int> delta;
    delta.reserve(X.size());
    for (int x : X) {
        delta.push_back(std::abs(y - x));
    }
    return delta;
}

bool BBdAlgorithm::isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
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
        return current.X;
    }
    int y = *std::max_element(current.D.begin(), current.D.end());
    int width = current.X.back();

    // Try y
    auto deltaY = calculateDelta(y, current.X);
    if (isSubset(deltaY, current.D)) {
        std::vector<int> newX = current.X;
        newX.push_back(y);
        std::sort(newX.begin(), newX.end());
        std::vector<int> newD = removeSubset(current.D, deltaY);

        Node next(newD, newX, current.level + 1);
        if (auto result = place(next)) {
            return result;
        }
    }

    // Try (width - y) if different
    int complement = width - y;
    if (complement != y) {
        auto deltaC = calculateDelta(complement, current.X);
        if (isSubset(deltaC, current.D)) {
            std::vector<int> newX2 = current.X;
            newX2.push_back(complement);
            std::sort(newX2.begin(), newX2.end());
            std::vector<int> newD2 = removeSubset(current.D, deltaC);

            Node next2(newD2, newX2, current.level + 1);
            if (auto result2 = place(next2)) {
                return result2;
            }
        }
    }
    return std::nullopt;
}
