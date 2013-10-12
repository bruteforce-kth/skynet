#!/bin/bash 

if [ $# -lt 1 ]; then
    ./main < maps/sokoban3
    exit
fi

case "$1" in
    1) ./main < maps/sokoban1;;
    2) ./main < maps/sokoban2;;
    3) ./main < maps/sokoban3;;
    00) ./main < maps/00_sample.in;;
    01) ./main < maps/01_sample.in;;
    02) ./main < maps/02_sample.in;;
    *) ./main < maps/sokoban3;;
esac
