./#!/bin/bash

# Run "nohup ./run_hardware_solver.sh &""

make clean

# Run make (can use make noinline for C++ with no inlining)
make noinline

# Move the new executable
cp -f "build/release/bin/minisat" "bin/"
MSAT="./bin/minisat"

# Input file
input_file="../input.txt"

# Directory name of outputs (change if needed)
directory="../outputs_hardware_solver"

echo "Using directory: $directory"


# Read each line from input file
while IFS= read -r filepath; do
    # Check if the line starts with #
    if [[ "$filepath" != \#* ]]; then
        # Go up a level (since hardware_solver is its own directory, one level lower)
        filepath="../$filepath"

        # Run the command for each filepath
        echo "Running $filepath ..."

        filename=$(basename "$filepath")
    
        outputfile="$directory/${filename}_out.txt"
        gprof_outputfile="$directory/$filename.gprof.txt"

        echo -e "\tOutputfile: $outputfile"
        echo -e "\tGprof outputfile: $gprof_outputfile"

        echo -e "Running: minisat $filepath 2>&1 >> $outputfile\n" > $outputfile
        $MSAT $filepath 2>&1 >> $outputfile
        gprof $MSAT gmon.out > $gprof_outputfile

    fi
done < "$input_file"

echo -e "\nDone!\n"