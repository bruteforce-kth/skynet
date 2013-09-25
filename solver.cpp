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
    g_score_map.reserve(600000);
    // Initialize 2d vectors g_score and f_score for aStar.
    g_score.resize(b.getNumRows(), vector<int>(b.getLongestRow(),0));
    // f_score.resize(b.getNumRows(), vector<float>(b.getLongestRow(),0));
    // A 2d-vector of positions and their corresponding move char.
    // At position [i][j] we have the parent for [i][j] and the direction we came from. Used in backtrack
    vector< pair<pair<int,int>,char> > filler;
    previous.resize(b.getNumRows(), vector<vector< pair<pair<int,int>,char> > >(b.getLongestRow(), filler));

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
        return  a.second > b.second ? true : false;
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
    previous[px][py].push_back(make_pair(make_pair(-1,-1), '\0'));
    g_score_map.insert(make_pair(b.getBoardString(), 1));
    // f_score[px][py] = 1 + heuristicDistance(b.getBoxPositions());

    float starting_heuristic = 1 + heuristicDistance(b.getBoxPositions());
    openQueue.push(make_pair(b, starting_heuristic));
    int numTested = 0;
    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();
        int x = currentBoard.getPlayerPosition().first;
        int y = currentBoard.getPlayerPosition().second;

        currentBoard.printBoard();

        numTested++;

        // cout << "Popped coordinate: ";
        // printCoordinates(x,y);
        // cout << endl;

        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<board> moves;
        currentBoard.getAllValidMoves(moves);
        std::unordered_map<std::string,int>::const_iterator map_it;
        // cout << "Number of possible moves: " << moves.size() << endl;
        // std::cout << "Standing on (" << currentBoard.getPlayerPosition().first << ", " << currentBoard.getPlayerPosition().second << ")" << std::endl;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];
            if(isRepeatedMove(currentBoard.getWhatGotMeHere(), tempBoard.getWhatGotMeHere()))
                continue;
            // std::cout << "valid move:" << std::endl;
            // tempBoard.printBoard();
            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();
            int tempX = tempPlayerPos.first;
            int tempY = tempPlayerPos.second;
            int temp_g = g_score_map.at(currentBoard.getBoardString());
            int current_g;
            map_it = g_score_map.find(tempBoard.getBoardString());
            if ( map_it != g_score_map.end() )
                current_g = map_it->second;
            else
                current_g = 0;

            // cout << "Proposed move is: ";
            // printCoordinates(tempX, tempY);
            // cout << endl;
            
            // Skip move if the position is in the open or closed set with a lower g_score
            // g_scores are initalized to 0 and start at 1, so an initialized g_score is always positive
            if (current_g > 0 && current_g <= temp_g ) {
                // std::cout << "bad g_score " << temp_g << ", continuing" << std::endl;
                continue;
            }
            // Calculate path-cost, set parent (previous) position and add to possible moves
            else {
                previous[tempX][tempY].push_back(make_pair(make_pair(x,y), tempBoard.getWhatGotMeHere()));
                if (tempBoard.isFinished()) {
                    // cout << "Board solved! Backtracking!" << endl;
                    backtrack(previous, tempX, tempY);
                    return true;
                }
                g_score_map.insert(make_pair(tempBoard.getBoardString(),temp_g));
                // g_score[tempX][tempY] = temp_g;
                // std::cout << "pushing move to position (" << tempX << ", " << tempY << ")" << std::endl;
                openQueue.push(make_pair(tempBoard, temp_g + heuristicDistance(tempBoard.getBoxPositions())));
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
void solver::backtrack(vector<vector<vector<pair<pair<int,int>, char> > >  >&previous, int i, int j) {
    std::ostringstream s1;
    char direction;
    pair<pair<int,int>, char> previousMove = previous[i][j].back();
    // if (previous[i][j].size() > 1)
    //     previous[i][j].pop_back();
    while(!(previousMove.second == '\0')) {
        s1 << previousMove.second;
        // Get index for the previous coordinates' previousPos
        int currentX = previousMove.first.first;
        int currentY = previousMove.first.second;
        // Update position using the calculated index
        previousMove = previous[currentX][currentY].back();

        if (previous[currentX][currentY].size() > 1)
            previous[currentX][currentY].pop_back();
    }
    string reversedString = s1.str();
    mPath = string(reversedString.rbegin(),reversedString.rend());
}

bool solver::isRepeatedMove(char a, char b) {
    if(
        a == 'U' && b == 'D' ||
        a == 'D' && b == 'U' ||
        a == 'L' && b == 'R' ||
        a == 'R' && b == 'L') {
        return true;
    }
    return false;
}


/*
 * Debug printing of a pair of coordinates
 */
 void solver::printCoordinates(int x, int y) {
    cout << "(" << x << ", " << y << ")";
 }

// string previousMapHash(board &b, int x, int y) {
//     string hash = b.getBoardString();
//     hash = hash + std::to_string(x) + " " + std::to_string(y);
//     return hash;
// }

