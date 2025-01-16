#ifndef BBB_ALGORITHM_H
#define BBB_ALGORITHM_H

#include <vector>
#include <optional>
#include <algorithm>
#include <set>

/**
 * BBbAlgorithm - BFS + usuwanie duplikatów.
 *
 * 1) solve(std::vector<int> D)  - dotychczasowa metoda, która zawsze startuje
 *    od X0 = {0, max(D)} i usuwa width z D;
 *
 * 2) solvePartial(const std::vector<int>& partialX, std::vector<int> leftoverD)  - NOWA METODA,
 *    pozwala kontynuować budowanie X, mając już pewien fragment partialX
 *    (np. {0, width} + jakieś dodatkowe punkty), oraz leftoverD (pozostałe
 *    jeszcze niewyjaśnione odległości).
 */
class BBbAlgorithm {
public:
    BBbAlgorithm() = default;

    // Oryginalna metoda, startująca od zera (X0 = {0, max(D)})
    std::optional<std::vector<int>> solve(std::vector<int> D);

    // NOWA METODA: kontynuuje od partialX i leftoverD (nie zdejmuje width = max(D))
    std::optional<std::vector<int>> solvePartial(const std::vector<int>& partialX,
                                                 std::vector<int> leftoverD);

private:
    // Pomocnicze funkcje do obliczania i usuwania różnic
    std::vector<int> calculateDelta(int y, const std::vector<int>& X);
    bool isSubset(const std::vector<int>& subset, const std::vector<int>& set);
    std::vector<int> removeSubset(const std::vector<int>& set, const std::vector<int>& subset);

    // Główne generowanie kolejnego poziomu w BFS (używane w solve)
    void generateNextLevel(std::vector<std::vector<int>>& LD, 
                           std::vector<std::vector<int>>& LX);

    // NOWA FUNKCJA: generowanie kolejnego poziomu, z założeniem stałego width 
    // (z partialX.back()) i bez usuwania max(D) na starcie.
    void generateNextLevelPartial(std::vector<std::vector<int>>& LD,
                                  std::vector<std::vector<int>>& LX,
                                  int width);
};

#endif //BBB_ALGORITHM_H
