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
    mBoardSize = b.getBoardSize();
    mGoalPositions = b.getGoalPositions();
    return aStar(b);
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
string solver::aStar(const board &b) {
    // A priority queue of moves that are sorted based on their f_score
    priority_queue<pair<board,float>, vector< pair<board,float> >, fcomparison> openQueue;
    // Set starting position with null char to let backtrack know we're finished.
    pair<int,int> playerPos = b.getPlayerPosition();
    int px = playerPos.first;
    int py = playerPos.second;
    // previous[px][py].push_back(make_pair(make_pair(-1,-1), '\0'));
    g_score_map.insert(make_pair(b.getBoardString(), 1));
    // f_score[px][py] = 1 + heuristicDistance(b.getBoxPositions());

    float starting_heuristic = 1 + heuristicDistance(b.getBoxPositions());
    openQueue.push(make_pair(b, starting_heuristic));
    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();
        int x = currentBoard.getPlayerPosition().first;
        int y = currentBoard.getPlayerPosition().second;

        // currentBoard.printBoard();

        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<board> moves;
        currentBoard.getAllValidMoves(moves, currentBoard.getPath());
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
                if (tempBoard.isFinished()) {
                    return tempBoard.getPath();
                }
                g_score_map.insert(make_pair(tempBoard.getBoardString(),temp_g));
                openQueue.push(make_pair(tempBoard, temp_g + heuristicDistance(tempBoard.getBoxPositions())));
            }
        }
    }
    return "no path";
}

/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
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





