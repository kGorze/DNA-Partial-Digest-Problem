//
// Created by konrad_guest on 14/01/2025.
//

#include <algorithm>
#include <cmath>
#include <iostream>

#include "../include/map_solver.h"

MapSolver::MapSolver(const std::vector<int>& inputDistances, int length) 
    : distances(inputDistances), totalLength(length) {
    // Oblicz liczbę miejsc cięć na podstawie rozmiaru multiset
    // Wzór: n = (-1 + sqrt(1 + 8m))/2, gdzie m to rozmiar multiset
    maxind = (-1 + std::sqrt(1 + 8 * distances.size())) / 2;
    currentMap.resize(maxind);
    
    // Sortuj odległości dla łatwiejszego porównywania
    std::sort(distances.begin(), distances.end());
}

bool MapSolver::isValidPartialSolution() const {
    std::vector<int> currentDistances;
    
    // Generuj wszystkie odległości z aktualnej częściowej mapy
    for (size_t i = 0; i < currentMap.size(); i++) {
        for (size_t j = i + 1; j < currentMap.size(); j++) {
            int distance = currentMap[j] - currentMap[i];
            // Sprawdź, czy odległość nie przekracza całkowitej długości
            if (distance > totalLength) return false;
            currentDistances.push_back(distance);
        }
    }
    
    // Sortuj wygenerowane odległości
    std::sort(currentDistances.begin(), currentDistances.end());
    
    // Sprawdź, czy wszystkie wygenerowane odległości występują w multiset distances
    std::vector<int> tempDistances = distances;
    for (size_t i = 0; i < currentDistances.size(); i++) {
        auto it = std::find(tempDistances.begin(), tempDistances.end(), 
                           currentDistances[i]);
        if (it == tempDistances.end()) {
            return false;
        }
        tempDistances.erase(it);
    }
    
    return true;
}

void MapSolver::szukaj(int ind, bool* jest) {
    // Jeśli już znaleziono rozwiązanie, zakończ
    if (*jest) return;
    
    // Warunek zakończenia rekurencji
    if (ind == maxind) {
        // Sprawdź, czy znalezione rozwiązanie jest poprawne
        if (isValidPartialSolution()) {
            *jest = true;
            std::cout << "Znaleziono rozwiązanie:\n";
            for (int site : currentMap) {
                std::cout << site << " ";
            }
            std::cout << "\n";
        }
        return;
    }
    
    // Określ zakres poszukiwań dla nowego miejsca cięcia
    int start, end;
    
    if (ind == 0) {
        // Pierwszy element musi być 0
        start = end = 0;
    } else if (ind == maxind - 1) {
        // Ostatni element musi być totalLength
        start = end = totalLength;
    } else {
        // Dla pozostałych elementów
        start = currentMap[ind - 1] + 1;  // Musi być większe niż poprzedni
        // Nie może być za duże, by pozostawić miejsce na pozostałe punkty
        end = totalLength - (maxind - ind - 1);
    }
    
    // Przeszukaj możliwe pozycje dla bieżącego punktu
    for (int pos = start; pos <= end; pos++) {
        currentMap[ind] = pos;
        
        // Sprawdź, czy warto kontynuować z tym częściowym rozwiązaniem
        if (isValidPartialSolution()) {
            szukaj(ind + 1, jest);
        }
        
        // Jeśli znaleziono rozwiązanie, przerwij pętlę
        if (*jest) break;
    }
}

bool MapSolver::solve() {
    // Zainicjuj zmienną kontrolną
    bool jest = false;
    
    // Rozpocznij rekurencyjne przeszukiwanie
    szukaj(0, &jest);
    
    // Zwróć informację, czy znaleziono rozwiązanie
    return jest;
}

const std::vector<int>& MapSolver::getSolution() const {
    return currentMap;
}