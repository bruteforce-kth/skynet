#include <string.h>
#include <vector>
#include <utility>
#include "board.h"
#include <unordered_map>

class solver {
    public:
        solver();
        std::string solve(const board &b);

    private:
        std::string mSolution;
        int mBoardSize;
        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::pair<int,int> mStartingPos;
        std::string mPath;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::unordered_map<std::string, std::vector<std::pair<int,int> > > visited;
        std::vector< std::vector<std::vector<std::pair<std::pair<int,int>, char> > > > previous;
        std::string IDA(const board &b);
        float aStar(const board &b, float bound);
        bool aStarPlayer(const board &b, std::pair<int,int> goal);
        int distance(int i1, int j1, int i2, int j2);
        int heuristicDistance(const board &b);
        int heuristicPlayerDistance(std::pair<int,int> from, std::pair<int,int> to);
        bool isRepeatedMove(char a, char b);
        void printCoordinates(int x, int y);
        std::string hashState(const std::vector<std::pair<int,int> > boxPositions) const;
        bool isReachable(const board &b, std::vector<std::pair<int,int> > playerPositions);
        board getLockedDownBoxesBoard(const board &boardToConvert);
        //std::vector< std::pair<int,int> > getPushDirections(board &b);
};
