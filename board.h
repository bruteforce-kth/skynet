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
#define DEAD            'X'

// Move directions
#define MOVE_UP         'U'
#define MOVE_DOWN       'D'
#define MOVE_LEFT       'L'
#define MOVE_RIGHT      'R'

#include <vector>
#include <algorithm>

class board {

    public:
        board (const std::vector<std::vector<char> > &chars);
        board(const std::vector<std::vector<char> > &chars, 
            bool wasPush, char whatGotMeHere,
             std::vector<std::pair<int,int> > deadPositions, std::string path);
        board (const board &source, bool wasPush, 
             char whatGotMeHere, std::pair<int,int> playerPos,
             std::vector<std::pair<int,int> > boxPositions);
        const std::vector<std::pair<int,int> > getGoalPositions() const
        { return mGoalPositions; }
        const std::vector<std::pair<int,int> > getBoxPositions() const
        { return mBoxPositions; }
        const std::pair<int,int> getPlayerPosition() const
        { return mPlayerPos; }
        const std::vector<std::pair<int,int> > getDeadPositions() const
        { return mDeadPositions; }
        const int getBoardSize() const
        { return mBoardSize; }
        std::string getBoardString() const
        { return mBoardString; }
        std::string getPath() const 
        { return mPath; }
        std::vector<std::vector<char> > getBoardCharVector() const
        {return mBoard; }
        bool isAccessible(int row, int col, int prevRow, int prevCol) const;
        bool isWalkable(int row, int col) const;
        bool isGoal(int row, int col) const;
        bool isBox(int row, int col) const;
        void getAllValidMoves(std::vector<board> &moves, std::string prevPath) const;
        bool isFinished() const;
        bool isPush() const
        { return mWasPush; }
        char getWhatGotMeHere() const
        { return mWhatGotMeHere; }
        void printBoard();
    private:
        std::string mPath;
        void initializeIndexAndPositions(const std::vector<std::vector<char> > &chars);
        void findDeadlocks(const std::vector<std::vector<char> > &chars);
        std::vector<std::vector<char> > mBoard;
        std::string mBoardString;
        int mBoardSize;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::vector<std::pair<int,int> > mDeadPositions;
        std::pair<int,int> mPlayerPos;
        bool mWasPush;
        char mWhatGotMeHere;
        board* doMove(std::pair<int,int> newPlayerPos, char direction, std::string prevPath) const;
};

#endif
