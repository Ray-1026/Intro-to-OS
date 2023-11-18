#!/bin/bash

echo "Compile main.c ..."
gcc -o main main.c

echo "Compile bf.c ..."
gcc -fPIC -g -shared -o bf.so bf.c

echo "Compile ff.c ..."
gcc -fPIC -g -shared -o ff.so ff.c

echo -e "Done\n\n---------------------------------\nBest fit :"
LD_PRELOAD=./bf.so ./main

echo -e "---------------------------------\nFirst fit :"
LD_PRELOAD=./ff.so ./main

echo "---------------------------------"