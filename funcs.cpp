#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <queue>
#include <stdlib.h>
#include <time.h>
#include <iomanip>
#include <math.h>
#include <algorithm>
#include <stack>
#include <random>
#include <list>
#include <dirent.h>
#include <chrono>
#include <iomanip>

#include "old_funcs.hpp"
#include "funcs.hpp"

using namespace std;

/**
 * Given a certain literal (positive or negative), finds the set of clauses this doesn't appear in
 * Ex: if -5 appears in all but the first 3 clauses, then this function returns {1, 2, 3}
 * 
 * Params:
 * - formula: the SAT formula
 * - literal: the literal used to find distjoint clauses
 * 
 * Return:
 * - set of clauses which are disjoint
*/
set<int> findDisjointClauses(vector<vector<int>>& formula, int literal) {
    set<int> disjoint;

    // Iterate through all clauses
    for(int c = 0; c < formula.size(); ++c) {
        // If clause doesn't contain the literal
        if(!literalInClause(formula, literal, c+1)) {
            disjoint.insert(c+1);
        }
    }

    return disjoint;
}

/**
 * Given two literals, finds if they are disjoint or not
 * 
 * Params:
 * - formula: the SAT formula
 * - lit1: the first literal
 * - lit2: the second literal
 * 
 * Return:
 * - true if both variables are disjoint, and false otherwise (i.e., occur in a clause together)
*/
bool areLiteralsDisjoint(vector<vector<int>>& formula, int lit1, int lit2) {
    // Iterate through each clause
    for(int c = 0; c < formula.size(); ++c) {
        // If both literals are in the same clause
        if(literalInClause(formula, lit1, c+1) && literalInClause(formula, lit2, c+1)) {
            return false;
        }
    }
    return true;
}

/**
 * Given two literals, finds if they are disjoint or not
 * 
 * Params:
 * - lit_clauses: map from each literal to set of clauses it is in
 * - lit1
 * - lit2
 * 
 * Return:
 * - true if both variables are disjoint, false otherwise
*/
bool areLiteralsDisjoint(map<int, set<int>>& lit_clauses, int lit1, int lit2) {
    set<int> s1 = lit_clauses[lit1];
    set<int> s2 = lit_clauses[lit2];

    for(int i : s1) {
        if(s2.find(i) != s2.end()) {
            return false;
        }
    }
    return true;
}

/**
 * Given a formula, calculates the number of disjoint pairs
 * A disjoint pair is a pair (x, y) where x and y are literals (|y| > |x|)
 * where x and y are disjoint
 * 
 * Params:
 * - formula: the SAT formula
 * - vars: the number of variables in the formula
 * 
 * Returns:
 * - vector<pair<int, int>>: list of pairs
*/
vector<pair<int, int>> numDisjointLiteralPairs(vector<vector<int>>& formula, int vars) {
    vector<pair<int, int>> disjointPairs;

    for(int v1 = 1; v1 <= vars-1; ++v1) {
        for(int v2 = v1+1; v2 <= vars; ++v2) {
            // Try all pairs of literals
            vector<pair<int, int>> pairs = {make_pair(-v1, -v2), make_pair(-v1, v2), make_pair(v1, -v2), make_pair(v1, v2)};
            for(pair<int, int> pair : pairs) {
                // Check for disjointness
                if(areLiteralsDisjoint(formula, pair.first, pair.second)) disjointPairs.push_back(pair);
            }
        }
    }

    return disjointPairs;
}

/**
 * Given a total count for number of variables, returns the total number of pairs
 * A pair is any combination (x, y) of variables where |y| > |x|
 * 
 * Params:
 * - vars: the number of variables
 * 
 * Returns:
 * - int: the number of pairs
*/
int totalNumLiteralPairs(int vars) {
    /**
     * Examples
     * vars = 2 -> {(1, 2)} *4 (for literals) = 1 * 4 = 4
     * vars = 3 -> {(1, 2), (1, 3), (2, 3)} = 3 * 4 = 12
     * vars = 4 -> {(1, 2), (1, 3), (1, 4), (2, 3), (2, 4), (3, 4)} = 6 * 4 = 24
    */

   return (vars) * (vars-1) * 2;
}


/**
 * Given a SAT formula, identifies all unipolar literals
 * A literal x is unipolar if ~x never appears in the formula
 * 
 * Params:
 * - formula: the SAT formula
 * 
 * Returns:
 * - set<int>: set of unipolar literals
*/
set<int> findUnipolarLiterals(vector<vector<int>>& formula) {
    unordered_map<int, int> occurrences;

    for(vector<int> c : formula) {
        for(int x : c) {
            if(occurrences.find(x) == occurrences.end()) occurrences[x] = 1;
            else occurrences[x]++;
        }
    }

    // Check for unipolarity
    set<int> unipolar;
    for(pair<int, int> p : occurrences) {
        if(occurrences.find(-p.first) == occurrences.end()) {
            unipolar.insert(p.first);
        }
    }

    return unipolar;
}


/**
 * Given a SAT formula, identifies a unipolar literal
 * A literal x is unipolar if ~x never appears in the formula
 * 
 * Params:
 * - formula: the SAT formula
 * 
 * Returns:
 * - int: a unipolar literal (or 0 if there are none)
*/
int findUnipolarLiteral(vector<vector<int>>& formula) {
    unordered_map<int, int> occurrences;

    for(vector<int> c : formula) {
        for(int x : c) {
            if(occurrences.find(x) == occurrences.end()) occurrences[x] = 1;
            else occurrences[x]++;
        }
    }

    // Check for unipolarity
    for(pair<int, int> p : occurrences) {
        if(occurrences.find(-p.first) == occurrences.end()) {
            return p.first;
        }
    }

    return 0;
}


/**
 * Finds all literals that are part of unit clauses
 * 
 * Params:
 * - formula: the SAT formula
 * 
 * Returns:
 * - set<int>: set of literals
*/
set<int> findUnitClauseLiterals(vector<vector<int>>& formula) {
    set<int> literals;

    for(vector<int> c : formula) {
        if(c.size() == 1) {
            literals.insert(c[0]);
        }
    }

    return literals;
}


/**
 * Finds a literal that is part of a unit clause
 * 
 * Params:
 * - formula: the SAT formula
 * 
 * Returns:
 * - int: a unit clause literal (or 0 if there are none)
*/
int findUnitClauseLiteral(vector<vector<int>>& formula) {
    for(vector<int> c : formula) {
        if(c.size() == 1) {
            return c[0];
        }
    }

    return 0;
}


/**
 * Given a literal to assign as true, this function updates the formula
 * if the literal is in the clause, the clause can be removed
 * if the negative of the literal is in the clause, the negative literal is removed
 * (Assumes problem is not UNSAT)
 * 
 * Params:
 * - formula: the SAT formula
 * - lit: the literal assigned to true
 * 
 * Returns:
 * - updates formula in place
 * 
*/
void updateFormulaWithLiteral(vector<vector<int>>& formula, int lit) {
    for(auto it = formula.begin(); it != formula.end(); ) {
        // Check if literal is in clause
        bool inClause = false;
        bool negationInClause = false;

        for(int x : *it) {
            if(x == lit) {
                inClause = true;
                break;
            }
            if(-x == lit) {
                negationInClause = true;
                break;
            }
        }

        if(inClause && negationInClause) cout << "ERROR: " << lit << " and " << -lit << " in same clause.\n";

        if(inClause) {
            // Remove clause
            it = formula.erase(it);
        } else if(negationInClause) {
            // Remove literal from clause
            it->erase(find(it->begin(), it->end(), -lit));
        } else {
            // Iterate to next clause
            ++it;
        }
    }
}

/**
 * Given a SAT formula, simplifies the problem by iteratively setting unipolar
 * literals and unit clauses
 * 
 * Params:
 * - formula: the SAT formula
 * - method1: uses method1 or method2 (true / false, respectively)
 * - print: if True, prints log messages (default - false)
 * 
 * Return:
 * - unordered_set<int>: the assignment
 * - formula is modified in place
*/
unordered_set<int> simplify(vector<vector<int>>& formula, bool method1, bool print) {
    // Maintain assignment
    unordered_set<int> assignment;

    // Method 1: calculate literals in bunches
    if(method1) {
        // Maintain set of unipolar literals, and set of unit clauses
        set<int> unipolarLiterals = findUnipolarLiterals(formula);
        set<int> unitClauseLiterals = findUnitClauseLiterals(formula);

        // Iterate until both empty
        while(unipolarLiterals.size() > 0 || unitClauseLiterals.size() > 0) {
            // Update formula and assignment for all unipolar literals
            for(int l : unipolarLiterals) {
                updateFormulaWithLiteral(formula, l);
                assignment.insert(l);
                if(print) cout << "Assigned " << l << " (unipolar)" << endl;
            }

            // Update formula and assignment for all unit clause literals
            for(int l : unitClauseLiterals) {
                updateFormulaWithLiteral(formula, l);
                assignment.insert(l);
                if(print) cout << "Assigned " << l << " (unit clause)" << endl;
            }

            // Recalculate unipolar literals and unit clause literals
            unipolarLiterals = findUnipolarLiterals(formula);
            unitClauseLiterals = findUnitClauseLiterals(formula);

            // If formula is empty, then quit
            if(formula.size() == 0) break;
        }
    } 
    // Method 2: calculate literal every iteration
    else {
        // Maintain unipolar literal and unit clause literal
        int unipolarLiteral = findUnipolarLiteral(formula);
        int unitClauseLiteral = findUnitClauseLiteral(formula);

        while(unipolarLiteral != 0 || unitClauseLiteral != 0) {
            // Update assignments and formula
            unipolarLiteral = findUnipolarLiteral(formula);
            if(unipolarLiteral != 0) {
                if(print) cout << "Assigned " << unipolarLiteral << " (unipolar)" << endl;
                updateFormulaWithLiteral(formula, unipolarLiteral);
                assignment.insert(unipolarLiteral);
            }

            unitClauseLiteral = findUnitClauseLiteral(formula);
            if(unitClauseLiteral != 0) {
                if(print) cout << "Assigned " << unitClauseLiteral << " (unitClause)" << endl;
                updateFormulaWithLiteral(formula, unitClauseLiteral);
                assignment.insert(unitClauseLiteral);
            }

            unipolarLiteral = findUnipolarLiteral(formula);
            unitClauseLiteral = findUnitClauseLiteral(formula);
        }
    }

    return assignment;
}


/**
 * Given a simplified SAT formula, renumbers the variables so that there
 * are no gaps in the numbering
 * 
 * Params:
 * - formula: the SAT formula
 * 
 * Returns:
 * - int: number of vars when simplified
 * - modifies formula in place
*/
int renumberFormula(vector<vector<int>>& formula) {
    // Maintain map from old variable number to new variable number
    unordered_map<int, int> m;

    // Assign next number for each new variable seen
    int curr_num = 1;
    for(int c = 0; c < formula.size(); ++c) {
        for(int i = 0; i < formula[c].size(); ++i) {
            int literal = formula[c][i];
            int var = abs(literal);

            if(m.find(var) == m.end()) {
                // Make pairing
                m[var] = curr_num;
                ++curr_num;
            }

            // Update formula
            if(literal > 0) {
                formula[c][i] = m[var];
            } else {
                formula[c][i] = -1 * m[var];
            }
        }
    }

    // Return number of vars used
    return curr_num - 1;
}


/**
 * Given a SAT formula, saves it to a file
 * 
 * Params:
 * - formula: the SAT formula
 * - vars: the number of variables
 * - outputFilename
 * - description: added to top of file as a comment (default - empty)
 * 
 * Returns:
 * - creates file
*/
void writeFormulaToFile(vector<vector<int>>& formula, int vars, string outputFilename, string description) {
    // Create outputfile
    ofstream myfile(outputFilename);

    // Add description
    if(description.size()) {
        myfile << "c " << description << endl;
    }

    // Write first line (num vars, num clauses)
    myfile << "p cnf " << vars << " " << formula.size() << endl;

    // Write lines
    for(vector<int> c : formula) {
        for(int n : c) {
            myfile << n << " ";
        }
        myfile << "0" << endl;
    }
}


/**
 * Given a SAT formula and two variables, checks if the pair can be implemented
 * This means a variable and its negation is placed above another variable and its negation (two folded column lines)
 * 
 * Params:
 * - formula: the SAT formula
 * - var1
 * - var2
 * 
 * Returns:
 * - int:
 *      0 if not disjoint
 *      1 if v1 & v2, and -v1 & -v2 are disjoint
 *      2 if v1 & -v2 are disjoint, and -v1 & v2 are disjoint
 *      3 if all combinations of literals are disjoint
*/
int areVarsDisjoint(vector<vector<int>>& formula, int var1, int var2) {
    // Variables are always positive
    var1 = abs(var1);
    var2 = abs(var2);

    // Four comparisons to make
    bool v1v2_disjoint = areLiteralsDisjoint(formula, var1, var2);
    bool v1v2bar_disjoint = areLiteralsDisjoint(formula, var1, -1*var2);
    bool v1barv2_disjoint = areLiteralsDisjoint(formula, -1*var1, var2);
    bool v1barv2bar_disjoint = areLiteralsDisjoint(formula, -1*var1, -1*var2);

    // Pair v1 with v2, and pair -v1 with -v2
    bool pairing1 = false, pairing2 = false;
    if(v1v2_disjoint && v1barv2bar_disjoint) {
        pairing1 = true;
    }
    // Pair v1 with -v2, and pair -v1 with v2
    if(v1v2bar_disjoint && v1barv2_disjoint) {
        pairing2 = true;
    }

    // Return based on specs
    if(!pairing1 && !pairing2) return 0;
    else if(pairing1 && !pairing2) return 1;
    else if(!pairing1 && pairing2) return 2;
    else return 3;
}


/**
 * Same as above, but uses lit_clauses instead of formula
 * 
 * lit_clauses: map from literal to clauses it is in
*/
int areVarsDisjoint(map<int, set<int>>& lit_clauses, int var1, int var2) {
    // Variables are always positive
    var1 = abs(var1);
    var2 = abs(var2);

    // Four comparisons to make
    bool v1v2_disjoint = areLiteralsDisjoint(lit_clauses, var1, var2);
    bool v1v2bar_disjoint = areLiteralsDisjoint(lit_clauses, var1, -1*var2);
    bool v1barv2_disjoint = areLiteralsDisjoint(lit_clauses, -1*var1, var2);
    bool v1barv2bar_disjoint = areLiteralsDisjoint(lit_clauses, -1*var1, -1*var2);

    // Pair v1 with v2, and pair -v1 with -v2
    bool pairing1 = false, pairing2 = false;
    if(v1v2_disjoint && v1barv2bar_disjoint) {
        pairing1 = true;
    }
    // Pair v1 with -v2, and pair -v1 with v2
    if(v1v2bar_disjoint && v1barv2_disjoint) {
        pairing2 = true;
    }

    // Return based on specs
    if(!pairing1 && !pairing2) return 0;
    else if(pairing1 && !pairing2) return 1;
    else if(!pairing1 && pairing2) return 2;
    else return 3;
}


/**
 * Given a formula, calculates the number of disjoint pairs
 * A disjoint variable pair is a pair (x, y) where y > x and
 * where x and y are disjoint
 * 
 * Params:
 * - formula: the SAT formula
 * - vars: the number of variables in the formula
 * 
 * Returns:
 * - vector<pair<int, int>>: list of pairs
*/
vector<pair<int, int>> numDisjointVarPairs(vector<vector<int>>& formula, int vars) {
    vector<pair<int, int>> results;
    
    for(int i = 1; i < vars; ++i) {
        for(int j = i+1; j <= vars; ++j) {
            if(areVarsDisjoint(formula, i, j)) {
                results.push_back(make_pair(i, j));
            }
        }
    }

    return results;
}


/**
 * Given a total count for number of variables, returns the total number of pairs
 * A variable pair is any combination (x, y) of variables where y > x
 * 
 * Params:
 * - vars: the number of variables
 * 
 * Returns:
 * - int: the number of pairs
*/
int totalNumVarPairs(int vars) {
    return totalNumLiteralPairs(vars) / 4;
}

// Default Column constructor
Column::Column() {}

// Column constructor
Column::Column(int l, int c) {
    lits.push_back(l); // lits.size() == 1 means unfolded line
    clauses = c;
    minUpperLine = clauses-1;
    maxUpperLine = clauses-1;
}

// Returns true if the column is folded (has more than one line)
bool Column::isFolded() {
    return (lits.size() > 1);
}

/**
 * Creates fold in a Column by adding a literal to a lower line, shrinking the original line
 * So the literal that is added is the lower line, and the original literal is the upper line
 * 
 * Params:
 * - lit: the literal to add
 * - numClauses1: the number of clauses that the upper literal is in
 * - max: the maximum value of the upper line
 *      - depends on number of clauses that lower lit is in
*/
void Column::makeFold(int lit, int numClauses1, int numClauses2) {
    lits.push_back(lit);
    minUpperLine = numClauses1 - 1;
    maxUpperLine = clauses - numClauses2 - 1;
}


// Circuit constructor
Circuit::Circuit(string f) {
    // Parse file
    filename = f;
    parse(filename, formula, vars, clauses);

    // Create lit clause assignments
    for(int i = 0; i < formula.size(); ++i) {
        int clause_num = i+1;
        for(int lit : formula[i]) {
            // Add to lit clause map
            if(lit_clauses.find(lit) == lit_clauses.end()) {
                set<int> s;
                s.insert(clause_num);
                lit_clauses[lit] = s;
            } else {
                lit_clauses[lit].insert(clause_num);
            }
        }
    }

    // Create unfolded circuit representation
    for(int var = 1; var <= vars; ++var) {
        // Positive literal line
        Column* c1 = new Column(var, clauses);
        lit_col[var] = c1;

        // Negative literal line
        Column* c2 = new Column(-1*var, clauses);
        lit_col[-1*var] = c2;
    }

    // Create unfolded spans
    for(int c = 1; c <= clauses; ++c) {
        clause_spans[c] = make_pair(0, clauses-1);
    }
}

// Circuit copy constructor
Circuit::Circuit(Circuit& c) {
    filename = c.filename;
    formula = c.formula;
    vars = c.vars;
    clauses = c.clauses;

    clause_spans = c.clause_spans;
    lit_clauses = c.lit_clauses;

    // Make deep copy of pointers
    for(pair<int, Column*> p : c.lit_col) {
        if(lit_col.find(p.first) != lit_col.end()) continue;

        Column* newCol = new Column();
        newCol->minUpperLine = p.second->minUpperLine;
        newCol->maxUpperLine = p.second->maxUpperLine;
        newCol->clauses = p.second->clauses;
        for(int l : p.second->lits) {
            newCol->lits.push_back(l);
            lit_col[l] = newCol;
        }
    }
}

// Circuit destructor
Circuit::~Circuit() {
    // Delete all Line pointers
    unordered_set<int> deleted_literals;
    for(pair<int, Column*> p : lit_col) {
        // Check if deleted already
        if(deleted_literals.find(p.first) != deleted_literals.end()) continue;

        for(int l : p.second->lits) {
            deleted_literals.insert(l);
        }

        delete p.second;
    }
}

/**
 * Checks possible pairing of variables with a specific orientation
 * 
 * Params:
 * - u: upper variable in line
 * - l: lower variable in line
 * - print: prints extra info if true
 * 
 * Returns:
 * - bool: true if this orientation of a pairing is possible
*/
bool Circuit::checkPairing(int u, int l, bool print) {

    if(print) {
        cout << "Validating pairing for literals " << u << " / " << l << " ..." << endl;
    }

    // Trivial case
    if(lit_clauses.find(u) == lit_clauses.end() || lit_clauses.find(l) == lit_clauses.end()) {
        if(print) cout << "Trivially SUCCEEDED" << endl << endl;
        return true;
    }

    // Calculate min and max upper line values 
    int line1UpperMin = lit_clauses[u].size() - 1;
    int line1UpperMax = clauses - lit_clauses[l].size() - 1;

    // Validate clauses
    for(int c : lit_clauses[u]) {
        if(clause_spans[c].first > line1UpperMax) {
            if(print) {
                cout << "FAILED: Clause " << c << " has span (" << clause_spans[c].first << ", " << clause_spans[c].second << ") ";
                cout << "but the upper line in this column (with " << u << " / " << l << ") can only possibly extend up to maximum value " << line1UpperMax << endl;
            }
            return false;
        }
    }
    for(int c : lit_clauses[l]) {
        if(clause_spans[c].second < line1UpperMin) {
            if(print) {
                cout << "FAILED: Clause " << c << " has span (" << clause_spans[c].first << ", " << clause_spans[c].second << ") ";
                cout << "but the lower line in this column (with " << u << " / " << l << ") can only start from minimum value " << line1UpperMin << endl;
            }
            return false;
        }
    }

    if(print) {
        cout << "SUCCEEDED" << endl << endl;
    }
    return true;
}

/**
 * Checks whether two variables can be folded to be part of the same column
 * If either variable is already folded, returns false
 * 
 * Params:
 * - var1
 * - var2
 * - vector<bool> info: is modified in place, and stores info
 *  - info[0] is true iff this pairing is viable (v1/v2 and -v1/-v2)
 *  - info[1] is true iff this pairing is viable (v2/v1 and -v2/-v1)
 *  - info[2] is true iff this pariing is viable (v1/-v2 and -v1/v2)
 *  - info[3] is true iff this pariing is viable (-v2/v1 and v2/-v1)
 * - print: prints extra logs
 * 
 * Returns:
 * - bool: True if the vars can be folded to share a column
*/
bool Circuit::checkVarsFold(int var1, int var2, vector<bool>& info, bool print) {
    Column* c1 = lit_col[var1];
    Column* c1bar = lit_col[-1*var1];
    Column* c2 = lit_col[var2];
    Column* c2bar = lit_col[-1*var2];

    // Return false if either var is already folded
    // (can implement in future)
    if(c1->isFolded() || c1bar->isFolded()) return false;
    if(c2->isFolded() || c2bar->isFolded()) return false;


    // If the variables are not disjoint, can't fold
    int varsDisjointCombo = areVarsDisjoint(formula, var1, var2);
    if(varsDisjointCombo == 0) {
        if(print) {
            cout << "Variables " << var1 << " and " << var2 << " are not disjoint." << endl;
        }
        return false;
    }

    info.clear();

    // Check pairing of var1 & var2, -var1 & -var2 (validate clauses)

    bool check1 = false, check2 = false;
    if(varsDisjointCombo == 1 || varsDisjointCombo == 3) {
        // Check with var1 as upper
        check1 = checkPairing(var1, var2, print) && checkPairing(-var1, -var2, print);
        // Check with var2 as upper
        check2 = checkPairing(var2, var1, print) && checkPairing(-var2, -var1, print);
    }

    bool pairing1 = check1 || check2;
    info.push_back(check1);
    info.push_back(check2);

    
    // Check pairing of var1 & -var2, -var1 & var2 (validate clauses)
    bool check3 = false, check4 = false;
    if(varsDisjointCombo == 2 || varsDisjointCombo == 3) {
        // Check with var1 as upper
        check3 = checkPairing(var1, -var2, print) && checkPairing(var1, -var2, print);
        // Check with var2 as upper
        check4 = checkPairing(-var2, var1, print) && checkPairing(var2, -var1, print);
    }

    bool pairing2 = check3 || check4;

    info.push_back(check3);
    info.push_back(check4);


    // If pairing 1 is needed
    if(varsDisjointCombo == 1) {
        return pairing1;
    }
    // If pairing 2 is needed
    if(varsDisjointCombo == 2) {
        return pairing2;
    }
    // Else, either pairing works
    return pairing1 || pairing2;
}


/**
 * Creates fold and updates internal states, given orientation.
 * 
 * Params:
 * - u: upper literal
 * - l: lower literal
 * - print: show more info
 * 
 * Returns:
 * - bool: True if fold was successful
*/
bool Circuit::makeFoldH(int u, int l, bool print) {
    if(print) {
        cout << "Making fold for " << u << " / " << l << endl;
    }

    // If either literal is missing, can finish trivially
    if(lit_clauses.find(u) == lit_clauses.end() || lit_clauses.find(l) == lit_clauses.end()) {
        if(print) cout << "Folding finished trivially." << endl;
        return true;
    }
    
    int uClauses = lit_clauses[u].size();
    int lClauses = lit_clauses[l].size();

    // Update clause spans
    if(print) cout << "Updating clause spans ... " << endl;
    for(int c : lit_clauses[u]) {
        if(print) {
            cout << "Clause " << c << ": (" << clause_spans[c].first << ", " << clause_spans[c].second << ")  ->  ";
            cout << "(" << clause_spans[c].first << ", " << clauses - lClauses - 1 << ")" << endl;
        }
        clause_spans[c].second = clauses - lClauses - 1;
    }
    for(int c : lit_clauses[l]) {
        if(print) {
            cout << "Clause " << c << ": (" << clause_spans[c].first << ", " << clause_spans[c].second << ")  ->  ";
            cout << "(" << uClauses << ", " << clause_spans[c].second << ")" << endl;
        }
        clause_spans[c].first = uClauses;
    }

    // Update Columns for the upper line
    lit_col[u]->makeFold(l, uClauses, lClauses);

    // Update lower literal to refer to correct column
    delete lit_col[l];
    lit_col[l] = lit_col[u];

    if(print) cout << endl;

    return true;
}


/**
 * Creates fold and updates internal state. If multiple folds possible, one
 * chosen arbitrarily.
 * 
 * Params:
 * - var1
 * - var2
 * - print: extra information printed to console
 * 
 * Returns:
 * - bool: true if fold was successful
*/
bool Circuit::makeFold(int var1, int var2, bool print) {
    // Check foldability
    vector<bool> info;
    bool possible = checkVarsFold(var1, var2, info, print);
    for(auto i : info) cout << i << " ";
    cout << "--------------\n" << endl;

    // Return false if can't fold
    if(!possible) {
        if(print) {
            cout << "Folding FAILED: variables " << var1 << " and " << var2 << " appear in the same clause." << endl;
        }
        return false;
    }


    // If v1/v2 and -v1/-v2 folding is possible
    if(info[0]) {
        makeFoldH(var1, var2, print);
        makeFoldH(-var1, -var2, print);
    }
    // Else if v2/v1 and -v2/-v1 folding is possible
    else if(info[1]) {
        makeFoldH(var2, var1, print);
        makeFoldH(-var2, -var1, print);
    }
    // Else if v1/-v2 and -v1/v2 folding is possible
    else if(info[2]) {
        makeFoldH(var1, -var2, print);
        makeFoldH(-var1, var2, print);
    }
    // Else, make -v2/v1 and v2/-v1 folding
    else {
        makeFoldH(-var2, var1, print);
        makeFoldH(var2, -var1, print);
    }

    // Fold successful
    return true;
}

/**
 * Helper function for implement(), uses backtracking search
 * 
 * Params:
 * - curr_ind: current index in clause_positions
 * - clause_positions: the row index is the index, the value is the clause number
 * - possible_clauses: shows which clauses can be placed in each index
 * - placed: set of placed clause nums
 * 
 * Returns:
 * - bool: true if placement was successful
 */
bool Circuit::implementH(int curr_ind, vector<int>& clause_positions, map<int, set<int>>& possible_clauses, unordered_set<int>& placed, bool print) {
    // Base case
    if(curr_ind >= clauses) {
        return true;
    }

    // Remove all options that have been used
    // vector<int> remove;
    // for(int c : possible_clauses[curr_ind]) {
    //     if(placed.find(c) != placed.end()) {
    //         remove.push_back(c);
    //     }
    // }
    // for(int val : remove) possible_clauses[curr_ind].erase(val);

    // // If no options for curr 
    // if(possible_clauses[curr_ind].size() == 0) {
    //     return false;
    // }

    // // Iterate through options for curr
    // for(int option : possible_clauses[curr_ind]) {
    //     // Place the variable
    //     clause_positions[curr_ind] = option;
    //     placed.insert(option);

    //     // Recurse
    //     bool result = implementH(curr_ind+1, clause_positions, possible_clauses, placed, print);

    //     if(result) return true;

    //     // Move on to next option
    //     clause_positions[curr_ind] = -1;
    //     placed.erase(option);

    // }

    return false;
    
}

/**
 * Implement a Circuit into a specific order
 * 
 * Params:
 * - print: print logs
 * 
 * Return:
 * - vector<int>: vec[i] is the clause number on the i-th row
*/
vector<int> Circuit::implement(bool print) {
    vector<int> clause_positions(clauses, -1);

    // Create data structure that goes from index number to possible clauses that go there
    map<int, set<int>> possible_clauses;

    for(pair<int, pair<int, int>> p : clause_spans) {
        for(int i = p.second.first; i <= p.second.second; ++i) {
            possible_clauses[i].insert(p.first);
        }
    }

    // Maintain set of placed clauses
    unordered_set<int> placed;

    implementH(0, clause_positions, possible_clauses, placed, print);
    return clause_positions;
}


// Prints a Circuit
ostream &operator<<(ostream &os, Circuit const &cir) { 
    os << endl << "Filename: " << cir.filename << endl;
    os << "Vars: " << cir.vars << "\tClauses: " << cir.clauses << endl << endl;

    os << "Lines: " << endl;
    // For each var, print its column
    unordered_set<int> printed_lits;
    for(int lit = 1; abs(lit) <= cir.vars; ) {
        // Only print if not printed yet
        if(printed_lits.find(lit) == printed_lits.end()) {
            cout << "Col with lits: ";
            // All literals that share a column with "lit" (including "lit")
            for(int l : cir.lit_col.at(lit)->lits) {
                cout << l << " ";
                printed_lits.insert(l);
                if(cir.lit_clauses.find(l) == cir.lit_clauses.end()) cout << "(is empty) ";
            }
            cout << "\n\tLine Cut-Off: " << cir.lit_col.at(lit)->minUpperLine << " - " << cir.lit_col.at(lit)->maxUpperLine << endl;
        }

        // Iterate properly
        if(lit > 0) lit *= -1;
        else lit = (-1*lit) + 1;
    }

    // For each clause, print its span
    os << endl << "Clause Spans: " << endl;
    for(pair<int, pair<int, int>> p : cir.clause_spans) {
        os << "  " << p.first << ":\t(" << p.second.first << ", " << p.second.second << ")" << endl;
    }

    return os;
}




// Line constructor
Line::Line(int l, int c, int start, int end, int row_n) {
    lit = l;
    col = c;
    start_row = start;
    end_row = end;
    row_num = row_n;
}

// Architecture constructor
Architecture::Architecture(int v, int c) {
    vars = v;
    clauses = c;
}

// Architecture destructor
Architecture::~Architecture() {
    for(pair<int, vector<Line*>> p : lines) {
        for(Line* l : p.second) {
            delete l;
        }
    }
}

/**
 * Creates equal-length lines for an Architecture
 * 
 * Params:
 * - vector<int> numLines: the list of number of lines, in order
 *  Ex: [1, 1, ... 1] means all columns have unfolded lines
 *  Ex: [2, 2, ... 2] means all columns have 2 equal-length lines
 * 
 * Returns:
 * - true if the creation was successful, 
 * false if the number of literals was not the same as the number of lines
*/
bool Architecture::createEqualLines(vector<int> numLines) {
    // Check if number of literals is same as number of lines
    int count = 0;
    for(int num : numLines) count += num;
    if(count != 2*vars) {
        cout << "FAILED: number of lines needs to be same as number of 2*vars" << endl;
        cout << count << " vs. " << 2*vars << endl;
        return false;
    }

    // Create lines
    for(int c = 0; c < numLines.size(); ++c) {
        vector<Line*> v;
        double line_size = (1.0*clauses)/(1.0*numLines[c]);

        double curr_row = 0;
        for(int i = 0; i < numLines[c]; ++i) {
            // Create line
            double start = curr_row, end = curr_row + line_size - 1;
            Line* line = new Line(0, c, start, end, i);

            v.push_back(line);
            line_ids.push_back(line);

            curr_row += line_size;
        }
        lines[c] = v;
    }

    return true;
}

/**
 * Order variables based on greedy heuristic of occurrences
 * - Each positive and negative literal counts as an occurrence towards that variable
 * 
 * Params:
 * - vector<vector<int>> formula: the SAT formula
 *   - must have same number of variables as "vars"
 * - bool descending: if true, ranks from most to least occurrences
 * 
 * Returns:
 * - list<int>: returns list of variables
 *  the first val will be the variable that appears in the most clauses
*/
list<int> Architecture::orderVars(vector<vector<int>>& formula, bool descending) {
    map<int, int> occ; // occ[i] is the number of clauses that var i appears in

    // Add occurrences of all positive and negative literals
    for(vector<int> c : formula) {
        for(int l : c) {
            ++occ[abs(l)];
        }
    }

    // Order the literals by occurrence
    priority_queue<pair<int, int>> tmp;
    for(pair<int, int> p : occ) {
        tmp.push(make_pair(p.second, p.first));
    }

    list<int> result;
    while(!tmp.empty()) {
        pair<int, int> p = tmp.top();
        tmp.pop();
        result.push_back(p.second);
    }

    if(!descending) {
        reverse(result.begin(), result.end());
    }

    return result;
}


/**
 * Order spans of lines by length of the span
 * 
 * Params:
 * - map<pair<int, int>, int>: shows each set of lines (each unique span)
 * - descending: if false, ranks from shortest line spans to largest
*/
vector<pair<int, int>> Architecture::orderLinesMap(map<pair<int, int>, int>& lines_map_assigned, bool descending) {
    priority_queue<pair<int, pair<int, int>>> tmp;
    for(auto p : lines_map_assigned) {
        int len = p.first.second - p.first.first;
        tmp.push(make_pair(len, p.first));
    }

    vector<pair<int, int>> result;
    while(tmp.size()) {
        auto p = tmp.top();
        tmp.pop();
        result.push_back(p.second);
    }

    if(!descending) {
        reverse(result.begin(), result.end());
    }

    return result;
}


/**
 * Helper function to check if a literal can be placed in a certain line and update spans
 * 
 * Params:
 * - lit: the literal to be added
 * - start_r: the start row of the line where the literal will be added
 * - end_r: the end row of the line where the literal will be added
 * - lit_clauses: provides information of the formula 
 * - spans: clause spans
 * 
 * Returns:
 * - bool: true if the span update was successful
 * - modifies spans in place
 * 
*/
bool Architecture::updateSpans(int lit, int start_r, int end_r, map<int, pair<int, int>>& spans) {
    // Iterate through all clauses where this var is
    for(int c : lit_clauses[lit]) {
        int span_start = spans[c].first, span_end = spans[c].second;

        // Check for conflicts
        if(span_start > end_r || span_end < start_r) {
            return false;
        }

        // Update span 
        spans[c].first = max(start_r, span_start);
        spans[c].second = min(end_r, span_end);
    }

    return true;
}


/**
 * Helper function for implementFormulaOld() that implements backtrack search
 * 
 * Params:
 * - var_order: the order of variables to implement
 * - line_order: the order of lines to add literals to
 * - curr_ind: the current index of line_order that we are on
 * - lit_clauses: provides info on the clauses that each literal is a part of 
 * - spans: the clause spans
 * 
 * Returns:
 * - bool: true if the implement was successful
 *   - if true, clause_spans data member is updated
*/
bool Architecture::backtrackOld(list<int> var_order, vector<Line*> line_order, int curr_ind, map<int, pair<int, int>> spans) {

    if(debug) {
        cout << "Backtrack for line " << curr_ind;
        if(line_order.size() > curr_ind)  cout << " (col = " << line_order[curr_ind]->col << ", rows = (" << line_order[curr_ind]->start_row << ", " << line_order[curr_ind]->end_row << "))";
        cout << ": " << endl;

    }

    // Base case - finished search (check clause placements)
    if(curr_ind >= line_order.size()) {
        clause_spans = spans;
        if(debug) cout << "\t(" << curr_ind << ") Backtrack finished, checking clause placement" << endl;

        map<int, int> tmp_assignments;
        bool result = placeClauses(clause_spans, tmp_assignments);
        
        if(result) {
            if(debug) cout << "\t(" << curr_ind << ") Clause placement successful" << endl;
            row_to_clause = tmp_assignments;
            return true;
        } else {
            if(debug) cout << "\t(" << curr_ind << ") Clause placement not successful" << endl;
            return false;
        }
    }
    // Base case - line already filled 
    if(line_order[curr_ind]->lit != 0) {
        if(debug) cout << "\t(" << curr_ind << ") Line already filled with lit " << line_order[curr_ind]->lit << endl;
        return backtrackOld(var_order, line_order, curr_ind+1, spans);
    }

    // Find line of next column (for the second literal to be added)
    int curr_col = line_order[curr_ind]->col;
    int row_n = line_order[curr_ind]->row_num;

    // If line 2 doesn't exist
    // cout << lines.size() << " " << curr_col+1 << " " << lines[curr_col+1].size() << " " << row_n << endl;
    if(lines.find(curr_col+1) == lines.end() || lines[curr_col+1].size() <= row_n) {
        if(debug) cout << "\t(" << curr_ind << ") Line 2 does not exist!" << endl;
        return false;
    }
    // If line 2 already has a lit
    if(line_order[curr_ind]->lit != 0) {
        if(debug) cout << "\t(" << curr_ind << ") Line 2 already has a var" << endl;
        return false;
    }

    if(debug) cout << "\t(" << curr_ind << ") Assigning line 2 with col " << curr_col+1 << " and row num " << row_n << endl;
    Line* line2 = lines[curr_col+1][row_n];

    // Iterate through variables in the provided order
    list<int>::iterator it = var_order.begin();
    while(it != var_order.end()) {
        int curr = *it;

        if(debug) cout << "\t(" << curr_ind << ") Attempting variable " << curr << endl;

        // Check if this literal can be placed in this line, and update spans
        if(debug) cout << "\t(" << curr_ind << ") Updating spans for " << curr << endl;
        map<int, pair<int, int>> spans_copy = spans;
        bool b = updateSpans(curr, line_order[curr_ind]->start_row, line_order[curr_ind]->end_row, spans_copy);
        if(!b) {
            // Skip to next possible var if curr does not work
            if(debug) cout << "\t\t(" << curr_ind << ") Does not work" << endl;
            ++it;
            continue;
        }

        // Check if negative literal can be placed on an adjacent line // Check if negative literal can be placed on an adjacent line 
        if(debug) cout << "\t(" << curr_ind << ") Updating spans for " << -1*curr << endl;
        b = updateSpans(-1*curr, line2->start_row, line2->end_row, spans_copy);
        if(!b) {
            // Skip to next possible var if curr doesn't work
            if(debug) cout << "\t\t(" << curr_ind << ") Does not work" << endl;
            ++it;
            continue;
        }

        // Remove curr int from list, temporarily 
        it = var_order.erase(it);

        // Place lits in line
        line_order[curr_ind]->lit = curr;
        line2->lit = curr * -1;

        // Recurse
        if(debug) cout << "\t(" << curr_ind << ") Recurse: " << endl;
        bool result = backtrackOld(var_order, line_order, curr_ind+1, spans_copy);
        if(debug) cout << "\t(" << curr_ind << ") Recurse done" << endl;

        if(result) {
            if(debug) cout << "(" << curr_ind << ") Return true" << endl;
            return true;
        }

        // Remove literals from line 
        line_order[curr_ind]->lit = 0;
        line2->lit = 0;

        // Re-add variable to the list and iterate 
        it = var_order.insert(it, curr);
        ++it;
    }

    if(debug) cout << "(" << curr_ind << ") Return false" << endl;
    return false;
}



/**
 * Implements a SAT formula onto an Architecture
 * Uses greedy heuristic (insert literal with most clauses) + backtracking search
 * Requires even amount of columns: for all even i, that each line in column i has an equal-length counterpart in column i+1 
 * 
 * Params:
 * - vector<vector<int>> formula: the SAT formula
 * - v: number of variables
 * 
 * Returns:
 * - bool: true if implement was successful
*/
bool Architecture::implementFormulaOld(vector<vector<int>>& formula, int v) {
    lit_clauses.clear();

    // Assert that number of variables matches
    if(v != vars) {
        cout << "Implement FAILED: number of variables does not match" << endl;
        return false;
    }

    // Assert that number of clauses match 
    if(formula.size() != clauses) {
        cout << "Implement FAILED: number of clauses does not match" << endl;
        return false;
    }

    // Order vars for backtrack search 
    list<int> var_order = orderVars(formula);

    // Create order of lines (keep in order of lengths)
    vector<Line*> line_order = line_ids;


    // Generate initial clause spans
    map<int, pair<int, int>> spans;
    for(int c = 1; c <= clauses; ++c) {
        spans[c] = make_pair(0, clauses-1);
    }

    // Map from literals to the clauses they are in 
    for(int c = 0; c < clauses; ++c) {
        for(int l : formula[c]) {
            lit_clauses[l].insert(c+1);
        }
    }

    // Start backtrack search
    bool result = backtrackOld(var_order, line_order, 0, spans);

    return result;
}


/**
 * Helper function for implementFormula() that implements backtrack search
 * 
 * Params:
 * - curr_ind: the current index of var_order that we are on
 * - spans: the clause spans
 * - lit_spans: variable spans
 * 
 * Returns:
 * - bool: true if the implement was successful
 *   - if true, clause_spans data member is updated
*/
bool Architecture::backtrack(int curr_ind,  map<int, pair<int, int>> spans) {
    if(debug) {
        cout << currTimestamp() << "Backtrack for index " << curr_ind;
        if(var_order.size() > curr_ind)  cout << " (var = " << var_order[curr_ind] << ") ";
        cout << ": " << endl;
    }
    ++recursions_made;

    // Base case - finished search (check clause placements)
    if(curr_ind >= var_order.size()) {
        clause_spans = spans;
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Backtrack finished, checking clause placement" << endl;

        // Check for contradictions
        if(areSpanContradictions(spans)) {
            if(debug) cout << currTimestamp() <<  "\t(" << curr_ind << ") Clause placement unsuccessful (contradictions)" << endl;
            return false;
        }

        // No contradiction found - create assignments
        map<int, int> tmp_assignments;
        bool result = placeClauses(clause_spans, tmp_assignments);
        if(result) {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement successful" << endl;
            row_to_clause = tmp_assignments;
            return true;
        } else {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement not successful" << endl;
            return false;
        }
    }

    // Get current variable
    int curr_var = var_order[curr_ind];

    // Iterate through line sets in provided order
    for(pair<int, int> curr_pair : lines_map_order) {
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Attempting lines with span (" << curr_pair.first << ", " << curr_pair.second << ")" << endl;

        // If all lines for this line set already placed, skip to next set
        int assigned = lines_map_assigned[curr_pair];
        if(assigned >= lines_map[curr_pair].size()) {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Lines set already fully assigned" << endl;
            continue;
        }
        
        // Check if this literal can be placed in this line, and update spans
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating clause spans for " << curr_var << endl;
        map<int, pair<int, int>> spans_copy = spans;
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            bool b = updateSpans(curr_var, curr_pair.first, curr_pair.second, spans_copy);
            if(!b) {
                // Skip to next possible line if curr does not work
                if(debug) cout << currTimestamp() << "\t\t(" << curr_ind << ") Does not work" << endl;
                continue;
            }

            // Check if negative literal can be placed on an adjacent line 
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating clause spans for " << -1*curr_var << endl;
            b = updateSpans(-1*curr_var, curr_pair.first, curr_pair.second, spans_copy);
            if(!b) {
                // Skip to next possible line if curr doesn't work
                if(debug) cout << currTimestamp() << "\t\t(" << curr_ind << ") Does not work" << endl;
                continue;
            }
        }

        // Select the two lines
        Line* line1 = lines_map[curr_pair][assigned];
        Line* line2 = lines_map[curr_pair][assigned+1];

        if(debug) {
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 1 is assignment " << assigned << " out of " << lines_map[curr_pair].size()-1 << endl;
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 2 is assignment " << assigned+1 << " out of " << lines_map[curr_pair].size()-1 << endl;
        }

        // Place lits in line
        line1->lit = curr_var;
        line2->lit = curr_var * -1;

        // Mark both lines as assigned
        lines_map_assigned[curr_pair] += 2;

        // Mark var as assigned
        vars_assigned.insert(curr_var);

        // Recurse
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse: " << endl;
        bool result = backtrack(curr_ind+1, spans_copy);
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse done" << endl;

        if(result) {
            if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return true" << endl;
            return true;
        }

        // Unassign var
        vars_assigned.erase(curr_var);

        // Remove literals from line 
        line1->lit = 0;
        line2->lit = 0;

        // Mark both lines as unassigned
        lines_map_assigned[curr_pair] -= 2;
    }
    
    
    if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return false" << endl;
    return false;
}



/**
 * Implements a SAT formula onto an Architecture
 * More efficient - considers equal lines to be the same
 * Efficiency - x^n where x is the number of TYPES of lines (upper half, lower half, full)
 * 
 * Params:
 * - vector<vector<int>> formula: the SAT formula
 * - v: number of variables
 * - descending: if false, checks shortest lines first with least used vars
 * 
 * Returns:
 * - bool: true if implement was successful
*/
bool Architecture::implementFormula(vector<vector<int>>& formula, int v, bool descending) {
    // Set start time
    start = chrono::high_resolution_clock::now();

    // Set formula
    sat_formula = formula;

    // Clear other helper vars
    var_order.clear();
    lines_map.clear();
    lines_map_order.clear();
    lit_clauses.clear();
    lines_map_assigned.clear();
    vars_assigned.clear();
    // lit_neighbors.clear();
    recursions_made = 0;
    
    // Assert that number of variables matches
    if(v != vars) {
        cout << "Implement FAILED: number of variables does not match" << endl;
        return false;
    }

    // Assert that number of clauses match 
    if(formula.size() != clauses) {
        cout << "Implement FAILED: number of clauses does not match" << endl;
        return false;
    }

    // Order vars for backtrack search in reverse order
    list<int> var_order_list = orderVars(formula, descending);

    // Create order of lines (keep in order of lengths)
    vector<Line*> line_order = line_ids;

    // Maintain sets of lines based on each span (span is only distinguishing characteristic of each Line)
    for(Line* l : line_order) {
        lines_map[make_pair(l->start_row, l->end_row)].push_back(l);
    }

    // Make sure each set has even number of lines
    for(auto p : lines_map) {
        int len = p.second.size();
        if(len % 2 == 1) {
            cout << "FAIL: each line set must have even number of lines" << endl;
            return false;
        }
    }

    // Maintain how many lines in each set have been assigned
    for(auto p : lines_map) {
        lines_map_assigned[p.first] = 0;
    }

    // Order the sets of lines in order to assign
   lines_map_order = orderLinesMap(lines_map_assigned, descending);


    // Generate initial clause spans
    map<int, pair<int, int>> spans;
    for(int c = 1; c <= clauses; ++c) {
        spans[c] = make_pair(0, clauses-1);
    }

    // Map from literals to the clauses they are in 
    for(int c = 0; c < clauses; ++c) {
        for(int l : formula[c]) {
            lit_clauses[l].insert(c+1);
        }
    }

    // Set var order
    for(int v : var_order_list) var_order.push_back(v);

    // Start backtrack search
    bool result = backtrack(0, spans);

    // Set end time
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << endl << "Recursions Made: " << recursions_made << endl;
    if(result) {
        cout << "TIME TAKEN: " << duration.count() << " seconds" << endl;
    } else {
        cout << "FAILED in " << endl << duration.count() << " seconds" << endl;
    }

    return result;
}






/**
 * Helper function for implementFormulaPrune() that implements backtrack search
 * Includes pruning - keeps track of literal spans, and updates for each 
 * neighbor of an assigned literal
 * 
 * Params:
 * - curr_ind: the current index of var_order that we are on
 * - spans: the clause spans
 * - lit_banned_lines: banned lines for a literal
 * 
 * Returns:
 * - bool: true if the implement was successful
 *   - if true, clause_spans data member is updated
*/
bool Architecture::backtrackPrune(int curr_ind,  map<int, pair<int, int>> spans, map<int, set<pair<int, int>>>& lit_banned_lines) {
    if(debug) {
        cout << currTimestamp() << "Backtrack (w/prune) for index " << curr_ind;
        if(var_order.size() > curr_ind)  cout << " (var = " << var_order[curr_ind] << ") ";
        cout << ": " << endl;
    }
    ++recursions_made;

    // Base case - finished search (check clause placements)
    if(curr_ind >= var_order.size()) {
        clause_spans = spans;
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Backtrack finished, checking clause placement" << endl;

        // Check for contradictions
        if(areSpanContradictions(spans)) {
            if(debug) cout << currTimestamp() <<  "\t(" << curr_ind << ") Clause placement unsuccessful (contradictions)" << endl;
            return false;
        }

        // No contradiction found - create assignments
        map<int, int> tmp_assignments;
        bool result = placeClauses(clause_spans, tmp_assignments);
        if(result) {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement successful" << endl;
            row_to_clause = tmp_assignments;
            return true;
        } else {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement not successful" << endl;
            return false;
        }
    }

    // Get current variable
    int curr_var = var_order[curr_ind];

    // Iterate through line sets in provided order
    for(pair<int, int> curr_pair : lines_map_order) {
        //if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Attempting lines with span (" << curr_pair.first << ", " << curr_pair.second << ")" << endl;

        // If all lines for this line set already placed, skip to next set
        int assigned = lines_map_assigned[curr_pair];
        if(assigned >= lines_map[curr_pair].size()) {
            //if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Lines set already fully assigned" << endl;
            continue;
        }

        // If this var can't belong in this span, skip
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            if(lit_banned_lines[curr_var].find(curr_pair) != lit_banned_lines[curr_var].end()) {
                if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") banned line for literal " << curr_var << endl;
                continue; 
            }
            if(lit_banned_lines[-1*curr_var].find(curr_pair) != lit_banned_lines[-1*curr_var].end()) {
                if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") banned line for literal " << -1*curr_var << endl;
                continue; 
            }
        }
        
        // Check if this literal can be placed in this line, and update spans
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating clause spans for " << curr_var << endl;
        map<int, pair<int, int>> spans_copy = spans;
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            bool b = updateSpans(curr_var, curr_pair.first, curr_pair.second, spans_copy);
            if(!b) {
                // Skip to next possible line if curr does not work
                if(debug) cout << currTimestamp() << "\t\t(" << curr_ind << ") Does not work" << endl;
                continue;
            }

            // Check if negative literal can be placed on an adjacent line 
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating clause spans for " << -1*curr_var << endl;
            b = updateSpans(-1*curr_var, curr_pair.first, curr_pair.second, spans_copy);
            if(!b) {
                // Skip to next possible line if curr doesn't work
                if(debug) cout << currTimestamp() << "\t\t(" << curr_ind << ") Does not work" << endl;
                continue;
            }
        }

        // Select the two lines
        Line* line1 = lines_map[curr_pair][assigned];
        Line* line2 = lines_map[curr_pair][assigned+1];

        if(debug) {
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 1 is assignment " << assigned << " out of " << lines_map[curr_pair].size()-1 << endl;
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 2 is assignment " << assigned+1 << " out of " << lines_map[curr_pair].size()-1 << endl;
        }

        // Update banned lines for neighbors of curr_var
        map<int, set<pair<int, int>>> lit_banned_lines_copy;
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating banned lines for neighbors for " << curr_var << endl;
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            // Create set of all neighbors
            unordered_set<int> all_neighbors;
            for(int neighbor_lit : lit_neighbors[curr_var]) all_neighbors.insert(neighbor_lit);
            for(int neighbor_lit : lit_neighbors[curr_var*-1]) all_neighbors.insert(neighbor_lit);

            
            // Generate set of banned liens
            set<pair<int, int>> banned_lines;
            for(pair<int, int> line_pairs : lines_map_order) {
                bool b = spansOverlap(curr_pair, line_pairs);
                if(!b) banned_lines.insert(line_pairs);
            }
            
            for(int neighbor_lit : all_neighbors) {
                // Add banned lines
                for(pair<int, int> banned_line : banned_lines) {
                    lit_banned_lines_copy[neighbor_lit].insert(banned_line);
                }
            }
        }

        // Place lits in line
        line1->lit = curr_var;
        line2->lit = curr_var * -1;

        // Mark both lines as assigned
        lines_map_assigned[curr_pair] += 2;

        // Mark var as assigned
        vars_assigned.insert(curr_var);

        // Recurse
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse: " << endl;
        bool result = backtrackPrune(curr_ind+1, spans_copy, lit_banned_lines_copy);
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse done" << endl;

        if(result) {
            if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return true" << endl;
            return true;
        }

        // Unassign var
        vars_assigned.erase(curr_var);

        // Remove literals from line 
        line1->lit = 0;
        line2->lit = 0;

        // Mark both lines as unassigned
        lines_map_assigned[curr_pair] -= 2;
    }
    
    if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return false" << endl;

    return false;
}



/**
 * Implements a SAT formula onto an Architecture (clause_spans and lit_spans used)
 * More efficient - considers equal lines to be the same
 * Efficiency - x^n where x is the number of TYPES of lines (upper half, lower half, full)
 * 
 * Params:
 * - vector<vector<int>> formula: the SAT formula
 * - v: number of variables
 * - descending: if false, checks shortest lines first with least used vars
 * 
 * Returns:
 * - bool: true if implement was successful
*/
bool Architecture::implementFormulaPrune(vector<vector<int>>& formula, int v, bool descending) {
    // Set start time
    start = chrono::high_resolution_clock::now();

    // Set formula
    sat_formula = formula;

    // Clear other helper vars
    var_order.clear();
    lines_map.clear();
    lines_map_order.clear();
    lit_clauses.clear();
    lines_map_assigned.clear();
    vars_assigned.clear();
    lit_neighbors.clear();
    recursions_made = 0;
    
    // Assert that number of variables matches
    if(v != vars) {
        cout << "Implement FAILED: number of variables does not match" << endl;
        return false;
    }

    // Assert that number of clauses match 
    if(formula.size() != clauses) {
        cout << "Implement FAILED: number of clauses does not match" << endl;
        return false;
    }

    // Order vars for backtrack search in reverse order
    list<int> var_order_list = orderVars(formula, descending);

    // Create order of lines (keep in order of lengths)
    vector<Line*> line_order = line_ids;

    // Maintain sets of lines based on each span (span is only distinguishing characteristic of each Line)
    for(Line* l : line_order) {
        lines_map[make_pair(l->start_row, l->end_row)].push_back(l);
    }

    // Make sure each set has even number of lines
    for(auto p : lines_map) {
        int len = p.second.size();
        if(len % 2 == 1) {
            cout << "FAIL: each line set must have even number of lines" << endl;
            return false;
        }
    }

    // Maintain how many lines in each set have been assigned
    for(auto p : lines_map) {
        lines_map_assigned[p.first] = 0;
    }

    // Order the sets of lines in order to assign
   lines_map_order = orderLinesMap(lines_map_assigned, descending);


    // Generate initial clause spans
    map<int, pair<int, int>> spans;
    for(int c = 1; c <= clauses; ++c) {
        spans[c] = make_pair(0, clauses-1);
    }

    // Map from literals to the clauses they are in 
    for(int c = 0; c < clauses; ++c) {
        for(int l : formula[c]) {
            lit_clauses[l].insert(c+1);
        }
    }

    // Set var order
    for(int v : var_order_list) var_order.push_back(v);

    // Maintain literal banned lines
    map<int, set<pair<int, int>>> lit_banned_lines;

    // Find neighbors for each literal
    for(pair<int, set<int>> p : lit_clauses) {
        int lit = p.first;
        for(int clause_num : p.second) {
            for(int neighbor : formula[clause_num-1]) {
                if(neighbor != lit) lit_neighbors[lit].insert(neighbor);
            }
        }
    }

    // Start backtrack search
    bool result = backtrackPrune(0, spans, lit_banned_lines);

    // Set end time
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << endl << "Recursions Made: " << recursions_made << endl;
    if(result) {
        cout << "TIME TAKEN: " << duration.count() << " seconds" << endl;
    } else {
        cout << "FAILED in " << duration.count() << " seconds" << endl;
    }

    return result;
}


/**
 * Helper function for implementFormulaLitsOnly() that implements backtrack search
 * Includes pruning - keeps track of literal spans, and updates for each 
 * neighbor of an assigned literal
 * 
 * Params:
 * - curr_ind: the current index of var_order that we are on
 * - lit_banned_lines: banned lines for a literal
 * 
 * Returns:
 * - bool: true if the implement was successful
 *   - if true, clause_spans data member is updated
*/
bool Architecture::backtrackLitsOnly(int curr_ind, map<int, set<pair<int, int>>>& lit_banned_lines) {
    if(debug) {
        cout << currTimestamp() << "Backtrack (w/lits only) for index " << curr_ind;
        if(var_order.size() > curr_ind)  cout << " (var = " << var_order[curr_ind] << ") ";
        cout << ": " << endl;
    }
    ++recursions_made;

    // Base case - finished search (check clause placements)
    if(curr_ind >= var_order.size()) {
        clause_spans = createClauseSpans();

        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Backtrack finished, checking clause placement" << endl;

        // Check for contradictions
        if(clause_spans.size() == 0 || areSpanContradictions(clause_spans)) {
            if(debug) cout << currTimestamp() <<  "\t(" << curr_ind << ") Clause placement unsuccessful (contradictions)" << endl;
            return false;
        }

        // No contradiction found - create assignments
        map<int, int> tmp_assignments;
        bool result = placeClauses(clause_spans, tmp_assignments);
        if(result) {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement successful" << endl;
            row_to_clause = tmp_assignments;
            return true;
        } else {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Clause placement not successful" << endl;
            return false;
        }
    }

    // Get current variable
    int curr_var = var_order[curr_ind];

    // Iterate through line sets in provided order
    for(pair<int, int> curr_pair : lines_map_order) {
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Attempting lines with span (" << curr_pair.first << ", " << curr_pair.second << ")" << endl;

        // If all lines for this line set already placed, skip to next set
        int assigned = lines_map_assigned[curr_pair];
        if(assigned >= lines_map[curr_pair].size()) {
            if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Lines set already fully assigned" << endl;
            continue;
        }

        // If this var can't belong in this span, skip
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            if(lit_banned_lines[curr_var].find(curr_pair) != lit_banned_lines[curr_var].end()) {
                if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") banned line for literal " << curr_var << endl;
                continue; 
            }
            if(lit_banned_lines[-1*curr_var].find(curr_pair) != lit_banned_lines[-1*curr_var].end()) {
                if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") banned line for literal " << -1*curr_var << endl;
                continue; 
            }
        }

        // Select the two lines
        Line* line1 = lines_map[curr_pair][assigned];
        Line* line2 = lines_map[curr_pair][assigned+1];

        if(debug) {
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 1 is assignment " << assigned << " out of " << lines_map[curr_pair].size()-1 << endl;
            cout << currTimestamp() << "\t(" << curr_ind << ") Line 2 is assignment " << assigned+1 << " out of " << lines_map[curr_pair].size()-1 << endl;
        }

        // Update banned lines for neighbors of curr_var
        map<int, set<pair<int, int>>> lit_banned_lines_copy;
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Updating banned lines for neighbors for " << curr_var << endl;
        if(curr_pair.first != 0 || curr_pair.second != clauses - 1) {
            // Create set of all neighbors
            unordered_set<int> all_neighbors;
            for(int neighbor_lit : lit_neighbors[curr_var]) all_neighbors.insert(neighbor_lit);
            for(int neighbor_lit : lit_neighbors[curr_var*-1]) all_neighbors.insert(neighbor_lit);

            
            // Generate set of banned liens
            set<pair<int, int>> banned_lines;
            for(pair<int, int> line_pairs : lines_map_order) {
                bool b = spansOverlap(curr_pair, line_pairs);
                if(!b) banned_lines.insert(line_pairs);
            }
            
            for(int neighbor_lit : all_neighbors) {
                // Add banned lines
                for(pair<int, int> banned_line : banned_lines) {
                    lit_banned_lines_copy[neighbor_lit].insert(banned_line);
                }
            }
        }

        // Place lits in line
        line1->lit = curr_var;
        line2->lit = curr_var * -1;

        // Mark both lines as assigned
        lines_map_assigned[curr_pair] += 2;

        // Mark var as assigned
        vars_assigned.insert(curr_var);

        // Recurse
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse: " << endl;
        bool result = backtrackLitsOnly(curr_ind+1, lit_banned_lines_copy);
        if(debug) cout << currTimestamp() << "\t(" << curr_ind << ") Recurse done" << endl;

        if(result) {
            if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return true" << endl;
            return true;
        }

        // Unassign var
        vars_assigned.erase(curr_var);

        // Remove literals from line 
        line1->lit = 0;
        line2->lit = 0;

        // Mark both lines as unassigned
        lines_map_assigned[curr_pair] -= 2;
    }
    
    
    if(debug) cout << currTimestamp() << "(" << curr_ind << ") Return false" << endl;
    return false;
}

/**
 * Implements a SAT formula onto an Architecture (using only lit_spans)
 * More efficient - considers equal lines to be the same
 * Efficiency - x^n where x is the number of TYPES of lines (upper half, lower half, full)
 * 
 * Params:
 * - vector<vector<int>> formula: the SAT formula
 * - v: number of variables
 * - descending: if false, checks shortest lines first with least used vars
 * 
 * Returns:
 * - bool: true if implement was successful
*/
bool Architecture::implementFormulaLitsOnly(vector<vector<int>>& formula, int v, bool descending) {
    // Set start time
    start = chrono::high_resolution_clock::now();

    // Set formula
    sat_formula = formula;

    // Clear other helper vars
    var_order.clear();
    lines_map.clear();
    lines_map_order.clear();
    lit_clauses.clear();
    lines_map_assigned.clear();
    vars_assigned.clear();
    lit_neighbors.clear();
    recursions_made = 0;
    
    // Assert that number of variables matches
    if(v != vars) {
        cout << "Implement FAILED: number of variables does not match" << endl;
        return false;
    }

    // Assert that number of clauses match 
    if(formula.size() != clauses) {
        cout << "Implement FAILED: number of clauses does not match" << endl;
        return false;
    }

    // Order vars for backtrack search in reverse order
    list<int> var_order_list = orderVars(formula, descending);

    // Create order of lines (keep in order of lengths)
    vector<Line*> line_order = line_ids;

    // Maintain sets of lines based on each span (span is only distinguishing characteristic of each Line)
    for(Line* l : line_order) {
        lines_map[make_pair(l->start_row, l->end_row)].push_back(l);
    }

    // Make sure each set has even number of lines
    for(auto p : lines_map) {
        int len = p.second.size();
        if(len % 2 == 1) {
            cout << "FAIL: each line set must have even number of lines" << endl;
            return false;
        }
    }

    // Maintain how many lines in each set have been assigned
    for(auto p : lines_map) {
        lines_map_assigned[p.first] = 0;
    }

    // Order the sets of lines in order to assign
   lines_map_order = orderLinesMap(lines_map_assigned, descending);

    // Map from literals to the clauses they are in 
    for(int c = 0; c < clauses; ++c) {
        for(int l : formula[c]) {
            lit_clauses[l].insert(c+1);
        }
    }

    // Set var order
    for(int v : var_order_list) var_order.push_back(v);


    // Maintain literal banned lines
    map<int, set<pair<int, int>>> lit_banned_lines;

    // Find neighbors for each literal
    for(pair<int, set<int>> p : lit_clauses) {
        int lit = p.first;
        for(int clause_num : p.second) {
            for(int neighbor : formula[clause_num-1]) {
                if(neighbor != lit) lit_neighbors[lit].insert(neighbor);
            }
        }
    }

    // Start backtrack search
    bool result = backtrackLitsOnly(0, lit_banned_lines);

    // Set end time
    end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    cout << endl << "Recursions Made: " << recursions_made << endl;
    if(result) {
        cout << "TIME TAKEN: " << duration.count() << " seconds" << endl;
    } else {
        cout << "FAILED in " << duration.count() << " seconds" << endl;
    }

    return result;
}



// Gets current timestamp
string Architecture::currTimestamp() {
    stringstream ss;

    end = chrono::high_resolution_clock::now();
    chrono::duration<double> duration = end - start;

    ss << "[" << fixed << setprecision(5) << duration.count() << "] ";
    return ss.str();
}


/**
 * Validates architecture after a backtrack
 * 
 * Returns:
 * - bool: if the implement is valid
*/
bool Architecture::validateImplement() {
    if(debug) cout << "Validating implement..." << endl;
    // Iterate through each clause
    for(int row = 0; row < clauses; ++row) {
        int c = row_to_clause[row];
        if(debug) cout << "Checking row " << row << " = clause " << c << endl;

        unordered_set<int> clause_lits;
        for(int l : sat_formula[c-1]) clause_lits.insert(l);

        // Ensure that all clause lits are available at that row
        for(Line* line : line_ids) {
            if(line->start_row <= row && row <= line->end_row) {
                clause_lits.erase(line->lit);
            }
        }

        // If any clause literals remain, architecture fails
        if(!clause_lits.empty()) {
            if(debug) cout << "\tCheck failed!" << endl;
            return false;
        }

    }

    return true;
}


// Prints an architecture
ostream &operator<<(ostream &os, Architecture const &arc) {
    os << endl << endl;

    os << " ---------------- ARCHITECTURE START ---------------- " << endl;

    os << endl << "vars: " << arc.vars << "\tclauses: " << arc.clauses << endl << endl;

    for(pair<int, vector<Line*>> p : arc.lines) {
        os << "Column " << p.first << ": " << endl;

        for(Line* l : p.second) {
            os << "Line (" << l->start_row << ", " << l->end_row << "): " << l->lit << endl;
        }

        os << endl;
    }

    // Print row to clause if there is any
    if(arc.row_to_clause.size()) {
        os << "Row: Clause" << endl;
        for(pair<int, int> p : arc.row_to_clause) {
            os << p.first << ": " << p.second << endl;
        }
        os << endl;
    }
    // Print clause spans if there are any
    if(arc.clause_spans.size()) {
        os << "Clause Spans: " << endl; 

        for(pair<int, pair<int, int>> p : arc.clause_spans) {
            os << p.first << ": (" << p.second.first << ", " << p.second.second << ")" << endl;
        }

        os << endl;   
    }

    os << " ---------------- ARCHITECTURE END ---------------- " << endl;

    return os;
}



/**
 * Checks for contradictions in clause spans
 * Ex: 5 different clauses have a span of length 4 (like (0, 3))
 * 
 * Params:
 * - clause_spans: the span of each clause
 * 
 * Returns:
 * - bool: true if contradiction exists, false if none found
*/
bool areSpanContradictions(map<int, pair<int, int>>& clause_spans) {
    // Create a map from clause span to occurrences of that span
    map<pair<int, int>, int> span_occurr;
    for(pair<int, pair<int, int>> p : clause_spans) {
        ++span_occurr[p.second];
    }

    // Check for contradiction
    for(pair<pair<int ,int>, int> p : span_occurr) {
        int len = p.first.second - p.first.first + 1;
        if(p.second > len) return true;
    }

    // No contradiction found
    return false;
}


/**
 * Backtracks to place clauses
 * 
 * Params:
 * - clause_spans: the input clause spans
 * - row_to_clause: returned by reference as output
 * - clause_order: order of clauses to place
 * - curr_ind: index of clause_order we are at
 * 
 * Returns:
 * - bool: true iff placement is valid
*/
bool clauseBacktrack(map<int, pair<int, int>>& clause_spans, map<int, int>& row_to_clause, vector<int>& clause_order, int curr_ind) {
    // Base case - search is finished
    if(curr_ind >= clause_order.size()) {
        return true;
    }
    
    int curr_clause = clause_order[curr_ind];

    // Iterate through all possible rows
    for(int r = clause_spans[curr_clause].first; r <= clause_spans[curr_clause].second; ++r) {
        // If row has been occupied already, then skip
        if(row_to_clause.find(r) != row_to_clause.end()) {
            continue;
        }

        // Create placement
        row_to_clause[r] = curr_clause;

        // Backtrack
        bool result = clauseBacktrack(clause_spans, row_to_clause, clause_order, curr_ind+1);

        // If goal state reached
        if(result) {
            return true;
        }

        // Remove temp placement made
        row_to_clause.erase(r);
    }
    // No placement found
    return false;
}


/**
 * Given clause spans, checks if they can be implemented in a specific order of rows or not.
 * 
 * Params:
 * - clause_spans: the input clause spans
 * - row_to_clause: is returned by reference as output
 * 
 * Returns:
 * - bool: true iff the placement was successful
*/
bool placeClauses(map<int, pair<int, int>>& clause_spans, map<int, int>& row_to_clause) {
    // Create an order of clauses - from shortest to longest length of span
    vector<int> clause_order;

    // Sort
    priority_queue<pair<int, int>> pq;
    for(pair<int, pair<int, int>> p : clause_spans) {
        pq.push(make_pair(p.second.first - p.second.second, p.first));
    }
    while(!pq.empty()) {
        pair<int, int> p = pq.top();
        pq.pop();
        clause_order.push_back(p.second);
    }

    return clauseBacktrack(clause_spans, row_to_clause, clause_order, 0);
}


/**
 * Checks if two spans have an overlap
 * 
 * Params:
 * - span1: first span
 * - span2: second span
 * 
 * Returns:
 * - bool: true if there is some overlap (Ex: [1, 3] and [3, 20] overlap)
*/
bool spansOverlap(const pair<int, int> span1, const pair<int, int> span2) {
    int l1 = span1.first, l2 = span2.first, h1 = span1.second, h2 = span2.second;
    if(h1 < l2 || h2 < l1) return false;
    return true;
}


/**
 * Gets overlap between two spans
 * 
 * Params:
 * - span1: first span
 * - span2: second span
 * 
 * Returns:
 * - pair<int, int>: overlap of both spans, or (-1, -1) if disjoint
 * 
*/
pair<int, int> getSpansOverlap(const pair<int, int> span1, const pair<int, int> span2) {
    int low = max(span1.first, span2.first);
    int high = min(span1.second, span2.second);

    if(low >= high) {
        return make_pair(-1, -1);
    } else {
        return make_pair(low, high);
    }
}



/**
 * Creates clause spans from literal placements on lines, and the SAT formula
 * 
 * Returns:
 * - map<int, pair<int, int>>: clause spans
*/
map<int, pair<int, int>> Architecture::createClauseSpans() {
    // Create default clause spans
    map<int, pair<int, int>> clause_spans;
    for(int c = 1; c <= sat_formula.size(); ++c) {
        clause_spans[c] = make_pair(0, sat_formula.size() - 1);
    }

    // Create lit spans from lines
    map<int, pair<int, int>> lit_spans;
    for(Line* line : line_ids) {
        lit_spans[line->lit] = make_pair(line->start_row, line->end_row);
    }

    // Take span overlaps for each literal in clause
    for(int c = 0; c < sat_formula.size(); ++c) {
        for(int l : sat_formula[c]) {
            clause_spans[c+1] = getSpansOverlap(clause_spans[c+1], lit_spans[l]);

            // If there is an error
            if(clause_spans[c+1].first == -1) {
                map<int, pair<int, int>> error;
                return error;
            }
        }
    }

    return clause_spans;
}



/**
 * Creates graph from a SAT formula
*/
void Partition::createGraph() {
    // Iterate through each clause to create nodes and edges
    for(vector<int> c : formula) {
        for(int i = 0; i < c.size(); ++i) {
            for(int j = 0; j < c.size(); ++j) {
                if(i == j) continue;

                // Create edge
                int v1 = abs(c[i]), v2 = abs(c[j]);

                edges[v1].insert(v2);
                edges[v2].insert(v1);
            }
        }
    }
}


/**
 * Checks if graph is partitioned using BFS
 * 
 * Params:
 * - unordered_set<int> ignore: variables to ignore
 * 
 * Returns:
 * - vector<int>: sizes of different partitions
*/
vector<int> Partition::partitionBFS(unordered_set<int> ignore) {
    vector<int> partition_sizes;

    // If graph is empty return false
    if(edges.size() == 0) {
        return partition_sizes;
    }

    // Pick start variable of search
    int start = 1;

    // Maintain explored set and BFS queue
    unordered_set<int> explored;
    queue<int> bfs_queue;

    // Mark ignored nodes as explored
    for(int v : ignore) explored.insert(v);

    // Try every start var
    for(int start = 1; start <= vars; ++start) {
        // If already explored, skip 
        if(explored.find(start) != explored.end()) continue;

        explored.insert(start);
        bfs_queue.push(start);

        // Start search
        int curr = 0;
        int partition_size = 0;
        while(bfs_queue.size()) {
            // Pop from queue
            curr = bfs_queue.front();
            bfs_queue.pop();
            ++partition_size;

            // cout << "Popped " << curr << endl;

            // Explore neighbors
            for(int neighbor : edges[curr]) {
                // Add to queue if neighbors not explored yet
                if(explored.find(neighbor) == explored.end()) {
                    bfs_queue.push(neighbor);
                    explored.insert(neighbor);
                }
            }
        }

        // Add partition to set
        partition_sizes.push_back(partition_size);
    }

    return partition_sizes;
}


/**
 * Checks if graph is partitioned using BFS
 * 
 * Returns:
 * - vector<int>: sizes of the different partitions
*/
vector<int> Partition::partitionBFS() {
    unordered_set<int> ignore;
    return partitionBFS(ignore);
}


/**
 * Helper function to generate combinations (to remove)
 * 
 * Params:
 * - result: resulting combinations
 * - combination: current combination
 * - start: index
 * - d: length of combination
 * - vars: total number of vars
 * 
*/
void generateCombinations(set<vector<int>>& result, vector<int>& combination, int start, int d, int vars) {
    if (d == 0) {
        result.insert(combination);
        return;
    }

    for (int i = start; i <= vars; ++i) {
        combination.push_back(i);
        generateCombinations(result, combination, i + 1, d - 1, vars);
        combination.pop_back();
    }
}



/**
 * Finds a (minimal) set of variables to remove from graph to form a partition
 * Uses Iterative Deepening Search
 * 
 * Params:
 * - int start_limit: starting depth of search
 * - int end_limit: ending depth of search
 * - string heur: used for DLS
 *    - "half": find most optimal partition that splits into equal halves
 *    - "until_partition": ignores "start_depth" and "end_depth", and keeps iterating until a partition (of size 2 or more) is found
 *    - other: returns first partition
 * 
 * Returns:
 * - unordered_set<int> of variables to remove
*/
unordered_set<int> Partition::removeAndPartitionIDS(int start_depth, int end_depth, string heur) {
    // Current set of variables to remove
    unordered_set<int> removed_vars;

    // Maintain the best combination and its score so far
    int best_score = INT_MAX;

    // Maintain best partition siezs
    vector<int> best_partition_sizes;

    // Maintain average score
    int total_score = 0;
    int score_count = 0;

    if(heur == "until_partition") {
        start_depth = 1;
        end_depth = INT_MAX;
    }

    // Iterate through all depths
    for(int depth = start_depth; depth <= end_depth; ++depth) {
        if(debug) cout << "Attempting depth " << depth << "..." << endl;

        // Create all combinations of variables to remove of this depth
        set<vector<int>> combinations;
        vector<int> vec;
        generateCombinations(combinations, vec, 1, depth, vars);

        // Check all combinations
        for(vector<int> combination : combinations) {
            unordered_set<int> c;
            for(int v : combination) c.insert(v);

            // Check if partitioned
            vector<int> partition_sizes = removeAndCheckPartition(c, edges);

            // cout << "Curr Combination: ";
            // for(int curr_num : c) cout << curr_num << " ";
            // cout << endl << "Partition Sizes: ";
            // for(int ps : partition_sizes) cout << ps << " ";
            // cout << endl << endl;

            // Check heuristic
            if(heur == "half") {
                // Score the partition by comparing it to half of the number of remaining variables
                // int half = (vars - combination.size()) / 2;
                int half = vars/2;

                int score = 0;
                for(int s : partition_sizes) {
                    score += (s - half)*(s - half);
                }

                // Update if new optimal score found
                if(score < best_score) {
                    best_score = score;
                    removed_vars = c;
                    best_partition_sizes = partition_sizes;
                }

                // Add to count
                total_score += score;
                ++score_count;
            }
            // Until first partition
            else if(heur == "until_partition") {
                if(partition_sizes.size() > 1) {
                    if(debug) {
                        cout << "Best vars to remove: ";
                        for(int v : c) cout << v << " ";
                        cout << endl;
                        cout << "Resulting Partition Sizes: ";
                        for(int ps : partition_sizes) cout << ps << " ";
                        cout << endl << endl;
                    }

                    return c;
                }
            }
            // No heuristic - just return first partition found
            else {
                if(partition_sizes.size() > 1) {
                    return c;
                }
            }
            
        }
    }

    if(debug) {
        cout << "Best vars to remove: ";
        for(int v : removed_vars) cout << v << " ";
        cout << endl;
        cout << "Score: " << best_score << endl;
        cout << "Resulting Partition Sizes: ";
        for(int ps : best_partition_sizes) cout << ps << " ";
        cout << endl << endl;

        cout << "Average Score: " << total_score/score_count << endl;
    }


    return removed_vars;
}


/**
 * Removes variables (by highest degree in graph) until graph is partitioned
 * 
 * 
 * Params:
 * - num_remove: number of vars to remove
 * 
 * Returns:
 * - map<int, vector<int>>: map from # of vars removed to partition sizes
 * 
*/
map<int, vector<int>> Partition::removeAndPartitionGreedy(int num_remove) {
    // Maintain set of vars to remove
    unordered_set<int> remove;

    // Map to return
    map<int, vector<int>> result;

    // Loop indefinitely
    for(int i = 1; i <= num_remove; ++i) {
        // Find the var with most edges
        int curr_var = 0, count = INT_MIN;
        for(pair<int, set<int>> p : edges) {
            if((int)p.second.size() > count) {
                count = p.second.size();
                curr_var = p.first;
            }
        }

        // Remove var
        removeNode(curr_var, edges);
        remove.insert(curr_var);

        // Calculate partitions
        vector<int> partition_sizes = partitionBFS(remove);

        result[i] = partition_sizes;

        if(debug) {
            cout << "Vars Removed: ";
            for(int v : remove) cout << v << " ";
            cout << endl << "Partition Sizes: ";
            for(int ps : partition_sizes) cout << ps << " ";
            cout << endl << endl;
        }
    }

    return result;
}


/**
 * Removes node from graph
*/
void removeNode(int var, map<int, set<int>>& edges) {
    // Remove this var from all neighbors' edges
    for(int n : edges[var]) {
        edges[n].erase(var);
    }

    // Remove var
    edges.erase(var);
}

/**
 * Check for partition after removing vars
 * 
 * Params:
 * - unordered_set<int> remove: variables to remove
 * - map<int, set<int>> edges_copy: graph
 * 
 * Returns:
 * - vector<int> partition sizes
*/
vector<int> removeAndCheckPartition(unordered_set<int> remove, map<int, set<int>>& edges) {
    // map<int, set<int>> edges = edges_copy;

    // for(int v : remove) {
    //     removeNode(v, edges);
    // }

    Partition p(edges);
    return p.partitionBFS(remove);
}


/**
 * Helper function to determine cost of a partition on the graph
 * Cost - number of edges crossing the partition
 * 
 * Params:
 * - vector<set<int>>: partitioned vars
 * 
 * Returns:
 * - int: cost (number of overlapping edges
*/
int Partition::calculatePartitionCost(vector<set<int>> const partitions) {
    int cost = 0;

    // Count number of edges which cross the partition
    for(int a : partitions[0]) {
        for(int b : partitions[1]) {
            if(edges[a].count(b)) ++cost;
        }
    }


    return cost;
}


/**
 * Helper function to determine cost of a partition on the graph, with removed nodes
 * Cost - number of edges crossing the partition
 * 
 * Params:
 * - vector<set<int>>: partitioned vars
 * - set<int>: removed vars
 * 
 * Returns:
 * - int: cost (number of overlapping edges
*/
int Partition::calculatePartitionCost(vector<set<int>> const partitions, set<int> const removed) {
    int cost = 0;

    // Count number of edges which cross the partition
    for(int a : partitions[0]) {
        // Skip edges that contain a removed var
        if(removed.count(a)) continue;
        for(int b : partitions[1]) {
            if(removed.count(b)) continue;

            // Edge crossing partition found
            if(edges[a].count(b)) ++cost;
        }
    }


    return cost;
}


/**
 * Helper function to swap vars in partition
 * 
 * Params:
 * - vector<set<int>>: partitioned vars
 * - int a: a var in the first partition (to be swapped to second)
 * - int b: a var in second partition (to be swapped to first)
 * 
*/
void Partition::swapBetweenPartitions(vector<set<int>>& partitions, int a, int b) {
    partitions[0].erase(a);
    partitions[1].erase(b);
    partitions[0].insert(b);
    partitions[1].insert(a);
}

/**
 * Helper function to swap vars in any two sets
 * 
 * Params:
 * - set<int> s1: set 1
 * - set<int> s2: set 2
 * - int a: a var to be swapped
 * - int b: a var to be swapped
 * 
*/
void Partition::swapBetweenSets(set<int>& s1, set<int>& s2, int a, int b) {
    // a in s1, b in s2
    if(s1.count(a) && s2.count(b)) {
        s1.erase(a);
        s2.erase(b);
        s1.insert(b);
        s2.insert(a);
    }
    // a in s2, b in s1
    else if(s1.count(b) && s2.count(a)) {
        s1.erase(b);
        s2.erase(a);
        s1.insert(a);
        s2.insert(b);
    }
}


/**
 * Uses Kernighan-Lin algorithm to find a partition of equal size of the graph which 
 * minimizes the amount of edges that cross the partitions
 * 
 * 
 * Returns:
 * - vector<set<int>> - a vector of size 2, representing vars of each partition
*/
vector<set<int>> Partition::kernighanLinAlg() {
    // Arbitrarily create two partitions of equal size
    vector<set<int>> partitions(2);
    for(int v = 1; v <= vars/2; ++v) partitions[0].insert(v);
    for(int v = (vars/2)+1; v <= vars; ++v) partitions[1].insert(v);

    // Create set of vars that can be explored
    unordered_set<int> remaining;
    for(int v = 1; v <= vars; ++v) remaining.insert(v);

    // Maintain a overall minimum cost, and best partition
    vector<set<int>> best_partitions = partitions;
    int minimum_cost = INT_MAX;

    // Iterate until no (or 1) vars remaining
    while(remaining.size() > 1) {
        // Calculate curr cost of partition
        int curr_cost = calculatePartitionCost(partitions);

        if(debug) {
            cout << "Current Partitions: " << endl << partitions;
            cout << "Cost: " << curr_cost << endl << endl;
        }

        // Maintain best cost, and candidates A and B to swap
        int best_cost = INT_MAX;
        int best_a = 0, best_b = 0;

        // Iterate through each var in sets A and B
        set<int> A = partitions[0], B = partitions[1];
        for(int a : A) {
            // Ensure that this var hasn't been chosen yet
            if(remaining.find(a) == remaining.end()) {
                continue;
            }

            for(int b : B) {
                // Ensure that this var hasn't been chosen yet
                if(remaining.find(b) == remaining.end()) {
                    continue;
                }

                // Temporarily swap a and b
                swapBetweenPartitions(partitions, a, b);

                // Check cost
                int new_cost = calculatePartitionCost(partitions);

                // Swap back a and b
                swapBetweenPartitions(partitions, b, a);

                // Update cost if needed
                if(new_cost <= best_cost) {
                    best_cost = new_cost;
                    best_a = a;
                    best_b = b;
                }
            }
        }

        if(debug) {
            cout << "\tBest Swap: " << best_a << " and " << best_b << endl;
            cout << "\tSwapped Cost: " << best_cost << endl << endl;
        }


        // If best cost is same as or worse than curr cost, exit loop (no improvement)
        if(best_cost >= curr_cost) {
            if(debug) cout << "Swap doesn't improve score. Breaking..." << endl << endl;
            break;
        }

        // Make the best swap
        swapBetweenPartitions(partitions, best_a, best_b);

        // Mark best_a and best_b as used
        remaining.erase(best_a);
        remaining.erase(best_b);

        // Update best partitions if needed
        if(best_cost < minimum_cost) {
            minimum_cost = best_cost;
            best_partitions = partitions;
        }

    }

    cout << "Best Partitions: " << endl << best_partitions;
    cout << "Best Cost: " << minimum_cost << endl << endl;

    return best_partitions;
}


/**
 * Modified KL-algorithm to find partition of equal size plus some variables removed to
 * minimize amount of edges that cross the partitions
 * 
 * Params:
 * - int d: denotes number of vars to remove
 * - set<int> removed: used to return by reference the vars to remove
 * 
 * Returns:
 * - vector<set<int>> - a vector of size 2, representing vars of each partition
*/
vector<set<int>> Partition::nodeKLAlg1(int d, set<int>& removed) {
    // Arbitrarily create partitions and set vars to be removed
    vector<set<int>> partitions(2);
    for(int v = 1; v <= (vars-d)/2; ++v) partitions[0].insert(v);
    for(int v = ((vars-d)/2)+1; v <= (vars-d); ++v) partitions[1].insert(v);
    for(int v = vars-d+1; v <= vars; ++v) removed.insert(v);

    // Create set of variables that can be explored
    unordered_set<int> remaining;
    for(int v = 1; v <= vars; ++v) remaining.insert(v);

    // Maintain an overall minimum cost, and best partition
    vector<set<int>> best_partitions = partitions;
    int minimum_cost = calculatePartitionCost(partitions, removed);

    // Iterate until not enough vars remaining
    while(remaining.size() > 0) {
        // Calculate current cost of partition
        int curr_cost = calculatePartitionCost(partitions, removed);

        if(debug) {
            cout << "Current Partitions: " << endl << partitions;
            cout << "Removed: ";
            for(int r : removed) cout << r << " ";
            cout << endl << "Cost: " << curr_cost << endl << endl;
        }

        // Maintain best cost, and candidates x and r to swap 
        // x is either in A or B, and r is in removed
        int best_cost = INT_MAX;
        int best_x = 0, best_r = 0;

        // Iterate through each var that is not in removed (essentially, all vars in A and B)
        for(int x = 1; x <= vars; ++x) {
            if(removed.count(x)) continue;

            // Ensure variable hasn't been used yet 
            if(!remaining.count(x)) continue;

            // Iterate through each var in removed
            set<int> removed_copy = removed;
            for(int r : removed_copy) {
                // Temporarily swap x and r, remembering which set x came from
                bool x_in_A = false;
                if(partitions[0].count(x)) {
                    swapBetweenSets(partitions[0], removed, x, r);
                    x_in_A = true;
                } else {
                    swapBetweenSets(partitions[1], removed, x, r);
                }

                // Check cost
                int new_cost = calculatePartitionCost(partitions, removed);

                // Swap back x and r 
                if(x_in_A) {
                    swapBetweenSets(partitions[0], removed, r, x);
                } else {
                    swapBetweenSets(partitions[1], removed, r, x);
                }

                // Update cost if needed
                if(new_cost <= best_cost) {
                    best_cost = new_cost;
                    best_x = x;
                    best_r = r;
                }

            }
        }

        if(debug) {
            cout << "\tBest Swap: " << best_x << " and " << best_r << endl;
            cout << "\tSwapped Cost: " << best_cost << endl << endl;
        }

        // If best cost same as or worse than curr cost, exit
        if(best_cost >= curr_cost) {
            if(debug) cout << "Swap doesn't improve score. Breaking..." << endl << endl;
            curr_cost = best_cost;
            break;
        }

        // Make the best swap 
        if(partitions[0].count(best_x)) {
            swapBetweenSets(partitions[0], removed, best_x, best_r);
        } else {
            swapBetweenSets(partitions[1], removed, best_x, best_r);
        }

        // Mark best_x as used 
        remaining.erase(best_x);

        // Update best partitions if needed 
        if(best_cost < minimum_cost) {
            minimum_cost = best_cost;
            best_partitions = partitions;
        }

    }

    // cout << "Best Partitions: " << endl << best_partitions;
    // cout << "Remove: ";
    // for(int r : removed) cout << r << " ";
    // cout << endl;
    cout << "Best Cost: " << minimum_cost << endl << endl;

    return partitions;
}


// Print partitions (2 sets in a vector)
ostream &operator<<(ostream &os, vector<set<int>> const &partitions) {
    os << "A: ";
    for(int a : partitions[0]) os << a << " ";
    os << endl << "B: ";
    for(int b : partitions[1]) os << b << " ";
    os << endl;

    return os;
}

// Print a Partition class
ostream &operator<<(ostream &os, Partition const &p) {
    os << "Vars: " << p.vars << " \tClauses: " << p.clauses << endl;

    // Print nodes and their edges
    for(pair<int, set<int>> n : p.edges) {
        cout << n.first << ": ";
        for(int neighbor : n.second) {
            cout << neighbor << " ";
        }
        cout << endl;
    }
    cout << endl;

    return os;
}




