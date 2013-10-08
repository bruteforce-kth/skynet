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
    
     //vector<board> boards;
     //b.getPossibleStateChanges(boards);
     /*for(int i = 0; i < boards.size(); i++){
         boards[i].printBoard();
         cout << boards[i].getPath() << endl;
     }*/
    
    //b.printBoard();
    //return "no path";

    // return aStar(b, 99999999);

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
    string solution = "no path";
    float bound = 6;
    mBoundUsed = true;
    while(solution == "no path" && mBoundUsed) {
        solution =  aStar(b, bound);
        ++bound;
    }
    return solution;
}

string solver::aStar(const board &b, float bound) {
    mBoundUsed = false;
    std::unordered_map<std::string, int> visited;
    std::unordered_map<std::string,vector<pair<int,int> > >::const_iterator visited_it;
    std::unordered_map<std::string, int> g_score_map;
    // A priority queue of moves that are sorted based on their f_score
    priority_queue<pair<board,float>, vector< pair<board,float> >, fcomparison> openQueue;
    g_score_map.insert(make_pair(b.getBoardString(), 1));
    int starting_box_distances = heuristicDistance(b);
    openQueue.push(make_pair(b, (float)starting_box_distances));
    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;
        openQueue.pop();

        currentBoard.printBoard();

        // Iterate through all valid pushes
        vector<board> moves;
        std::unordered_map<std::string, std::vector<board> >::const_iterator board_map_it;
        board_map_it = mTransTable.find(currentBoard.getBoardString());
        if(board_map_it != mTransTable.end()) {
            // cout << "moves found!" << endl;
            moves = board_map_it->second;
        }else{
            // cout << "no moves found!" << endl;
            currentBoard.getPossibleStateChanges(moves);
            mTransTable.insert(make_pair(currentBoard.getBoardString(), moves));
        }

        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];
            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();

            if (tempBoard.isFinished()) {
                return tempBoard.getPath();
            }

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
                mBoundUsed = true;
                continue;
            }
            else {
                g_score_map.insert(make_pair(tempBoard.getBoardString(),temp_g));
                openQueue.push(make_pair(tempBoard, tempHeuristic));
            }
        }
    }
    return "no path";
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
