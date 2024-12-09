#include "player.h"
#include "min_max.h"
#include <iostream>
#include <limits.h>

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

char Player::getPiece() const {
    return piece;
}

HumanPlayer::HumanPlayer(Board* board, char piece) : Player(board, piece) {}

void HumanPlayer::makeMove() {
    int x, y;
    std::cout << "Enter your move (x, y): ";
    std::cin >> x >> y;
    if (!board->placePiece(x, y, piece)) {
        std::cout << "Invalid move! Try again." << std::endl;
        makeMove();  // 重试
    }
}

AIPlayer::AIPlayer(Board* board, char piece) : Player(board, piece) {}

void AIPlayer::makeMove() {
    // Min-Max 算法决策
    /*int bestScore = INT_MIN;
    int bestMoveX = -1, bestMoveY = -1;

    for (int x = 0; x < board->getSize(); ++x) {
        for (int y = 0; y < board->getSize(); ++y) {
            if (board->getPiece(x, y) == '.') {
                board->placePiece(x, y, piece);  // 试探性落子
                int score = minimax(3, false);  // 评估
                if (score > bestScore) {
                    bestScore = score;
                    bestMoveX = x;
                    bestMoveY = y;
                }
                board->placePiece(x, y, '.');  // 撤回落子
            }
        }
    }

    // 落子
    board->placePiece(bestMoveX, bestMoveY, piece);*/
    /*int x,y;
    do{
        x=rand()%(board->getSize()),y=rand()%(board->getSize());
    }while(!board->placePiece(x,y,piece));*/\
    int score[25][25]={};
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            
        }
    }
}

int AIPlayer::minimax(int depth, bool isMaximizing) {
    // Min-Max 算法实现
    // 返回当前局面评分
    return 0;
}
