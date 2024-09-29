#!/bin/bash
export PATH=$PATH:/home/aditya/minisat/bin

# Usage
if [ "$#" -ne 1 ]; then
  echo "Usage: $0 <input_file>"
  exit 1
fi

# Extract file name from input argument
filename=$(basename "$1")

# Define the file
# inputfile="../VLSAT1_Count3/$filename"
inputfile="$1"

# Define the file to save the output to
outputfile="outputs/${filename}_out.txt"

# Number of trials
trials=1

# Clear output file
> $outputfile

# Array to store cpu times
cpu_times=()

# Loop through the command and save the output to the file
for ((i=1; i <= $trials; ++i))
do
  $(minisat $inputfile  -no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -no-elim -verb=1 -rnd-seed=42 2>&1 > $outputfile)
  
  result=$(echo "$output" | tail -n1)
  #echo "Command $i: $result"

  cpu_time=$(echo "$output" | tail -n3 | head -n1 | grep -oP '[-+]?(\d*\.\d+|\d+(\.\d*)?)([eE][-+]?\d+)?')
  
  if [ -z "$cpu_time" ]; then
    cpu_time="0"
  elif [[ "$cpu_time" == *e* ]]; then
    cpu_time="0"
  else
    cpu_time=$(echo "scale=3; $cpu_time * 1000" | bc)
  fi

  # Check if cpu_time is valid
  if [ "$cpu_time" == "" ]; then
    cpu_time="0"
  fi

  # Add cpu_time to array
  cpu_times+=("$cpu_time")

  # Append cpu_time to output file
  # echo $output >> $outputfile
  echo "$cpu_time" >> $outputfile

done

# Calculate average cpu time
avg_cpu_time=$(echo "${cpu_times[@]}" | awk '{ total=0; for(i=1; i<=NF; i++) total+=$i; print total/NF }')

# Print average cpu time
echo "Average CPU time: $avg_cpu_time"

echo "Done!"
