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
    findDeadlocks(chars);
    mWasPush = false;
    mWhatGotMeHere = '\0';
    // printBoard();
}

board::board (const vector<vector<char> > &chars, bool wasPush, char whatGotMeHere, std::set<pair<int,int> > deadPositions, string path){
    this->mBoard = chars;
    this->mDeadPositions = deadPositions;
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
                    mDeadPositions.insert(make_pair(i,j));
                }
                if(down == WALL && (left == WALL || right == WALL)) {
                    mDeadPositions.insert(make_pair(i,j));
                }
            }
        }
    }
    findWallDeadlocks();
    //cout << "Size of mDeadPositions: " << mDeadPositions.size() << endl;
}

pair<int,int> board::getRelativePosition(char direction, pair<int,int> position){
    pair<int,int> newPos = position;
    switch(direction){
        case 'N':
            newPos.first--;
            break;
        case 'S':
            newPos.first++;
            break;
        case 'W':
            newPos.second--;
            break;
        case 'E':
            newPos.second++;
            break;    
    }

    return newPos;
}

bool board::stillHuggingWall(char wallDirection, pair<int,int> position){
    pair<int,int> wallPosition = getRelativePosition(wallDirection, position);
    if(wallPosition.first >= mBoard.size() || position.second >= mBoard[position.first].size())
        return false;
    if(wallPosition.first < 0 || wallPosition.second < 0)
        return false;
    if(mBoard[wallPosition.first][wallPosition.second] == WALL)
        return true;
    return false;
}

bool board::investigateWall(char direction, char wallDirection, pair<int,int> position){
    
    position = getRelativePosition(direction, position);
    if(position.first >= mBoard.size() || position.second >= mBoard[position.first].size())
        return false; //Outside of map
    if(position.first < 0 || position.second < 0)
        return false; 
    
    if(mBoard[position.first][position.second] == GOAL || mBoard[position.first][position.second] == BOX_ON_GOAL)
        return false;
    if(!stillHuggingWall(wallDirection, position))
        return false;
    if(mBoard[position.first][position.second] == WALL)
        return true;
    
    if(investigateWall(direction, wallDirection, position)){
        mDeadPositions.insert(position);
        //cout << "Inserted: " << position.first << ", " << position.second << endl;
    }
}

void board::findWallDeadlocks(){
    pair<int,int> currentDeadPosition;
    std::set<pair<int,int> >::iterator it;
    for(it = mDeadPositions.begin(); it != mDeadPositions.end(); it++){
        currentDeadPosition = *it;
        investigateWall('N', 'W', currentDeadPosition);
        investigateWall('N', 'E', currentDeadPosition);
        investigateWall('S', 'W', currentDeadPosition);
        investigateWall('S', 'E', currentDeadPosition);
        investigateWall('W', 'N', currentDeadPosition);
        investigateWall('W', 'S', currentDeadPosition);
        investigateWall('E', 'N', currentDeadPosition);
        investigateWall('E', 'S', currentDeadPosition);
    }
}


board* board::doMove(std::pair<int,int> newPlayerPos, char direction, string prevPath) const{
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
    prevPath = prevPath + direction;
    return new board(newMap, boxPush, direction, mDeadPositions, prevPath);
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
         //std::cout << "looking for (" << prevRow+(row-prevRow)*2 << ", " << prevCol+(col-prevCol)*2 << ")" << std::endl;
        if (std::find(mDeadPositions.begin(), mDeadPositions.end(), make_pair(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2)) != mDeadPositions.end()) {
           // std::cout << "Deadlock found, not pushing!" << std::endl;
            return false;
        }/*
        std::set<pair<int,int> >::iterator it = mDeadPositions.find(make_pair(prevRow+(row-prevRow)*2,prevCol+(col-prevCol)*2));
        if(it == mDeadPositions.end())
            return false;*/
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
void board::getAllValidMoves(vector<board> &moves, string prevPath) const{
    int row = getPlayerPosition().first;
    int col = getPlayerPosition().second;
    // std::cout << "getAllValidMoves(" << row << ", " << col << ")" << std::endl;
    if (isAccessible(row-1, col, row, col)) {
        moves.push_back(*doMove(make_pair(row-1,col), MOVE_UP, prevPath));
    }
    if (isAccessible(row+1, col, row, col)) {
        moves.push_back(*doMove(make_pair(row+1,col), MOVE_DOWN, prevPath));
    }
    if (isAccessible(row, col-1, row, col)) {
        moves.push_back(*doMove(make_pair(row,col-1), MOVE_LEFT, prevPath));
    }
    if (isAccessible(row, col+1, row, col)) {
        moves.push_back(*doMove(make_pair(row,col+1), MOVE_RIGHT, prevPath));
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
    // cout << "Dead positions: ";
    // for(int i = 0; i < mDeadPositions.size(); i++) {
    //     cout << "(" << mDeadPositions[i].first << ", " << mDeadPositions[i].second << ") ";
    // }
    // cout << std::endl;
}
