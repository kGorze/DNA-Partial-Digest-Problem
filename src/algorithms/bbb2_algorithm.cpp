#include "../../include/algorithms/bbb2_algorithm.h"
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <iostream>

// Pomocnicze: kodowanie stanu (X, D) do stringa
static std::string encodeState(const std::vector<int>& X, const MultisetD& mD){
    std::stringstream ss;
    ss<<"X:";
    for(int x : X){
        ss<<x<<"|";
    }
    ss<<"D:";
    auto vec = mD.toVector();
    std::sort(vec.begin(), vec.end());
    for(int v : vec){
        ss<<v<<"|";
    }
    return ss.str();
}

// ==============================
// Główna metoda 'solve'
std::optional<std::vector<int>> BBb2Algorithm::solve(std::vector<int> D)
{
    // zapamiętujemy oryginalne D (przyda się do weryfikacji gotowych rozwiązań)
    originalDistances = D;

    // Sortujemy D malejąco i ustalamy width = D[0]
    std::sort(D.begin(), D.end(), std::greater<int>());
    if(D.empty()){
        debugPrint("D is empty, nothing to solve.");
        return std::nullopt;
    }
    int width = D[0];

    debugPrint("Starting BBb2 solve(...) with D="+toStr(MultisetD::fromVector(D))+
               ", width="+std::to_string(width));

    // Usuwamy jednorazowo width z D
    auto it = std::find(D.begin(), D.end(), width);
    if(it != D.end()) {
        D.erase(it);
    }
    // Początkowe X = {0, width}
    std::vector<int> X0 = {0, width};

    // Obliczamy n (liczbę punktów w X = n)
    // UWAGA: D.size() to n(n-1)/2 - 1 usunięcie width
    int n = calculateN((int)D.size());
    int alphaM = findAlphaM((int)D.size());
    debugPrint("n = "+std::to_string(n)+", alphaM = "+std::to_string(alphaM));

    // BFS do poziomu alphaM
    std::vector<AlphaNode> alphaNodes;
    buildToAlpha(alphaNodes, D, X0, alphaM);
    debugPrint("buildToAlpha done. Number of alphaNodes = "+std::to_string(alphaNodes.size()));

    // Sortujemy alphaNodes np. według wielkości D, nie jest to konieczne, ale czasem pomaga
    std::sort(alphaNodes.begin(), alphaNodes.end(),
        [this](auto &a, auto &b){
            auto va = a.D.toVector();
            auto vb = b.D.toVector();
            return va.size()<vb.size();
        }
    );

    // Drugi etap - bierzemy każdy węzeł z poziomu alpha i wywołujemy processAlphaNode
    int idx=0;
    for(const auto& node: alphaNodes){
        debugPrint("-----\nProcessing alpha node #"+std::to_string(idx++));
        debugPrint("Node.X = "+toStr(node.X));
        debugPrint("Node.D = "+toStr(node.D));

        if(node.D.empty()){
            // Być może X jest już pełne
            debugPrint("Node.D is empty => checking if node.X is a valid solution...");
            if(isValidSolution(node.X, originalDistances)){
                debugPrint("Valid solution found from an empty-D node!");
                return node.X;
            }
            debugPrint("Node.X is NOT a valid solution => skip");
            continue;
        }

        // W przeciwnym razie próbujemy dokończyć węzeł BBb w trybie partial
        auto solution = processAlphaNode(node);
        if(solution){
            debugPrint("processAlphaNode returned solution = "+toStr(*solution));
            if(isValidSolution(*solution, originalDistances)){
                debugPrint("Solution from processAlphaNode is valid => returning it!");
                return solution;
            } else {
                debugPrint("Solution from processAlphaNode is NOT valid => continue searching...");
            }
        } else {
            debugPrint("No solution returned by processAlphaNode => continue searching...");
        }
    }

    debugPrint("No solution found (BBb2). Returning nullopt.");
    return std::nullopt;
}

// ==============================
// Metoda buildToAlpha - BFS do poziomu alpha
void BBb2Algorithm::buildToAlpha(std::vector<AlphaNode>& alphaNodes,
                                 const std::vector<int>& initialD,
                                 const std::vector<int>& initialX,
                                 int alpha)
{
    debugPrint("buildToAlpha: BFS up to level="+std::to_string(alpha));
    MultisetD msD = MultisetD::fromVector(initialD);
    std::queue<AlphaNode> Q;
    Q.push( AlphaNode(msD, initialX) );

    std::unordered_set<std::string> visited;

    // Zapisujemy stan początkowy
    {
        auto st = encodeState(initialX, msD);
        visited.insert(st);
        debugPrint("Init BFS state => X0="+toStr(initialX)+", D0="+toStr(msD));
    }

    int level = 0;
    while(!Q.empty() && level<alpha){
        int levelSize = (int)Q.size();
        debugPrint("\n-- BFS level = "+std::to_string(level)+
                   ", queue size = "+std::to_string(levelSize)+" --");

        for(int i=0; i<levelSize; i++){
            auto current = Q.front();
            Q.pop();

            auto dVec = current.D.toVector();
            if(dVec.empty()){
                // Jeśli D puste, to nie rozwijamy dalej, ale dopisujemy do alphaNodes
                debugPrint("BFS: found empty D at level="+std::to_string(level)+
                           ". X="+toStr(current.X)+" => add to alphaNodes.");
                alphaNodes.push_back(current);
                continue;
            }

            int m = *std::max_element(dVec.begin(), dVec.end());
            int width = current.X.back(); // zakładamy, że X jest posortowane => X.back()=width

            debugPrint("  Expanding node => X="+toStr(current.X)+
                       ", D="+toStr(current.D)+
                       ". max(D)="+std::to_string(m)+
                       ", width="+std::to_string(width));

            // Kandydat 1: y = m
            if(m>=0 && m<=width){
                MultisetD newD = current.D;
                if(removeDelta(newD, m, current.X)){
                    std::vector<int> newX = current.X;
                    newX.push_back(m);
                    std::sort(newX.begin(), newX.end());

                    auto st = encodeState(newX, newD);
                    if(visited.find(st)==visited.end()){
                        visited.insert(st);
                        Q.push(AlphaNode(std::move(newD), std::move(newX)));
                        debugPrint("    -> Add child (y=m): newX, newD => X="+
                                   toStr(Q.back().X)+", D="+toStr(Q.back().D));
                    } else {
                        debugPrint("    -> Child (y=m) is duplicate => skip");
                    }
                } else {
                    debugPrint("    -> removeDelta(m) failed => skip");
                }
            }

            // Kandydat 2: y = width - m
            int cmpl = width - m;
            if(cmpl!=m && cmpl>=0 && cmpl<=width){
                MultisetD newD2 = current.D;
                if(removeDelta(newD2, cmpl, current.X)){
                    std::vector<int> newX2 = current.X;
                    newX2.push_back(cmpl);
                    std::sort(newX2.begin(), newX2.end());

                    auto st2 = encodeState(newX2, newD2);
                    if(visited.find(st2)==visited.end()){
                        visited.insert(st2);
                        Q.push(AlphaNode(std::move(newD2), std::move(newX2)));
                        debugPrint("    -> Add child (y=width-m): newX2, newD2 => X="+
                                   toStr(Q.back().X)+", D="+toStr(Q.back().D));
                    } else {
                        debugPrint("    -> Child (y=width-m) is duplicate => skip");
                    }
                } else {
                    debugPrint("    -> removeDelta(width-m) failed => skip");
                }
            }

        } // end for levelSize
        level++;
    }

    // Pozostałe węzły w kolejce (jeśli alpha warunek przerwał) -> alphaNodes
    while(!Q.empty()){
        alphaNodes.push_back(Q.front());
        Q.pop();
    }
}

// ==============================
// Drugi etap: processAlphaNode => wywołuje solvePartial w BBbAlgorithm
std::optional<std::vector<int>> BBb2Algorithm::processAlphaNode(const AlphaNode& node){
    auto remainVec = node.D.toVector();
    debugPrint("processAlphaNode: remainVec="+toStr(MultisetD::fromVector(remainVec))+
               ", node.X="+toStr(node.X));

    // Tutaj klucz: zamiast bbbSolver.solve(remainVec), 
    // wołamy solvePartial(node.X, remainVec)
    auto partialSol = bbbSolver.solvePartial(node.X, remainVec);
    if(!partialSol){
        debugPrint("bbbSolver returned no solution => nullopt");
        return std::nullopt;
    }
    // partialSol to już pełne X
    return partialSol;
}

// ==============================
// Porównanie X z oryginalnym D
bool BBb2Algorithm::isValidSolution(const std::vector<int>& X, 
                                    const std::vector<int>& origD) const
{
    if(X.size()<2) return false;

    std::vector<int> genD;
    genD.reserve((X.size()*(X.size()-1))/2);
    for(size_t i=0;i<X.size();i++){
        for(size_t j=i+1;j<X.size();j++){
            genD.push_back(std::abs(X[j]-X[i]));
        }
    }
    std::sort(genD.begin(), genD.end());

    auto sortedOrig = origD;
    std::sort(sortedOrig.begin(), sortedOrig.end());

    return (genD==sortedOrig);
}

// ==============================
// removeDelta - usuwa wszystkie |y - x| dla x w X, jeśli występują
bool BBb2Algorithm::removeDelta(MultisetD &mD, int y, const std::vector<int>& X){
    // sprawdzamy najpierw, czy wszystkie |y - x| są w mD
    for(int x : X){
        int d = std::abs(y - x);
        if(!mD.contains(d)){
            return false;
        }
    }
    // jeśli tak, to usuwamy
    for(int x : X){
        int d = std::abs(y - x);
        mD.remove(d);
    }
    return true;
}

// ==============================
// Funkcje do obliczenia n (liczby punktów) i alphaM 
int BBb2Algorithm::calculateN(int setSize) const {
    // n(n-1)/2 = setSize => n ~ (1 + sqrt(1+8*setSize)) / 2
    double disc = 1.0 + 8.0*setSize;
    double val = (std::sqrt(disc)+1.0)/2.0;
    return (int)std::round(val);
}

int BBb2Algorithm::findAlphaM(int N) const {
    // Heurystyka minimalizująca potencjalne zużycie pamięci 
    int n = calculateN(N);
    if(n<=3) return 1;

    double bestScore = 1e18;
    int bestAlpha = 1;
    for(int alpha=1; alpha<n; alpha++){
        // Koszt: n^2 * 2^alpha + n^2 * 2^(n-alpha)
        double c = (n*n * std::pow(2.0, alpha)) + (n*n * std::pow(2.0, (double)(n-alpha)));
        if(c<bestScore){
            bestScore = c;
            bestAlpha = alpha;
        }
    }
    return bestAlpha;
}
