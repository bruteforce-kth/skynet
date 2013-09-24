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

struct goalPos{

    struct coordinate coordinate;
    struct goalPos* next;

};

struct coordinate* playerPos;
struct goalPos* goalPositions;
char** maze;
int rowLength;
int colLength;
char lastVisited;
int nrOfGoals;

//int absolutePosition(int row, int col){ return (row*rowLength)+col; }
