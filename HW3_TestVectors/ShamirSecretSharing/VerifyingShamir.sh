#!/bin/bash

gcc shamirgen.c -lssl -lcrypto -o gen
gcc shamirrecon.c -lssl -lcrypto -o recon

for i in 1 2 3
do
    echo "Testing case $i..."
    
	rm -f Share*.txt
	rm -f Recovered.txt

    # Run gen
	./gen Secret$i.txt Modulus$i.txt > gen$i.log

    # Verify outputs
    echo "Verifying outputs for test case $i..."
    
    for j in 1 2 3
    do
        # Run reconalt 
        ./reconalt Modulus$i.txt > reconalt$i.log

        # Check output of reconalt against Secret
        if cmp -s "Recovered.txt" "Secret${i}.txt"; then
            echo "Reconstruction ${j} of test case ${i} is valid."
        else
            echo "Reconstruction ${j} of test case ${i} does not match! ===ERROR==="
        fi
    done

    # Run recon
	./recon Modulus$i.txt > recon$i.log

    for file in Recovered
    do
        if cmp -s "${file}.txt" "Correct${file}${i}.txt"; then
            echo "${file}${i} is valid."
        else
            echo "${file}${i} does not match!"
            echo "Differences between Correct${file}${i}.txt and ${file}.txt:"
            # Using hexdump to show differences in hex format
            echo "Expected:"
            hexdump -C "Correct${file}${i}.txt"
            echo "Got:"
            hexdump -C "${file}.txt"
            echo "---"
        fi
    done
    
done
