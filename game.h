#ifndef GAME_H
#define GAME_H

#include "board.h"
#include "player.h"

class Game {
public:
    Game(int boardSize = 15);
    void start();  // 启动游戏
    void switchPlayer();  // 切换玩家
    bool isGameOver() const;  // 判断游戏是否结束

private:
    Board board;  // 棋盘对象
    Player* currentPlayer;  // 当前玩家
    Player* player1;  // 玩家1
    Player* player2;  // 玩家2
    bool gameOver;  // 游戏是否结束
};

#endif // GAME_H
