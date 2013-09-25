#!/bin/bash 

gcc visualizer.c -lncurses -o visualizer.out

./visualizer.out -m ../maps/sokoban3 -s Path.txt -i
