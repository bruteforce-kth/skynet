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
        int h_coeff;
        int mBoardSize;
        int mNumQueued;
        std::string mPath;
        bool mBoundUsed;
        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::pair<int,int> mStartingPos;
        std::unordered_map<std::string, std::vector<board> > mTransTable;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::vector< std::vector<int> > mDistanceMatrix;

        //FUNCTIONS
        std::string search(board &b, int depth);
        int distance(int i1, int j1, int i2, int j2);
        int distance(std::pair<int,int> a, std::pair<int,int> b);
        int heuristicDistance(const board &b);
        void calculateDistances(const board &b);
        int heuristicPlayerDistance(std::pair<int,int> from, std::pair<int,int> to);
        bool isReachable(const board &b, std::vector<std::pair<int,int> > playerPositions);
        bool isRepeatedMove(char a, char b);
        std::string IDA(const board &b);
        int aStar(const board &b, int bound);

        void printCoordinates(int x, int y);
        void printMatrix(std::vector<std::vector<int> > &m);
};
