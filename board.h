//
//  board.h
//  xcode_ai_hw_1_cc
//
//  Created by Martin Runelöv on 2013-07-16.
//  Copyright (c) 2013 Martin Runelöv. All rights reserved.
//

#ifndef BOARD_H     // Header guard
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

#include <string.h>
#include <vector>
#include <map>
#include <iostream>

class board {    
    
public:
    // Member functions
    board(const std::vector<std::vector<char> > &chars);
    std::string solve();
    std::vector<float> f_score;    // Heuristic cost used in A*
    void printBoard();
    
private:
        
    // Member variables
    std::vector<std::vector<char> > charMap;
    std::map<std::pair<int,int>, int> boardIndexes;
    std::pair<int,int> playerPos;
    std::vector<std::pair<int,int> > goalPositions;
    std::pair<int,int> averageGoalPos;
    int boardSize;
    
    // Member functions
    bool initializeIndexAndPositions(const std::vector<std::vector<char> > &chars);
    int getBoardSize();
    int getIndex(int i, int j);
    int getIndex(std::pair<int,int> coord);
    bool checkPosition(int i, int j);
    bool isGoal(int i, int j);
    // Algorithms + helper methods
    std::string dfs();
    std::string aStar();
    std::vector<std::pair<std::pair<int,int>,char> > getAllValidMoves(int i, int j);
    int heuristic_distance_to_goal_estimate(std::pair<int,int>);
    int diagonal_distance(int i1, int j1, int i2, int j2); 
    std::string backtrack(const std::vector<std::pair<int, char> > &previous, int i, int j);
};


#endif /* defined(BOARD_H) - Header guard */
