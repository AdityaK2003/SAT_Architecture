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
 * 
*/
bool fitFormulaToArchitecture(int vars, int clauses, vector<vector<int>> formula, vector<int> lines_param, bool debug=false) {
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

    // Implement formula
    a.debug = debug;
    bool result = a.implementFormulaPrune(formula, vars, true);
    
    if(result) {
        // cout << a << endl;
    } else {
        cout << "RETURNED FALSE" << endl;
    }

    return result;
}


int main() {
    // Parse formula
    string path = OSTROWSKI_PATH;
    string file = OSTROWSKI_FILES[1];

    // Create architecture
    //string path = SIMPLE_PATH;
    //string file = "uf16_18.cnf";
    Circuit c(path+file);

    // Create architecture
    Architecture a(c.vars, c.clauses);
    bool debug = true;
    
    // Number of full-lines, half-lines, quarter lines
    vector<int> lines_param = {0, 64, 0};
    bool result = fitFormulaToArchitecture(c.vars, c.clauses, c.formula, lines_param, debug);
    

    // Partitioning problem
    Partition p(c.vars, c.formula);


    return 0;
}