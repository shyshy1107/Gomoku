#include "player.h"
#include <iostream>
#include <limits.h>
#include <math.h>

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
    }while(!board->placePiece(x,y,piece));*/
    double score[25][25]={};
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            score[i][j]+=1;
        }
    }
    for(int i=3;i<board->getSize()-3;i++){
        for(int j=3;j<board->getSize()-3;j++){
            score[i][j]+=5;
        }
    }
    for(int i=5;i<board->getSize()-5;i++){
        for(int j=5;j<board->getSize()-5;j++){
            score[i][j]+=10;
        }
    }
    char oppo=(piece=='X'?'O':'X');
        for(int i=0;i<board->getSize();i++){
            for(int j=0;j<board->getSize();j++){
                if(board->getPiece(i,j)!='.')continue;
                int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
                for(int k=0;k<4;k++){
                    char pd='.';
                    int cnt0=0,cnt1=0;
                    int xx1=i+dir[k][0],yy1=j+dir[k][1],xx2=i-dir[k][0],yy2=j-dir[k][1];
                    if(board->isInBoard(xx1,yy1)){
                        if(board->getPiece(xx1,yy1)!='.'){
                            pd=board->getPiece(xx1,yy1);
                            cnt1++;
                            int flag=0;//0指被挡住1指空格
                            while(board->isInBoard(xx1,yy1)){
                                xx1+=dir[k][0],yy1+=dir[k][1];
                                if(!board->isInBoard(xx1,yy1))break;
                                if(board->getPiece(xx1,yy1)=='.'){
                                    cnt0++;
                                    if(cnt0>1){
                                        flag=1;
                                        break;
                                    }
                                }
                                else if(board->getPiece(xx1,yy1)==pd)cnt1++;
                                else{
                                    cnt1--;
                                    break;
                                }
                            }
                            score[i][j]+=(pow(10,cnt1)*pow(0.3,cnt0))*(pd==piece?1:1.2)*pow(10,flag);
                            cnt0=cnt1=0;
                        }
                    else{
                        cnt0++;
                        xx1-=dir[k][0],yy1-=dir[k][1]; 
                        if(board->isInBoard(xx1,yy1)){
                            if(board->getPiece(xx1,yy1)!='.'){
                                pd=board->getPiece(xx1,yy1);
                                cnt1++;
                                while(board->isInBoard(xx1,yy1)){
                                    xx1-=dir[k][0],yy1-=dir[k][1];
                                    if(!board->isInBoard(xx1,yy1))break;
                                    if(board->getPiece(xx1,yy1)=='.'){
                                        cnt0++;
                                        if(cnt0>1)break;
                                    }
                                    else if(board->getPiece(xx1,yy1)==pd)cnt1++;
                                    else{
                                        cnt1--;
                                        break;
                                    }
                                }
                                score[i][j]+=(pow(10,cnt1)*pow(0.3,cnt0))*(pd==piece?1:1.2);
                                cnt0=cnt1=0;
                            }
                        }
                    }
                }
                if(board->isInBoard(xx2,yy2)){
                    if(board->getPiece(xx2,yy2)!='.'){
                        pd=board->getPiece(xx2,yy2);
                        int flag=0;//0指被挡住1指空格
                        cnt1++;
                        while(board->isInBoard(xx2,yy2)){
                            xx2-=dir[k][0],yy2-=dir[k][1];
                            if(!board->isInBoard(xx2,yy2))break;
                            if(board->getPiece(xx2,yy2)=='.'){
                                cnt0++;
                                if(cnt0>1){
                                    flag=1;
                                    break;
                                }
                            }
                            else if(board->getPiece(xx2,yy2)==pd)cnt1++;
                            else{
                                cnt1--;
                                break;
                            }
                        }
                        score[i][j]+=(pow(10,cnt1)*pow(0.3,cnt0))*(pd==piece?1:1.2)*pow(10,flag);
                        cnt0=cnt1=0;
                    }
                    else{
                        cnt0++;
                        xx2-=dir[k][0],yy2-=dir[k][1]; 
                        if(board->isInBoard(xx2,yy2)){
                            if(board->getPiece(xx2,yy2)!='.'){
                                pd=board->getPiece(xx2,yy2);
                                cnt1++;
                                while(board->isInBoard(xx2,yy2)){
                                    xx2-=dir[k][0],yy2-=dir[k][1];
                                    if(!board->isInBoard(xx2,yy2))break;
                                    if(board->getPiece(xx2,yy2)=='.'){
                                        cnt0++;
                                        if(cnt0>1)break;
                                    }
                                    else if(board->getPiece(xx2,yy2)==pd)cnt1++;
                                    else{
                                        cnt1--;
                                        break;
                                    }
                                }
                                score[i][j]+=(pow(10,cnt1)*pow(0.3,cnt0))*(pd==piece?1:1.2);
                                cnt0=cnt1=0;
                            }
                        }
                    }
                }
            }
        }
    }
    int bestscore=0,bestx,besty;
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            if(board->getPiece(i,j)!='.')continue;
            if(board->checkjs(i,j,0)||board->checkjs(i,j,1)||board->checkjs(i,j,2))score[i][j]=0;
            if(score[i][j]>bestscore){
                bestscore=score[i][j];
                bestx=i;
                besty=j;
            }
        }
    }
    board->placePiece(bestx,besty,piece);
}
int AIPlayer::minimax(int depth, bool isMaximizing) {
    // Min-Max 算法实现
    // 返回当前局面评分
    return 0;
}
