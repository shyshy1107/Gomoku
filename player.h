#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

class Player {
public:
    Player(Board* board, char piece);
    virtual void makeMove() = 0;  // 纯虚函数，派生类实现
    char getPiece() const;

protected:
    Board* board;
    char piece;
};

class HumanPlayer : public Player {
public:
    HumanPlayer(Board* board, char piece);
    void makeMove() override;  // 人类玩家的落子方法
};

class AIPlayer : public Player {
public:
    AIPlayer(Board* board, char piece);
    void makeMove() override;  // 计算机玩家的落子方法
    int evaluateBoard();  // 评估棋盘状态
    int minimax(int depth, bool isMaximizing);  // Min-Max 算法
};

#endif // PLAYER_H
