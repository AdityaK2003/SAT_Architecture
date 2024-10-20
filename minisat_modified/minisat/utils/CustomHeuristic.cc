#include "minisat/core/Solver.h"
#include "minisat/utils/CustomHeuristic.h"

#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <cmath>

using namespace Minisat;

// Returns vector of variables in order based on the heuristic
// - activity / random: custom heuristic class not used
// - dynamic_var_occurrences: recomputes each time the number of total occurrences
// - dynamic_jeroslow_wang: recomputes each time the jeroslow-wang heuristic (exponential weightage of shorter clause occurrences)
// - dynamic_mom: maximum occurrences of clauses of minimum size formula, and recomputes each time
// - static_var_occurrences: computes map once at beginning then doesn't recompute
// - chb: this function is not used
std::vector<Var> CustomHeuristic::findHeuristicVector(Solver& solver) {
    // Do nothing if this function is called with "activity" or "random", since already taken care of
    if (heuristic_ == "activity" || heuristic_ == "random") {
        return std::vector<Minisat::Var>();
    }

    // printClauses(solver);

    if (heuristic_ == "dynamic_var_occurrences") {
        // Calculate occurrences of each variable
        std::unordered_map<Var, int> varOccurrences;
        for(int i = 0; i < solver.clauses.size(); ++i) {
            const Clause& clause = solver.ca[solver.clauses[i]];
            if (solver.satisfied(clause)) continue;
            for (int j = 0; j < clause.size(); ++j) {
                // Only include variables that have not been set yet
                Var var = Minisat::var(clause[j]);
                if (solver.value(var) == l_Undef) varOccurrences[var]++;
            }
        }
        // Sort using pair <occurrences, var>
        std::priority_queue<std::pair<int, Var>> pq;
        for(const auto& entry : varOccurrences) {
            pq.push(std::make_pair(entry.second, entry.first));
        }
        std::vector<Var> result;
        // Sort vars
        while(pq.size()) {
            result.push_back(pq.top().second);
            pq.pop();
        }
        return result;
    } else if (heuristic_ == "dynamic_jeroslow_wang") {
        // Create a map from each variable to its J(v), in which J(v) = sum of 2^-|w| over w, where w is a clause which contains variable v
        std::unordered_map<Var, double> heurMap;
        for(int i = 0; i < solver.clauses.size(); ++i) {
            const Clause& clause = solver.ca[solver.clauses[i]];
            if (solver.satisfied(clause)) continue;
            double increment = pow(2, -1 * clause.size());
            for (int j = 0; j < clause.size(); ++j) {
                Var var = Minisat::var(clause[j]);
                if (solver.value(var) == l_Undef) heurMap[var] += increment;
            }
        }
        // Sort using pair <J(var), var>
        std::priority_queue<std::pair<double, Var>> pq;
        for(const auto& entry : heurMap) {
            pq.push(std::make_pair(entry.second, entry.first));
        }
        std::vector<Var> result;
        // Sort vars
        while(pq.size()) {
            result.push_back(pq.top().second);
            pq.pop();
        }
        return result;
    } else if (heuristic_ == "dynamic_mom") {
        // Essentially same as var occurrences heuristic, only considering minimum sized clauses
        // Find minimum size
        int minimum_clause_size = INT_MAX;
        for(int i = 0; i < solver.clauses.size(); ++i) {
            const Clause& clause = solver.ca[solver.clauses[i]];
            if(!solver.satisfied(clause) && clause.size() < minimum_clause_size) {
                minimum_clause_size = clause.size();
                if (minimum_clause_size == 2) break;
            }
        }
        // Calculate occurrences of each literal
        std::unordered_map<int, int> litOccurrences;
        for(int i = 0; i < solver.clauses.size(); ++i) {
            const Clause& clause = solver.ca[solver.clauses[i]];
            if (solver.satisfied(clause)) continue;
            if (clause.size() > minimum_clause_size) continue;
            for (int j = 0; j < clause.size(); ++j) {
                Var var = Minisat::var(clause[j]);
                if (solver.value(var) == l_Undef) {
                    bool isNegated = sign(clause[j]);
                    if (isNegated) litOccurrences[-1 * var]++;
                    else litOccurrences[var]++;
                }
            }
        }
        // Sort using pair <occurrences, var>
        std::priority_queue<std::pair<int, int>> pq;
        for(const auto& entry : litOccurrences) {
            pq.push(std::make_pair(entry.second, entry.first));
        }
        std::vector<Var> result;
        // Sort vars (keep track so literals aren't added twice)
        std::unordered_set<int> seen;
        while(pq.size()) {
            // std::cout << pq.top().second << " has " << pq.top().first << " occurrences" << std::endl;
            int var = abs(pq.top().second);
            if(seen.find(var) == seen.end()) {
                result.push_back(var);
                seen.insert(var);
            }
            pq.pop();
        }
        return result;
    }
    else if (heuristic_ == "static_var_occurrences") {
        if (!var_occurrences_.empty()) {
            // Already computed
            return var_occurrences_;
        }
        // Calculate occurrences of each variable
        std::unordered_map<Var, int> varOccurrences;
        for(int i = 0; i < solver.clauses.size(); ++i) {
            const Clause& clause = solver.ca[solver.clauses[i]];
            // DO NOT ignore satisfied clauses! since this is static.
            // if (solver.satisfied(clause)) continue;
            for (int j = 0; j < clause.size(); ++j) {
                // Only include variables that have not been set yet
                Var var = Minisat::var(clause[j]);
                if (solver.value(var) == l_Undef) varOccurrences[var]++;
            }
        }
        // Sort using pair <occurrences, var>
        std::priority_queue<std::pair<int, Var>> pq;
        for(const auto& entry : varOccurrences) {
            pq.push(std::make_pair(entry.second, entry.first));
        }
        // Sort vars
        while(pq.size()) {
            var_occurrences_.push_back(pq.top().second);
            pq.pop();
        }
        return var_occurrences_;
    } else if (heuristic_ == "lazy_var_occurrences") {
        // Calculate occurrences of each variable
        std::unordered_map<Var, int> varOccurrences;
        for(int v = 0; v < solver.nVars(); ++v) {
            // Only use variables without assignments
            if (solver.value((Var)v) != l_Undef) continue;

            int pos_count = solver.watches[mkLit(v, false)].size();
            int neg_count = solver.watches[mkLit(v, true)].size();
            varOccurrences[(Var)v] = pos_count + neg_count;
            // std::cout << "Var " << v << " has count " << pos_count + neg_count << std::endl;
        }
        // Sort using pair <occurrences, var>
        std::priority_queue<std::pair<int, Var>> pq;
        for(const auto& entry : varOccurrences) {
            pq.push(std::make_pair(entry.second, entry.first));
        }
        std::vector<Var> result;
        // Sort vars
        while(pq.size()) {
            result.push_back(pq.top().second);
            pq.pop();
        }
        return result;
    } else if (heuristic_ == "chb") {
        // Nothing to do
        return std::vector<Minisat::Var>();
    }

    // Unidentified heuristic (error)
    else {
        std::cerr << "Unidentified heuristic: " << heuristic_ << "\n";
        return std::vector<Minisat::Var>();
    }
}

void CustomHeuristic::printClauses(Solver& solver) {
    int printedClauses = 0;
    for(int i = 0; i < solver.clauses.size(); ++i) {
        const Clause& clause = solver.ca[solver.clauses[i]];
        if (solver.satisfied(clause)) continue;
        for (int j = 0; j < clause.size(); ++j) {
            Lit literal = clause[j];
            int varValue = var(literal);
            int isNegated = sign(literal);
            std::cout << (isNegated ? "-" : "") << varValue << " ";
            ++printedClauses;
        }
        std::cout << std::endl;
    }
    // std::cout << "Printed clauses: " << printedClauses << std::endl << std::endl;
}


// For use in CHB
void CustomHeuristic::add_to_plays(Var v) {
    assert(v!=var_Undef);
    if(!inplays_[v]) {	
        inplays_[v]=true;
	    plays_.push(v);
    }
}

void CustomHeuristic::chb_update(IntMap<Var, double>&  activity, int conflicts, bool was_conflict) {
    if (was_conflict) {
        multiplier_ = 1;
    } else {
        multiplier_ = 0.9;
    }
    for (int i = 0; i < plays_.size() ;++i) {
        Var x = plays_[i];
        reward_ = multiplier_ / ((long double)(conflicts - last_conflict_[x]+1.0));
        activity[x]=(((1-alpha_) * activity[x])+(alpha_ * reward_));
    }
}

void CustomHeuristic::chb_encountered_conflict() {
    if (alpha_ > 0.06)
        alpha_ = alpha_ - (1e-6);
    conflict_analysis_.clear();
}

void CustomHeuristic::chb_update_last_conflicts(int conflicts) {
    for (int i=0; i< conflict_analysis_.size(); ++i)
    {  
        last_conflict_[(conflict_analysis_[i])]=conflicts;
    }
}

void CustomHeuristic::chb_reinitialize() {
    alpha_ = 0.4;
    for (int i=0;i<plays_.size();++i)
	{ 
        inplays_[plays_[i]]=false;
	}
    	
    plays_.clear();
}

void CustomHeuristic::chb_new_var() {
    last_conflict_.push(0);
    inplays_.push(false);
}