#!/bin/bash

cat $1 | ./main > Path.txt

cd advancedVisualizer

./visualizer.out -m ../$1 -s ../Path.txt -i
