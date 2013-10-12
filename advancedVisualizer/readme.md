`gcc visualizer.c -lncurses` to compile visualizer

(`apt-get install libncurses5-dev` to install ncurses library)

`-m` specify map file
`-s` specify path file (solution string)
`-f` if you want to fast forward (leave out for interactive)

Example

`./visualizer.out -m mazeTest.txt -s pathTest.txt -f`
