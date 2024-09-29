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
