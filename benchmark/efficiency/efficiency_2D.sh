#!/bin/bash

TEST=$1
SIZE1=$2
SIZE2=$3
P_NUM=$4

for ((i=1; i<=$P_NUM; i=$i)); do
	if grep --quiet -i "MPI" <<< "$1" ;then 
		test $# -eq 6 || { echo "usage ./compile.sh SIZE1 SIZE2 processes strategy_header_1 strategy_header_2" >&2; exit; } 
		OUT=$(mpirun --use-hwthread-cpus -n $i $TEST $SIZE1 $SIZE2)
	else
		test $# -eq 6 || { echo "usage ./compile.sh SIZE1 SIZE2 processes strategy_header_1 strategy_header_2" >&2 ; exit; } 
		OUT=$(OMP_NUM_THREADS=$i $TEST $SIZE1 $SIZE2)
	fi
	
	SPEEDUP_F=$(cut -d',' -f 7 <<< $OUT)
	EFFICIENCY_F=$(awk "BEGIN { printf(\"%.2f\", $SPEEDUP_F / $i) }")
	SPEEDUP_B=$(cut -d',' -f 8 <<< $OUT)
	EFFICIENCY_B=$(awk "BEGIN { printf(\"%.2f\", $SPEEDUP_B / $i) }")

	echo "N_PROC $i: Speedup forward $SPEEDUP_F, efficiency $EFFICIENCY_F"
	echo "N_PROC $i: Speedup forward $SPEEDUP_B, efficiency $EFFICIENCY_B"

	if grep --quiet -i "MPI" <<< "$1" ;then 
		i=$(( $i * 2))
	else
		i=$(($i + 1))
	fi
done