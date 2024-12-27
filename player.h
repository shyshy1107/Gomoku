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
    double score[25][25]={},cal[15]={1,20,5000,800000,1e8,3919093202,3190932029,3199093202,3190932902,3190932902,3199093202,319093202,3199093202,3190932092,3190993202};
    AIPlayer(Board* board, char piece);
    void iniscore();
    void calscore(Board* board, char piece);
    struct move{
        int x,y;
        double score;
        bool operator<(const move& other) const {
            return score < other.score;  // 小于号的逻辑反转
        }   
    };
    struct Node{
        Node* parent=nullptr;
        std::vector<Node*> children;
        int x,y,wins=0,visits=0;
        Node(int x,int y, Node* p = nullptr) : x(x),y(y),parent(p) {}
    };
    std::priority_queue<move> bestmove(Board* board);
    void mtcl(int cnt);
    void minmax(move& best,move m,int depth,bool isPlayer,std::priority_queue<move> q,double& minscore,double& maxscore);
    void makeMove() override;  // 计算机玩家的落子方法
};

#endif // PLAYER_H
