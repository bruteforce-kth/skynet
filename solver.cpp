#include <iostream>
#include <queue>
#include <list>
#include <sstream>
#include "solver.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::priority_queue;
using std::pair;
using std::make_pair;


/*
 * c-tor
 */
solver::solver() {}


/*
 * The solver method. It takes a board as a parameter and returns a solution
 */
string solver::solve(const board &b) {
    // Initialize 2d vectors g_score and f_score for aStar.
    g_score.resize(b.getNumRows(), vector<int>(b.getLongestRow(),0));
    f_score.resize(b.getNumRows(), vector<float>(b.getLongestRow(),0));
    // A 2d-vector of positions and their corresponding move char.
    // At position [i][j] we have the parent for [i][j] and the direction we came from. Used in backtrack
    previous.resize(b.getNumRows(), vector< pair<pair<int,int>,char> >(b.getLongestRow(),
        make_pair(make_pair(-1,-1),'\0')));

    //mStartingPos = b.getPlayerPosition();
    mBoardSize = b.getBoardSize();
    mGoalPositions = b.getGoalPositions();

    bool solved = aStar(b);
    return mPath;
}

/*
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
struct fcomparison {
    bool operator() (pair<board,float> a, pair<board,float> b) {
        return  a.second >= b.second ? true : false;
    }
};


/*
 * IDA*
 */
// cutoff at g() or f() ? 
// this one has for f() http://www.informatik.uni-osnabrueck.de/papers_html/ai_94/node2.html 
// also, make some local vectors member variables to avoid reinitialization
 


/*
 * Finds pushable boxes using A*
 */
bool solver::aStar(const board &b) {
    // A priority queue of moves that are sorted based on their f_score
    priority_queue<pair<board,float>, vector< pair<board,float> >, fcomparison> openQueue;
    // Set starting position with null char to let backtrack know we're finished.
    pair<int,int> playerPos = b.getPlayerPosition();
    int px = playerPos.first;
    int py = playerPos.second;
    previous[px][py] = make_pair(make_pair(-1,-1), '\0');
    g_score[px][py] = 1;
    f_score[px][py] = 1 + heuristicDistance(b.getBoxPositions());

    openQueue.push(make_pair(b,f_score[px][py]));

    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();
        int x = currentBoard.getPlayerPosition().first;
        int y = currentBoard.getPlayerPosition().second;

        cout << "Popped coordinate: ";
        printCoordinates(x,y);
        cout << endl;


        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<board> moves;
        currentBoard.getAllValidMoves(moves);
        cout << "Number of possible moves: " << moves.size() << endl;
        std::cout << "Standing on (" << currentBoard.getPlayerPosition().first << ", " << currentBoard.getPlayerPosition().second << ")" << std::endl;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];
            std::cout << "valid move:" << std::endl;
            tempBoard.printBoard();
            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();
            int tempX = tempPlayerPos.first;
            int tempY = tempPlayerPos.second;
            int temp_g = g_score[x][y] + 1;
            int current_g = g_score[tempX][tempY];

            cout << "Proposed move is: ";
            printCoordinates(tempX, tempY);
            cout << endl;
            
            // Skip move if the position is in the open or closed set with a lower g_score
            // g_scores are initalized to 0 and start at 1, so an initialized g_score is always positive
            if (current_g > 0 && current_g <= temp_g ) {
                continue;
            }
            // Calculate path-cost, set parent (previous) position and add to possible moves
            else {
                previous[tempX][tempY] = make_pair(make_pair(x,y), tempBoard.getWhatGotMeHere());
                if (b.isFinished()) {
                    cout << "Board solved! Backtracking!" << endl;
                    backtrack(previous, tempX, tempY);
                    return true;
                }
                g_score[tempX][tempY] = temp_g;
                f_score[tempX][tempY] = temp_g + heuristicDistance(tempBoard.getBoxPositions());
                std::cout << "pushing move to position (" << tempX << ", " << tempY << ")" << std::endl;
                openQueue.push(make_pair(tempBoard, f_score[tempX][tempY]));
            }
        }
    }
    return false;
}

/*
 * OBS Old heuristic. Update with distance to boxes!
 * Estimate of distance to goal used in the aStar() algorithm
 *
 * Returns the diagonal distance to the closest goal
 */
int solver::heuristicDistance(const vector< pair<int,int> > &boxPositions) {
    std::cout << "heuristicDistance" << std::endl;
    int totalDistances = 0;

    for (int i = 0; i < boxPositions.size(); ++i) {
        int shortestDistance = mBoardSize;
        int x = boxPositions[i].first;
        int y = boxPositions[i].second;
        for (int j = 0; j < mGoalPositions.size(); ++j) {
            int d = distance(x, y, mGoalPositions[j].first, mGoalPositions[j].second);
            if (d < shortestDistance) {
                shortestDistance = d;
            }
        }
        totalDistances += shortestDistance;
    }
    return totalDistances;
}


/*
 * Returns the estimated distance between two positions
 */
int solver::distance(int i1, int j1, int i2, int j2) {
    //std::cout << "distance(" << i1 << ", " << j1 << ", " << i2 << ", " << j2 << ")" << std::endl;
    // Manhattan method (10 instead of 1)
    return 1*abs(i2-i1) + abs(j2-j1);
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


/*
 * Backtracks from the specified position to find a path
 * from the player's starting position to the goal
 *
 * 'previous' is a vector containing pairs of coordinates.
 * it is indexed using the function 'position' which
 * maps two coordinates to a unique index in the vector.
 * Using this hash we find our way from the end coordinates
 * to the start coordinates, adding direction chars to our string
 * along the way.
 */
void solver::backtrack(const vector<vector<pair<pair<int,int>, char> > > &previous, int i, int j) {
    std::ostringstream s1;
    char direction;
    pair<pair<int,int>, char> previousMove = previous[i][j];
    while(!(previousMove.second == '\0')) {
        s1 << previousMove.second;
        // Get index for the previous coordinates' previousPos
        int currentX = previousMove.first.first;
        int currentY = previousMove.first.second;
        // Update position using the calculated index
        previousMove = previous[currentX][currentY];
    }
    string reversedString = s1.str();
    mPath = string(reversedString.rbegin(),reversedString.rend());
}

/*
 * Find all possible directions we can push a box.
 * Uses the global variables mBoxPos and mPlayerPos
 * (We originally set these variables with an aStar search)
 */
/*vector< std::pair<int,int> > solver::getPushDirections(board &b) {
    vector< std::pair<int,int> > pushDirections;
    // This direction is pushable after an A*. Not true
    // If we want to call this method in some recursion.
    pushDirections.push_back(mPlayerPos);
}


bool isPushable(board &b, pair<int,int> fromPosition) {

}*/

/*
 * Debug printing of a pair of coordinates
 */
 void solver::printCoordinates(int x, int y) {
    cout << "(" << x << ", " << y << ")";
 }

