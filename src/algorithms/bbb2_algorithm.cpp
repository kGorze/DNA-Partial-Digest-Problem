#include "../../include/algorithms/bbb2_algorithm.h"


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

std::optional<std::vector<int>> BBb2Algorithm::solve(std::vector<int> D)
{
    originalDistances = D;
    std::sort(D.begin(), D.end(), std::greater<int>());
    if(D.empty()){
        return std::nullopt;
    }
    int width = D[0];

    auto it = std::find(D.begin(), D.end(), width);
    if(it != D.end()) {
        D.erase(it);
    }
    std::vector<int> X0 = {0, width};

    int n = calculateN((int)D.size());
    int alphaM = findAlphaM((int)D.size());

    std::vector<AlphaNode> alphaNodes;
    buildToAlpha(alphaNodes, D, X0, alphaM);

    std::sort(alphaNodes.begin(), alphaNodes.end(),
        [this](auto &a, auto &b){
            auto va = a.D.toVector();
            auto vb = b.D.toVector();
            return va.size()<vb.size();
        }
    );

    for(const auto& node: alphaNodes){
        if(node.D.empty()){
            if(isValidSolution(node.X, originalDistances)){
                return node.X;
            }
            continue;
        }

        auto solution = processAlphaNode(node);
        if(solution){
            if(isValidSolution(*solution, originalDistances)){
                return solution;
            }
        }
    }

    return std::nullopt;
}

void BBb2Algorithm::buildToAlpha(std::vector<AlphaNode>& alphaNodes,
                                 const std::vector<int>& initialD,
                                 const std::vector<int>& initialX,
                                 int alpha)
{
    MultisetD msD = MultisetD::fromVector(initialD);
    std::queue<AlphaNode> Q;
    Q.push( AlphaNode(msD, initialX) );

    std::unordered_set<std::string> visited;
    auto st = encodeState(initialX, msD);
    visited.insert(st);

    int level = 0;
    while(!Q.empty() && level<alpha){
        int levelSize = (int)Q.size();

        for(int i=0; i<levelSize; i++){
            auto current = Q.front();
            Q.pop();

            auto dVec = current.D.toVector();
            if(dVec.empty()){
                alphaNodes.push_back(current);
                continue;
            }

            int m = *std::max_element(dVec.begin(), dVec.end());
            int width = current.X.back();

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
                    }
                }
            }

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
                    }
                }
            }
        }
        level++;
    }

    while(!Q.empty()){
        alphaNodes.push_back(Q.front());
        Q.pop();
    }
}

std::optional<std::vector<int>> BBb2Algorithm::processAlphaNode(const AlphaNode& node){
    auto remainVec = node.D.toVector();
    auto partialSol = bbbSolver.solvePartial(node.X, remainVec);
    if(!partialSol){
        return std::nullopt;
    }
    return partialSol;
}

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

bool BBb2Algorithm::removeDelta(MultisetD &mD, int y, const std::vector<int>& X){
    for(int x : X){
        int d = std::abs(y - x);
        if(!mD.contains(d)){
            return false;
        }
    }
    for(int x : X){
        int d = std::abs(y - x);
        mD.remove(d);
    }
    return true;
}

int BBb2Algorithm::calculateN(int setSize) const {
    double disc = 1.0 + 8.0*setSize;
    double val = (std::sqrt(disc)+1.0)/2.0;
    return (int)std::round(val);
}

int BBb2Algorithm::findAlphaM(int N) const {
    int n = calculateN(N);
    if(n<=3) return 1;

    double bestScore = 1e18;
    int bestAlpha = 1;
    for(int alpha=1; alpha<n; alpha++){
        double c = (n*n * std::pow(2.0, alpha)) + (n*n * std::pow(2.0, (double)(n-alpha)));
        if(c<bestScore){
            bestScore = c;
            bestAlpha = alpha;
        }
    }
    return bestAlpha;
}