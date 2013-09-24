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

class board {

    public:
        board(const std::vector<std::vector<char> > &chars);
        const std::vector<std::pair<int,int> > getGoalPositions() const
        { return mGoalPositions; }
        const std::pair<int,int> getPlayerPosition() const
        { return mPlayerPos; }
        const int getLongestRow() const
        { return mLongestRow; }
        const int getNumRows() const
        { return mNumRows; }
        const int getBoardSize() const
        { return mBoardSize; }
        bool isAccessible(int row, int col, int prevRow, int prevCol) const;
        bool isWalkable(int row, int col) const;
        bool isGoal(int row, int col) const;
        bool isBox(int row, int col) const;
        std::vector<std::pair<std::pair<int,int>,char> > getAllValidMoves(int row, int col) const;

    private:
        int mLongestRow;
        int mNumRows;
        void initializeIndexAndPositions(const std::vector<std::vector<char> > &chars);
        std::vector<std::vector<char> > mBoard;
        int mBoardSize;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::pair<int,int> mPlayerPos;

};

#endif
