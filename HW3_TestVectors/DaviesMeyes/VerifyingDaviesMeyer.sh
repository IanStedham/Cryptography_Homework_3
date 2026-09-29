#!/bin/bash

gcc dm.c -lssl -lcrypto -o dm

for i in 1 2 3
do
    echo "Testing case $i..."
    
    rm -f FirstHash.txt
    rm -f FinalHash.txt

    # Run program
    ./dm Message$i.txt > dm$i.log

    # Verify outputs
    echo "Verifying outputs for test case $i..."
    
    for file in FirstHash FinalHash
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
