#!/bin/bash

# clear
echo "Compile main.cpp ..."
g++ -o main 110550093_P5.cpp

echo -e "\nRun testcase : whois.txt ...\n-------------------------------------------------------------"
./main ./testcase/whois.txt
echo "-------------------------------------------------------------"

echo -e "\n\nRun testcase : zipf.txt ...\n-------------------------------------------------------------"
./main ./testcase/zipf.txt
echo "-------------------------------------------------------------"
