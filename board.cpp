#include <iostream>
#include "board.h"

using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;

board::board (const vector<vector<char> > &chars){
    this->mBoard = chars;
    initializeIndexAndPositions(chars);
}

/*
 * Loops over the 2d vector of chars and initializes
 * player position, goal positions and indexes.
 */
void board::initializeIndexAndPositions(const vector<vector<char> > &chars) {
    int index = 0;
    int size = 0;
    int longestRow = -1;
    for (int i = 0; i < chars.size(); i++) {
        int currentRowSize = chars[i].size();
        size += currentRowSize;
        if (longestRow < currentRowSize)
            longestRow = currentRowSize;
        for (int j = 0; j < chars[i].size(); j++) {
            char c = chars[i][j];
            // Store goal positions
            if (c == GOAL) {
                mGoalPositions.push_back(make_pair(i,j));
            }
            // Store player position
            else if (c == PLAYER || c == PLAYER_ON_GOAL) {
                mPlayerPos = make_pair(i,j);
            }/*
            else if (c == PLAYER_ON_GOAL) {
                this->playerPos = make_pair(i,j);
                return false;
            }*/
            ++index;
        }
    }
    mBoardSize = size;
    mLongestRow = longestRow;
    mNumRows = chars.size();
    return;
}

/*
 * Checks if a position on the board is accessible.
 */
bool board::isAccessible(int row, int col, int prevRow, int prevCol) const{
    // Check regular move
    if (isWalkable(row, col))
        return true;
    // Check box push
    else if (isBox(row, col)) {
        if (isWalkable(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2))
            return true;
    }
    return false;
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
vector<pair<pair<int,int>,char> > board::getAllValidMoves(int row, int col) const{
    vector<pair<pair<int,int>,char> > validMoves;
    if (isAccessible(row-1, col, row, col)) {
        validMoves.push_back(make_pair(make_pair(row-1,col), MOVE_UP));
    }
    if (isAccessible(row+1, col, row, col)) {
        validMoves.push_back(make_pair(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isAccessible(row, col-1, row, col)) {
        validMoves.push_back(make_pair(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isAccessible(row, col+1, row, col)) {
        validMoves.push_back(make_pair(make_pair(row,col+1), MOVE_RIGHT));
    }
    return validMoves;
}
