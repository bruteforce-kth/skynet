#!/bin/bash 

gcc visualizer.c -lncurses -o visualizer.out

./visualizer.out -m mazeTest.txt -s pathTest.txt -i
