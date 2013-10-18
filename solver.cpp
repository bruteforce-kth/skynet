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

    h_coeff = 1;
    g_coeff = 5;
    h1 = 6;
    t1 = 1;
    goalPow = 3;
    
    if (mBoardSize < 50) {
        goalPow = 2;
        g_coeff = 2;
        h1 = 5;
    }

    mGoalPositions = b.getGoalPositions();
    calculateDistances(b);
    // printMatrix(mDistanceMatrix);

    // A*
    return aStar(b);

}

/*
 * Custom comparator for A* that compares the f_score of two coordinates.
 */
 struct fcomparison {
    bool operator() (pair<board,int> a, pair<board,int> b) {
        return a.second >= b.second ? true : false;
    }
};

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
        if (boxPositions.empty())
            break;
    }

    // cout << "Heuristic = " << h1*heuristic << " + " << t1*totalDistances << endl;
    // cout << "total: " << h1*heuristic + t1*totalDistances << endl;
    // b.printBoard();
    return h1*heuristic + t1*totalDistances;
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
    if ((pos.first+1) < board.size()-1) {
        char down = board[pos.first+1][pos.second];
        if (isPushable(down))
            moves.push_back(make_pair(pos.first+1, pos.second));
    }
    if ((pos.second-1) >= 0) {
        char left = board[pos.first][pos.second-1];
        if (isPushable(left))
            moves.push_back(make_pair(pos.first, pos.second-1));
    }
    if ((pos.second+1) < board[pos.first].size()-1) {
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
        mDistanceMatrix[i] = vector<int>(board[i].size());
        for(int j = 0; j < board[i].size(); j++) {
        //     int shortestDistance = b.getBoardSize(); // Set to a high value
        //     for(int k = 0; k < mGoalPositions.size(); k++) {
        //         pair<int,int> goal = mGoalPositions[k];
        //         int d = distance(i, j, goal.first, goal.second);
        //         if( d < shortestDistance) {
        //             shortestDistance = d;
        //             if (shortestDistance == 0)
        //                 break;
        //         }
        //     }
        //     mDistanceMatrix[i][j] = shortestDistance;
        // }
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
                    || board[goal.first-1][goal.second] == DEAD)) {
                    // || board[goal.first-1][goal.second] == PLAYER_ON_DEAD)) {
                ++numBlocked;
        }
        if(goal.first < board.size()-1
            && ( board[goal.first+1][goal.second] == WALL
                || board[goal.first+1][goal.second] == DEAD)) {
                    // || board[goal.first+1][goal.second] == PLAYER_ON_DEAD)) {
            ++numBlocked;
    }
    if(goal.second > 0
        && ( board[goal.first][goal.second-1] == WALL
            || board[goal.first][goal.second-1] == DEAD)) {
                    // || board[goal.first][goal.second-1] == PLAYER_ON_DEAD)) {
        ++numBlocked;
}
if(goal.second < board[goal.first].size()-1
    && ( board[goal.first][goal.second+1] == WALL
        || board[goal.first][goal.second+1] == DEAD)) {
                    // || board[goal.first][goal.second+1] == PLAYER_ON_DEAD)) {
    ++numBlocked;
}

mDistanceMatrix[goal.first][goal.second] -= pow(goalPow,numBlocked + 1);

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
 * (Assumes that aStar returns minimal pruned f_score. 
 *  The current version does not, and doesn't take bound as an argument). 
 * 
 * The A* will skip branches with f_score > bound,
 * and the next iteration will run with the bound set to the lowest 
 * skipped f_score.
 */
//  string solver::IDA(const board &b) {
//     mNumQueued = 0;
//     mPath = "no path";
//     int start_h = heuristicDistance(b);
//     // Arbitrary start bound. Preferably board-dependent.
//     int bound = h_coeff*start_h -4;
//     mBoundUsed = true;
//     while(mPath == "no path" && mBoundUsed) {
//         // A* returns the lowest f_score that was skipped
//         bound = aStar(b, bound);
//     }
//     return mPath;
// }

string solver::aStar(const board &b) {
#if DEBUG
    // cout << "RUNNING A*" << endl;
#endif
    // mBoundUsed = false;
    // int minCost = mBoardSize*100;
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
    board currentBoard = openQueue.top().first;
    vector<board> moves;
    board tempBoard = currentBoard;
    pair<int,int> tempPlayerPos;
    int t_g_score;
    int t_f_score;
    while(!openQueue.empty()) {
        currentBoard = openQueue.top().first;
#if DEBUG
        // cout << "f_score: " << openQueue.top().second << endl;
        // currentBoard.printBoard();
#endif
        openQueue.pop();

        // Transposition table used with IDA* to avoid duplicate calls to getPossibleStateChanges().
        // board_map_it = mTransTable.find(currentBoard.getBoardString());
        // if(board_map_it != mTransTable.end()) {
        //     // cout << "moves found!" << endl;
        //     moves = board_map_it->second;
        // }
        // else{
        currentBoard.getPossibleStateChanges(moves);
            // mTransTable.insert(make_pair(currentBoard.getBoardString(), moves));
        // }

        // Loop over all possible pushes
        std::unordered_map<std::string,int>::const_iterator map_it;
        for (int k = 0; k < moves.size(); ++k) {
            tempBoard = moves[k];

            tempPlayerPos = tempBoard.getPlayerPosition();

            // Finished? Set path and return
            if (tempBoard.isFinished()) {
#if DEBUG 
                // cout << "states explored: " << count << endl;
#endif
                return tempBoard.getPath();
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

            // Used in IDA*
            // if (bound < t_f_score) {
            //     if (t_f_score < minCost)
            //         minCost = t_f_score;
            //     continue;
            // }

            // Add the push to the queue and store g and f.
            g_score.insert(make_pair(tempBoard.getBoardString(), t_g_score));
            f_score.insert(make_pair(tempBoard.getBoardString(), t_f_score));
            openQueue.push(make_pair(tempBoard, t_f_score));
            // ++mNumQueued;
            // cout << "mNumQueued: " << mNumQueued << endl;
            // Mark as processed
            closed.insert(make_pair(tempBoard.getBoardString(), 0));
        }
        moves.clear();

#if DEBUG 
        // count++;
#endif
    }
#if DEBUG 
    // cout << "states explored: " << count << endl;
#endif
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
