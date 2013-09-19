#include "board.h"

using std::pair;
using std::vector;
using std::map;
using std::make_pair;

int main(){
return 0;
}

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
    for (int i = 0; i < chars.size(); i++) {
        size += chars[i].size();
        for (int j = 0; j < chars[i].size(); j++) {
            char c = chars[i][j];
            // Store goal positions
            if (c == GOAL) {
                this->mGoalPositions.push_back(make_pair(i,j));
            }
            // Store player position
            else if (c == PLAYER) {
                this->mPlayerPos = make_pair(i,j);
            }/*
            else if (c == PLAYER_ON_GOAL) {
                this->playerPos = make_pair(i,j);
                return false;
            }*/
            mBoardIndexes.insert(make_pair(make_pair(i,j),index));
            ++index;
        }
    }
    this->mBoardSize = size;
    return;
}

// Get the index for the coordinates (row,col) (row-wise)
int board::getIndex(int row, int col){
    return mBoardIndexes[make_pair(row, col)];
}

/*
 * Checks if a position on the board is accessible.
 * Bounds checking + type checking.
 */
bool board::isAccessible(int row, int col){

    char t = mBoard[row][col];
    if(t == FLOOR || t == GOAL){
        return true;    
    }
    return false;

}

bool board::isGoal(int row, int col){
    if(mBoard[row][col] == GOAL) {
        return true;    
    }
    return false;
}

/*
 * Returns all valid moves from the specified position
 */
vector<pair<pair<int,int>,char> > board::getAllValidMoves(int row, int col) {
    vector<pair<pair<int,int>,char> > validMoves;
    if (isAccessible(row-1, col)) {
        validMoves.push_back(make_pair(make_pair(row-1,col), MOVE_UP));
    }
    if (isAccessible(row+1, col)) {
        validMoves.push_back(make_pair(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isAccessible(row, col-1)) {
        validMoves.push_back(make_pair(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isAccessible(row, col+1)) {
        validMoves.push_back(make_pair(make_pair(row,col+1), MOVE_RIGHT));
    }
    return validMoves;
}
