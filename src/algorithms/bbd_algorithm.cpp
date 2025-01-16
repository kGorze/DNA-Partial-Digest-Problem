//
// Created by konrad_guest on 15/01/2025.
//

#include "../../include/algorithms/bbd_algorithm.h"

// Główna metoda solve
std::optional<std::vector<int>> BBdAlgorithm::solve(std::vector<int> D) {
    // Jeśli D jest puste, nie ma czego rozwiązywać
    if (D.empty()) return std::nullopt;

    // **NOWOŚĆ**: sortujemy D malejąco, tak by D[0] = width (największa odległość)
    std::sort(D.begin(), D.end(), std::greater<int>());

    // width to pierwszy (największy) element
    int width = D.front();

    // Początkowe X: {0, width}
    std::vector<int> initialX = {0, width};

    // Usuwamy jednorazowo width z D (ponieważ odcinek [0, width] już mamy w X)
    auto it = std::find(D.begin(), D.end(), width);
    if (it != D.end()) {
        D.erase(it);
    }

    // Tworzymy węzeł początkowy i odpalamy rekurencyjną funkcję place (DFS)
    Node root(D, initialX, 0);
    return place(root);
}

// Obliczenie różnic między punktem y a wszystkimi punktami w X
std::vector<int> BBdAlgorithm::calculateDelta(int y, const std::vector<int>& X) {
    std::vector<int> delta;
    delta.reserve(X.size());
    for (int x : X) {
        delta.push_back(std::abs(y - x));
    }
    return delta;
}

// Sprawdzenie, czy wszystkie elementy "subset" znajdują się w "set" z zachowaniem wielokrotności
bool BBdAlgorithm::isSubset(const std::vector<int>& subset, const std::vector<int>& set) {
    std::vector<int> temp = set;
    for (int element : subset) {
        auto it = std::find(temp.begin(), temp.end(), element);
        if (it == temp.end()) {
            return false; // elementu nie znaleziono
        }
        // usuwamy tylko jedno wystąpienie
        temp.erase(it);
    }
    return true;
}

// Usunięcie elementów "subset" ze zbioru "set" (również z zachowaniem wielokrotności)
std::vector<int> BBdAlgorithm::removeSubset(const std::vector<int>& set, const std::vector<int>& subset) {
    std::vector<int> result = set;
    for (int element : subset) {
        auto it = std::find(result.begin(), result.end(), element);
        if (it != result.end()) {
            result.erase(it);
        }
        // jeśli z jakiegoś powodu brakuje elementu w result, 
        // (np. błąd w isSubset) to po prostu go nie usuwamy
    }
    return result;
}

// Główna funkcja DFS, próbująca umieścić maks. element lub jego "odbicie" (width - element).
std::optional<std::vector<int>> BBdAlgorithm::place(Node& current) {
    // Jeśli D puste, to znaczy, że wszystkie odległości są "wyjaśnione" -> mamy rozwiązanie
    if (current.D.empty()) {
        return current.X;
    }

    // Pobierz maksymalny element z D
    int y = *std::max_element(current.D.begin(), current.D.end());

    // Największy punkt w X traktujemy jako width
    int width = current.X.back(); 
    // Zakładamy, że X jest posortowane rosnąco, więc X.back() = width

    // Spróbuj dodać y do X
    {
        std::vector<int> deltaY = calculateDelta(y, current.X);
        if (isSubset(deltaY, current.D)) {
            std::vector<int> newX = current.X;
            newX.push_back(y);
            std::sort(newX.begin(), newX.end()); // trzymamy X w porządku rosnącym

            std::vector<int> newD = removeSubset(current.D, deltaY);
            Node next(newD, newX, current.level + 1);

            auto result = place(next);
            if (result) return result; // Jeśli udało się rozwiązać -> zwracamy
        }
    }

    // Spróbuj dodać (width - y), jeśli to nie to samo co y 
    // (bo gdy y == width-y, to próbujemy 2x ten sam punkt).
    int complementY = width - y;
    if (complementY != y) {
        std::vector<int> deltaComplement = calculateDelta(complementY, current.X);
        if (isSubset(deltaComplement, current.D)) {
            std::vector<int> newX = current.X;
            newX.push_back(complementY);
            std::sort(newX.begin(), newX.end());

            std::vector<int> newD = removeSubset(current.D, deltaComplement);
            Node next(newD, newX, current.level + 1);

            auto result = place(next);
            if (result) return result;
        }
    }

    // Jeśli obie próby (y i width-y) się nie powiodły, nie ma rozwiązania w tej gałęzi
    return std::nullopt;
}