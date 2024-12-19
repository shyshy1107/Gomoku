#include "player.h"
#include <iostream>
#include <limits.h>
#include <math.h>
#include <string.h>

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

char Player::getPiece() const {
    return piece;
}

HumanPlayer::HumanPlayer(Board* board, char piece) : Player(board, piece) {}

void HumanPlayer::makeMove() {
}

AIPlayer::AIPlayer(Board* board, char piece) : Player(board, piece) {}

void AIPlayer::makeMove() {
    memset(score,0,sizeof(score));
    iniscore();
    char oppo=(piece=='X'?'O':'X');
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            if(board->getPiece(i,j)!='.')continue;
            int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
            for(int k=0;k<4;k++){
                char pd1='.',pd2='.';
                int cnt[2][2]={{0,0},{0,0}};//前一个表示方向后一个表示是否空白
                int flag1=0,flag2=0;
                int xx1=i+dir[k][0],yy1=j+dir[k][1],xx2=i-dir[k][0],yy2=j-dir[k][1];
                bool pos=board->isInBoard(xx1,yy1),neg=board->isInBoard(xx2,yy2);
                if(board->isInBoard(xx1,yy1)){
                    if(board->getPiece(xx1,yy1)!='.'){
                        pd1=board->getPiece(xx1,yy1);
                        cnt[0][1]++;
                        while(board->isInBoard(xx1,yy1)){
                            xx1+=dir[k][0],yy1+=dir[k][1];
                            if(!board->isInBoard(xx1,yy1))break;
                            if(board->getPiece(xx1,yy1)=='.'){
                                cnt[0][0]++;
                                if(cnt[0][0]>1){
                                    flag1=2;
                                    break;
                                }
                            }
                            else if(board->getPiece(xx1,yy1)==pd1){
                                cnt[0][1]++;
                            }
                            else{
                                flag1=cnt[0][1]-2;
                                cnt[0][1]--;
                                break;
                            }
                        }
                    }
                    else{
                        cnt[0][0]++;
                        xx1+=dir[k][0],yy1+=dir[k][1]; 
                        if(board->isInBoard(xx1,yy1)){
                            if(board->getPiece(xx1,yy1)!='.'){
                                pd1=board->getPiece(xx1,yy1);
                                cnt[0][1]++;
                                while(board->isInBoard(xx1,yy1)){
                                    xx1+=dir[k][0],yy1+=dir[k][1];
                                    if(!board->isInBoard(xx1,yy1))break;
                                    if(board->getPiece(xx1,yy1)=='.'){
                                        flag1=1;
                                        break;
                                    }
                                    else if(board->getPiece(xx1,yy1)==pd1){
                                        cnt[0][1]++;
                                    }
                                    else{
                                        if(pd1==piece)flag1=cnt[0][1]-2;
                                        else flag1=-2;
                                        cnt[0][1]--;
                                        break;
                                    }
                                }
                                if(pd1!=piece)flag1-=2;
                            }
                        }
                    }
                }
                if(board->isInBoard(xx2,yy2)){
                    if(board->getPiece(xx2,yy2)!='.'){
                        pd2=board->getPiece(xx2,yy2);
                        cnt[1][1]++;
                        while(board->isInBoard(xx2,yy2)){
                            xx2-=dir[k][0],yy2-=dir[k][1];
                            if(!board->isInBoard(xx2,yy2))break;
                            if(board->getPiece(xx2,yy2)=='.'){
                                cnt[1][0]++;
                                if(cnt[1][0]>1){
                                    flag2=2;
                                    break;
                                }
                            }
                            else if(board->getPiece(xx2,yy2)==pd2){
                                cnt[1][1]++;
                            }
                            else{
                                flag2=cnt[1][1]-2;
                                cnt[1][1]--;
                                break;
                            }
                        }
                    }
                    else{
                        cnt[1][0]++;
                        xx2-=dir[k][0],yy2-=dir[k][1]; 
                        if(board->isInBoard(xx2,yy2)){
                            if(board->getPiece(xx2,yy2)!='.'){
                                pd2=board->getPiece(xx2,yy2);
                                cnt[1][1]++;
                                while(board->isInBoard(xx2,yy2)){
                                    xx2-=dir[k][0],yy2-=dir[k][1];
                                    if(!board->isInBoard(xx2,yy2))break;
                                    if(board->getPiece(xx2,yy2)=='.'){
                                        flag2=1;
                                        break;
                                    }
                                    else if(board->getPiece(xx2,yy2)==pd2){
                                        cnt[1][1]++;
                                    }
                                    else{
                                        flag2=cnt[1][1]-2;
                                        cnt[1][1]--;
                                        break;
                                    }
                                }
                                if(pd2!=piece)flag2-=2;
                            }
                        }
                    }
                }
                if(pd1!=pd2)score[i][j]=std::max(score[i][j],std::max((cal[cnt[0][1]]*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd1==piece?1:(pd1=='.'?0:50))*pow(5,flag1),(cal[cnt[1][1]]*pow(0.2,cnt[1][0]+cnt[0][0]))*(pd2==piece?1:(pd2=='.'?0:50))*pow(5,flag2)));
                else score[i][j]=std::max(score[i][j],(cal[cnt[0][1]+cnt[1][1]]*pow(0.2,cnt[0][0]+cnt[1][0]>1?1:cnt[0][0]+cnt[1][0]))*(pd2==piece?1:(pd2=='.'?0:50))*pow(5,flag1+flag2));
                if(board->checkWinFrom(i,j,piece))score[i][j]=1e9;
                if(board->checkWinFrom(i,j,oppo))score[i][j]=1e9-1;
            }
        }
    }
    int bestx,besty;
    double bestscore=0;
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

void AIPlayer::iniscore(){
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
}