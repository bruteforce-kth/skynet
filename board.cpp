#include <iostream>
#include "board.h"
#include <string.h>
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;
using std::string;

board::board (const vector<vector<char> > &chars) {
    this->mBoard = chars;
    initializeIndexAndPositions(chars);
    mWasPush = false;
    mWhatGotMeHere = '\0';
}

board::board (const vector<vector<char> > &chars, 
              bool wasPush, char whatGotMeHere, string path){
    this->mBoard = chars;
    initializeIndexAndPositions(chars);
    mWasPush = wasPush;
    mWhatGotMeHere = whatGotMeHere;
    mPath = path;
}

/*
 * Loops over the 2d vector of chars and initializes
 * player position, goal positions and indexes.
 */
void board::initializeIndexAndPositions(const vector<vector<char> > &chars) {
    std::string boardString = "";
    int index = 0;
    int size = 0;
    for (int i = 0; i < chars.size(); i++) {
        size += chars[i].size();
        for (int j = 0; j < chars[i].size(); j++) {
            boardString += chars[i][j];
            char c = chars[i][j];
            // Store goal positions
            if (c == GOAL || c == BOX_ON_GOAL || c == PLAYER_ON_GOAL) {
                mGoalPositions.push_back(make_pair(i,j));
            }
            // Store player position
            if (c == PLAYER || c == PLAYER_ON_GOAL) {
                mPlayerPos = make_pair(i,j);
            }
            if(c == BOX || c == BOX_ON_GOAL){
                mBoxPositions.push_back(make_pair(i,j));
            }
            ++index;
        }
    }
    mBoardString = boardString;
    mBoardSize = size;
    return;
}

board* board::doMove(std::pair<int,int> newPlayerPos, char direction) const{
    bool boxPush = false;
    std::vector<std::vector<char> > newMap = mBoard;
    if( isAccessible(newPlayerPos.first, newPlayerPos.second,
         mPlayerPos.first, mPlayerPos.second) ){
        // Has the player "moved into" a box (trying to push)?
        if(isBox(newPlayerPos.first, newPlayerPos.second)){
            boxPush = true;
            std::pair<int,int> boxPos = make_pair(newPlayerPos.first,newPlayerPos.second);
            switch(direction){            
                case 'U':
                    boxPos.first--;
                    break;
                case 'D':
                    boxPos.first++;
                    break;
                case 'L':
                    boxPos.second--;
                    break;
                case 'R':
                    boxPos.second++;
                    break;
            }

            /* IF PLAYER ON GOAL */
            if(newMap[newPlayerPos.first][newPlayerPos.second] == BOX_ON_GOAL)
                newMap[newPlayerPos.first][newPlayerPos.second] = '+';
            /* NORMAL PUSH */
            else 
                newMap[newPlayerPos.first][newPlayerPos.second] = '@';

            /* IF BOX LANDED ON GOAL */                
            if(newMap[boxPos.first][boxPos.second] == '.')
                newMap[boxPos.first][boxPos.second] = '*';
            /* NORMAL BOX MOVE */
            else
                newMap[boxPos.first][boxPos.second] = '$';

            /* REMOVE OLD PLAYER */
            /* DID WE LEAVE A GOAL POSITION */
            if(newMap[mPlayerPos.first][mPlayerPos.second] == '+')
                newMap[mPlayerPos.first][mPlayerPos.second] = '.';
            /* PREVIOUS POSITION WAS NORMAL */
            else
                newMap[mPlayerPos.first][mPlayerPos.second] = ' ';

        //Just a normal move        
        }else{
            /* DID WE LAND ON A GOAL */
            if(newMap[newPlayerPos.first][newPlayerPos.second] == '.')
                newMap[newPlayerPos.first][newPlayerPos.second] = '+';
            /* NORMAL MOVE */
            else
                newMap[newPlayerPos.first][newPlayerPos.second] = '@';
            
            /* DID WE LEAVE A GOAL POSITION */
            if(newMap[mPlayerPos.first][mPlayerPos.second] == '+')
                newMap[mPlayerPos.first][mPlayerPos.second] = '.';
            /* PREVIOUS POSITION WAS NORMAL */
            else
                newMap[mPlayerPos.first][mPlayerPos.second] = ' ';
        }
    }
    return new board(newMap, boxPush, direction, getPath() + direction);
}

/*
 * Checks if a position on the board is accessible.
 */
bool board::isAccessible(int row, int col, int prevRow, int prevCol) const{
    // Check regular move
    if (isWalkable(row, col)){
        return true;
    }
    // Check box push
    else if (isBox(row, col)) {
        pair<int,int> boxPos = make_pair(prevRow+(row-prevRow)*2,
                                         prevCol+(col-prevCol)*2);

        //DYNAMIC DEADLOCK
        char up = WALL;
        char upr = WALL;
        char upl = WALL;
        char down = WALL;
        char downr = WALL;
        char downl = WALL;
        char left = WALL;
        char right = WALL;
        if(row > 0){                            // SET UP
            up = mBoard[row-1][col];
            if(col > 0){                        // SET UP LEFT CORNER
                upl = mBoard[row-1][col-1];
            }
            if(col < mBoard[row-1].size() - 1){ // SET UP RIGHT CORNER
                upr = mBoard[row-1][col+1];
            }
        }
        if(row < mBoard.size() - 1){            // SET DOWN
            down = mBoard[row+1][col];
            if(col > 0){                        // SET DOWN LEFT CORNER
                downl = mBoard[row+1][col-1];
            }
            if(col < mBoard[row+1].size() - 1) {
                downr = mBoard[row+1][col+1];
            }
        }
        if(col > 0){                            // SET LEFT
            left = mBoard[row][col-1];
        }
        if(col < mBoard[row].size() - 1){       // SET RIGHT
            right = mBoard[row][col+1];
        }

        if(up == WALL || up == BOX) {
            if(upl == WALL || upl == BOX) {
                if(left == WALL || left == BOX) {
                    return false;
                }
            }
            if(upr == WALL || upr == BOX) {
                if(right == WALL || right == BOX) {
                    return false;
                }
            }
        }
        if(down == WALL || down == BOX){
            if(downl == WALL || downl == BOX) {
                if(left == WALL || left == BOX) {
                    return false;
                }
            }
            if(downr == WALL || downr == BOX) {
                if(right == WALL || right == BOX) {
                    return false;
                }
            }
        }
        // END DYNAMIC DEADLOCK

        if (isWalkable(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2)){
            return true;
        }
    }
    return false;
}



/*
 * Checks if all boxes are on goals (finish state).
 */
bool board::isFinished() const{
    if (mBoardString.find('$')!= std::string::npos)
        return false;
    return true;
}

bool board::isWalkable(int row, int col) const {

    char t = mBoard[row][col];
    // Check regular move
    if(t == FLOOR || t == GOAL){
        return true;    
    }
    return false;
}

bool board::isGoal(int row, int col) const{
    if(mBoard[row][col] == GOAL) {
        return true;    
    }
    return false;
}

bool board::isBox(int row, int col) const{
    if((mBoard[row][col] == BOX) || (mBoard[row][col] == BOX_ON_GOAL)) {
            return true;    
    }
    return false;
}

/*
 * Returns all valid moves from the specified position
 */
void board::getAllValidMoves(vector<board> &moves) const{
    int row = getPlayerPosition().first;
    int col = getPlayerPosition().second;
    // std::cout << "getAllValidMoves(" << row << ", " << col << ")" << std::endl;
    if (isAccessible(row-1, col, row, col)) {
        moves.push_back(*doMove(make_pair(row-1,col), MOVE_UP));
    }
    if (isAccessible(row+1, col, row, col)) {
        moves.push_back(*doMove(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isAccessible(row, col-1, row, col)) {
        moves.push_back(*doMove(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isAccessible(row, col+1, row, col)) {
        moves.push_back(*doMove(make_pair(row,col+1), MOVE_RIGHT));
    }
}

void board::getAllValidWalkMoves(vector<board> &moves) const{
    int row = getPlayerPosition().first;
    int col = getPlayerPosition().second;
    // std::cout << "getAllValidMoves(" << row << ", " << col << ")" << std::endl;
    if (isWalkable(row-1, col)) {
        moves.push_back(*doMove(make_pair(row-1,col), MOVE_UP));
    }
    if (isWalkable(row+1, col)) {
        moves.push_back(*doMove(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isWalkable(row, col-1)) {
        moves.push_back(*doMove(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isWalkable(row, col+1)) {
        moves.push_back(*doMove(make_pair(row,col+1), MOVE_RIGHT));
    }
}


void board::printBoard() {
    // std::cout << "printBoard" << std::endl;
    for (int i = 0; i < mBoard.size(); i++) {
        for (int j = 0; j < mBoard[i].size(); j++) {
            cout << mBoard[i][j];
        }
        cout << '\n';
    }
}
