./#!/bin/bash

# Run "nohup ./run_minisat_exp.sh &"

# Input file
input_file="input.txt"

# Directory name of outputs (change if needed)
directory="outputs"

# Extract argument for heuristic
heuristic="$1"
if [ -z "heuristic" ]; then
  heuristic="activity"
else
  directory="outputs_$heuristic"
fi
echo "Using heuristic: $heuristic and directory: $directory"

# Arguments for MiniSAT
# Custom Heuristic options: activity, dynamic_var_occurrences, dynamic_jeroslow_wang, dynamic_mom, static_var_occurrences, random
args="-no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -no-elim -rnd-seed=42 -verb=2 -custom-heuristic=$heuristic"

#  ../SAT_Architecture/sat_files/SAT2017_Soowang/g2-ak128astepbg2msisc.cnf

# Read each line from input file
while IFS= read -r filepath; do
    # Check if the line starts with #
    if [[ "$filepath" != \#* ]]; then
        # Run the command for each filepath
        echo "Running $filepath..."
        # ./minisat_exp.sh "$filepath" "$directory" "$args"
        ./minisat_gprof.sh "$filepath" "$directory" "$args"
    fi
done < "$input_file"