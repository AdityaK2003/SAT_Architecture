#!/bin/sh
SOLUTIONS=0
inputfile="../50VarsCopy/uf50-033.cnf"
outputfile="out.txt"
export PATH=$PATH:/home/aditya/minisat/bin

while :; do
  minisat $inputfile $outputfile -no-luby -rinc=1.5 -phase-saving=0 -rnd-freq=0.02 -verb=0
  if [ "$(head -1 $outputfile)" = "UNSAT" ]; then
    break
  fi
  SOLUTIONS=$((SOLUTIONS + 1))
  tail -1 $outputfile |
    awk '{
      for(i=1;i<NF;++i) { $i = -$i }
      print
    }' >> $inputfile

done

echo There are $SOLUTIONS solutions.