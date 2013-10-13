#include <deque>
#include "board.h"
using std::cout;
using std::endl;
using std::pair;
using std::vector;
using std::make_pair;
using std::string;
using std::unordered_map;
using std::priority_queue;

board::board (const vector<vector<char> > &chars) {
    this->mBoard = chars;
    initializeIndexAndPositions(chars);
    findDeadlocks(chars);
    mWasPush = false;
    mWhatGotMeHere = '\0';
    mPath = "";
}

board::board (const vector<vector<char> > &chars, 
  bool wasPush, char whatGotMeHere,
  string path, std::vector<std::pair<int,int> > corners, 
  std::string newBoardString, int boardSize,
  vector<pair<int, int> > goalPositions,
  pair<int, int> newPlayerPosition,
  vector<pair<int, int> > newBoxPositions){
    this->mBoard = chars;
    this->mCornerPositions = corners;
    mGoalPositions = goalPositions;
    mBoxPositions = newBoxPositions;
    mPlayerPos = newPlayerPosition;
    //initializeIndexAndPositions(chars);
    mWasPush = wasPush;
    mWhatGotMeHere = whatGotMeHere;
    mPath = path;
    mBoardString = newBoardString;
    mBoardSize = boardSize;
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
        if(mBoard[position.first][position.second] == PLAYER ||
            mBoard[position.first][position.second] == PLAYER_ON_DEAD)
            mBoard[position.first][position.second] = PLAYER_ON_DEAD;
        else
            mBoard[position.first][position.second] = DEAD;
        //cout << "Inserted: " << position.first << ", " << position.second << endl;
    }
}



void board::findWallDeadlocks(){
    pair<int,int> currentDeadPosition;
    for(int i = 0; i < mCornerPositions.size(); i++){
        currentDeadPosition = mCornerPositions[i];
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
                    if(mBoard[i][j] == PLAYER){
                        mBoard[i][j] = PLAYER_ON_DEAD;
                    }else{
                        mBoard[i][j] = DEAD;
                    }
                    mCornerPositions.push_back(make_pair(i,j));
                }
                if(down == WALL && (left == WALL || right == WALL)) {
                    if(mBoard[i][j] == PLAYER){
                        mBoard[i][j] = PLAYER_ON_DEAD;
                    }else{
                        mBoard[i][j] = DEAD;
                    }
                    mCornerPositions.push_back(make_pair(i,j));
                }
            }
        }
    }
    findWallDeadlocks();
    //cout << "Size of mDeadPositions: " << mDeadPositions.size() << endl;
}

/*
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
 struct fcomparison {
    bool operator() ( pair< pair<pair<int,int>, string> ,int> a, pair< pair<pair<int,int>, string> ,int> b) {
        return  a.second >= b.second ? true : false;
    }
};

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
            if (c == PLAYER || c == PLAYER_ON_GOAL || c == PLAYER_ON_DEAD) {
                mPlayerPos = make_pair(i,j);
            }
            if(c == BOX || c == BOX_ON_GOAL){
                mBoxPositions.push_back(make_pair(i,j));
            }
            ++index;
        }
        boardString += '\n';
    }
    mBoardString = boardString;
    mBoardSize = size;
    return;
}

// SHOULD ONLY BE CALLED IF THE MOVE INCLUDES A BOX PUSH
board board::doLongMove(std::pair<int,int> newPlayerPos, std::pair<int,int> newBoxPos,
                         char lastMove, string path, int movedBox_positionInVector){

    
    // cout << "old playerpos: " << mPlayerPos.first << ", " << mPlayerPos.second << endl;
    // cout << "new playerpos: " << newPlayerPos.first << ", " << newPlayerPos.second << endl;
    std::vector<std::vector<char> > newMap = mBoard;
    std::string newBoardString;
    std::vector<std::pair<int,int> > newBoxPositions = mBoxPositions;
    newBoxPositions[movedBox_positionInVector] = newBoxPos;

    if(newMap[mPlayerPos.first][mPlayerPos.second] == '+')
        newMap[mPlayerPos.first][mPlayerPos.second] = '.';
    
    else if(newMap[mPlayerPos.first][mPlayerPos.second] == PLAYER_ON_DEAD)
        newMap[mPlayerPos.first][mPlayerPos.second] = DEAD;
    else
        newMap[mPlayerPos.first][mPlayerPos.second] = ' ';

    if(newMap[newBoxPos.first][newBoxPos.second] == GOAL)
        newMap[newBoxPos.first][newBoxPos.second] = '*';
    else
        newMap[newBoxPos.first][newBoxPos.second] = '$';

    if(newMap[newPlayerPos.first][newPlayerPos.second] == BOX_ON_GOAL)
        newMap[newPlayerPos.first][newPlayerPos.second] = '+';
    else if(newMap[newPlayerPos.first][newPlayerPos.second] == DEAD)
        newMap[newPlayerPos.first][newPlayerPos.second] = PLAYER_ON_DEAD;
    else
        newMap[newPlayerPos.first][newPlayerPos.second] = '@';

#if DEBUG
    int size;
    for (int i = 0; i < newMap.size(); i++) {
        size += newMap[i].size();
        for (int j = 0; j < newMap[i].size(); j++) {
            newBoardString += newMap[i][j];
        }
        newBoardString += '\n';
    }
#endif

    return board(newMap, true, lastMove, path + lastMove, mCornerPositions, newBoardString,
                 mBoardSize, mGoalPositions, newPlayerPos, newBoxPositions);        
}

board board::doMove(std::pair<int,int> newPlayerPos, char direction) {
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
            else if(newMap[newPlayerPos.first][newPlayerPos.second] == DEAD)
                newMap[newPlayerPos.first][newPlayerPos.second] = PLAYER_ON_DEAD;
            /* NORMAL MOVE */
            else
                newMap[newPlayerPos.first][newPlayerPos.second] = '@';
            
            /* DID WE LEAVE A GOAL POSITION */
            if(newMap[mPlayerPos.first][mPlayerPos.second] == '+')
                newMap[mPlayerPos.first][mPlayerPos.second] = '.';
            else if(newMap[mPlayerPos.first][mPlayerPos.second] == PLAYER_ON_DEAD)
                newMap[mPlayerPos.first][mPlayerPos.second] = DEAD;
            /* PREVIOUS POSITION WAS NORMAL */
            else
                newMap[mPlayerPos.first][mPlayerPos.second] = ' ';
        }
    }
    //return board(newMap, boxPush, direction, 
    //                 getPath() + direction, mCornerPositions);
    
}

void board::isAccessibleRestore(int row, int col){

    if(mBoard[row][col] == GOAL)
        mBoard[row][col] = BOX_ON_GOAL;
    else
        mBoard[row][col] = BOX;
    return;

}

bool board::isDynamicDeadlock(int row, int col, pair<int,int> boxPos){
    if(mBoard[row][col] == BOX_ON_GOAL)
            mBoard[row][col] = GOAL;
    else
        mBoard[row][col] = FLOOR;

    /* USED FOR DEBUG
    char orig1 = mBoard[boxPos.first][boxPos.second];
    char orig2 = mBoard[row][col];
    mBoard[boxPos.first][boxPos.second] = 'O';
    mBoard[row][col] = 'F';

    cout << "evaluating board" << endl;
    printBoard();

    mBoard[boxPos.first][boxPos.second] = orig1;
    mBoard[row][col] = orig2;
    */

    char up = WALL;
    char upr = WALL;
    char upl = WALL;
    char down = WALL;
    char downr = WALL;
    char downl = WALL;
    char left = WALL;
    char right = WALL;

    if(boxPos.first > 0){                            // SET UP
        up = mBoard[boxPos.first-1][boxPos.second];
        if(boxPos.second > 0){                        // SET UP LEFT CORNER
            upl = mBoard[boxPos.first-1][boxPos.second-1];
        }
        if(boxPos.second < mBoard[boxPos.first-1].size() - 1){ // SET UP RIGHT CORNER
            upr = mBoard[boxPos.first-1][boxPos.second+1];
        }
    }
    if(boxPos.first < mBoard.size() - 1){            // SET DOWN
        down = mBoard[boxPos.first+1][boxPos.second];
        if(boxPos.second > 0){                        // SET DOWN LEFT CORNER
            downl = mBoard[boxPos.first+1][boxPos.second-1];
        }
        if(boxPos.second < mBoard[boxPos.first+1].size() - 1) {
            downr = mBoard[boxPos.first+1][boxPos.second+1];
        }
    }
    if(boxPos.second > 0){                            // SET LEFT
        left = mBoard[boxPos.first][boxPos.second-1];
    }
    if(boxPos.second < mBoard[boxPos.first].size() - 1){       // SET RIGHT
        right = mBoard[boxPos.first][boxPos.second+1];
    }

    if(up == WALL || up == BOX || up == BOX_ON_GOAL) {
        if(upl == WALL || upl == BOX || upl == BOX_ON_GOAL) {
            if(left == WALL || left == BOX || left == BOX_ON_GOAL) {
                isAccessibleRestore(row, col);
                /*
                cout << "Deadlock 1" << endl;
                cout << upl << up << upr << endl;
                cout << left << "O" << right << endl;
                cout << downl << down << downr << endl;
                */
                return true;
            }
        }
        if(upr == WALL || upr == BOX || upr == BOX_ON_GOAL) {
            if(right == WALL || right == BOX || right == BOX_ON_GOAL) {
                isAccessibleRestore(row, col);
                /*
                cout << "Deadlock 2" << endl;
                cout << upl << up << upr << endl;
                cout << left << "O" << right << endl;
                cout << downl << down << downr << endl;
                */
                return true;
            }
        }
    }
    if(down == WALL || down == BOX || down == BOX_ON_GOAL){
        if(downl == WALL || downl == BOX || downl == BOX_ON_GOAL) {
            if(left == WALL || left == BOX || left == BOX_ON_GOAL) {
                isAccessibleRestore(row, col);
                /*
                cout << "Deadlock 3" << endl;
                cout << upl << up << upr << endl;
                cout << left << "O" << right << endl;
                cout << downl << down << downr << endl;
                */
                return true;
            }
        }
        if(downr == WALL || downr == BOX || downr == BOX_ON_GOAL) {
            if(right == WALL || right == BOX || right == BOX_ON_GOAL) {
                isAccessibleRestore(row, col);
                /*
                cout << "Deadlock 4" << endl;
                cout << upl << up << upr << endl;
                cout << left << "O" << right << endl;
                cout << downl << down << downr << endl;
                */
                return true;
            }
        }
    }

    //cout << "boxPos: (" << boxPos.first << ", " << boxPos.second << ")" << endl;

    //printBoard();
    // if(mBoard[boxPos.first][boxPos.second] == GOAL) {
    //     mBoard[boxPos.first][boxPos.second] = BOX_ON_GOAL;
    // }else if(mBoard[boxPos.first][boxPos.second] == FLOOR) {
    //     mBoard[boxPos.first][boxPos.second] = BOX;
    // }

    // if(left == DEAD) {
    //     if(isDeadspace(boxPos.first, boxPos.second-1)) {
    //         //cout << "returning deadspace" << endl;
    //         isAccessibleRestore(row, col);
    //         if(mBoard[boxPos.first][boxPos.second] == BOX_ON_GOAL) {
    //             mBoard[boxPos.first][boxPos.second] = GOAL;
    //         }else if(mBoard[boxPos.first][boxPos.second] == BOX) {
    //             mBoard[boxPos.first][boxPos.second] = FLOOR;
    //         }
    //         return true;
    //     }
    // }
    // if(right == DEAD) {
    //     if(isDeadspace(boxPos.first, boxPos.second+1)) {
    //         //cout << "returning deadspace" << endl;
    //         isAccessibleRestore(row, col);
    //         if(mBoard[boxPos.first][boxPos.second] == BOX_ON_GOAL) {
    //             mBoard[boxPos.first][boxPos.second] = GOAL;
    //         }else if(mBoard[boxPos.first][boxPos.second] == BOX) {
    //             mBoard[boxPos.first][boxPos.second] = FLOOR;
    //         }
    //         return true;
    //     }
    // }
    // if(up == DEAD) {
    //     if(isDeadspace(boxPos.first-1, boxPos.second)) {
    //         //cout << "returning deadspace" << endl;
    //         isAccessibleRestore(row, col);
    //         if(mBoard[boxPos.first][boxPos.second] == BOX_ON_GOAL) {
    //             mBoard[boxPos.first][boxPos.second] = GOAL;
    //         }else if(mBoard[boxPos.first][boxPos.second] == BOX) {
    //             mBoard[boxPos.first][boxPos.second] = FLOOR;
    //         }
    //         return true;
    //     }
    // }
    // if(down == DEAD) {
    //     if(isDeadspace(boxPos.first+1, boxPos.second)) {
    //         //cout << "returning deadspace" << endl;
    //         isAccessibleRestore(row, col);
    //         if(mBoard[boxPos.first][boxPos.second] == BOX_ON_GOAL) {
    //             mBoard[boxPos.first][boxPos.second] = GOAL;
    //         }else if(mBoard[boxPos.first][boxPos.second] == BOX) {
    //             mBoard[boxPos.first][boxPos.second] = FLOOR;
    //         }
    //         return true;
    //     }
    // }

    // if(mBoard[boxPos.first][boxPos.second] == BOX_ON_GOAL) {
    //     mBoard[boxPos.first][boxPos.second] = GOAL;
    // }else if(mBoard[boxPos.first][boxPos.second] == BOX) {
    //     mBoard[boxPos.first][boxPos.second] = FLOOR;
    // }

    //printBoard();
    // END DYNAMIC DEADLOCK
    
    isAccessibleRestore(row, col);

    return false;
}

bool board::isDeadspace(int row, int col) {
    //cout << "looking for deadspace from: (" << row << ", " << col << ")" << endl;
    std::queue<pair<int,int> > bfs_queue;
    std::set<pair<int,int> > visited;
    bfs_queue.push(make_pair(row,col));
    visited.insert(make_pair(row,col));
    while(!bfs_queue.empty()) {
        pair<int,int> node = bfs_queue.front();
        //if(row == 1 && col == 5) {
            //cout << "bfsing position (" << node.first << ", " << node.second << ")" << endl;
        //}
        bfs_queue.pop();
        visited.insert(node);
        if(node.first > 0) {
            pair<int,int> up = make_pair(node.first-1, node.second);
            if(visited.find(up) == visited.end()) {
                if(mBoard[up.first][up.second] == FLOOR || mBoard[up.first][up.second] == GOAL || mBoard[up.first][up.second] == PLAYER_ON_GOAL || mBoard[up.first][up.second] == PLAYER_ON_DEAD) {
                    //cout << "not deadspace up: (" << up.first << ", " << up.second << ")" << endl;
                    //printBoard();
                    return false;
                }
                if(mBoard[up.first][up.second] == DEAD) {
                    bfs_queue.push(up);
                }
            }
        }
        if(node.first < mBoard.size()-1) {
            pair<int,int> down = make_pair(node.first+1, node.second);
            if(visited.find(down) == visited.end()) {
                if(mBoard[down.first][down.second] == FLOOR || mBoard[down.first][down.second] == GOAL || mBoard[down.first][down.second] == PLAYER_ON_GOAL || mBoard[down.first][down.second] == PLAYER_ON_DEAD) {
                    //cout << "not deadspace down: (" << down.first << ", " << down.second << ")" << endl;
                    //printBoard();
                    return false;
                }
                if(mBoard[down.first][down.second] == DEAD) {
                    bfs_queue.push(down);
                }
            }
        }
        if(node.second > 0) {
            pair<int,int> left = make_pair(node.first, node.second-1);
            if(visited.find(left) == visited.end()) {
                if(mBoard[left.first][left.second] == FLOOR || mBoard[left.first][left.second] == GOAL || mBoard[left.first][left.second] == PLAYER_ON_GOAL || mBoard[left.first][left.second] == PLAYER_ON_DEAD) {
                    //cout << "not deadspace left: (" << left.first << ", " << left.second << ")" << endl;
                    //printBoard();
                    return false;
                }
                if(mBoard[left.first][left.second] == DEAD) {
                    bfs_queue.push(left);
                }
            }
        }
        if(node.second < mBoard[node.first].size()) {
            pair<int,int> right = make_pair(node.first, node.second+1);
            if(visited.find(right) == visited.end()) {
                if(mBoard[right.first][right.second] == FLOOR || mBoard[right.first][right.second] == GOAL || mBoard[right.first][right.second] == PLAYER_ON_GOAL || mBoard[right.first][right.second] == PLAYER_ON_DEAD) {
                    //cout << "not deadspace right: (" << right.first << ", " << right.second << ")" << endl;
                    //printBoard();
                    return false;
                }
                if(mBoard[right.first][right.second] == DEAD) {
                    bfs_queue.push(right);
                }
            }
        }
    }
    //cout << "deadspace found!" << endl;
    return true;
}

/*
 * Checks if a position on the board is accessible.
 */
 bool board::isAccessible(int row, int col, int prevRow, int prevCol) {
    // If we can't stand here    
    if (!isWalkable(prevRow, prevCol))
        return false;

    // Check regular move
    if (isWalkable(row, col)){
        return true;
    }
    // Check box push
    else if (isBox(row, col)) {
        pair<int,int> boxPos = make_pair(prevRow+(row-prevRow)*2,
           prevCol+(col-prevCol)*2);
        
        
                
        //STATIC DEADLOCKS
        if (mBoard[boxPos.first][boxPos.second] == DEAD){
            return false;
        }

        if (!isWalkable(boxPos.first,boxPos.second)){
            return false;
        }


        //DYNAMIC DEADLOCKS
        /*
        if(isDynamicDeadlock(row, col, boxPos)){
           //cout << "dynamic deadlock found" << endl;
           return false;
        }*/

        if (mBoard[boxPos.first][boxPos.second] == GOAL){
            return true;        
        }

        if(isWalkable(boxPos.first,boxPos.second)) {
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
    if(t == FLOOR || t == GOAL || t == PLAYER || t == PLAYER_ON_GOAL || 
       t == DEAD || t == PLAYER_ON_DEAD){
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
 void board::getAllValidMoves(vector<board> &moves) {
    int row = getPlayerPosition().first;
    int col = getPlayerPosition().second;
    // std::cout << "getAllValidMoves(" << row << ", " << col << ")" << std::endl;
    if (isAccessible(row-1, col, row, col)) {
        moves.push_back(doMove(make_pair(row-1,col), MOVE_UP));
    }
    if (isAccessible(row+1, col, row, col)) {
        moves.push_back(doMove(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isAccessible(row, col-1, row, col)) {
        moves.push_back(doMove(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isAccessible(row, col+1, row, col)) {
        moves.push_back(doMove(make_pair(row,col+1), MOVE_RIGHT));
    }
}

void board::getAllValidWalkMoves(vector< pair<pair<int,int>, char> > &moves, pair<int,int> playerPos) {
    int row = playerPos.first;
    int col = playerPos.second;
    // std::cout << "getAllValidMoves(" << row << ", " << col << ")" << std::endl;
    if (isWalkable(row-1, col)) {
        moves.push_back(make_pair(make_pair(row-1,col), MOVE_UP));
    }
    if (isWalkable(row+1, col)) {
        moves.push_back(make_pair(make_pair(row+1,col), MOVE_DOWN));
    }
    if (isWalkable(row, col-1)) {
        moves.push_back(make_pair(make_pair(row,col-1), MOVE_LEFT));
    }
    if (isWalkable(row, col+1)) {
        moves.push_back(make_pair(make_pair(row,col+1), MOVE_RIGHT));
    }
}

/*
 * This method investigates whether the collected adjacent positions
 * of the box defined by possibleBoxPush.boxPosition that are
 * contained in possibles are positions from which
 * the box actually can be pushed. If they are, they
 * are added into possibleBoxPush.positionsAroundBox (if not already in there)
 */
void board::investigateThesePositions(struct possibleBoxPush &possibleBoxPush, 
 vector<pair<int,int> > &possibles, vector<board> &moves, string path){

    for(int i = 0; i < possibles.size(); i++){
            
            if(!vectorContainsPair(possibleBoxPush.positionsAroundBox, possibles[i])){

                    possibleBoxPush.positionsAroundBox.push_back(possibles[i]);

                    std::pair<int,int> pushedBoxCoordinates = getPushCoordinates(
                                                  possibles[i],
                                                  possibleBoxPush.boxPosition);

                    char lastMove = translateDirection(getDirectionToPos(possibles[i],
                                          possibleBoxPush.boxPosition));
                    moves.push_back(doLongMove(possibleBoxPush.boxPosition, 
                                    pushedBoxCoordinates, lastMove, path, 
                                    possibleBoxPush.movedBox_positionInVector));
                    
            }
        }
}

/*
 * Do a local search on the box specified by possibleBoxPush.boxPosition.
 * The argument directionToBox specifies the location of the box
 * relative to the player specified by possibleBoxPush.playerPosition.
 */
void board::circleBox(struct possibleBoxPush &possibleBoxPush, char directionToBox, vector<board> &moves, string path){

    // Is the box on the same col or the same row?
    char axis;
    // Investigatorpos will hold the coordinates of the currently investigated
    // adjacent position to box
    pair<int, int> investigatorPos = possibleBoxPush.playerPosition;
    // This vector will hold positions that are reachable by local
    // search but are yet to be determined if the box can be pushed from that
    // position.
    vector<pair<int, int> > possiblePositions;

    if(possiblePositions.size() > 3)
        cout << "Problem in circleBox!" << endl;

    
    possiblePositions.push_back(possibleBoxPush.playerPosition);
    investigateThesePositions(possibleBoxPush, possiblePositions, moves, path);
    
}

// Determine the position of box relative to player
char board::getDirectionToPos(std::pair<int, int> player, std::pair<int, int> box){

    int rowDelta;
    int colDelta;

    rowDelta = player.first - box.first;
    colDelta = player.second - box.second;

    if(rowDelta != 0 && colDelta != 0)
        cout << "Player is not standing next to box!" << endl;

    if(rowDelta > 0)
        return 'N';
    else if(rowDelta < 0)
        return 'S';
    else if(colDelta > 0)
        return 'W';
    else
        return 'E';

}

char board::translateDirection(char nsew){

    switch(nsew){
        case 'N':
            return 'U';
            break;
        case 'S':
            return 'D';
            break;
        case 'E':
            return 'R';
            break;
        case 'W':
            return 'L';
            break;    
    }
}

std::pair<int,int> board::getPushCoordinates(std::pair<int,int> playerCoordinates,
                                    std::pair<int,int> boxCoordinates){

    std::pair<int,int> pushedBoxCoordinates = boxCoordinates;
    
    char directionToBox = getDirectionToPos(playerCoordinates, boxCoordinates);

    if(directionToBox == 'N')
        pushedBoxCoordinates.first--;
    else if(directionToBox == 'S')
        pushedBoxCoordinates.first++;
    else if(directionToBox == 'W')
        pushedBoxCoordinates.second--;
    else 
        pushedBoxCoordinates.second++;
    return pushedBoxCoordinates;

}

void board::updatePlayerPosition(std::pair<int, int> newPlayerPosition){

    if(mBoard[mPlayerPos.first][mPlayerPos.second] == '+')
        mBoard[mPlayerPos.first][mPlayerPos.second] = '.';
    else if(mBoard[mPlayerPos.first][mPlayerPos.second] == PLAYER_ON_DEAD)
        mBoard[mPlayerPos.first][mPlayerPos.second] = DEAD;
    else
        mBoard[mPlayerPos.first][mPlayerPos.second] = ' ';

    if(mBoard[newPlayerPosition.first][newPlayerPosition.second] == GOAL)
        mBoard[newPlayerPosition.first][newPlayerPosition.second] = PLAYER_ON_GOAL;
    else if(mBoard[newPlayerPosition.first][newPlayerPosition.second] == DEAD)
        mBoard[newPlayerPosition.first][newPlayerPosition.second] = PLAYER_ON_DEAD;
    else
        mBoard[newPlayerPosition.first][newPlayerPosition.second] = PLAYER;
    mPlayerPos = newPlayerPosition;

}

/*
 * This function investigates if and possibly in how many directions the box
 * who's coordinates is included in currentBox can be pushed. 
 * Will insert related state changes that occur when the box is 
 * pushed in the found possible directions.
 */
void board::investigatePushBoxDirections(struct possibleBoxPush &currentBox, vector<board> &moves){
    
    // Temp variable
    pair<int, int> possiblePosition;
    pair<int, int> boxPosition;
    // Will hold all the directly adjacent positions to the box (N S E W)
    vector<pair<int,int> > possiblePositions;
    vector<std::string> possiblePaths;
    possiblePosition = currentBox.boxPosition;
    boxPosition = currentBox.boxPosition;    

    // BELOW CODE IS TRYING TO NOT DO IMMEDIATE REPEAT MOVES
    /*
    possiblePosition.first--;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second)){
        possiblePosition.first += 2;
        if(possiblePosition != mPlayerPos){
            possiblePosition.first -=2;
            possiblePositions.push_back(possiblePosition);
        }
        else
            possiblePosition.first -=2;
    }
    possiblePosition.first += 2;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second)){
        possiblePosition.first -= 2;
        if(possiblePosition != mPlayerPos){
            possiblePosition.first += 2;
            possiblePositions.push_back(possiblePosition);
        }
        else
            possiblePosition.first += 2;
    }
    possiblePosition.first--;
    possiblePosition.second--;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second)){
        possiblePosition.second += 2;
        if(possiblePosition != mPlayerPos){
            possiblePosition.second -= 2;
            possiblePositions.push_back(possiblePosition);
        }
        else
            possiblePosition.second -= 2;
    }
    possiblePosition.second += 2;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second)){
        possiblePosition.second -= 2;
        if(possiblePosition != mPlayerPos){
            possiblePosition.second += 2;
            possiblePositions.push_back(possiblePosition);
        }
        else
            possiblePosition.second += 2;
    }

    possiblePosition.second--;*/

    possiblePosition.first--;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.first += 2;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.first--;
    possiblePosition.second--;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.second += 2;
    if(isAccessible(boxPosition.first, boxPosition.second,
                    possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);

    possiblePosition.second--;


    
    
    // Will hold the direction of the box relative to the player.
    // If the box is to the left ot the player directionToBox will be 'W' (west)
    char directionToBox;
    std::string currPath;
    // Loop through all of the directly adjacent positions to the box
    for(int i = 0; i < possiblePositions.size(); i++){
       
        // If we've already determined that this position is reachable, 
        // we don't need to do it again
        if(!vectorContainsPair(currentBox.positionsAroundBox, possiblePositions[i])){
            
            // Is the possiblePositions[i] reachable from our position?
            currPath = boxSearch(possiblePositions[i]);
            
            
            currentBox.boxPosition = possiblePosition;
            if(currPath != "x"){
                // UNCOMMENT TO UPDATE PLAYER POSITION BETWEEN EACH BOXSEARCH RUN
                //updatePlayerPosition(possiblePositions[i]);
                //mPath = currPath;
                currentBox.playerPosition = possiblePositions[i];

                // Determine the relative position of the box
                directionToBox = getDirectionToPos(currentBox.playerPosition,
                                                currentBox.boxPosition);
                // Can we find adjacent positions through a local search?
                circleBox(currentBox, directionToBox, moves, currPath);
            }
        }
    }
}

/*
 * This will insert all the possible state changes
 * that will occur due to box movement into moves.
 */ 
void board::getPossibleStateChanges(vector<board> &moves){

    // This struct will hold information
    // used on a per-box-basis.
    // A new one is set each time a new box
    // on the board is investigated.

    struct possibleBoxPush currentBox;
    pair<int, int> prevBoxPos;
    // Loop through all boxes on the board
    for(int i = 0; i < mBoxPositions.size(); i++){
        // Start by determining its coordinates
        currentBox.boxPosition = mBoxPositions[i];
        currentBox.positionsAroundBox.clear();
        currentBox.movedBox_positionInVector = i;
        // Let's look at how many directions it can go
        
        investigatePushBoxDirections(currentBox, moves);
        
    }
}

void board::printBoard() const{
    cout << mBoardString << endl;
    // std::cout << "printBoard" << std::endl;
    //for (int i = 0; i < mBoard.size(); i++) {
    //    for (int j = 0; j < mBoard[i].size(); j++) {
    //        cout << mBoard[i][j];
    //    }
    //    cout << '\n';
    //}
}

/*
 * Finds pushable boxes using A*
 */
 string board::boxSearch(pair<int,int> goalPos){
    pair<int,int> playerPos = getPlayerPosition();
    string path = getPath();
    if (playerPos == goalPos) {
        return path;
    }
    std::unordered_map<string, int> closed;

    std::deque< pair<pair<int,int>, string> > q;
    q.push_front(make_pair(playerPos, path));
    
    while(!q.empty()) {
        pair<pair<int,int>, string> currentPos = q.front();
        q.pop_front();

        vector< pair<pair<int,int>, char> > moves;
        getAllValidWalkMoves(moves, currentPos.first);
        std::unordered_map<string,int>::const_iterator map_it;
        // Iterate through all valid moves (neighbours)
        for (int k = 0; k < moves.size(); ++k) {
            pair<int,int> tempPos = moves[k].first;
            char direction = moves[k].second;
            if (tempPos == goalPos) {
                return currentPos.second + direction;
            }
            string key = std::to_string(tempPos.first) + "-" + std::to_string(tempPos.second);
            map_it = closed.find(key);
            if (map_it != closed.end()) {
                continue;
            }
            string updatedPath = currentPos.second + direction;
            q.push_front(make_pair(tempPos, updatedPath));
            closed.insert(make_pair(key, 0));
        }
    }
    return "x";
}

/*
 * Returns the estimated distance between two positions
 */
 int board::distance(pair<int,int> i, pair<int,int> j ) {
    // Manhattan method (10 instead of 1)
    return 1*abs(j.first-i.first) + abs(j.second-i.second);
    // Diagonal shortcut
/*    int xDelta = abs(i2-i1);
    int yDelta =  abs(j2-j1);
    if (xDelta > yDelta) {
        return 14*yDelta + 10*(xDelta-yDelta);
    }
    else {
        return 14*xDelta + 10*(yDelta-xDelta);
    }
*/
}

bool board::vectorContainsPair(vector<pair<int, int> > &vector, pair<int, int> &pair){

    for(int i = 0; i < vector.size(); i++){
        if(pair == vector[i])
            return true;    
    }

    return false;
}

void board::setBoxPositionsString() {
    string boxString = "";
    for (int i = 0; i < mBoxPositions.size(); ++i) {
        boxString += (mBoxPositions[i].first + "-" + mBoxPositions[i].second);
    }
    mBoxString = boxString;
}



