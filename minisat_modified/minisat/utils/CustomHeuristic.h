#ifndef CUSTOM_HEURISTIC_H
#define CUSTOM_HEURISTIC_H

#include "minisat/core/SolverTypes.h"

#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

namespace Minisat {

class Solver;

class CustomHeuristic
{
public:
    CustomHeuristic() : heuristic_("activity") {};
    CustomHeuristic(std::string heuristic) : heuristic_(heuristic) {};    
    std::string getHeuristic() { return heuristic_; };

    std::vector<Var> findHeuristicVector(Solver& solver);

    void initializeVarOccurrences(Solver& solver);

    void printClauses(Solver& solver);


    // Heuristic options: activity, static_var_occurrences, static_jeroslow_wang, dynamic_mom, random
    std::string heuristic_;

    std::vector<Var> var_occurrences_;
};

}

#endif // CUSTOM_HEURISTIC_H