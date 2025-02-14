./#!/bin/bash

# Run "nohup ./run_minisat_exp.sh activity -1 -1 true &"
# activity -> heuristic
# -1 -> threshold (set to positive to cap running time)
# -1 -> threshold iterations (set to positive to cap running iterations)
# true -> whether to use gprof or not

# Input file
input_file="input.txt"

# Directory name of outputs (change if needed)
directory="outputs"

# Extract argument for heuristic
heuristic="$1"
if [ -z "$heuristic" ]; then
  heuristic="activity"
else
  directory="outputs_$heuristic"
fi
echo "Using heuristic: $heuristic and directory: $directory"

# Extract argument for threshold time
threshold="$2"
if [ -z "$threshold" ]; then
  threshold="-1"
fi
echo "Using threshold: $threshold seconds"

# Extract argument for threshold iterations
threshold_iterations="$3"
if [ -z "$threshold_iterations" ]; then
  threshold_iterations="-1"
fi
echo "Using threshold: $threshold_iterations iterations" 

# Extract gprof flag
gprof=false
if [ "$4" == "true" ]; then
    gprof=true
fi
echo "Using gprof? $gprof"

# Arguments for MiniSAT
# Custom Heuristic options: activity, dynamic_var_occurrences, dynamic_jeroslow_wang, dynamic_mom, static_var_occurrences, lazy_var_occurrences, chb, random
args="-no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -no-elim -rnd-seed=42 -verb=2 -custom-heuristic=$heuristic -duration-threshold-seconds=$threshold -max-iterations=$threshold_iterations"

#  ../SAT_Architecture/sat_files/SAT2017_Soowang/g2-ak128astepbg2msisc.cnf

# Assign a bench for each heuristic
# Initialize bench variable based on heuristic using if-else
bench=1
if [ "$heuristic" == "activity" ]; then
    bench=1
elif [ "$heuristic" == "dynamic_jeroslow_wang" ]; then
    bench=2
elif [ "$heuristic" == "dynamic_var_occurrences" ]; then
    bench=3
elif [ "$heuristic" == "dynamic_mom" ]; then
    bench=4
elif [ "$heuristic" == "static_var_occurrences" ]; then
    bench=5
elif [ "$heuristic" == "random" ]; then
    bench=6
else
    bench=7
fi

# Read each line from input file
while IFS= read -r filepath; do
    # Check if the line starts with #
    if [[ "$filepath" != \#* ]]; then
        # Run the command for each filepath
        echo "Running $filepath..."
    
        # Decide which script to run based on the gprof flag
        if [ "$gprof" = true ]; then
            # make cpp

            # # Move the new executable
            # mv -f "build/release/bin/minisat" "bin/"

            echo "Running with gprof flag enabled"
            ./minisat_gprof.sh "$filepath" "$directory" "$bench" "$threshold_iterations" "$threshold" "$args"
        else
            echo "Running without gprof flag"
            ./minisat_exp.sh "$filepath" "$directory" "$args"
        fi
    fi
done < "$input_file"