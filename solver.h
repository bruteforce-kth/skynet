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
        int mBoardSize;
        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::pair<int,int> mStartingPos;
        std::string mPath;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::vector< std::vector<float> > f_score;    // Heuristic cost used in A*
        std::vector< std::vector<int> > g_score;    // Number of steps taken, used in A*
        std::unordered_map<std::string, int> g_score_map;
        std::vector< std::vector<std::vector<std::pair<std::pair<int,int>, char> > > > previous;
        // std::unordered_map<std::string, std::pair<std::pair<int,int>, char> > previous_map;
        std::string previousMapHash(board &b, int x, int y);
        bool aStar(const board &b);
        void backtrack(std::vector<std::vector<std::vector<std::pair<std::pair<int,int>, char> > > >&previous, int i, int j);
        int distance(int i1, int j1, int i2, int j2);
        int heuristicDistance(const std::vector< std::pair<int,int> > &boxPositions);
        void printCoordinates(int x, int y);
        //std::vector< std::pair<int,int> > getPushDirections(board &b);
};
