/*****************************************************************************************[Main.cc]
Copyright (c) 2003-2006, Niklas Een, Niklas Sorensson
Copyright (c) 2007,      Niklas Sorensson

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal in the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or
substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT
OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**************************************************************************************************/

#include <errno.h>
#include <zlib.h>

#include "minisat/utils/System.h"
#include "minisat/utils/ParseUtils.h"
#include "minisat/utils/Options.h"
#include "minisat/core/Dimacs.h"
#include "minisat/simp/SimpSolver.h"

#include <vector>

using namespace Minisat;

//=================================================================================================


static Solver* solver;
// Terminate by notifying the solver and back out gracefully. This is mainly to have a test-case
// for this feature of the Solver as it may take longer than an immediate call to '_exit()'.
static void SIGINT_interrupt(int) { solver->interrupt(); }

// Note that '_exit()' rather than 'exit()' has to be used. The reason is that 'exit()' calls
// destructors and may cause deadlocks if a malloc/free function happens to be running (these
// functions are guarded by locks for multithreaded use).
static void SIGINT_exit(int) {
    printf("\n"); printf("*** INTERRUPTED ***\n");
    if (solver->verbosity > 0){
        solver->printStats();
        printf("\n"); printf("*** INTERRUPTED ***\n"); }
    _exit(1); }



// Keeping track of stats
static void print_accumulated_stats(SimpSolver& S, int sections) {
    std::vector<int> accumulated_sums(sections, 0);
    std::vector<int> accumulated_counts(sections, 0);
    int section_size = S.nVars() / sections;
    for (int i = 0; i <= S.nVars(); i++) {
        // Skip if no vars bumped
        int num_vars_bumped = S.trail_size_to_vars_bumped[i].size();
        if (num_vars_bumped == 0) continue;

        int section_index = i / section_size;
        if (section_index >= sections) section_index = sections - 1;

        int sum = 0;
        for(int n : S.trail_size_to_vars_bumped[i]) sum += n;
        accumulated_sums[section_index] += sum;
        accumulated_counts[section_index] += num_vars_bumped;
    }

    // Print cumulative averages for sections
    printf("\nCumulative Averages by %d Sections:\n", sections);
    for (int s = 0; s < sections; s++) {
        double cumulative_avg = (accumulated_counts[s] > 0) 
                                ? (double) accumulated_sums[s] / accumulated_counts[s] 
                                : 0.0;
        printf("Section %d (%d-%d): %.2f (Vars Bumped: %d)\n", 
            s + 1, 
            s * section_size, 
            (s == sections - 1) ? S.nVars() : (s + 1) * section_size - 1,
            cumulative_avg,
            accumulated_counts[s]); // Print total number of variables bumped
    }
}



//=================================================================================================
// Main:

int main(int argc, char** argv)
{
    try {
        setUsageHelp("USAGE: %s [options] <input-file> <result-output-file>\n\n  where input may be either in plain or gzipped DIMACS.\n");
        setX86FPUPrecision();
        
        // Extra options:
        //
        IntOption    verb   ("MAIN", "verb",   "Verbosity level (0=silent, 1=some, 2=more).", 1, IntRange(0, 2));
        BoolOption   pre    ("MAIN", "pre",    "Completely turn on/off any preprocessing.", true);
        BoolOption   solve  ("MAIN", "solve",  "Completely turn on/off solving after preprocessing.", true);
        StringOption dimacs ("MAIN", "dimacs", "If given, stop after preprocessing and write the result to this file.");
        IntOption    cpu_lim("MAIN", "cpu-lim","Limit on CPU time allowed in seconds.\n", 0, IntRange(0, INT32_MAX));
        IntOption    mem_lim("MAIN", "mem-lim","Limit on memory usage in megabytes.\n", 0, IntRange(0, INT32_MAX));
        BoolOption   strictp("MAIN", "strict", "Validate DIMACS header during parsing.", false);

        parseOptions(argc, argv, true);
        
        SimpSolver  S;
        double      initial_time = cpuTime();

        if (!pre) S.eliminate(true);

        S.verbosity = verb;
        
        solver = &S;
        // Use signal handlers that forcibly quit until the solver will be able to respond to
        // interrupts:
        sigTerm(SIGINT_exit);

        // Try to set resource limits:
        if (cpu_lim != 0) limitTime(cpu_lim);
        if (mem_lim != 0) limitMemory(mem_lim);

        if (argc == 1)
            printf("Reading from standard input... Use '--help' for help.\n");

        gzFile in = (argc == 1) ? gzdopen(0, "rb") : gzopen(argv[1], "rb");
        if (in == NULL)
            printf("ERROR! Could not open file: %s\n", argc == 1 ? "<stdin>" : argv[1]), exit(1);
        
        if (S.verbosity > 0){
            printf("============================[ Problem Statistics ]=============================\n");
            printf("|                                                                             |\n"); }
        
        parse_DIMACS(in, S, (bool)strictp);
        gzclose(in);
        FILE* res = (argc >= 3) ? fopen(argv[2], "wb") : NULL;

        if (S.verbosity > 0){
            printf("|  Number of variables:  %12d                                         |\n", S.nVars());
            printf("|  Number of clauses:    %12d                                         |\n", S.nClauses()); }
        
        double parsed_time = cpuTime();
        if (S.verbosity > 0)
            printf("|  Parse time:           %12.2f s                                       |\n", parsed_time - initial_time);

        // Change to signal-handlers that will only notify the solver and allow it to terminate
        // voluntarily:
        sigTerm(SIGINT_interrupt);

        S.eliminate(true);
        double simplified_time = cpuTime();
        if (S.verbosity > 0){
            printf("|  Simplification time:  %12.2f s                                       |\n", simplified_time - parsed_time);
            printf("|                                                                             |\n"); }

        if (!S.okay()){
            if (res != NULL) fprintf(res, "UNSAT\n"), fclose(res);
            if (S.verbosity > 0){
                printf("===============================================================================\n");
                printf("Solved by simplification\n");
                S.printStats();
                printf("\n"); }
            printf("UNSATISFIABLE\n");
            exit(20);
        }

        lbool ret = l_Undef;

        if (solve){
            vec<Lit> dummy;
            ret = S.solveLimited(dummy);
        }else if (S.verbosity > 0)
            printf("===============================================================================\n");

        if (dimacs && ret == l_Undef)
            S.toDimacs((const char*)dimacs);

        if (S.verbosity > 0){
            S.printStats();
            printf("\n"); }
        printf(ret == l_True ? "SATISFIABLE\n" : ret == l_False ? "UNSATISFIABLE\n" : "INDETERMINATE\n");
        // printf("\nFinal Bump Count: %lld", S.order_heap.global_count);
        // printf("\nFinal Rescale Count: %lld\n", S.order_heap.rescale_count);
        // printf("\nFinal Heap Size: %d\n\n", S.order_heap.size());

        // Print stats on trail size -> num vars bumped
        printf("\nTrail Size: Average k\n");
        for (int i = 0; i <= S.nVars(); i++) {
            // Skip if no vars bumped
            int num_vars_bumped = S.trail_size_to_vars_bumped[i].size();
            if (num_vars_bumped == 0) continue;

            int sum = 0;
            for(int n : S.trail_size_to_vars_bumped[i]) sum += n;
            double avg = (double) sum / (double) num_vars_bumped;
            printf("%d: %.2f\n", i, avg);
        }

        // Also keep track of stats for different sections, i.e. if 100 vars then 0-33 trail size, 34-66, 67-100
        print_accumulated_stats(S, 3);
        print_accumulated_stats(S, 10);
        printf("\n");


        if (ret == l_True) {
            printf("SAT\n");
            for (int i = 0; i < S.nVars(); i++)
                if (S.model[i] != l_Undef)
                    printf("%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
            printf(" 0\n");
        }
        if (res != NULL){
            if (ret == l_True){
                fprintf(res, "SAT\n");
                for (int i = 0; i < S.nVars(); i++)
                    if (S.model[i] != l_Undef)
                        fprintf(res, "%s%s%d", (i==0)?"":" ", (S.model[i]==l_True)?"":"-", i+1);
                fprintf(res, " 0\n");
            }else if (ret == l_False)
                fprintf(res, "UNSAT\n");
            else
                fprintf(res, "INDET\n");
            fclose(res);
        }

#ifdef NDEBUG
        exit(ret == l_True ? 10 : ret == l_False ? 20 : 0);     // (faster than "return", which will invoke the destructor for 'Solver')
#else
        return (ret == l_True ? 10 : ret == l_False ? 20 : 0);
#endif
    } catch (OutOfMemoryException&){
        printf("===============================================================================\n");
        printf("INDETERMINATE\n");
        exit(0);
    }
}
