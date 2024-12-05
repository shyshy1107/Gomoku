#include "min_max.h"

MinMax::MinMax(Board* board, char player) : board(board), player(player) {}

std::pair<int, int> MinMax::getBestMove(int depth) {
    int bestScore = INT_MIN;
    std::pair<int, int> bestMove = {-1, -1};

    // 扫描棋盘所有空格
    for (int x = 0; x < board->getSize(); ++x) {
        for (int y = 0; y < board->getSize(); ++y) {
            if (board->getPiece(x, y) == '.') {
                board->placePiece(x, y, player);
                int score = minimax(depth - 1, false);
                board->placePiece(x, y, '.'); // 撤销落子

                if (score > bestScore) {
                    bestScore = score;
                    bestMove = {x, y};
                }
            }
        }
    }
    return bestMove;
}

int MinMax::minimax(int depth, bool isMaximizingPlayer) {
    if (depth == 0 || checkWin(player)) {
        return evaluateBoard();
    }

    int bestScore = isMaximizingPlayer ? INT_MIN : INT_MAX;

    for (int x = 0; x < board->getSize(); ++x) {
        for (int y = 0; y < board->getSize(); ++y) {
            if (board->getPiece(x, y) == '.') {
                board->placePiece(x, y, isMaximizingPlayer ? player : (player == 'X' ? 'O' : 'X'));
                int score = minimax(depth - 1, !isMaximizingPlayer);
                board->placePiece(x, y, '.'); // 撤销落子

                if (isMaximizingPlayer) {
                    bestScore = std::max(bestScore, score);
                } else {
                    bestScore = std::min(bestScore, score);
                }
            }
        }
    }

    return bestScore;
}

int MinMax::evaluateBoard() {
    // 一个简单的评估函数，可以根据游戏规则扩展
    if (checkWin(player)) {
        return 10;  // 计算机胜利
    }
    if (checkWin(player == 'X' ? 'O' : 'X')) {
        return -10;  // 对方胜利
    }
    return 0;  // 平局或没有结束
}

bool MinMax::checkWin(char player) {
    // 检查是否有五子连线
    // 水平、垂直和对角线检查可以参考前面 `Board` 类的实现
    return false; // 这是一个简化示例，你可以根据需要补充具体的胜利条件判断
}
