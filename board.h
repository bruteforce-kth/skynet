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
#define DEAD            'x'
#define PLAYER_ON_DEAD  '&'

// Move directions
#define MOVE_UP         'U'
#define MOVE_DOWN       'D'
#define MOVE_LEFT       'L'
#define MOVE_RIGHT      'R'

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>
#include <queue>
#include <string.h>
#include <unordered_map>



class board {

    public:
            struct possibleBoxPush {

            std::pair<int, int> playerPosition;
            std::pair<int, int> boxPosition;
            int movedBox_positionInVector;
            std::string path;
            std::vector<std::pair<int, int> > positionsAroundBox;

        };
        board (const std::vector<std::vector<char> > &chars);
        board(const std::vector<std::vector<char> > &chars,
            bool wasPush, char whatGotMeHere, std::string path, 
             std::vector<std::pair<int,int> > corners,
             std::string newBoardString, int boardSize,
             std::vector<std::pair<int, int> > goalPositions,
             std::pair<int, int> newPlayerPos,
             std::vector<std::pair<int, int> > newBoxPositions);
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
        bool isAccessible(int row, int col, int prevRow, int prevCol);
        bool isWalkable(int row, int col) const;
        bool isGoal(int row, int col) const;
        bool isBox(int row, int col) const;
        void getAllValidMoves(std::vector<board> &moves);
        void getAllValidWalkMoves(std::vector< std::pair<std::pair<int,int>, char> > &moves, std::pair<int,int> playerPos);
        bool isFinished() const;
        bool isPush() const
        { return mWasPush; }
        char getWhatGotMeHere() const
        { return mWhatGotMeHere; }
        void printBoard() const;
        void getPossibleStateChanges(std::vector<board> &board);
        std::string boxSearch(std::pair<int,int> goalBox);
        int heuristicDistanceToBox(const std::vector< std::pair<int,int> > &boxPositions, std::pair<int,int> currentPos);
        int distance(std::pair<int,int> i, std::pair<int,int> j);
        board doLongMove(std::pair<int,int> newPlayerPos, std::pair<int,int> newBoxPos,
                         char lastMove, std::string path, int i);
        void setBoxPositionsString();
        std::string getBoxString() const
        { return mBoxString; }
    private:
        bool isDynamicDeadlock(int row, int col, std::pair<int,int> boxPos);
        void isAccessibleRestore(int row, int col);
        std::pair<int,int> getRelativePosition(char direction, std::pair<int,int> position);
        bool stillHuggingWall(char wallDirection, std::pair<int,int> position);
        bool investigateWall(char direction, char wallDirection, std::pair<int,int> position);
        void findWallDeadlocks();
        void findDeadlocks(const std::vector<std::vector<char> > &chars);
        std::vector<std::pair<int,int> > mCornerPositions;
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
        board doMove(std::pair<int,int> newPlayerPos, char direction);
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

        bool isDeadspace(int row, int col);
};

#endif
