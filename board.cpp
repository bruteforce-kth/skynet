//
//  board.cpp
//  xcode_ai_hw_1_cc
//
//  Created by Martin Runelöv on 2013-07-16.
//  Copyright (c) 2013 Martin Runelöv. All rights reserved.
//

#include <list>
#include <queue>
#include <sstream>
#include <utility>
#include "board.h"

using std::cout;
using std::endl;
using std::string;
using std::pair;
using std::vector;
using std::map;
using std::priority_queue;
using std::pair;
using std::make_pair;

/*
 * Compares two entries in the openQueue used in A*.
 * The second value of the outer tuple (f_score) is compared.
 * The first value is a pair of coordinates corresponding to 
 * a row and a column in the 2d vector representing the board.
 */
struct fcomparison {
    bool operator() (pair<pair<int,int>,float> a, pair<pair<int,int>,float> b) {
        return a.second >= b.second ? true : false;
    }
};

// board constructor
board::board (const vector<vector<char> > &chars) {
    this->charMap = chars;
    bool initialized = initializeIndexAndPositions(chars);
    if(initialized) {
        this->f_score.resize(boardSize);
    }
}

// Get the index for the coordinates (i,j) (row-wise)
int board::getIndex(int i, int j) {
    return boardIndexes[make_pair(i,j)];
}

// Get the index for the coordinate pair.
int board::getIndex(pair<int,int> position) {
    return boardIndexes[position];
}


/*
 * Loops over the 2d vector of chars and initializes
 * player position, goal positions and indexes.
 */
bool board::initializeIndexAndPositions(const vector<vector<char> > &chars) {
    int index = 0;
    int size = 0;
    for (int i = 0; i < chars.size(); i++) {
        size += chars[i].size();
        for (int j = 0; j < chars[i].size(); j++) {
            char c = chars[i][j];
            // Store goal positions
            if (c == GOAL) {
                this->goalPositions.push_back(make_pair(i,j));
            }
            // Store player position
            else if (c == PLAYER) {
                this->playerPos = make_pair(i,j);
            }
            else if (c == PLAYER_ON_GOAL) {
                this->playerPos = make_pair(i,j);
                return false;
            }
            boardIndexes.insert(make_pair(make_pair(i,j),index));
            ++index;
        }
    }
    this->boardSize = size;
    return true;
}

/*
 * Checks if a position on the board is accessible.
 * Bounds checking + type checking.
 */
bool board::checkPosition(int i, int j) {
    char t = charMap[i][j];
    if (t == FLOOR || t == GOAL) {
        return true;
    }
    return false;   // all other types are invalid
}

bool board::isGoal(int i, int j) {
    if (charMap[i][j] == GOAL) {
        return true;
    }
    return false;
}

/*
 * Returns all valid moves from the specified position
 */
vector<pair<pair<int,int>,char> > board::getAllValidMoves(int i, int j) {
    vector<pair<pair<int,int>,char> > validMoves;
    if (checkPosition(i-1, j)) {
        validMoves.push_back(make_pair(make_pair(i-1,j), MOVE_UP));
    }
    if (checkPosition(i+1, j)) {
        validMoves.push_back(make_pair(make_pair(i+1,j), MOVE_DOWN));
    }
    if (checkPosition(i, j-1)) {
        validMoves.push_back(make_pair(make_pair(i,j-1), MOVE_LEFT));
    }
    if (checkPosition(i, j+1)) {
        validMoves.push_back(make_pair(make_pair(i,j+1), MOVE_RIGHT));
    }
    return validMoves;
}

string board::solve() {
    cout << "\n";
    printBoard();
    cout << "\n";
    if (charMap[playerPos.first][playerPos.second] == PLAYER_ON_GOAL) {
        return "";
    }
    if(this->boardSize >= 56) {
        return aStar();
    }    
    else {
        return dfs();
    }    
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
 *
 */
string board::backtrack(const vector<pair<int, char> > &previous, int i, int j) {
    std::ostringstream s1;
    char direction;
    int currentIndex = getIndex(i,j);
    pair<int, char> previousMove = previous[currentIndex];
    // previous coordinates set to (-1,-1) for the start node in 'bfs()'
    while(!(previousMove.first == -1)) {
        s1 << previousMove.second;
        // Get index for the previous coordinates' previousPos
        currentIndex = previousMove.first;
        // Update position using the calculated index
        previousMove = previous[currentIndex];
    }
    string reversedString = s1.str();
    return string(reversedString.rbegin(),reversedString.rend());
}

string board::aStar() {
    priority_queue<pair<pair<int,int>,float>, vector<pair<pair<int,int>,float> >, fcomparison> openQueue;
    int currentIndex;
    pair<int,int> currentPos;
    vector<int> g_score(boardSize);
    vector<pair<int, char> > previous(boardSize);
    
    // Initialize starting values
    int playerPosIndex = getIndex(playerPos);
    previous[playerPosIndex] = make_pair(-1, '\0');
    g_score[playerPosIndex] = 1;
    f_score[playerPosIndex] = float(g_score[playerPosIndex]) +
        heuristic_distance_to_goal_estimate(playerPos);
    
    openQueue.push(make_pair(playerPos,f_score[playerPosIndex]));

    while(!openQueue.empty()) {
        currentPos = openQueue.top().first;
        openQueue.pop();
        currentIndex = getIndex(currentPos);
        int x = currentPos.first;
        int y = currentPos.second;
        
        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<pair<pair<int,int>,char> > moves = getAllValidMoves(x, y);
        for (int k = 0; k < moves.size(); ++k) {
            pair<int,int> tempPos = moves[k].first;
            int tempPosIndex = getIndex(tempPos);
            int temp_g = g_score[currentIndex] + 1;
            int current_g = g_score[tempPosIndex];
            
            // Skip move if the position is in the open or closed set with a lower g_score
            // g_scores are initalized to 0 and start at 1, so an initialized g_score is always positive
            if (current_g > 0 && current_g <= temp_g ) {
                continue;
            }
            // Calculate path-cost, set parent (previous) position and add to possible moves
            else {
                previous[tempPosIndex] = make_pair(currentIndex, moves[k].second);
                if (isGoal(tempPos.first,tempPos.second)) {
                    return backtrack(previous, tempPos.first, tempPos.second);
                }
                g_score[tempPosIndex] = temp_g;
                f_score[tempPosIndex] = temp_g + heuristic_distance_to_goal_estimate(tempPos);
                openQueue.push(make_pair(tempPos,f_score[tempPosIndex]));
            }
        }
    }
    return "no path";
}

/*
 * Estimate of distance to goal used in the aStar() algorithm
 *
 * Returns the diagonal distance to the closest goal
 */
int board::heuristic_distance_to_goal_estimate(pair<int,int> p) {
    int shortestDistance = boardSize;
    int distance;
    
    for (int k = 0; k < goalPositions.size(); ++k) {
        distance = diagonal_distance(p.first, p.second, goalPositions[k].first, goalPositions[k].second);
        if (distance < shortestDistance) {
            shortestDistance = distance;
        }
    }
    return shortestDistance;
}

int board::diagonal_distance(int i1, int j1, int i2, int j2) {
    
    // Manhattan method
    return abs(i2-i1) + 8*abs(j2-j1);


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


string board::dfs() {
    using std::deque;
    
    pair<int,int> currentPos;       // current coordinates
    int neighbourIndex;   // temp index variable for neighbours
    deque<pair<int,int> > q;         // DFS stack
    vector<pair<int, char> > previous(boardSize);
    vector<bool> visited(boardSize); // Keeps track of visited coordinates
    int i,j;    // Temp coordinate variables
    
    q.push_front(playerPos);    // Set starting position
    int startingPosition = getIndex(playerPos);
    previous[startingPosition] = make_pair(-1,'\0');
    visited[startingPosition] = true;
    
    while(!q.empty()) {
        currentPos = q.back();
        q.pop_back();
        i = currentPos.first;
        j = currentPos.second;
        if (charMap[i][j] == GOAL) {
            return backtrack(previous, i, j);
        }
        vector<pair<pair<int,int>,char> > moves = getAllValidMoves(i, j);
        for (int k = 0; k < moves.size(); k++) {
            // Get the index (hash) for the neighbour
            neighbourIndex = getIndex(moves[k].first);
            if (!visited[neighbourIndex]) {
                // Add the move to the queue (pair of coordinates)
                q.push_front(moves[k].first);
                // Set previous coordinates for the move
                previous[neighbourIndex] = make_pair(getIndex(i,j), moves[k].second);
                // Mark as visited
                visited[neighbourIndex] = true;
            }
        }
    }
    return "no path";
}


void board::printBoard() {
    for (int i = 0; i < charMap.size(); i++) {
        for (int j = 0; j < charMap[i].size(); j++) {
            cout << charMap[i][j] << "";
        }
        cout << '\n';
    }
    cout << "Player position is:  ";
    cout << playerPos.first << ", " << playerPos.second << '\n';
}





