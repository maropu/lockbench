#!/bin/bash

# Number of counter increments per cycle
D=1

# Number of seconds to run each test
T=10

# Numbers of workers
N="1 2 4 8 16"

# lock-type to test
I="none tas tas_mov tas_nopause cas_nolock cas cas_mfence cas_mov cas_nopause"

# Build
for impl in $I; do
	gcc -DLOCKIMPL=\"lock_$impl.h\" -lpthread -o lockbench_$impl lockbench.c err_utils.c >&2 || exit 1
done

# Output headers
echo "$D counter increments per cycle"
echo "---- show cycle throughput ----"

for n in $N; do
	echo -en "$n"'\t\t'
done
echo 'worker'

# Run tests, output table
for impl in $I; do
	for n in $N; do
		./lockbench_$impl $D $n $T 1 > temp.output || exit 1
		sed -n 's/^Averaged cycle throughput: \(.*\)$/\1/p' < temp.output | xargs echo -n
		echo -en '\t\t'
	done
	echo $impl
done

# Remove
for impl in $I; do
	rm -rf lockbench_$impl
	rm -rf temp.output
done
