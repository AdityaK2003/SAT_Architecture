#!/bin/bash
export PATH=$PATH:/home/aditya/SAT_Architecture/minisat_modified/bin/

# Usage
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <input_file>"
  exit 1
fi

# ../SAT_Architecture/sat_files/SAT2017_Soowang/
# nohup time ./minisat_exp.sh ../SAT_Architecture/sat_files/SAT2017_Soowang/g2-ak128astepbg2msisc.cnf &
# nohup time ./minisat_exp.sh ../SAT_Architecture/sat_files/nonrandom_sat_files/ &

# Extract file name from input argument
filename="$1"
stripped_filename=$(basename "$1")

# Extract arguments for minisat from next input argument
directory="$2"
args="$3"
if [ -z "$args" ]; then
  # Default value of arguments if none provided
  args="-no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -no-elim -rnd-seed=42 -verb=1"
fi
echo $args

# Define the file to save the output to
outputfile="$directory/${stripped_filename}_out.txt"

# Clear output file
> $outputfile

# Run make (can use make cpp for C++)
# make clean
make cpp

# Move the new executable
mv -f "build/release/bin/minisat" "bin/"

# Clean previous outputs
# rm -f "output_assignments"/*
# rm -f "outputs"/*

# Run command
# -no-elim: no variable elimination
# -no-pre: no preprocessing
# output=$(minisat $filename  -rinc=1.5 -phase-saving=0 -rnd-freq=0.0 -verb=2 -no-elim -no-pre 2>&1)

echo "Running: minisat $filename $args 2>&1 >> $outputfile" > $outputfile
minisat $filename $args 2>&1 >> $outputfile

# Save to output file
# echo "$output" >> $outputfile

# Done
echo "Done!"
