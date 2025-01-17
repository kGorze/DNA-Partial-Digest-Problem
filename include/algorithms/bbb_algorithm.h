#ifndef BBB_ALGORITHM_H
#define BBB_ALGORITHM_H

#include <vector>
#include <optional>
#include <algorithm>
#include <set>

class BBbAlgorithm {
public:
    BBbAlgorithm() = default;
    std::optional<std::vector<int>> solve(std::vector<int> D);
    std::optional<std::vector<int>> solvePartial(const std::vector<int>& partialX,
                                                 std::vector<int> leftoverD);
private:
    std::vector<int> calculateDelta(int y, const std::vector<int>& X);
    bool isSubset(const std::vector<int>& subset, const std::vector<int>& set);
    std::vector<int> removeSubset(const std::vector<int>& set, const std::vector<int>& subset);
    void generateNextLevel(std::vector<std::vector<int>>& LD,
                           std::vector<std::vector<int>>& LX);
    void generateNextLevelPartial(std::vector<std::vector<int>>& LD,
                                  std::vector<std::vector<int>>& LX,
                                  int width);
};

#endif //BBB_ALGORITHM_H
