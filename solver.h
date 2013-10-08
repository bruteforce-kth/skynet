#include <string.h>
#include <vector>
#include <utility>
#include <unordered_map>
#include <iostream>
#include <queue>
#include <list>
#include <sstream>
#include <stack>
#include "board.h"

class solver {
    public:
        solver();
        std::string solve(board &b);

    private:

        //VAIRABLES
        int mBoardSize;
        std::string mPath;

        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::pair<int,int> mStartingPos;
        std::unordered_map<std::string, std::vector<board> > mTransTable;

        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::vector< std::vector<int> > mDistanceMatrix;
        std::vector< std::vector<float> > f_score;    // Heuristic cost used in A*
        std::vector< std::vector<int> > g_score;    // Number of steps taken, used in A*

        std::unordered_map<std::string, int> g_score_map;
        std::unordered_map<std::string, std::vector<std::pair<int,int> > > visited;

        std::vector< std::vector<std::vector<std::pair<std::pair<int,int>, char> > > > previous;

        //FUNCTIONS
        std::string search(board &b, int depth);

        int distance(int i1, int j1, int i2, int j2);
        int heuristicDistance(const board &b);
        void calculateDistances(const board &b);
        int heuristicPlayerDistance(std::pair<int,int> from, std::pair<int,int> to);

        bool isReachable(const board &b, std::vector<std::pair<int,int> > playerPositions);
        bool isRepeatedMove(char a, char b);

        board getLockedDownBoxesBoard(const board &boardToConvert);

        std::string IDA(const board &b);
        std::string aStar(const board &b, float bound);

        void printCoordinates(int x, int y);
        void printMatrix(std::vector<std::vector<int> > &m);
};
