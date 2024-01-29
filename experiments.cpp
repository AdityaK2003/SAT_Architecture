#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <map>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <list>
#include <chrono>
#include <ctime>
#include <cstdlib>

#include "old_funcs.hpp"
#include "funcs.hpp"

using namespace std;

// Paths
string OSTROWSKI_PATH = "sat_files/nonrandom_sat_files/sat_2002_handmade_ostrowski/";
string MOSOI_PATH = "sat_files/nonrandom_sat_files/sat_2011_crafted_mosoi/";
string SIMPLE_PATH = "sat_files/simple_files/";
string VLSAT2_PATH = "sat_files/nonrandom_sat_files/VLSAT2/";
string VLSAT2_SIMPLIFIED_PATH = "sat_files/nonrandom_sat_files/VLSAT2_simplified/";
string VLSAT1_PATH = "sat_files/nonrandom_sat_files/VLSAT1/";
string SAT2017_PATH = "sat_files/SAT2017_Soowang/";

// Filenames

// Size: 10
vector<string> OSTROWSKI_FILES = {
    "handmade_34_150.cnf", 
    "handmade_64_298.cnf",
    "handmade_97_444.cnf",
    "handmade_149_738.cnf",
    "handmade_185_788.cnf",
    "handmade_249_1118.cnf",
    "handmade_890_4082.cnf",
    "handmade_1566_7016.cnf",
    "handmade_2484_11402.cnf",
    "handmade_3622_17076.cnf",
};
// Size: 15
vector<string> MOSOI_FILES = {
    "mosoi_128_896.cnf",
    "mosoi_160_1400.cnf",
    "mosoi_168_1554.cnf",
    "mosoi_176_1628.cnf",
    "mosoi_192_2016.cnf",
    "mosoi_216_2538.cnf",
    "mosoi_396_8613.cnf",
    "mosoi_440_10670.cnf",
    "mosoi_448_10976.cnf",
    "mosoi_456_11058.cnf",
    "mosoi_468_12051.cnf",
    "mosoi_480_12240.cnf",
    "mosoi_560_15260.cnf",
    "mosoi_672_25200.cnf",
    "mosoi_720_28980.cnf"
};
// Size: 7
vector<string> VLSAT2_FILES = {
    "vlsat2_1000.cnf",
    "vlsat2_1022.cnf",
    "vlsat2_1125.cnf",
    "vlsat2_1134.cnf",
    "vlsat2_1155.cnf",
    "vlsat2_1183.cnf",
    "vlsat2_1209.cnf"
};
// Size: 7
vector<string> VLSAT2_SIMPLIFIED_FILES = {
    "vlsat2_1000_simplified.cnf",
    "vlsat2_1022_simplified.cnf",
    "vlsat2_1125_simplified.cnf",
    "vlsat2_1134_simplified.cnf",
    "vlsat2_1155_simplified.cnf",
    "vlsat2_1183_simplified.cnf",
    "vlsat2_1209_simplified.cnf"
};
// Sorted by number of vars, ascending
// Size: 60
unordered_map<int, string> SAT2017_FILES = {
    {0, "g2-modgen-n200-m90860q08c40-3230.cnf"},
    {1, "g2-modgen-n200-m90860q08c40-13698.cnf"},
    {2, "g2-modgen-n200-m90860q08c40-29667.cnf"},
    {3, "g2-modgen-n200-m90860q08c40-6967.cnf"},
    {4, "g2-modgen-n200-m90860q08c40-16823.cnf"},
    {5, "g2-hwmcc15deep-6s399b02-k02.cnf"},
    {6, "g2-hwmcc15deep-6s399b03-k02.cnf"},
    {7, "g2-hwmcc15deep-6s179-k17.cnf"},
    {8, "g2-Sz512151281.smt2-cvc4.cnf"},
    {9, "g2-hwmcc15deep-6s33-k33.cnf"},
    {10, "g2-hwmcc15deep-6s33-k34.cnf"},
    {11, "g2-gss-28-s100.cnf"},
    {12, "g2-gss-30-s100.cnf"},
    {13, "g2-gss-32-s100.cnf"},
    {14, "g2-gss-34-s100.cnf"},
    {15, "g2-gss-36-s100.cnf"},
    {16, "g2-gss-38-s100.cnf"},
    {17, "g2-gss-40-s100 .cnf"},
    {18, "g2-hwmcc15deep-6s161-k17.cnf"},
    {19, "g2-hwmcc15deep-6s516r-k17.cnf"},
    {20, "g2-hwmcc15deep-6s516r-k18.cnf"},
    {21, "g2-hwmcc15deep-intel066-k10.cnf"},
    {22, "g2-slp-synthesis-aes-top24.cnf"},
    {23, "g2-hwmcc15deep-6s161-k18.cnf"},
    {24, "g2-hwmcc15deep-6s105-k35.cnf"},
    {25, "g2-slp-synthesis-aes-top25.cnf"},
    {26, "g2-slp-synthesis-aes-top26.cnf"},
    {27, "g2-hwmcc15deep-beemcmbrdg7f2-k32.cnf"},
    {28, "g2-slp-synthesis-aes-top28.cnf"},
    {29, "g2-slp-synthesis-aes-top29.cnf"},
    {30, "g2-mizh-md5-47-5.cnf"},
    {31, "g2-mizh-md5-47-3.cnf"},
    {32, "g2-mizh-md5-48-2.cnf"},
    {33, "g2-mizh-md5-48-5.cnf"},
    {34, "g2-slp-synthesis-aes-top30.cnf"},
    {35, "g2-hwmcc15deep-beemloyd3b1-k31.cnf"},
    {36, "g2-hwmcc15deep-6s340rb63-k16.cnf"},
    {37, "g2-hwmcc15deep-intel065-k11.cnf"},
    {38, "g2-hwmcc15deep-bobpcihm-k30.cnf"},
    {39, "g2-hwmcc15deep-beembkry8b1-k45.cnf"},
    {40, "g2-hwmcc15deep-bobpcihm-k31.cnf"},
    {41, "g2-hwmcc15deep-bobpcihm-k32.cnf"},
    {42, "g2-hwmcc15deep-6s366r-k72.cnf"},
    {43, "g2-hwmcc15deep-bobpcihm-k33.cnf"},
    {44, "g2-hwmcc15deep-6s188-k44.cnf"},
    {45, "g2-hwmcc15deep-bob12s02-k16.cnf"},
    {46, "g2-hwmcc15deep-6s188-k46.cnf"},
    {47, "g2-hwmcc15deep-bob12s02-k17.cnf"},
    {48, "g2-hwmcc15deep-6s44-k38.cnf"},
    {49, "g2-hwmcc15deep-6s44-k40.cnf"},
    {50, "g2-hwmcc15deep-6s340rb63-k22.cnf"},
    {51, "g2-hwmcc15deep-6s341r-k16.cnf"},
    {52, "g2-ak128diagodiagoisc.cnf"},
    {53, "g2-ak128astepbg2asisc.cnf"},
    {54, "g2-hwmcc15deep-intel032-k84.cnf"},
    {55, "g2-ak128paralparalisc.cnf"},
    {56, "g2-ak128modbtbg1asisc.cnf"},
    {57, "g2-hwmcc15deep-beemhanoi4b1-k32.cnf"},
    {58, "g2-ak128astepmodbtisc.cnf"},
    {59, "g2-ak128astepbg2msisc.cnf"}
};


/**
 * Preprocesses a SAT formula, taking the following steps:
 * - parses inputFilename for a SAT formula
 * - simplifies based on unipolarity and unit clauses
 * - renumbers variables so there are no gaps
 * 
 * Params:
 * - inputFolder
 * - inputFile
 * - outputFolder: if empty string (default), appends "_simplified" to inputFolder
 * - outputFile: if empty string (default), appends "_simplified" to inputFilename
 * - renumber: boolean, decides whether to renumber formula (default true)
 * 
 * Returns:
 * - unordered_set<int>: assignment used to simplify formula
 * - creates outputfile
*/
unordered_set<int> preprocess(string inputFolder, string inputFile, string outputFolder="", string outputFile="", bool renumber=true) {
    // Add '/' to inputFolder if needed
    if(inputFolder.size() > 0 && inputFolder.back() != '/') {
        inputFolder += "/";
    }

    // Remove '/' from start of inputFile if needed
    if(inputFile[0] == '/') {
        inputFile = inputFile.substr(1);
    }

    // Create output folder name if none provided
    if(outputFolder.size() == 0) {
        outputFolder = inputFolder.substr(0, inputFolder.size() - 1) + "_simplified/";
    }

    // Create output file name if none provided
    if(outputFile.size() == 0) {
        outputFile = inputFile.substr(0, inputFile.find_last_of('.'));
        outputFile += "_simplified.cnf";
    }

    // Create folder if doesn't exist
    struct stat sb;
    if (stat(outputFolder.c_str(), &sb) != 0) {
        mkdir(outputFolder.c_str(), 0700);
    }
    
    // Parse formula
    int vars = 0, clauses = 0;
    vector<vector<int>> formula;
    parse(inputFolder + inputFile, formula, vars, clauses);


    // Simplify
    unordered_set<int> assignment = simplify(formula);

    // Renumber if needed, updating number of vars
    if(renumber) {
        vars = renumberFormula(formula);
    }

    // Output to file
    string description = "This is a simplified version of " + inputFolder + inputFile + ".";
    writeFormulaToFile(formula, vars, outputFolder + outputFile, description);

    cout << "Saved simplified formula to: " << outputFolder + outputFile << endl;
    return assignment;
}


/**
 * Helper function to fit a formula onto an Architecture
 * 
 * Params:
 * - vars: number of variables in formula
 * - clauses: number of clauses in the formula
 * - formula: the SAT formula
 * - lines_param: indicates number of full, half, and quarter lines in the architecture
 * - method: "prune", "lits_only", default = regular
 * - bool: "descending" in implement
*/
bool fitFormulaToArchitecture(int vars, int clauses, vector<vector<int>> formula, vector<int> lines_param, bool debug, string method, bool descending) {
    // Create architecture
    Architecture a(vars, clauses);

    int ones = lines_param[0], twos = lines_param[1], fours = lines_param[2];

    cout << "Ones: " << ones << "\tTwos: " << twos << " \tFours: " << fours << endl;
    if(ones % 2 == 1 || twos % 2 == 1 || fours % 2 == 1) {
        cout << "ERROR: all line sets must have even number of lines" << endl;
        return 0;
    }

    vector<int> lines;
    for(int i = 0; i < ones; ++i) lines.push_back(1);
    for(int i = 0; i < twos; ++i) lines.push_back(2);
    for(int i = 0; i < fours; ++i) lines.push_back(4);
    
    a.createEqualLines(lines);

    cout << "METHOD: " << method << "\t\t";
    if(descending) {
        cout << "descending: true" << endl;
    } else {
        cout << "descending: false" << endl;
    }

    // Implement formula
    a.debug = debug;
    bool result = false;
    if(method == "prune") {
        result = a.implementFormulaPrune(formula, vars, descending);
    } else if(method == "lits_only") {
        result = a.implementFormulaLitsOnly(formula, vars, descending);
    } else {
        result = a.implementFormula(formula, vars, descending);
    }
    
    if(result) {
        // cout << "returned true" << endl;

        // Double check results
        bool double_check = a.validateImplement();
        if(double_check) {
            cout << "validation: correct!" << endl;
        } else {
            cout << "VALIDATION: FAILED" << endl;
        }
    } else {
        cout << "RETURNED FALSE" << endl;
    }
    cout << endl;

    return result;
}


int main() {
    cout << endl;
    // Parse formula
    // string path = OSTROWSKI_PATH;
    // string file = OSTROWSKI_FILES[0];

    string path = SAT2017_PATH;
    string file = SAT2017_FILES[59];

    // Create architecture
    // path = SIMPLE_PATH;
    // file = "uf16_18.cnf";

    Circuit c(path+file);
    cout << "File: " << file << endl;
    cout << "Vars: " << c.vars << "\tClauses: " << c.clauses << endl << endl;
    
    /*
    // Create architecture
    // Architecture a(c.vars, c.clauses);
    // bool debug = false;
    
    // Number of full-lines, half-lines, quarter lines
    // vector<int> lines_param = {86, 54, 0};

    // vector<string> METHODS = {"default", "prune", "lits_only"};
    // for(string method : METHODS) {
    //     if(method == "lits_only") continue;
    //     if(method == "prune") continue;

    //     // // Variables in descending order of occurrence, full lines first
    //     // fitFormulaToArchitecture(c.vars, c.clauses, c.formula, lines_param, debug, method, true);
    //     // cout << endl;

    //     // // Variables in increasing order of occurrence, half lines first
    //     // fitFormulaToArchitecture(c.vars, c.clauses, c.formula, lines_param, debug, method, false);
    //     // cout << endl;
    // }*/

    // Partitioning problem
    // Partition p(c.vars, c.formula);
    // p.debug = true;

    // Clause Partitioning
    ClausePartition clause_part(c.vars, c.formula);
    string heur = "round-robin";
    heur = "random";

    // clause_part.debug = true;
    // int trials = 10;
    // if(heur == "round-robin") trials = 1;
    // unordered_map<int, float> totals;
    // for(int t = 0; t < trials; ++t) {
    //     clause_part.createSubarrays(1024, heur);
    //     unordered_map<int, set<int>> groups_activated;
    //     groups_activated = clause_part.checkGroupings(0b11111);

    //     for(pair<int, set<int>> p : groups_activated) {
    //         totals[p.first] += p.second.size();
    //     }
    // }

    bool randomize = false;
    findGroupsHelper(c.clauses, randomize, 1024, 32);
   
    return 0;
}