#include "solver.h"
// #include <chrono>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::priority_queue;
using std::pair;
using std::make_pair;
using std::stringstream;
using std::stack;

// using std::chrono::duration_cast;
// using std::chrono::microseconds;
// using std::chrono::steady_clock;

/*
 * c-tor
 */
 solver::solver() {}

/*
 * The solver method. It takes a board as a parameter and returns a solution
 */
 string solver::solve(board &b) {
    mBoardSize = b.getBoardSize();

    // To solve 004 fast: pow2, h_coeff 1, g_coeff 2, heuristic*5, total*1
    h_coeff = 1;
    g_coeff = 2;
    mGoalPositions = b.getGoalPositions();
    // steady_clock::time_point start = steady_clock::now();
    calculateDistances(b);
    // steady_clock::time_point end = steady_clock::now();
    // std::cout << "calculate distances took "
    //     << duration_cast<microseconds>(end-start).count()
    //     << " microseconds\n";

    // printMatrix(mDistanceMatrix);
    
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
    
    // b.printBoard();
    //return "no path";

    // A* only
    // aStar(b, 99999999);
    // return mPath;

    // printMatrix(mDistanceMatrix);
    // b.printBoard();

    // IDA
    // mTime = 0;
    string result = IDA(b);
    // cout << "something took: " << mTime << endl;
    return result;
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
        return a.second >= b.second ? true : false;
    }
};

/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
 */
//  int solver::heuristicDistance(const board &b) {
//     vector <vector<char> > board = b.getBoardCharVector();
//     int totalDistances = 0;
//     vector< pair<int,int> > boxPositions = b.getBoxPositions();
//     for (int i = 0; i < boxPositions.size(); ++i) {
//         // if (board[boxPositions[i].first][boxPositions[i].second] == BOX_ON_GOAL)
//         //     continue;
//         totalDistances += mDistanceMatrix[boxPositions[i].first][boxPositions[i].second];
//     }
//     return totalDistances;
// }

/**
 *  Test of new heuristic. Sum of goals distance to boxes.
 * The same box is NOT used twice.
 * (Assumes that numBoxes == numGoals)
 */
int solver::heuristicDistance(const board &b) {
    int heuristic = 0;
    int totalDistances = 0;
    int boxesOnGoal = 1;
    vector< pair<int,int> > boxPositions = b.getBoxPositions();
    vector< vector<char> > board = b.getBoardCharVector();
   
    for (int i = 0; i < boxPositions.size(); ++i) {
        totalDistances += mDistanceMatrix[boxPositions[i].first][boxPositions[i].second];
    }

    for (int i = 0; i < mGoalPositions.size(); ++i) {
        int closestBox = mBoardSize;
        int closestBoxIndex = -1;
        for (int j = 0; j < boxPositions.size(); ++j) {
            if (mGoalPositions[i] != boxPositions[j] && board[boxPositions[j].first][boxPositions[j].second] == BOX_ON_GOAL)
                continue;
            int d = distance(mGoalPositions[i].first, mGoalPositions[i].second, boxPositions[j].first, boxPositions[j].second);
            if (d < closestBox) {
                closestBox = d;
                closestBoxIndex = j;
                if (closestBox == 0) {
                    ++boxesOnGoal;
                    break;
                }
            }
        }
        // Remove the closest box. each box is used once.
        boxPositions.erase(boxPositions.begin() + closestBoxIndex);
        heuristic += closestBox;
    }

    // cout << "Heuristic = " << 5*heuristic << " + " << totalDistances << endl;
    // cout << "total: " << heuristic + totalDistances << endl;
    // b.printBoard();
    return 5*heuristic + totalDistances;
}

int solver::distanceBFS(const vector< vector<char> > &board, pair<int,int> startPos){
    if (board[startPos.first][startPos.second] == BOX_ON_GOAL 
        || board[startPos.first][startPos.second] == GOAL
        || board[startPos.first][startPos.second] == PLAYER_ON_GOAL) {
        return 0;
    }
    std::unordered_map<string, int> closed;
    std::deque< pair< pair<int,int>, int> > q;
    q.push_back(make_pair(startPos, 0));

    while(!q.empty()) {
        pair<pair<int,int>, int> currentPos = q.front();
        q.pop_front();

        vector< pair<int,int> > moves = getAllValidDirections(board, currentPos.first);
        std::unordered_map<string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            pair<int,int> tempPos = moves[k];
            if (board[tempPos.first][tempPos.second] == GOAL 
                || board[tempPos.first][tempPos.second] == BOX_ON_GOAL 
                || board[tempPos.first][tempPos.second] == PLAYER_ON_GOAL ) {
                
                return currentPos.second + 1;
            }
            string key = std::to_string(tempPos.first) + "-" + std::to_string(tempPos.second);
            map_it = closed.find(key);
            if (map_it != closed.end()) {
                continue;
            }
            q.push_back(make_pair(tempPos, currentPos.second + 1));
            closed.insert(make_pair(key, 0));
        }
    }
    return mBoardSize*2;
}

vector< pair<int,int> > solver::getAllValidDirections(const vector< vector<char> > &board, pair<int,int> pos) {
    vector< pair<int,int> > moves;
    if ((pos.first-1) >= 0) {
        char up = board[pos.first-1][pos.second];
        if (isPushable(up))
            moves.push_back(make_pair(pos.first-1, pos.second));
    }
    if ((pos.first+1) < board.size()) {
        char down = board[pos.first+1][pos.second];
        if (isPushable(down))
            moves.push_back(make_pair(pos.first+1, pos.second));
    }
    if ((pos.second-1) >= 0) {
        char left = board[pos.first][pos.second-1];
        if (isPushable(left))
            moves.push_back(make_pair(pos.first, pos.second-1));
    }
    if ((pos.second+1) < board[pos.first].size()) {
        char right = board[pos.first][pos.second+1];
        if (isPushable(right))
            moves.push_back(make_pair(pos.first, pos.second+1));
    }
    return moves;
} 

bool solver::isPushable(char b) {
    if (b != FLOOR 
        && b != BOX
        && b != BOX_ON_GOAL
        && b != GOAL
        && b != PLAYER
        && b != PLAYER_ON_GOAL) {
        return false;
    }
    return true;
}

void solver::calculateDistances(const board &b) {
    vector<vector<char> > board = b.getBoardCharVector();
    mDistanceMatrix.resize(board.size());

    for(int i = 0; i < board.size(); i++) {
        mDistanceMatrix[i] = vector<int>(board[i].size(), mBoardSize*2);
        for(int j = 0; j < board[i].size(); j++) {
            // int shortestDistance = b.getBoardSize(); // Set to a high value
            // for(int k = 0; k < mGoalPositions.size(); k++) {
            //     pair<int,int> goal = mGoalPositions[k];
            //     int d = distance(i, j, goal.first, goal.second);
            //     if( d < shortestDistance) {
            //         shortestDistance = d;
            //         if (shortestDistance == 0)
            //             break;
            //     }
            // }
            // mDistanceMatrix[i][j] = shortestDistance;
            
            if(board[i][j] != WALL && board[i][j] != DEAD && board[i][j] != PLAYER_ON_DEAD) {
                mDistanceMatrix[i][j] = distanceBFS(board, make_pair(i,j));
            }
        }
    }
    for (int i = 0; i < mGoalPositions.size(); ++i) {
        int numBlocked = 0;
        pair<int,int> goal = mGoalPositions[i];
        if(goal.first > 0
                && ( board[goal.first-1][goal.second] == WALL
                    || board[goal.first-1][goal.second] == DEAD
                    || board[goal.first-1][goal.second] == PLAYER_ON_DEAD)) {
            ++numBlocked;
        }
        if(goal.first < board.size()-1
                && ( board[goal.first+1][goal.second] == WALL
                    || board[goal.first+1][goal.second] == DEAD
                    || board[goal.first+1][goal.second] == PLAYER_ON_DEAD)) {
            ++numBlocked;
        }
        if(goal.second > 0
                && ( board[goal.first][goal.second-1] == WALL
                    || board[goal.first][goal.second-1] == DEAD
                    || board[goal.first][goal.second-1] == PLAYER_ON_DEAD)) {
            ++numBlocked;
        }
        if(goal.second < board[goal.first].size()-1
                && ( board[goal.first][goal.second+1] == WALL
                    || board[goal.first][goal.second+1] == DEAD
                    || board[goal.first][goal.second+1] == PLAYER_ON_DEAD)) {
            ++numBlocked;
        }
        mDistanceMatrix[goal.first][goal.second] -= pow(2,numBlocked+1);
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
#if DEBUG
    // cout << "RUNNING A*" << endl;
    cout << "Running A* with bound: " << bound << endl;
#endif
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
    g_score.insert(make_pair(b.getBoardString(), 1));
    // f_score.insert(make_pair(b.getBoardString(), 1));

    openQueue.push(make_pair(b, 1));
#if DEBUG 
    int count = 0;
#endif
    board currentBoard = openQueue.top().first;;
    vector<board> moves;
    board tempBoard = currentBoard;
    pair<int,int> tempPlayerPos;
    int t_g_score;
    int t_f_score;
    while(!openQueue.empty()) {
        currentBoard = openQueue.top().first;

#if DEBUG
        cout << "f_score: " << openQueue.top().second << endl;
        currentBoard.printBoard();
#endif
        openQueue.pop();

        moves.clear();
        std::unordered_map<std::string, std::vector<board> >::const_iterator board_map_it;
        // Check if the pushes for this state has already been calculated. 
        board_map_it = mTransTable.find(currentBoard.getBoardString());
        if(board_map_it != mTransTable.end()) {
            // cout << "moves found!" << endl;
            moves = board_map_it->second;
        }
        else{
            // cout << "no moves found!" << endl;
            // steady_clock::time_point start = steady_clock::now();
            currentBoard.getPossibleStateChanges(moves);
            // steady_clock::time_point end = steady_clock::now();
            // mTime += duration_cast<microseconds>(end-start).count();
            mTransTable.insert(make_pair(currentBoard.getBoardString(), moves));
        }
        // Loop over all possible pushes
        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            tempBoard = moves[k];

            tempPlayerPos = tempBoard.getPlayerPosition();

            // Finished? Set path and return
            if (tempBoard.isFinished()) {
                mPath = tempBoard.getPath();
#if DEBUG 
                cout << "states explored: " << count << endl;
#endif
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
            t_g_score = g_score.at(currentBoard.getBoardString()) + 1;
            t_f_score = h_coeff*heuristicDistance(tempBoard) +  t_g_score*g_coeff;

#if DEBUG
            // cout << "g_score in f: " << t_g_score*g_coeff << endl;
            // cout << "h_score in f: " << h_coeff*heuristicDistance(tempBoard) << endl;
#endif

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
                // ++mNumQueued;
                // cout << "mNumQueued: " << mNumQueued << endl;
                // Mark as processed
                closed.insert(make_pair(tempBoard.getBoardString(), 0));
            }
        }

#if DEBUG 
        count++;
#endif
    }
#if DEBUG 
    cout << "states explored: " << count << endl;
#endif
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
