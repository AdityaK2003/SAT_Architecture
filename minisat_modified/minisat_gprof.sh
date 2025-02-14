#!/bin/bash
export PATH=$PATH:/home/aditya/SAT_Architecture/minisat_modified/build/release/bin/
MSATP=./build/release/bin/minisat

# USE FOR GPROF

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
bench="$3"
iterations="$4"
threshold_seconds="$5"
args="$6"
if [ -z "$bench" ]; then
  bench=1
fi
if [ -z "$args" ]; then
  # Default value of arguments if none provided
  args="-no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -no-elim -rnd-seed=42 -verb=1"
fi
echo "Using bench $bench"
echo $args


# Define the file to save the output to
if [ "$iterations" -le 0 ] && [ "$threshold_seconds" -le 0 ]; then
  outputfile="$directory/${stripped_filename}_out.txt"
  gprof_outputfile="$directory/$stripped_filename.gprof.txt"
elif [ "$threshold_seconds" -gt 0 ]; then
  outputfile="$directory/${stripped_filename}_${threshold_seconds}seconds_out.txt"
  gprof_outputfile="$directory/${stripped_filename}_${threshold_seconds}seconds.gprof.txt"
else
  outputfile="$directory/${stripped_filename}_iter${iterations}_out.txt"
  gprof_outputfile="$directory/${stripped_filename}_iter${iterations}.gprof.txt"
fi
echo "Outputfile: $outputfile"
echo "Gprof outputfile: $gprof_outputfile"

# Clear output file
> $outputfile


# Run make (can use make noinline for C++ with no inlining)
# make clean
make noinline

# Move the new executable
cp -f "build/release/bin/minisat" "bin/"
cp "bin/minisat" "benches/b$bench/"
MSAT="./benches/b$bench/minisat"

echo "Running: minisat $filename $args 2>&1 >> $outputfile" > $outputfile
$MSAT $filename $args 2>&1 >> $outputfile
gprof $MSAT gmon.out > $gprof_outputfile

# Save to output file
# echo "$output" >> $outputfile

# Done
echo "Done!"
