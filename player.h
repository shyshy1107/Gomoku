#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"
#include <queue>
#include <memory>
#include <thread>

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
    struct Node{
        Node* parent=nullptr;
        std::vector<Node*> children;
        int x,y,wins=0,visits=0;
        Node(int x,int y, Node* p = nullptr) : x(x),y(y),parent(p) {}
    };
    void simulate(Node* node,std::shared_ptr<Board> simboard,char currentPlayer);
    void mtcl(int cnt);
    void makeMove() override;  // 计算机玩家的落子方法
};

#endif // PLAYER_H
