#include <iostream>
#include "board.h"
#include <string.h>
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;

board::board (const vector<vector<char> > &chars) {
    this->mBoard = chars;
    initializeIndexAndPositions(chars);
    findDeadlocks(chars);
    mWasPush = false;
    mWhatGotMeHere = '\0';
    // printBoard();

}

board::board (const vector<vector<char> > &chars, bool wasPush, char whatGotMeHere, vector<pair<int,int> > deadPositions){
    this->mBoard = chars;
    this->mDeadPositions = deadPositions;
    initializeIndexAndPositions(chars);
    mWasPush = wasPush;
    mWhatGotMeHere = whatGotMeHere;
}
/*
board::board (const board &source, bool wasPush, 
    char whatGotMeHere, std::pair<int,int> playerPos,std::vector<std::pair<int,int> > boxPositions){

    mLongestRow = source.mLongestRow;
    mNumRows = source.mNumRows;
    mWasPush = wasPush;
    mWhatGotMeHere = whatGotMeHere;
    //memcpy((void*)&mGoalPositions, (void*)&source.mGoalPositions, sizeof(source.mGoalPositions));
    mGoalPositions = source.mGoalPositions;
    mBoard = source.mBoard;
    mBoxPositions = boxPositions;
    mPlayerPos = playerPos;
    mBoardSize = source.mBoardSize;
}*/

/*
 * Loops over the 2d vector of chars and initializes
 * player position, goal positions and indexes.
 */
void board::initializeIndexAndPositions(const vector<vector<char> > &chars) {
    std::string boardString = "";
    int index = 0;
    int size = 0;
    int longestRow = -1;
    for (int i = 0; i < chars.size(); i++) {
        int currentRowSize = chars[i].size();
        size += currentRowSize;
        if (longestRow < currentRowSize)
            longestRow = currentRowSize;
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
            /*
            else if (c == PLAYER_ON_GOAL) {
                this->playerPos = make_pair(i,j);
                return false;
            }*/
            ++index;
        }
    }
    mBoardString = boardString;
    mBoardSize = size;
    mLongestRow = longestRow;
    mNumRows = chars.size();
    return;
}

void board::findDeadlocks(const vector<vector<char> > &chars) {
    for (int i = 0; i < chars.size(); i++) {
        for (int j = 0; j < chars[i].size(); j++) {
            char c = chars[i][j];
            // DEADLOCK CHECK
            if(c == PLAYER || c == FLOOR){
                char up;
                char down;
                char left;
                char right;
                if(i > 0){
                    up = chars[i-1][j];
                }else{ up = WALL;}
                if(i < chars.size() - 1){
                    down = chars[i+1][j];
                }else{ down = WALL;}
                if(j > 0){
                    left = chars[i][j-1];
                }else{ left = WALL;}
                if(j < chars[i].size() - 1){
                    right = chars[i][j+1];
                }else{ right = WALL;}
                if(up == WALL && (left == WALL || right == WALL)) {
                    mDeadPositions.push_back(make_pair(i,j));
                }
                if(down == WALL && (left == WALL || right == WALL)) {
                    mDeadPositions.push_back(make_pair(i,j));
                }
            }
        }
    }
}


board* board::doMove(std::pair<int,int> newPlayerPos, char direction) const{
    // std::cout << "doMove(<" << newPlayerPos.first << "," << newPlayerPos.second << ">, " << direction << ")" << std::endl;
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
    return new board(newMap, boxPush, direction, mDeadPositions);
}

/*
 * Checks if a position on the board is accessible.
 */
bool board::isAccessible(int row, int col, int prevRow, int prevCol) const{
    // Check regular move
    if (isWalkable(row, col)){
        // std::cout << "isAccessible(" << row << ", " << col << ", " << prevRow << ", " << prevCol << "): yes" << std::endl;
        return true;
    }
    // Check box push
    else if (isBox(row, col)) {
        std::cout << "looking for (" << prevRow+(row-prevRow)*2 << ", " << prevCol+(col-prevCol)*2 << ")" << std::endl;
        if (std::find(mDeadPositions.begin(), mDeadPositions.end(), make_pair(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2)) != mDeadPositions.end()) {
            std::cout << "Deadlock found, not pushing!" << std::endl;
            return false;
        }
        if (isWalkable(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2)){
            // std::cout << "isAccessible(" << row << ", " << col << ", " << prevRow << ", " << prevCol << "): yes" << std::endl;
            return true;
        }
    }
    // std::cout << "isAccessible(" << row << ", " << col << ", " << prevRow << ", " << prevCol << "): no" << std::endl;
    return false;
}



/*
 * Checks if all boxes are on goals (finish state).
 */
bool board::isFinished() const{

    if (mBoardString.find('$')!= std::string::npos)
        return false;
    return true;

    // bool boxOnGoal = false;
    // for(int i = 0; i < mBoxPositions.size(); i++){
    //     for(int j = 0; j < mGoalPositions.size() && !boxOnGoal; j++){
    //         if(mBoxPositions.at(i) == mGoalPositions.at(j) ){
    //             boxOnGoal = true;
    //         }
    //     }
    //     if(!boxOnGoal)
    //         return false;
    //     boxOnGoal = false;    
    // }

    // std::cout << "FINISHED!" << std::endl;
    // return true;
}

bool board::isWalkable(int row, int col) const {
    // std::cout << "isWalkable(" << row << ", " << col << "): ";
    char t = mBoard[row][col];
    // Check regular move
    if(t == FLOOR || t == GOAL){
        // std::cout << "yes" << std::endl;
        return true;    
    }    
    // std::cout << "no" << std::endl;
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


void board::printBoard() {
    // std::cout << "printBoard" << std::endl;
    for (int i = 0; i < mBoard.size(); i++) {
        for (int j = 0; j < mBoard[i].size(); j++) {
            cout << mBoard[i][j];
        }
        cout << '\n';
    }
    cout << "Dead positions: ";
    for(int i = 0; i < mDeadPositions.size(); i++) {
        cout << "(" << mDeadPositions[i].first << ", " << mDeadPositions[i].second << ") ";
    }
    cout << std::endl;
}
