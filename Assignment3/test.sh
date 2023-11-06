#!/bin/bash

python3 testcase.py
echo "Testcase generated"
g++ -pthread 110550093_P3.cpp
echo "Compiled"
echo ""
./a.out