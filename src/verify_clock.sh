#!/bin/bash
# Exhaustive check of a(n) (OEIS A066385) for n = 3..N (default 44): no arrangement has largest sum of three
# neighbours a(n)-1, and some arrangement attains a(n) (found by the search for n < 33, Schoenfield's construction
# for n >= 33).  a(n) = ceil(3n/2)+3 except a(3)=6, a(5)=10, a(6)=11, a(9)=16, a(15)=25.
# Run from the repository root after building src/clock_bt3.  n >= 33 uses two cores via src/run_split.py.
N=${1:-44}
for n in $(seq 3 $N); do
  a=$(python3 -c "import math; n=$n; print({3:6,5:10,6:11,9:16,15:25}.get(n, math.ceil(3*n/2)+3))")
  if [ $n -lt 33 ]; then
    lo=$(./src/clock_bt3 $n $((a-1)) | head -1)
    w=$(./src/clock_bt3 $n $a | grep -q "^solution:" && echo "found by search" || echo "NOT FOUND")
  else
    lo=$(python3 src/run_split.py $n $((a-1)) 2 | tail -1)
    w=$(python3 src/schoenfield.py $n $n | tail -1); w=${w#*: }
  fi
  echo "n=$n a(n)=$a | largest sum a(n)-1: ${lo#*: } | a(n) attained: $w"
done
