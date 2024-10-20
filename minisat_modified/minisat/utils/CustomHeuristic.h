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
    CustomHeuristic(std::string heuristic) : heuristic_(heuristic) {
        if (heuristic_ == "chb") {
            alpha_ = 0.4;
            multiplier_ = 1.0;
            reward_ = 0.0;
        }
    };    
    std::string getHeuristic() { return heuristic_; };

    std::vector<Var> findHeuristicVector(Solver& solver);

    void initializeVarOccurrences(Solver& solver);

    void printClauses(Solver& solver);

    void add_to_plays(Var v);
    void conflict_analysis_push(Var v) {
        conflict_analysis_.push(v);
    };
    void chb_update(IntMap<Var, double>&  activity, int conflicts, bool was_conflict);
    void chb_encountered_conflict();
    void chb_update_last_conflicts(int conflicts);
    void chb_reinitialize();
    void chb_new_var();


    // Heuristic options: activity, dynamic_var_occurrences, dynamic_jeroslow_wang, dynamic_mom, static_var_occurrences, lazy_var_occurrences, chb, random
    std::string heuristic_;

    std::vector<Var> var_occurrences_; // static_var_occurrences

    double alpha_; // chb
    double multiplier_; // chb
    double reward_; // chb
    vec<int> last_conflict_; // chb
    vec<Var> plays_; // chb
    vec<bool> inplays_; // chb
    vec<Var> conflict_analysis_; // chb

};

}

#endif // CUSTOM_HEURISTIC_H