#!/bin/bash
i=1 #Counter for test number

# Find all the tests input in Testing/Input
for input in `find Testing/Input/* -type f`
do
	# Name of output file is in Testing/Output/x.out, where x is the same as the
	# number for the input
	output=${input/.in/.out}
	output=${output/Input/Output}
	
	# Run the tests, outputing to Testing/$i.log for info if it fails
	./Repl $input $output 2> Testing/$i.log
	if [ $? -eq 0 ]
	then
		echo "Test $i `cat $input` passed."
		rm Testing/$i.log
	else
		echo "Test $i `cat $input` failed. See Testing/$i.log for more info"
	fi
	
	let "i=$i+1"
done
