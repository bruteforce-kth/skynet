#ifndef BOARD_H
#define BOARD_H

// Position types
#define PLAYER          '@'
#define WALL            '#'
#define FLOOR           ' '
#define GOAL            '.'
#define PLAYER_ON_GOAL  '+'
#define BOX             '$'
#define BOX_ON_GOAL     '*'

// Move directions
#define MOVE_UP         'U'
#define MOVE_DOWN       'D'
#define MOVE_LEFT       'L'
#define MOVE_RIGHT      'R'

#include <vector>
#include <map>

class board {

    public:
        board(const std::vector<std::vector<char> > &chars);
        std::vector<std::pair<int,int> > getGoalPositions()
        { return mGoalPositions; }
        std::pair<int,int> getPlayerPosition() 
        { return mPlayerPos; }
        int getIndex(int row, int col);
        bool isAccessible(int row, int col);
        bool isGoal(int row, int col);
        std::vector<std::pair<std::pair<int,int>,char> > getAllValidMoves(int row, int col);

    private:
        void initializeIndexAndPositions(const std::vector<std::vector<char> > &chars);
        std::vector<std::vector<char> > mBoard;
        std::map<std::pair<int,int>, int> mBoardIndexes;
        int mBoardSize;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::pair<int,int> mPlayerPos;

};

#endif
