#!/bin/bash

cat $1 | ./main > Path.txt

cd advancedVisualizer
gcc visualizer.c -lncurses -o visualizer.out
./visualizer.out -m ../$1 -s ../Path.txt $2
