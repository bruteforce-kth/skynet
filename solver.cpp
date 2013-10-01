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
 string solver::solve(const board &b) {
    g_score_map.reserve(600000);
    visited.reserve(600000);
    mBoardSize = b.getBoardSize();
    mGoalPositions = b.getGoalPositions();

    //Iterative deepening
    int depth = 2;
    string solution;
    do {
        cout << "Depth: " << depth << endl;
        solution = search(b, depth);
        cout << "Solution: " << solution << endl;
        if(solution != "no path") {
            return solution;
        }
        depth++;
    }while(depth < 100);

    return "no path";
}

/*
 * Search the state space using dfs
 */
string solver::search(const board &b, int depth) {

    board bo = b;
    bo.printBoard();

    if (b.isFinished()) {
        cout << "finished" << endl;
        return b.getPath();
    }else if(depth==0) {
        cout << "no path" << endl;
        return "no path";
    }

    vector<board> moves;
    b.getAllValidMoves(moves);
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
        return  a.second > b.second ? true : false;
    }
};

bool solver::isReachable(const board &b, vector<pair<int,int> > playerPositions){
    bool res = false;
    for(int i = 0; i < playerPositions.size(); i++){
        //res = aStarPlayer(b, playerPositions[i]);
        if(res)
            return true;
    }
    return false;
}


/*
 * Heuristic for A* search. 
 * The heuristic value is the sum of 
 * all boxes shortest distance to a goal.
 */
 int solver::heuristicDistance(const board &b) {
    int totalDistances = 0;
    vector< pair<int,int> > boxPositions = b.getBoxPositions();
    vector<vector<char> > chars = b.getBoardCharVector();
    for (int i = 0; i < boxPositions.size(); ++i) {
        int shortestDistance = mBoardSize;
        int x = boxPositions[i].first;
        int y = boxPositions[i].second;
        if (chars[x][y] == BOX_ON_GOAL)
            continue;
        else {
            for (int j = 0; j < mGoalPositions.size(); ++j) {
                int d = distance(x, y, mGoalPositions[j].first, mGoalPositions[j].second);
                if (d < shortestDistance) {
                    shortestDistance = d;
                    if (d == 1)
                        break;
                }   
            }
        }
        totalDistances += shortestDistance;
    }
    return totalDistances;
}



/*
 * Heuristic for A* player search. 
 */
 int solver::heuristicPlayerDistance(pair<int,int> from, pair<int,int> to) {
    return distance(from.first,from.second, to.first, to.second);
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
