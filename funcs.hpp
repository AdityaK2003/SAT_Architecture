#include <vector>
#include <string>
#include <unordered_set>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;

set<int> findDisjointClauses(vector<vector<int>>& formula, int literal);
bool areLiteralsDisjoint(vector<vector<int>>& formula, int lit1, int lit2);
bool areLiteralsDisjoint(map<int, set<int>>& lit_clauses, int lit1, int lit2);
vector<pair<int, int>> numDisjointLiteralPairs(vector<vector<int>>& formula, int vars);
int totalNumLiteralPairs(int vars);

set<int> findUnipolarLiterals(vector<vector<int>>& formula);
int findUnipolarLiteral(vector<vector<int>>& formula);
set<int> findUnitClauseLiterals(vector<vector<int>>& formula);
int findUnitClauseLiteral(vector<vector<int>>& formula);
void updateFormulaWithLiteral(vector<vector<int>>& formula, int lit);
unordered_set<int> simplify(vector<vector<int>>& formula, bool method1=false, bool print=false);
int renumberFormula(vector<vector<int>>& formula);
void writeFormulaToFile(vector<vector<int>>& formula, int vars, string outputFilename, string description="");

int areVarsDisjoint(vector<vector<int>>& formula, int var1, int var2);
int areVarsDisjoint(map<int, set<int>>& lit_clauses, int var1, int var2);
vector<pair<int, int>> numDisjointVarPairs(vector<vector<int>>& formula, int vars);
int totalNumVarPairs(int vars);

// Representation of a column (can hold multiple lines)
class Column {
public:
    vector<int> lits; // list of literals, in order

    int minUpperLine; // minimum row index of upper line
    int maxUpperLine; // max row index of upper line (if = minUpperLine, then fixed)

    int clauses; // number of clauses in the formula

    Column();
    Column(int l, int c);
    bool isFolded();
    void makeFold(int lit, int min, int max);
};

// Representation for a circuit
class Circuit {
public:
    // Store original formula information
    string filename;
    vector<vector<int>> formula;
    int vars;
    int clauses; // = formula.size()

    // Map from literal number to the column it's on
    map<int, Column*> lit_col;

    // Represents freedom of each clause
    // If clause c must be between rows i and j:
    // map[c] = <i, j>
    // 1 <= c <= clauses
    map<int, pair<int, int>> clause_spans;

    // Map from literal number to set of clauses it is a part of 
    // Clause numbers start from 1
    map<int, set<int>> lit_clauses;

    Circuit(string f);
    Circuit() {
        // Default constructor
        vars = -1;
    };
    Circuit(Circuit& c);
    ~Circuit();

    bool checkPairing(int u, int l, bool print=false);
    bool checkVarsFold(int var1, int var2, vector<bool>& info, bool print=false);
    bool makeFoldH(int u, int l, bool print=false);
    bool makeFold(int var1, int var2, bool print=false);

    bool implementH(int curr_ind, vector<int>& clause_positions, map<int, set<int>>& possible_clauses, unordered_set<int>& placed, bool print);
    vector<int> implement(bool print=false);
};

ostream &operator<<(ostream &os, Circuit const &cir);


// Representation of a line for an Architecture
class Line {
public:
    // Literal (is 0 if none stored yet)
    int lit;

    // Column index of the line
    int col;

    // Row number of line (uppermost = 0, next = 1, ...)
    int row_num;

    // Starting row index of the line (inclusive)
    int start_row;
    // Ending row index of the line (inclusive)
    int end_row;

    // Constructor
    Line(int l, int c, int start, int end, int row_n);
};

// Representation of a SAT formula
class Formula {
public:
    int vars;
    vector<vector<int>> formula;
};


// Representation of a circuit architecture
class Architecture {
public:
    int vars;
    int clauses; // rows are from 0 to clauses-1

    // Maintain list of Lines for each column
    // map[i] is the vector of lines in column i
    map<int, vector<Line*>> lines;

    // Maintain each line pointer at unique index
    vector<Line*> line_ids;

    // Clause spans, used by implementFormula() and overwritten each call 
    map<int, pair<int, int>> clause_spans;

    // Clause orderings, returned by implementFormula()
    map<int, int> row_to_clause;


    // Helper members used by implementFormula()
    vector<vector<int>> sat_formula;
    vector<int> var_order;
    map<pair<int, int>, vector<Line*>> lines_map;
    vector<pair<int, int>> lines_map_order;
    map<int, set<int>> lit_clauses;
    map<pair<int, int>, int> lines_map_assigned;
    unordered_set<int> vars_assigned;
    map<int, set<int>> lit_neighbors;

    // More helper vars
    chrono::high_resolution_clock::time_point start;
    chrono::high_resolution_clock::time_point end;
    int recursions_made;
    bool debug = false;


    // Constructor
    Architecture(int v, int c);
    // Destructor
    ~Architecture();
    
    bool createEqualLines(vector<int> numLines);

    list<int> orderVars(vector<vector<int>>& formula, bool descending=true);
    vector<pair<int, int>> orderLinesMap(map<pair<int, int>, int>& lines_map_assigned, bool descending=false);
    bool updateSpans(int lit, int start_r, int end_r, map<int, pair<int, int>>& spans);
    
    bool backtrackOld(list<int> var_order, vector<Line*> line_order, int curr_ind,  map<int, pair<int, int>> spans);
    bool implementFormulaOld(vector<vector<int>>& formula, int v);
    
    bool backtrack(int curr_ind, map<int, pair<int, int>> spans);
    bool implementFormula(vector<vector<int>>& formula, int v, bool descending=false);

    bool backtrackPrune(int curr_ind, map<int, pair<int, int>> spans, map<int, set<pair<int, int>>>& lit_banned_lines);
    bool implementFormulaPrune(vector<vector<int>>& formula, int v, bool descending=false);

    bool backtrackLitsOnly(int curr_ind,  map<int, set<pair<int, int>>>& lit_banned_lines);
    bool implementFormulaLitsOnly(vector<vector<int>>& formula, int v, bool descending=false);

    string currTimestamp();

    map<int, pair<int, int>> createClauseSpans();
    bool validateImplement();
};

ostream &operator<<(ostream &os, Architecture const &arc);

bool areSpanContradictions(map<int, pair<int, int>>& clause_spans);
bool clauseBacktrack(map<int, pair<int, int>>& clause_spans, map<int, int>& row_to_clause, vector<int>& clause_order, int curr_ind);
bool placeClauses(map<int, pair<int, int>>& clause_spans, map<int, int>& row_to_clause);

bool spansOverlap(const pair<int, int> span1, const pair<int, int> span2);
pair<int, int> getSpansOverlap(const pair<int, int> span1, const pair<int, int> span2);


// Node in Graph representation of SAT circuit
// Represents a variable (both positive and negative literal)
/*
class Node {
public:
    int var;

    // pair<int, int> p where p.first is neighbor's variable, and p.second is the Node*
    map<int, Node*> edges;

    // Constructor
    Node(int v) { var = v; }
}; */

// Class for Partition method of SAT formula clustering
class Partition {
public:
    int vars;
    int clauses;
    vector<vector<int>> formula;

    bool debug;

    // Graph
    map<int, set<int>> edges;


    // Constructor
    Partition(int v, vector<vector<int>> f) {
        vars = v;
        formula = f;
        clauses = formula.size();
        debug = false;

        createGraph();
    }
    // Constructor
    Partition(map<int, set<int>> g) {
        vars = g.size();
        edges = g;
    }

    void createGraph();

    vector<int> partitionBFS(unordered_set<int> ignore);
    vector<int> partitionBFS();

    unordered_set<int> neighborsBFS(int start, int depth);
    int neighborsClauseCount(int start, int depth);


    unordered_set<int> removeAndPartitionIDS(int start_depth, int end_depth, string heur="half");
    map<int, vector<int>> removeAndPartitionGreedy(int num_remove);

    int calculatePartitionCost(vector<set<int>> const partitions);
    void swapBetweenPartitions(vector<set<int>>& partitions, int a, int b);
    vector<set<int>> kernighanLinAlg(bool shuffle_vars=false);

    int calculatePartitionCost(vector<set<int>> const partitions, set<int> const removed);
    void swapBetweenSets(set<int>& s1, set<int>& s2, int a, int b);
    vector<set<int>> nodeKLAlg1(int d, set<int>& removed, bool shuffle_vars=false);


    vector<set<int>> createClausePartition(int num_partitions, bool shuffle_order);
    int clausePartitionCost(vector<set<int>> partitions);
};

void removeNode(int var, map<int, set<int>>& edges);
vector<int> removeAndCheckPartition(unordered_set<int> remove, map<int, set<int>>& edges_copy);

ostream &operator<<(ostream &os, vector<set<int>> const &partitions);
ostream &operator<<(ostream &os, Partition const &p);


// Class for partitioning clauses (inspired from Soowang's power reduction in his thesis)
class ClausePartition {
public:
    bool debug;

    int vars;
    int clauses;
    vector<vector<int>> formula;
    map<int, set<int>> var_clauses;

    vector<set<int>> subarrays; // subarrays[i] is set of all clauses in subarray num i
    unordered_map<int, int> clause_subarrays; // clause_subarrays[i] is the subarray num for clause i


    // Constructor
    ClausePartition(int v, vector<vector<int>> f) {
        vars = v;
        formula = f;
        clauses = formula.size();
        debug = false;

        // Which clauses each var is in
        for(int c = 0; c < formula.size(); ++c) {
            for(int lit : formula[c]) {
                int var = abs(lit);
                var_clauses[var].insert(c+1);
            }
        }
    }

    void createSubarrays(int num, string heur = "round-robin");
    unordered_map<int, set<int>> checkGroupings(int mask);

};


unordered_map<int, int> findGroupsMappingHelper(int num_clauses, bool randomize=false, int num_subarrays=1024, int num_groups=32);
unordered_map<int, set<int>> runGroupsExperiment(int vars, vector<vector<int>>& formula, unordered_map<int, int>& mappings);
void printGroupsResults(unordered_map<int, set<int>> results, int num_groups);


// Class for "divide and conquer" approach: satisfy k variables to create 2^k smaller problems
class DivideFormula {
public:
    int vars;
    int clauses;
    vector<vector<int>> formula;

    int vars_2;
    int clauses_2;
    vector<vector<int>> formula_2;

    // Constructor
    DivideFormula(int v, vector<vector<int>> f) {
        vars = v;
        formula = f;
        clauses = f.size();
    };

    bool removeKLiterals(unordered_set<int> remove, bool recurse, bool renumber);
};


vector<unordered_set<int>> generateLitsCombos(unordered_set<int> remove_vars);
void generateLitsCombosHelper(vector<unordered_set<int>>& answer, vector<int> remove_vars, unordered_set<int> curr, int i);
vector<int> kMostOccurring(int vars, vector<vector<int>>& formula, int k);
void runDivideExperiment(Circuit c, unordered_set<int> remove_vars, bool show_stats=true, bool print=false);
vector<Formula> runDivideExperimentSingleVar(Formula c, int remove_var);
unordered_set<int> kRandomVariables(int vars, int k);


// Class for contiguous partitions (groups created using consecutive clauses)
class ContiguousPartition {
public:
    int vars;
    int clauses;
    vector<vector<int>> formula;

    // group_to_clauses[i] is set of all clauses in group i
    unordered_map<int, set<int>> group_to_clauses;
    // clause_to_group[i] is the group corresponding to clause i
    vector<int> clause_to_group;

    // Constructor
    ContiguousPartition(int v, vector<vector<int>> f) {
        vars = v;
        formula = f;
        clauses = f.size();

        for(int c = 0; c <= clauses; ++c) {
            // Initialize to no group
            clause_to_group.push_back(-1);
        }
    };

    int createPartitionsBySize(int max_vars);
    void evaluateGroupings();

};

void findVarMeanAndSDClause(int var, vector<vector<int>>& formula);
void findAllMeanAndSdClauses(int total_vars, vector<vector<int>>& formula);

int heurFindJeroslowWang(vector<vector<int>>& formula, int vars);
int heurFindMostOccurLit(vector<vector<int>>& formula, int vars);
int heurFindMostOccurVar(vector<vector<int>>& formula, int vars);
int heurFindMOM(vector<vector<int>>& formula, int vars);
void divideAndConquerHeur(Circuit c, int k, string heur, bool print=false);
