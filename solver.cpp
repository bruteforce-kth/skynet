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
    h_coeff = 10;
    mBoardSize = b.getBoardSize();
    mGoalPositions = b.getGoalPositions();
    calculateDistances(b);
    //printMatrix(mDistanceMatrix);
    
    // int depth = 2;c
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

    // A* only
    // aStar(b, 99999999);
    // return mPath;

    // printMatrix(mDistanceMatrix);
    // b.printBoard();

    // IDA
    return IDA(b);
}

/*
 * Search the state space using dfs
 */
string solver::search(board &b, int depth) {
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
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
 struct fcomparison {
    bool operator() (pair<board,int> a, pair<board,int> b) {
        return a.second > b.second ? true : false;
    }
};

/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
 */
 int solver::heuristicDistance(const board &b) {
    vector <vector<char> > board = b.getBoardCharVector();
    int totalDistances = 0;
    vector< pair<int,int> > boxPositions = b.getBoxPositions();
    for (int i = 0; i < boxPositions.size(); ++i) {
        // if (board[boxPositions[i].first][boxPositions[i].second] == BOX_ON_GOAL)
        //     continue;
        totalDistances += mDistanceMatrix[boxPositions[i].first][boxPositions[i].second];
    }
    return totalDistances;
}

/**
 *  Test of new heuristic. Sum of goals distance to boxes.
 * The same box is NOT used twice.
 * (Assumes that numBoxes == numGoals)
 */
// int solver::heuristicDistance(const board &b) {
//     int heuristic = 0;
//     int totalDistances = 0;
//     int boxesOnGoal = 1;
//     vector< pair<int,int> > boxPositions = b.getBoxPositions();
//     vector< vector<char> > board = b.getBoardCharVector();
   
//     // for (int i = 0; i < boxPositions.size(); ++i) {
//     //     if (board[boxPositions[i].first][boxPositions[i].second] == BOX_ON_GOAL)
//     //         continue;
//     //     totalDistances += mDistanceMatrix[boxPositions[i].first][boxPositions[i].second];
//     // }

//     for (int i = 0; i < mGoalPositions.size(); ++i) {
//         int closestBox = mBoardSize;
//         int closestBoxIndex = -1;
//         for (int j = 0; j < boxPositions.size(); ++j) {
//             if (mGoalPositions[i] != boxPositions[j] && board[boxPositions[j].first][boxPositions[j].second] == BOX_ON_GOAL)
//                 continue;
//             int d = distance(mGoalPositions[i].first, mGoalPositions[i].second, boxPositions[j].first, boxPositions[j].second);
//             if (d < closestBox) {
//                 closestBox = d;
//                 closestBoxIndex = j;
//                 if (closestBox == 0) {
//                     ++boxesOnGoal;
//                     break;
//                 }
//             }
//         }
//         // Remove the closest box. each box is used once.
//         boxPositions.erase(boxPositions.begin() + closestBoxIndex);
//         heuristic += closestBox + mBoardSize/10;
//     }

//     // cout << "Heuristic = " << heuristic << " + " << totalDistances << endl;
//     // cout << "total: " << heuristic + totalDistances << endl;
//     // b.printBoard();
//     return heuristic + totalDistances;
// }

 void solver::calculateDistances(const board &b) {
    vector<vector<char> > board = b.getBoardCharVector();
    mDistanceMatrix.resize(board.size());

    for(int i = 0; i < board.size(); i++) {
        mDistanceMatrix[i] = vector<int>(board[i].size());
        for(int j = 0; j < board[i].size(); j++) {
            int shortestDistance = b.getBoardSize(); // Set to a high value
            for(int k = 0; k < mGoalPositions.size(); k++) {
                pair<int,int> goal = mGoalPositions[k];
                int d = distance(i, j, goal.first, goal.second);
                if(goal.first > 0
                        && ( board[goal.first-1][goal.second] == WALL
                            || board[goal.first-1][goal.second] == DEAD
                            || board[goal.first-1][goal.second] == PLAYER_ON_DEAD)) {
                    d--;
                }
                if(goal.first < board.size()-1
                        && ( board[goal.first+1][goal.second] == WALL
                            || board[goal.first+1][goal.second] == DEAD
                            || board[goal.first+1][goal.second] == PLAYER_ON_DEAD)) {
                    d--;
                }
                if(goal.second > 0
                        && ( board[goal.first][goal.second-1] == WALL
                            || board[goal.first][goal.second-1] == DEAD
                            || board[goal.first][goal.second-1] == PLAYER_ON_DEAD)) {
                    d--;
                }
                if(goal.second < board[goal.first].size()-1
                        && ( board[goal.first][goal.second+1] == WALL
                            || board[goal.first][goal.second+1] == DEAD
                            || board[goal.first][goal.second+1] == PLAYER_ON_DEAD)) {
                    d--;
                }
                if( d < shortestDistance) {
                    shortestDistance = d;
                }
            }
            mDistanceMatrix[i][j] = shortestDistance; //+ 3;
        }
    }
}


/*
 * Returns the estimated distance between two positions
 */
 int solver::distance(int i1, int j1, int i2, int j2) {
    // Manhattan distance
    return abs(i2-i1) + abs(j2-j1);
}

/*
 * Returns the estimated distance between two positions
 */
 int solver::distance(pair<int,int> a, pair<int,int> b) {
    // Manhattan distance
    return abs(b.first-a.first) + abs(a.second-b.second);
}


/*
 * IDA*
 *
 * The A* will skip branches with f_score > bound,
 * and the next iteration will run with the bound set to the lowest 
 * skipped f_score.
 */
string solver::IDA(const board &b) {
    mNumQueued = 0;
    mPath = "no path";
    int start_h = heuristicDistance(b);
    // Arbitrary start bound. Preferably board-dependent.
    int bound = h_coeff*start_h;
    mBoundUsed = true;
    while(mPath == "no path" && mBoundUsed) {
        // A* returns the lowest f_score that was skipped
        bound = aStar(b, bound);
    }
    return mPath;
}

int solver::aStar(const board &b, int bound) {
    // cout << "RUNNING A*" << endl;
    cout << "Running A* with bound: " << bound << endl;
    mBoundUsed = false;
    // minCost is the lowest f score skipped. Used by IDA in the next iteration.
    // Set to +inf here.
    int minCost = b.getBoardSize()*100;
    // g = number of pushes made
    std::unordered_map<std::string, int> g_score;
    // f = heuristic
    std::unordered_map<std::string, int> f_score;
    // Keep track of visited states. Locally.
    std::unordered_map<std::string, int> closed;

    // A priority queue of moves that are sorted based on their heuristic
    priority_queue<pair<board,int>, vector< pair<board,int> >, fcomparison> openQueue;
    int start_h = h_coeff*heuristicDistance(b);
    g_score.insert(make_pair(b.getBoardString(), 1));
    f_score.insert(make_pair(b.getBoardString(), 1 + start_h));

    openQueue.push(make_pair(b, start_h));
    while(!openQueue.empty()) {
        board currentBoard = openQueue.top().first;

        // cout << "f_score: " << openQueue.top().second << endl;
        // currentBoard.printBoard();

        openQueue.pop();

        vector<board> moves;
        std::unordered_map<std::string, std::vector<board> >::const_iterator board_map_it;
        // Check if the pushes for this state has already been calculated. 
        board_map_it = mTransTable.find(currentBoard.getBoardString());
        if(board_map_it != mTransTable.end()) {
            // cout << "moves found!" << endl;
            moves = board_map_it->second;
        }
        else{
            // cout << "no moves found!" << endl;
            currentBoard.getPossibleStateChanges(moves);
            mTransTable.insert(make_pair(currentBoard.getBoardString(), moves));
        }

        // Loop over all possible pushes
        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            board tempBoard = moves[k];

            pair<int,int> tempPlayerPos = tempBoard.getPlayerPosition();

            // Finished? Set path and return
            if (tempBoard.isFinished()) {
                mPath = tempBoard.getPath();
                return -1; 
            }

            // If f_score check is done, this code block has to be moved to after
            // the f_score calculation below.
            map_it = closed.find(tempBoard.getBoardString());
            // If already visited, skip.
            if ( map_it != closed.end() ){
                // Optional. Only skip visited states if we had a lower f
                // if (f_score.at(tempBoard.getBoardString()) <= t_f_score ) {
                    continue;
                // }
            }

            // Calculate new g and f
            int t_g_score = g_score.at(currentBoard.getBoardString()) + 1;
            int t_f_score = h_coeff*heuristicDistance(tempBoard) +  t_g_score;

            // cout << "g_score in f: " << t_g_score << endl;
            // cout << "h_score in f: " << h_coeff*heuristicDistance(tempBoard) << endl;

            // IDA bounds checking. If we're above bound, skip this push.
            if (bound < t_f_score) {
                // Keep track of the lowest f_score skipped to use in the next iteration.
                if (t_f_score < minCost)
                    minCost = t_f_score;
                mBoundUsed = true;
                continue; 
            }
            else {
                // Add the push to the queue and store g and f.
                g_score.insert(make_pair(tempBoard.getBoardString(), t_g_score));
                f_score.insert(make_pair(tempBoard.getBoardString(), t_f_score));
                openQueue.push(make_pair(tempBoard, t_f_score));
                ++mNumQueued;
                // cout << "mNumQueued: " << mNumQueued << endl;
                // Mark as processed
                closed.insert(make_pair(tempBoard.getBoardString(), 0));
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
