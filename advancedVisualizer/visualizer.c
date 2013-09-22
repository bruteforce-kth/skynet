#include "visualizer.h"

void printStart(){
    box(stdscr, 0, 0);
    printw("%d, %d", COLS, LINES);
    mvprintw(LINES/2, COLS/2 - 20, "PRESS ANY KEY");
    getch();
}

void initalizeScreen(){
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    
}

void teardown(){
    endwin();
}


void printMaze(WINDOW *win, char *fileName){
    int bufferSize = 512;
    int position = 0;
    char currentChar = 'x';
    char *buffer = (char *)malloc(bufferSize);
    int row = 0;
    int col = 0;
    int counter = 0;
    FILE *file = fopen(fileName, "r");
    struct coordinate* goalPos;
    
    if(!file){
        perror("Unable to open file");
        exit(errno);
    }    

    do{
        position = 0;
        maze[row] = (char*)malloc(64);
            do{
                currentChar = fgetc(file);
                if(currentChar != EOF){ 
                    buffer[position++] = (char)currentChar;
                    if(currentChar == '@' || currentChar == '+'){
                        playerPos->row = row;
                        playerPos->col = col;
                    }
                    if(currentChar != '\n'){
                        maze[row][col] = currentChar;
                    }
                    if(currentChar == '.' || '+' || '*'){
                        goalPos = (struct coordinate*)malloc(sizeof(struct coordinate));
                        goalPos->row = row;
                        goalPos->col = col;
                        goalPositions[nrOfGoals] = goalPos;
                    }
                }
                if(position >= bufferSize - 1){
                    bufferSize*=2;
                    buffer = (char *)realloc(buffer, bufferSize);
                }
            col++;
            }while(currentChar != EOF && currentChar != '\n');
        buffer[position] = 0;
        wprintw(win, "%s", buffer);
        row++;
        col = 0;
        colLength++;
    }while(currentChar != EOF);
    rowLength = row;
    fclose(file);
    free(buffer);
    wrefresh(win);
    refresh();
    
    return;
}

char getCell(int row, int col){
    return maze[row][col];
}

void actuallyPrint(WINDOW *win, struct coordinate* box){
    lastVisited = maze[playerPos->row][playerPos->col];
    wmove(win, playerPos->row, playerPos->col);
    wprintw(win, "%c", '@');
    if(lastVisited == '$'){
                wmove(win, box->row, box->col);
                wprintw(win, "%c", '$');
                maze[box->row][box->col] = '$';
                if(lastVisited != '.')
                    lastVisited = ' ';
                else
                    lastVisited = '.';
    }
    maze[playerPos->row][playerPos->col] = lastVisited;
}

void printStep(WINDOW *win, char direction, int time){
    wmove(win, playerPos->row, playerPos->col);
    wprintw(win, "%c", lastVisited);
    struct coordinate* box = (struct coordinate*)malloc(sizeof(struct coordinate));
    switch(direction){
        case 'U':
            playerPos->row = playerPos->row - 1;
            box->row = playerPos->row-1;
            box->col = playerPos->col;
            actuallyPrint(win, box);  
            break;
        case 'D':
            playerPos->row = playerPos->row + 1;
            box->row = playerPos->row+1;
            box->col = playerPos->col;
            actuallyPrint(win, box);
            break;
        case 'L':
            playerPos->col = playerPos->col - 1;
            box->row = playerPos->row;
            box->col = playerPos->col-1;
            actuallyPrint(win, box);
            break;
        case 'R':
            playerPos->col = playerPos->col + 1;
            box->row = playerPos->row;
            box->col = playerPos->col+1;
            actuallyPrint(win, box);
            break;
        default:
            perror("Problem in printStep");
            abort();
    }
    free(box);
    wmove(win, rowLength+2, time*2);
    wprintw(win, "%c%c", direction, ' ');
    wrefresh(win);
    refresh();
    

}

void printPath(WINDOW *win, char *fileName, bool interactive){

    char currentChar;
    int time = 0;
    FILE *file = fopen(fileName, "r");
    if(!file){
        perror("Unable to open file");
        abort();
    }
    
    while(true){
        currentChar = fgetc(file);
        if(currentChar == EOF || currentChar == '\n'){
            mvwprintw(win, rowLength+3, 0, "%s", "END OF PATH\n");
            wrefresh(win);
            refresh();
            getch();
            break;
        }
        if(currentChar != ' '){
            printStep(win, currentChar, time++);
            if(interactive)
                getch();
            else
                sleep(1);  
        }
    }
    fclose(file);
}


int main(int argc, char **argv){

    playerPos = (struct coordinate*)malloc(sizeof(struct coordinate));
    goalPositions = (struct coordinate**)malloc(sizeof(struct coordinate*)*20);
    lastVisited = ' ';
    char *mazeFileName = NULL;
    char *solutionFileName = NULL;
    bool interactive = false;
    maze = (char**)malloc(64);
    int c;
    rowLength = 0;
    colLength = 0;
    nrOfGoals = 0;
    
    while ((c = getopt(argc, argv, "im:s:")) != -1){
        switch(c){
            case 'i':
                interactive = true;
                break;
            case 'm':
                mazeFileName = optarg;
                break;
            case 's':
                solutionFileName = optarg;
                break;
            default:
                abort();   
        }
    }

    initalizeScreen();
    printStart();
    clear();
    move(0,0);
    printMaze(stdscr, mazeFileName);
    maze[playerPos->row][playerPos->col] = ' ';
    getch();
    printPath(stdscr, solutionFileName, interactive);
    teardown();
    return 0;

}
