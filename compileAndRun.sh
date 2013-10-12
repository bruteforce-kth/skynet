#!/bin/bash 

make

for i in {1..3}
do
    #./main < maps/00_sample.in
    #./main < maps/01_sample.in
    #./main < maps/02_sample.in
    #./main < maps/sokoban1
    #./main < maps/sokoban2
    #./main < maps/sokoban3
    #./main < maps/20_maps/F6
    #./main < maps/simple
    ./main < maps/simple5
    #./main < maps/test100/test001.in
    #./main < maps/dynDeadlockMap
done

