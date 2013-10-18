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
#include <chrono>

class solver {
    public:
        solver();
        std::string solve(board &b);

    private:
        //VAIRABLES
        // long long int mTime;
        int h_coeff;
        int g_coeff;
        int h1;
        int t1;
        int goalPow;
        bool mFirstIter;
        int mBoardSize;
        int mNumQueued;
        std::string mPath;
        bool mBoundUsed;
        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::pair<int,int> mStartingPos;
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
        // std::string IDA(const board &b);
        std::string aStar(const board &b);
        int distanceBFS(const std::vector<std::vector<char> > &board, std::pair<int,int> startPos);
        bool isPushable(char b);
        std::vector<std::pair<int,int> > getAllValidDirections(
            const std::vector<std::vector<char> > &board, std::pair<int,int> pos);

        void printCoordinates(int x, int y);
        void printMatrix(std::vector<std::vector<int> > &m);
};
