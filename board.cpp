#include <iostream>
#include "board.h"
#include <string.h>
#include <unordered_map>
#include <queue>
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
    mWasPush = false;
    mWhatGotMeHere = '\0';
    mPath = "";
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
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
 struct fcomparison {
    bool operator() (pair<board,float> a, pair<board,float> b) {
        return  a.second > b.second ? true : false;
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

// SHOULD ONLY BE CALLED IF THE MOVE INCLUDES A BOX PUSH
board* board::doLongMove(std::pair<int,int> newPlayerPos, std::pair<int,int> newBoxPos,
                         char lastMove, string path){

    
    // cout << "old playerpos: " << mPlayerPos.first << ", " << mPlayerPos.second << endl;
    // cout << "new playerpos: " << newPlayerPos.first << ", " << newPlayerPos.second << endl;
    std::vector<std::vector<char> > newMap = mBoard;

    if(newMap[mPlayerPos.first][mPlayerPos.second] == '+')
        newMap[mPlayerPos.first][mPlayerPos.second] = '.';
    else
        newMap[mPlayerPos.first][mPlayerPos.second] = ' ';

    if(newMap[newBoxPos.first][newBoxPos.second] == GOAL)
        newMap[newBoxPos.first][newBoxPos.second] = '*';
    else
        newMap[newBoxPos.first][newBoxPos.second] = '$';

    if(newMap[newPlayerPos.first][newPlayerPos.second] == BOX_ON_GOAL)
        newMap[newPlayerPos.first][newPlayerPos.second] = '+';
    else
        newMap[newPlayerPos.first][newPlayerPos.second] = '@';
    
    return new board(newMap, true, lastMove, path + lastMove);        
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
    // If we can't stand here    
    //if (!isWalkable(prevRow, prevCol))
    //    return false;

    // Check regular move
    if (isWalkable(row, col)){
        return true;
    }
    // Check box push
    else if (isBox(row, col)) {
        pair<int,int> boxPos = make_pair(prevRow+(row-prevRow)*2,
           prevCol+(col-prevCol)*2);
        /*
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
                    cout << "Deadlock 1";
                    return false;
                }
            }
            if(upr == WALL || upr == BOX) {
                if(right == WALL || right == BOX) {
                    cout << "Deadlock 2";
                    return false;
                }
            }
        }
        if(down == WALL || down == BOX){
            if(downl == WALL || downl == BOX) {
                if(left == WALL || left == BOX) {
                    cout << "Deadlock 3";
                    return false;
                }
            }
            if(downr == WALL || downr == BOX) {
                if(right == WALL || right == BOX) {
                    cout << "Deadlock 4";
                    return false;
                }
            }
        }
        // END DYNAMIC DEADLOCK
        */
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
    if(t == FLOOR || t == GOAL || t == PLAYER || t == PLAYER_ON_GOAL){
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
        //Can we push the box from this position?
        
        if(isAccessible(possibleBoxPush.boxPosition.first,
                                possibleBoxPush.boxPosition.second,
                                possibles[i].first,
                                possibles[i].second)){
            //cout << "ISACCESIBLE\n";
            
            if(!vectorContainsPair(possibleBoxPush.positionsAroundBox, possibles[i])){
                    //cout << "INSERTED INTO POSITIONSAROUNDBOX\n";
                    possibleBoxPush.positionsAroundBox.push_back(possibles[i]);
                    //cout << possibleBoxPush.positionsAroundBox.size() << endl;
                    std::pair<int,int> pushedBoxCoordinates = getPushCoordinates(
                                                  possibles[i],
                                                  possibleBoxPush.boxPosition);

                    char lastMove = translateDirection(getDirectionToPos(possibles[i],
                                          possibleBoxPush.boxPosition));
                    moves.push_back(*doLongMove(possibleBoxPush.boxPosition, 
                                    pushedBoxCoordinates, lastMove, path));
                    
            }
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

    /*
     * REST OF THIS METHOD IS JUST STEPPING THROUGH
     * TILES AROUND THE BOX AND ADDING THE RELEVANT POSITIONS
     * INTO "possiblePositions". Ends with calling "investigateThesePositions".
     * LEFT TODO IS INVESTIGATING THE OPPOSITE SIDE OF THE BOX, IF POSSIBLE
     */
    /*
    if(directionToBox == 'N' || directionToBox == 'S')
        axis = 'x';
    else
        axis = 'y';

    if(axis == 'x'){
        //Step to the right
        investigatorPos.second++;
        if(isWalkable(investigatorPos.first, investigatorPos.second)){
            //Step up
            if(directionToBox == 'N'){
                investigatorPos.first--;
                //Maybe we can push from this direction as well!
                possiblePositions.push_back(investigatorPos);
            }
            else {
                investigatorPos.first++;
                possiblePositions.push_back(investigatorPos);
            }        
        }
        //Step to the left
        investigatorPos = possibleBoxPush.playerPosition;
        investigatorPos.second--;
        if(isWalkable(investigatorPos.first, investigatorPos.second)){
            //Step up
            if(directionToBox == 'N'){
                investigatorPos.first--;
                //Maybe we can push from this direction as well!
                possiblePositions.push_back(investigatorPos);
            }
            else {
                investigatorPos.first++;
                possiblePositions.push_back(investigatorPos);
            }        
        }
    }
    //Axis is y
    else {
        //Step up
        investigatorPos.first--;
        if(isWalkable(investigatorPos.first, investigatorPos.second)){
            //Step left
            if(directionToBox == 'W'){
                investigatorPos.second--;
                possiblePositions.push_back(investigatorPos);
            }
            //Step right
            else {
                investigatorPos.second++;
                possiblePositions.push_back(investigatorPos);            
            }
        }
        //Step down
        investigatorPos = possibleBoxPush.playerPosition;
        investigatorPos.first++;
        if(isWalkable(investigatorPos.first, investigatorPos.second)){
            //Step left
            if(directionToBox == 'W'){
                investigatorPos.second--;
                possiblePositions.push_back(investigatorPos);
            }
            //Step right
            else {
                investigatorPos.second++;
                possiblePositions.push_back(investigatorPos);            
            }
        }
    }*/

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
    else
        mBoard[mPlayerPos.first][mPlayerPos.second] = ' ';

    if(mBoard[newPlayerPosition.first][newPlayerPosition.second] == GOAL)
        mBoard[newPlayerPosition.first][newPlayerPosition.second] = PLAYER_ON_GOAL;
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
    // Will hold all the directly adjacent positions to the box (N S E W)
    vector<pair<int,int> > possiblePositions;
    vector<std::string> possiblePaths;
    possiblePosition = currentBox.boxPosition;
    possiblePosition.first--;
    if(isWalkable(possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.first += 2;
    if(isWalkable(possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.first--;
    possiblePosition.second--;
    if(isWalkable(possiblePosition.first, possiblePosition.second))
        possiblePositions.push_back(possiblePosition);
    possiblePosition.second += 2;
    if(isWalkable(possiblePosition.first, possiblePosition.second))
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
            
            currPath = boxAStar(possiblePositions[i]);
            // cout << "current player pos is: (" << getPlayerPosition().first << "," << getPlayerPosition().second << ")" << endl;  
            
            currentBox.boxPosition = possiblePosition;
            
            if(currPath != "\0"){
                updatePlayerPosition(possiblePositions[i]);
                mPath = currPath;
                // cout << "currPath: " << currPath << endl; 
                // cout << "for this board: " << endl;
                // printBoard();
                // Set the player position to be the just searched for position
                currentBox.playerPosition = possiblePositions[i];
                
                //cout << "PlayerPosition first: " << currentBox.playerPosition.first << " PlayerPosition.second: " << currentBox.playerPosition.second << endl;
                //cout << "BoxPosition first: " << currentBox.boxPosition.first << " BoxPosition.second: " << currentBox.boxPosition.second << endl;

                // Determine the relative position of the box
                directionToBox = getDirectionToPos(currentBox.playerPosition,
                                                currentBox.boxPosition);
                // Can we find adjacent positions through a local search?
                circleBox(currentBox, directionToBox, moves, currPath);
            }
        }
    }
    //cout << currentBox.positionsAroundBox.size() << endl;
    // Perform state changes on accepted positions and place them in moves
    /*
    for(int i = 0; i < currentBox.positionsAroundBox.size(); i++){

        std::pair<int,int> pushedBoxCoordinates = getPushCoordinates(
                                                  currentBox.positionsAroundBox[i],
                                                  currentBox.boxPosition);

        char lastMove = translateDirection(getDirectionToPos(currentBox.positionsAroundBox[i],
                                          currentBox.boxPosition));

        moves.push_back(*doLongMove(currentBox.boxPosition, 
                                    pushedBoxCoordinates, possiblePaths[i], lastMove));
    }*/
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
    // Loop through all boxes on the board
    for(int i = 0; i < mBoxPositions.size(); i++){
        // Start by determining its coordinates
        currentBox.boxPosition = mBoxPositions[i];
        // Let's look at how many directions it can go
        investigatePushBoxDirections(currentBox, moves);
    }
}

void board::printBoard() const{
    // std::cout << "printBoard" << std::endl;
    for (int i = 0; i < mBoard.size(); i++) {
        for (int j = 0; j < mBoard[i].size(); j++) {
            cout << mBoard[i][j];
        }
        cout << '\n';
    }
}

/*
 * Finds pushable boxes using A*
 */
 string board::boxAStar(pair<int,int> goalPos){
    // cout << "current player position: (" << getPlayerPosition().first << ", " << getPlayerPosition().second << ")" << endl;            
    unordered_map<string,int> g_score_map(200000);
    priority_queue<pair<board,float>, vector< pair<board,float> >, fcomparison> openQueue;
    std::unordered_map<std::string, int> g_score;
    pair<int,int> playerPos = getPlayerPosition();
    int px = playerPos.first;
    int py = playerPos.second;
    std::vector<std::pair<int,int> > boxPositions = getBoxPositions();
    float starting_heuristic = 1 + distance(goalPos, playerPos);
    g_score.insert(make_pair(getBoardString(), 1));
    openQueue.push(make_pair(*this, starting_heuristic));

    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();
        pair<int,int> currentPlayerPos = currentBoard.getPlayerPosition();
        int x = currentPlayerPos.first;
        int y = currentPlayerPos.second;

        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<board> moves;
        currentBoard.getAllValidWalkMoves(moves);
        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];
            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();
            if (tempPlayerPos == goalPos) {
                return tempBoard.getPath();
            }
            int tempX = tempPlayerPos.first;
            int tempY = tempPlayerPos.second;

            int temp_g = g_score.at(currentBoard.getBoardString()) + 1; 
            int current_g;
            map_it = g_score.find(tempBoard.getBoardString());
            if ( map_it != g_score.end() )
                current_g = map_it->second;
            else
                current_g = 0;

            // Skip move if it exists with a lower g_score
            if (current_g > 0 && current_g <= temp_g ) {
                continue;
            }
            // Calculate path-cost, set parent (previous) position and add to possible moves
            else {
                float new_f = temp_g + distance(goalPos, tempPlayerPos);
                g_score.insert(make_pair(tempBoard.getBoardString(),temp_g));
                openQueue.push(make_pair(tempBoard, new_f));
            }
        }
    }
    return "\0";
}

/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
 */
//  int board::heuristicDistanceToBox(const vector< pair<int,int> > &boxPositions, pair<int,int> currentPos) {
//     vector<vector<char> > chars = b.getBoardCharVector();
//     minDistance = b.getBoardSize();
//     for (int i = 0; i < boxPositions.size(); ++i) {
//         int boxDistance = distance(boxPositions[i], currentPos);
//         if (boxDistance < minDistance)
//             minDistance = boxDistance;
//     }
//     return minDistance;     
// }

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

