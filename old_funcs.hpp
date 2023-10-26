#include <vector>
#include <string>
#include <unordered_set>
#include <queue>
#include <set>
#include <iostream>
#include <fstream>

using namespace std;

void parse(string filename, vector<vector<int>>& formula, int& v, int& c);
void printFormula(vector<vector<int>>& formula);
void printFormula(vector<vector<int>>& formula, int v, int c);
void printSpecificClauses(vector<vector<int>>& formula, unordered_set<int> vars);
int countSpecificClauses(vector<vector<int>>& formula, unordered_set<int> vars);

int balancedSurvey(vector<vector<int>>& formula, int variable);
vector<int> findClauses(vector<vector<int>>& formula, int var, bool print = false);
unordered_set<int> findFriends(vector<vector<int>>& formula, int var, bool print = false);
int clauseValue(vector<vector<int>>& formula, unordered_set<int>& s, int clause);
vector<int> unitClauses(vector<vector<int>>& formula, unordered_set<int>& s);
void unitClauses(vector<vector<int>>& formula, unordered_set<int>& s, queue<int>& q);
vector<int> unitClausesHeur(vector<vector<int>>& formula, unordered_set<int>& s, int a, int b, int c);

int formulaValue(vector<vector<int>>& formula, unordered_set<int>& s, int& conflict, bool print = false);
int formulaValue(vector<vector<int>>& formula, unordered_set<int>& s, bool print = false);
bool varInClause(vector<vector<int>>& formula, int var, int clause);
bool literalInClause(vector<vector<int>>& formula, int literal, int clause);
int numUnassigned(vector<vector<int>>& formula, unordered_set<int>& s, int clause);
void printTime(clock_t start, clock_t end);

int survey(vector<vector<int>>& formula, unordered_set<int>& s, int v);
bool solve(vector<vector<int>>& formula, unordered_set<int>& s, int vars, string heuristic, bool cdcl, bool print, int& counter);

int clauseShout(vector<int>& clause, unordered_set<int>& s);
bool solveCircuit(vector<vector<int>>& formula, unordered_set<int>& s, int vars, int heuristic = 0, bool print = false);

class Task {
public:
    Task(string d, int n);
    bool operator<(const Task& other) const;
};
bool taskMatches(const Task& task, const string& description, int num);
bool solveCircuitDelay(vector<vector<int>>& formula, unordered_set<int>& s, int vars, int d1, int d2, bool print = false);

double run(string filename, string heuristic, int& counter, bool cdcl, bool print);
double runCircuit(string filename, int heuristic = 0, bool print = false);
double runCircuitDelay(string filename, int d1, int d2, bool print = false);

string numToFile(int i, int vars = 20);
void runAll(int min = 1, int max = 1000, int vars = 20, string heuristic = "order", bool cdcl = false, bool print = false);

void generateCombinationsH(vector<int>& v, vector<vector<int>>& combinations, vector<int>& c);
vector<vector<int>> generateCombinations(vector<int> vars);
void nCombosH(vector<vector<int>>& combos, vector<int>& c, int max, int len);
vector<vector<int>> nCombos(int max, int len = 3);
void nCombosH(vector<vector<int>>& combos, vector<int>& c, vector<int>& nums, int len, int ind = 0);
vector<vector<int>> nCombos(vector<int>& nums, int len = 3);
vector<vector<int>> randomCombos(int vars, int total, int len = 3);

bool isClause(vector<vector<int>>& formula, unordered_set<int>& s, int& clause);
bool isClause(vector<vector<int>>& formula, unordered_set<int>& s);
vector<vector<int>> sample(vector<vector<int>>& v, int n);

int countWeightedClauses(vector<vector<int>>& formula, unordered_set<int>& s);
int countPairs(vector<vector<int>>& formula, unordered_set<int>& s);
bool checkPairClause(vector<vector<int>>& formula, unordered_set<int>& s);
int balancedSurveyAll(vector<vector<int>>& formula, unordered_set<int>& s);
int findAux(vector<vector<int>>& formula, unordered_set<int>& s, int mode) ;

bool heur(vector<vector<int>>& formula, vector<vector<int>>& all, bool checkClause, bool print, bool hideAll, int& exp, ofstream* output = nullptr);
bool heur2(vector<vector<int>>& formula, vector<vector<int>>& all, bool checkClause, bool print, bool hideAll, int& exp, int mode = 1, int numAux = 1);

vector<int> mostOccurred(vector<vector<int>>& formula, int vars, int length, int mode);
vector<vector<int>> mostOccurredPairs(vector<vector<int>>& formula, int vars, int length);

int runHeuristic(int fileNumber, int vs, bool hideClauses = false, bool print = false, bool hideAll = false, int len = 3, int samples = -1);
int runHeuristic(int fileNumber, int vs, bool hideClauses, bool print, bool hideAll, int len, vector<vector<int>>& all, bool& conflictCaused, int sampleAmount = -1);
float runHeuristic(int fileNumber, int vs, bool hideClauses, bool print, bool hideAll, int len, vector<vector<int>>& all, int sampleAmount = -1);
vector<vector<int>> pairHeuristic(vector<vector<int>>& formula, int vars, int length, int k = 5);
void runHeurLoop();
void runHeurExp();
void runExp2(int problem, int vars, bool print, int sampleAmount);
void runExp2(int problem, int vars, ofstream& myfile, int sampleAmount);

vector<vector<int>> parseConflicts();
void reverseEngineer(int problem, int vars, int sampleAmount);
void runHeurExp3(int problem, int vars, int mode, bool control, int combs);
int runHeur2(int fileNumber, int vs, vector<vector<int>> all, bool hideClauses = false, bool print = false, bool hideAll = false, int len = 3, int samples = -1, int mode = 1, int numAux = 1);
int runHeur2(int fileNumber, int vs, bool hideClauses = false, bool print = false, bool hideAll = false, int len = 3, int samples = -1, int mode = 1, int numAux = 1);
void runAuxExp(int vars = 20, int max = 20, int samples = -1);

void choose4Exp(int vars, int max, int samples = -1);
void findClusters(int root, int height, unordered_map<int, set<int>>& m, vector<vector<int>>& formula, vector<set<int>>& h, int currHeight = 0);
string numToBigFile(int i);
void largeFilesTesting(int start, int end, int mode = 1);

void solverExp(int vars, int fileNum, int trials=10, ostream& myfile = cout);
void clusterExp(int vars, int i, int root = 0);
void toCSV(int vars, int i);
vector<vector<int>> reorder(vector<vector<int>> formula);

int findSpan(int var, vector<vector<int>> formula);
void printAllSpans(int vars, vector<vector<int>> formula);
int findMaxSpan(int vars, vector<vector<int>> formula);
int unipolarityMetric(int vars, vector<vector<int>>& formula);
int maxOccurrencesMetric(int vars, vector<vector<int>>& formula);
float sdOccurrencesMetric(int vars, vector<vector<int>>& formula);
float avgOccurrencesMetric(int vars, vector<vector<int>>& formula);
float medianOccurrencesMetric(int vars, vector<vector<int>>& formula);
int maxSpanMetric(int vars, vector<vector<int>>& formula);
float avgSpanMetric(int vars, vector<vector<int>>& formula);
float medianSpanMetric(int vars, vector<vector<int>>& formula);
int maxBalanceMetric(int vars, vector<vector<int>>& formula);
int minBalanceMetric(int vars, vector<vector<int>>& formula);
float avgBalanceMetric(int vars, vector<vector<int>>& formula);
float medianBalanceMetric(int vars, vector<vector<int>>& formula);
void unsatisfiedAndSolutionsMetric(int vars, vector<vector<int>>& formula, int n, double& unsatisfiedAvg, double& solutionsPercent);
float solutionsMetric(int vars, vector<vector<int>>& formula, int n);

void removePercent(string filename);
void removePercentFolder(string &folder);

vector<int> createLearntClause(vector<vector<int>>& formula, int vars, bool print = false);
int learntClauseRedundancies(vector<vector<int>>& formula, vector<int> learnt, bool print = false);
vector<vector<int>> parseLearntClauses(string input);

