#!/bin/bash 

make

for i in {1..1}
do
    #./main < maps/00_sample.in
    #./main < maps/01_sample.in
    #./main < maps/02_sample.in
    #./main < maps/sokoban1
    #./main < maps/sokoban2
    #./main < maps/sokoban3
    ./main < maps/20_maps/F3
done

