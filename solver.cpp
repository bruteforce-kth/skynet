#include <iostream>
#include <queue>
#include <list>
#include <sstream>
#include "solver.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::string;
using std::pair;
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
    f_score.resize(b.getNumRows(), vector<float>(b.getLongestRow(),0));
    mBoardSize = b.getBoardSize();
    std::vector<std::pair<int,int> > mGoalPositions = b.getGoalPositions();
    string boxWalk = aStar(b);
    cout << "Box at ";
    printCoordinates(mBoxPos.first,mBoxPos.second);
    cout << " can be pushed: "  << boxWalk.back() << endl;
    cout << "Player at ";
    printCoordinates(mPlayerPos.first,mPlayerPos.second);
    cout << endl;
    return boxWalk;
}

/*
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
struct fcomparison {
    bool operator() (pair<pair<int,int>,float> a, pair<pair<int,int>,float> b) {
        return a.second >= b.second ? true : false;
    }
};


/*
 * Finds pushable boxes using A*
 */
string solver::aStar(const board &b) {
    // A priority queue of moves that are sorted based on their f_score
    priority_queue<pair<pair<int,int>,float>, vector<pair<pair<int,int>,float> >, fcomparison> openQueue;
    // A 2d-vector of g_scores.
    vector< vector<int> > g_score(b.getNumRows(), vector<int>(b.getLongestRow(),0));
    // A 2d-vector of positions and their corresponding move char.
    // At position [i][j] we have the parent for [i][j] and the direction we came from. Used in backtrack
    vector< vector<pair<pair<int,int>, char> > > previous(b.getNumRows(), vector< pair<pair<int,int>,char> >(b.getLongestRow(),
        make_pair(make_pair(-1,-1),'\0')));

    pair<int,int> playerPos = b.getPlayerPosition();
    // Set starting position with null char to let backtrack know we're finished.
    previous[playerPos.first][playerPos.second] = make_pair(make_pair(playerPos.first,playerPos.second), '\0');
    g_score[playerPos.first][playerPos.second] = 1;
    f_score[playerPos.first][playerPos.second] = 1 + heuristicDistanceToGoal(playerPos);

    openQueue.push(make_pair(playerPos,f_score[playerPos.first][playerPos.second]));

    pair<int,int> currentPos;

    while(!openQueue.empty()) {
        currentPos = openQueue.top().first;
        openQueue.pop();
        int x = currentPos.first;
        int y = currentPos.second;

        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<pair<pair<int,int>,char> > moves = b.getAllValidMoves(x, y);
        for (int k = 0; k < moves.size(); ++k) {
            pair<int,int> tempPos = moves[k].first;
            int tempX = tempPos.first;
            int tempY = tempPos.second;
            int temp_g = g_score[x][y] + 1;
            int current_g = g_score[tempX][tempY];
            
            // Skip move if the position is in the open or closed set with a lower g_score
            // g_scores are initalized to 0 and start at 1, so an initialized g_score is always positive
            if (current_g > 0 && current_g <= temp_g ) {
                continue;
            }
            // Calculate path-cost, set parent (previous) position and add to possible moves
            else {
                previous[tempX][tempY] = make_pair(currentPos, moves[k].second);
                // If we found a pushable box, backtrack!
                if (b.isBox(tempX,tempY)) {
                    mBoxPos = make_pair(tempX,tempY);
                    mPlayerPos = make_pair(x,y);
                    // Currently skips the actual push
                    return backtrack(previous, tempX, tempY);
                }
                g_score[tempX][tempY] = temp_g;
                f_score[tempX][tempY] = temp_g + heuristicDistanceToGoal(tempPos);
                openQueue.push(make_pair(tempPos,f_score[tempX][tempY]));
            }
        }
    }
    return "no path";
}

/*
 * OBS Old heuristic. Update with distance to boxes!
 * Estimate of distance to goal used in the aStar() algorithm
 *
 * Returns the diagonal distance to the closest goal
 */
int solver::heuristicDistanceToGoal(pair<int,int> p) {
    int shortestDistance = mBoardSize;
    int d;
    
    for (int k = 0; k < mGoalPositions.size(); ++k) {
        d = distance(p.first, p.second, mGoalPositions[k].first, mGoalPositions[k].second);
        if (d < shortestDistance) {
            shortestDistance = d;
        }
    }
    return shortestDistance;
}


/*
 * Returns the estimated distance between two positions
 */
int solver::distance(int i1, int j1, int i2, int j2) {
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
string solver::backtrack(const vector<vector<pair<pair<int,int>, char> > > &previous, int i, int j) {
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
    return string(reversedString.rbegin(),reversedString.rend());
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


