#include "solver.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::priority_queue;
using std::pair;
using std::make_pair;
using std::stringstream;
using std::stack;

/*
 * c-tor
 */
 solver::solver() {}



/*
 * The solver method. It takes a board as a parameter and returns a solution
 */
 string solver::solve(board &b) {
    g_score_map.reserve(200000);
    // visited.reserve(600000);
    mBoardSize = b.getBoardSize();
    mGoalPositions = b.getGoalPositions();

    calculateDistances(b);
    //printMatrix(distanceMatrix);

    //Iterative deepening
    // TODO if IDA*:
    // Set bound to 0 or higher.
    // Break when heuristic is bigger than starting heuristic + bound.
    // Save the lowest value of the ones that are too big OR bound++
    // (for our box heuristic bound++ is the same as saving the lowest)
    // Also: check if bound never caused any branch-cutoffs. If so,
    // there are no possible solutions. End the iteration and return "no path".
    // (might be needed for DFS as well).
    
    // int depth = 2;
    // string solution;
    // do {
    //     // cout << "Depth: " << depth << endl;
    //     solution = search(b, depth);
    //     // cout << "Solution: " << solution << endl;
    //     if(solution != "no path") {
    //         return solution;
    //     }
    //     depth++;
    // }while(depth < 100);
    // /*
    // vector<board> boards;
    // b.getPossibleStateChanges(boards);
    // for(int i = 0; i < boards.size(); i++){
    //     boards[i].printBoard();
    //     cout << boards[i].getPath() << endl;
    // }*/
    // return "no path";

    return IDA(b);
}

/*
 * Search the state space using dfs
 */
string solver::search(board &b, int depth) {

    // cout << "current player position: (" << b.getPlayerPosition().first << ", " << b.getPlayerPosition().second << ")" << endl;
    // b.printBoard();

    if (b.isFinished()) {
        // cout << "finished" << endl;
        return b.getPath();
    }else if(depth==0) {
        //cout << "no path" << endl;
        return "no path";
    }

    vector<board> moves;
    b.getPossibleStateChanges(moves);
    string path;

    for (int i = 0; i < moves.size(); ++i) {
        path = search(moves[i], depth-1);
        if(path != "no path"){
            return path;
        }
    }
    return "no path";
}

/*
board solver::getLockedDownBoxesBoard(const board &boardToConvert){
    vector<vector<char> > boardChars = boardToConvert.getBoardCharVector();
    for(int row = 0; row < boardChars.size(); row++){
        for(int col = 0; col < boardChars[row].size(); col++){
            if(boardChars[row][col] == '*' || boardChars[row][col] == '$')
                boardChars[row][col] = '#';
        }
    }
    return board(boardChars, boardToConvert.isPush(), 
     boardToConvert.getWhatGotMeHere(), 
     boardToConvert.getDeadPositions(), boardToConvert.getPath()); 
}
*/

/*
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
 struct fcomparison {
    bool operator() (pair<board,float> a, pair<board,float> b) {
        return a.second >= b.second ? true : false;
    }
};

/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
 */
 int solver::heuristicDistance(const board &b) {
    int totalDistances = 0;
    vector< pair<int,int> > boxPositions = b.getBoxPositions();
    for (int i = 0; i < boxPositions.size(); ++i) {
        totalDistances += mDistanceMatrix[boxPositions[i].first][boxPositions[i].second];
    }
    return totalDistances;
}

void solver::calculateDistances(const board &b) {
    vector<vector<char> > board = b.getBoardCharVector();
    mDistanceMatrix.resize(board.size());

    for(int i = 0; i < board.size(); i++) {
        mDistanceMatrix[i] = vector<int>(board[i].size());
        for(int j = 0; j < board[i].size(); j++) {
            int shortestDistance = mBoardSize; // Set to a high value
            for(int k = 0; k < mGoalPositions.size(); k++) {
                int d = distance(i, j, mGoalPositions[k].first, mGoalPositions[k].second);
                if( d < shortestDistance) {
                    shortestDistance = d;
                }
            }
            mDistanceMatrix[i][j] = shortestDistance;
        }
    }
}


/*
 * Returns the estimated distance between two positions
 */
 int solver::distance(int i1, int j1, int i2, int j2) {
    // Manhattan distance
    return 1*abs(i2-i1) + abs(j2-j1);

    // Diagonal shortcut
    // int xDelta = abs(i2-i1);
    // int yDelta =  abs(j2-j1);
    // if (xDelta > yDelta) {
    //     return 4*yDelta + 3*(xDelta-yDelta);
    // }
    // else {
    //     return 4*xDelta + 3*(yDelta-xDelta);
    // }
}


/*
 * IDA*
 */
string solver::IDA(const board &b) {
    mPath = "no path";
    float bound = 2; 
    while(mPath == "no path") {
        float tempBound = aStar(b, bound);
        if (bound == tempBound)
            break;
        else 
            bound = tempBound;
        // if (bound >= 600)
        //     return "no path";
    }
    return mPath;
}

float solver::aStar(const board &b, float bound) {
    std::unordered_map<std::string, int> g_score_map;
    // A priority queue of moves that are sorted based on their f_score
    priority_queue<pair<board,float>, vector< pair<board,float> >, fcomparison> openQueue;
    // Set starting position with null char to let backtrack know we're finished.
    // pair<int,int> playerPos = b.getPlayerPosition();
    // int px = playerPos.first;
    // int py = playerPos.second;
    // previous[px][py].push_back(make_pair(make_pair(-1,-1), '\0'));
    g_score_map.insert(make_pair(b.getBoardString(), 1));
    // f_score[px][py] = 1 + heuristicDistance(b.getBoxPositions());

    int starting_box_distances = heuristicDistance(b);
    float minCost = starting_box_distances + bound + 2;
    float starting_heuristic = starting_box_distances;
    openQueue.push(make_pair(b, starting_heuristic));
    std::unordered_map<std::string,vector<pair<int,int> > >::const_iterator visited_it;
    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();

        // int x = currentBoard.getPlayerPosition().first;
        // int y = currentBoard.getPlayerPosition().second;

        // Iterate through all valid moves (neighbours)
        // A move is a pair consisting of a pair of coordinates and the 
        // direction taken to reach it from the current node.
        vector<board> moves;
        currentBoard.getPossibleStateChanges(moves);
        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];
            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();

            if (tempBoard.isFinished()) {
                mPath = tempBoard.getPath();
                return -1;
            }

            int tempX = tempPlayerPos.first;
            int tempY = tempPlayerPos.second;
            int temp_g = g_score_map.at(currentBoard.getBoardString()) + 1;
            int current_g;
            map_it = g_score_map.find(tempBoard.getBoardString());
            if ( map_it != g_score_map.end() ) {
                current_g = map_it->second;
            }
            else {
                current_g = 0;
            }

            // Skip move if the position is in the open or closed set with a lower g_score
            // g_scores are initalized to 0 and start at 1, so an initialized g_score is always positive
            if (current_g > 0 && current_g <= temp_g ) {
                continue;
            }

            float tempHeuristic = heuristicDistance(tempBoard);
            if (starting_box_distances + bound < tempHeuristic) {
                if (tempHeuristic < minCost)
                    minCost = tempHeuristic;
                continue;
            }
            else {
                g_score_map.insert(make_pair(tempBoard.getBoardString(),temp_g));
                openQueue.push(make_pair(tempBoard, tempHeuristic));
            }
        }
    }
    mPath = "no path";
    return minCost;
}


/*
 * Debug printing of a pair of coordinates
 */
 void solver::printCoordinates(int x, int y) {
    cout << "(" << x << ", " << y << ")";
}

void solver::printMatrix(vector<vector<int> > &m) {
    for(int i = 0; i < m.size(); i++) {
        for(int j = 0; j < m[i].size(); j++) {
                cout << m[i][j] << ", ";
        }
        cout << endl;
    }
}
