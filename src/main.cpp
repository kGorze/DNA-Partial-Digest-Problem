#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <string>
#include <sstream>
#include <ctime>
#include <map>

#include "../include/instance_generator.h"
#include "../include/test_framework.h"
#include "../include/map_solver.h"

int main() {
    // InstanceGenerator generator;
    //
    // std::cout << "Pliki beda zapisywane w katalogu: " << generator.getRunDirectory() << "\n\n";
    //
    // for(int cuts = 5; cuts <= 14; cuts += 3) {
    //     std::string filename1 = "instance" + std::to_string(cuts) + "a.txt";
    //     std::string filename2 = "instance" + std::to_string(cuts) + "b.txt";
    //     
    //     std::cout << "Generowanie instancji dla " << cuts << " ciec:\n";
    //     
    //     if(generator.generateInstance(cuts, filename1)) {
    //         std::cout << "Wygenerowano " << filename1 << "\n";
    //         if(generator.verifyInstance(filename1)) {
    //             std::cout << "Weryfikacja " << filename1 << " poprawna\n";
    //         }
    //     }
    //     
    //     if(generator.generateInstance(cuts, filename2)) {
    //         std::cout << "Wygenerowano " << filename2 << "\n";
    //         if(generator.verifyInstance(filename2)) {
    //             std::cout << "Weryfikacja " << filename2 << " poprawna\n";
    //         }
    //     }
    //     
    //     std::cout << "\n";
    // }

    InstanceGenerator generator(512);
    TestFramework framework(generator);
    
    std::cout << "Starting test suite...\n\n";
    
    // Test random instances
    std::cout << "Testing random instances...\n";
    if (framework.testRandomInstances(10, 5, 14)) {
        std::cout << "All random instance tests passed!\n\n";
    }
    
    // Test PDP instances
    std::cout << "Testing PDP instances...\n";
    if (framework.testPDPInstances("data")) {
        std::cout << "All PDP instance tests passed!\n\n";
    }
    
    // Test sorting impact
    // std::cout << "Analyzing sorting impact...\n";
    // for (int cuts = 5; cuts <= 14; cuts += 3) {
    //     std::string filename = "instance" + std::to_string(cuts) + "a.txt";
    //     if (framework.analyzeSortingImpact(filename)) {
    //         std::cout << "Sorting analysis completed for " << filename << "\n";
    //     }
    // }

    std::vector<int> distances = generator.loadInstance("./data/instance11aasc.txt");
    MapSolver solver(distances, 512);
    if (solver.solve()) {
        std::cout << "Rozwiazanie znalezione!\n";
        const auto& solution = solver.getSolution();
        for (int i = 0; i < solution.size(); i++) {
            std::cout << solution[i] << " ";
        }
        // Użyj rozwiązania...
    } else {
        std::cout << "Nie znaleziono rozwiazania.\n";
    }
    
    return 0;
    
}