#include <ncurses.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

struct coordinate{
    int row;
    int col;
};

struct coordinate* playerPos;
char* maze;
int rowLength;
int colLength;

int absolutePosition(int row, int col){ return (row*rowLength)+col; }
