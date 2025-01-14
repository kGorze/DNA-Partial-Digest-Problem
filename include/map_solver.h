//
// Created by konrad_guest on 14/01/2025.
//

#ifndef MAP_SOLVER_H
#define MAP_SOLVER_H

#include <vector>

class MapSolver {
private:
    std::vector<int> distances;    // Wejściowy multiset odległości
    std::vector<int> currentMap;   // Aktualnie konstruowana mapa
    int totalLength;               // Całkowita długość mapy
    int maxind;                    // Maksymalny indeks (liczba miejsc cięć + 2)
    
    // Metody prywatne
    bool isValidPartialSolution() const;
    void szukaj(int ind, bool* jest);

public:
    // Konstruktor
    MapSolver(const std::vector<int>& inputDistances, int length);
    
    // Główna metoda rozwiązująca
    bool solve();
    
    // Getter dla rozwiązania
    const std::vector<int>& getSolution() const;
    
    // Pomocnicze metody publiczne (opcjonalne)
    int getNumberOfCuts() const { return maxind - 2; }
    int getTotalLength() const { return totalLength; }
};

#endif //MAP_SOLVER_H
