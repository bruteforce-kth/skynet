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
#include <set>



class board {

    public:
            struct possibleBoxPush {

            std::pair<int, int> playerPosition;
            std::pair<int, int> boxPosition;
            std::string path;
            std::vector<std::pair<int, int> > positionsAroundBox;

        };
        board (const std::vector<std::vector<char> > &chars);
        board (const std::vector<std::vector<char> > &chars, 
               bool wasPush, char whatGotMeHere,
               std::string path);
        board (const board &source, bool wasPush, 
               char whatGotMeHere, std::pair<int,int> playerPos,
               std::vector<std::pair<int,int> > boxPositions);
        const std::vector<std::pair<int,int> > getGoalPositions() const
        { return mGoalPositions; }
        const std::vector<std::pair<int,int> > getBoxPositions() const
        { return mBoxPositions; }
        const std::pair<int,int> getPlayerPosition() const
        { return mPlayerPos; }
        const int getBoardSize() const
        { return mBoardSize; }
        std::string getBoardString() const
        { return mBoardString; }
        std::string getPath() const 
        { return mPath; }
        void appendToPath(std::string newPath)
        { mPath = mPath + newPath; }
        std::vector<std::vector<char> > getBoardCharVector() const
        {return mBoard; }
        bool isAccessible(int row, int col, int prevRow, int prevCol) const;
        bool isWalkable(int row, int col) const;
        bool isGoal(int row, int col) const;
        bool isBox(int row, int col) const;
        void getAllValidMoves(std::vector<board> &moves) const;
        void getAllValidWalkMoves(std::vector<board> &moves) const;
        bool isFinished() const;
        bool isPush() const
        { return mWasPush; }
        char getWhatGotMeHere() const
        { return mWhatGotMeHere; }
        void printBoard() const;
        void getPossibleStateChanges(std::vector<board> &board);
        std::string boxAStar(std::pair<int,int> goalBox);
        int heuristicDistanceToBox(const std::vector< std::pair<int,int> > &boxPositions, std::pair<int,int> currentPos);
        int distance(std::pair<int,int> i, std::pair<int,int> j);
        board* doLongMove(std::pair<int,int> newPlayerPos, std::pair<int,int> newBoxPos,
                         char lastMove, std::string path);
        void setBoxPositionsString();
        std::string getBoxString() const
        { return mBoxString; }
    private:
        std::string mBoxString;
        std::string mPath;
        void initializeIndexAndPositions(const std::vector<std::vector<char> > &chars);
        std::vector<std::vector<char> > mBoard;
        std::string mBoardString;
        int mBoardSize;
        std::vector<std::pair<int,int> > mGoalPositions;
        std::vector<std::pair<int,int> > mBoxPositions;
        std::pair<int,int> mPlayerPos;
        bool mWasPush;
        char mWhatGotMeHere;
        board* doMove(std::pair<int,int> newPlayerPos, char direction) const;
        void investigatePushBoxDirections(struct possibleBoxPush &possibleBoxPush, std::vector<board> &moves);
        char getDirectionToPos(std::pair<int, int> player, std::pair<int, int> box);
        void circleBox(struct possibleBoxPush &possibleBoxPush, char directionToBox, std::vector<board> &moves, std::string path);
        void investigateThesePositions(struct possibleBoxPush &possibleBoxPush,
                                      std::vector<std::pair<int, int> > &possibles, std::vector<board> &moves, std::string path);
        bool vectorContainsPair(std::vector<std::pair<int,int> > &vector, std::pair<int, int> &pair);
        std::pair<int,int> getPushCoordinates(std::pair<int,int> playerCoordinates,
                                    std::pair<int,int> boxCoordinates);
        char translateDirection(char nsew);
        void updatePlayerPosition(std::pair<int, int> newPlayerPosition);
};

#endif
