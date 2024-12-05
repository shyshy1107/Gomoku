#ifndef MINMAX_H
#define MINMAX_H

#include "board.h"
#include <limits.h> // For INT_MIN and INT_MAX

class MinMax {
public:
    MinMax(Board* board, char player);
    
    // 计算当前局面下的最佳落子
    std::pair<int, int> getBestMove(int depth);
    
private:
    Board* board;  // 指向棋盘的指针
    char player;   // 当前计算机玩家 ('X' 或 'O')

    // 评估函数，返回当前棋盘局面的得分
    int evaluateBoard();
    
    // Min-Max 算法递归实现
    int minimax(int depth, bool isMaximizingPlayer);
    
    // 检查是否有五子连线
    bool checkWin(char player);
};

#endif // MINMAX_H
