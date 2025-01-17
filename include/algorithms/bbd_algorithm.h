#ifndef BBD_ALGORITHM_H
#define BBD_ALGORITHM_H

#include <vector>
#include <optional>
#include <algorithm>
#include <cmath>


class BBdAlgorithm {
public:
    BBdAlgorithm() = default;
    std::optional<std::vector<int>> solve(std::vector<int> D);

private:
    struct Node {
        std::vector<int> D;
        std::vector<int> X;
        int level;
        Node(const std::vector<int>& d, const std::vector<int>& x, int lvl)
            : D(d), X(x), level(lvl) {}
    };

    std::vector<int> calculateDelta(int y, const std::vector<int>& X);
    bool isSubset(const std::vector<int>& subset, const std::vector<int>& set);
    std::vector<int> removeSubset(const std::vector<int>& set, const std::vector<int>& subset);

    std::optional<std::vector<int>> place(Node& current);
};

#endif // BBD_ALGORITHM_H
