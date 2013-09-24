#include <string.h>
#include <vector>
#include <utility>
#include "board.h"

class solver {
    public:
        solver();
        std::string solve(const board &b);

    private:
        int mBoardSize;
        std::pair<int,int> mBoxPos;
        std::pair<int,int> mPlayerPos;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector< std::vector<float> > f_score;    // Heuristic cost used in A*
        std::string aStar(const board &b);
        std::string backtrack(const std::vector<std::vector<std::pair<std::pair<int,int>, char> > >&previous, int i, int j);
        int distance(int i1, int j1, int i2, int j2);
        int heuristicDistanceToGoal(std::pair<int,int> p);
        void printCoordinates(int x, int y);
        std::vector< std::pair<int,int> > getPushDirections(board &b);
};
