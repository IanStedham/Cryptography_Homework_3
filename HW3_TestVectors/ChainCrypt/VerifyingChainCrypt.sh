#!/bin/bash

gcc alice.c -lssl -lcrypto -o alice
gcc bob.c -lssl -lcrypto -o bob

for i in 1 2 3
do
    echo "Testing case $i..."
    
	rm -f Keys.txt
	rm -f Ciphertexts.txt
	rm -f Plaintexts.txt

    # Run Alice
    ./alice SharedSeed$i.txt Messages$i.txt > alice$i.log

    # Run Bob
    ./bob SharedSeed$i.txt Ciphertexts.txt > bob$i.log

    # Verify outputs
    echo "Verifying outputs for test case $i..."
    
    for file in Keys Ciphertexts Plaintexts
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
