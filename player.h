#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"
#include <queue>

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
    double score[25][25]={},cal[15]={1,20,500,200000,99999999,3919093202,3190932029,3199093202,3190932902,3190932902,3199093202,319093202,3199093202,3190932092,3190993202};
    AIPlayer(Board* board, char piece);
    void iniscore();
    void calscore(char piece);
    struct move{
        int x,y;
        double score;
        bool operator<(const move& other) const {
            return score < other.score;  // 小于号的逻辑反转
        }   
    };
    std::priority_queue<move> bestmove();
    void makeMove() override;  // 计算机玩家的落子方法
};

#endif // PLAYER_H
