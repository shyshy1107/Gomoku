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
    /*int x,y;
    do{
        x=rand()%(board->getSize()),y=rand()%(board->getSize());
    }while(!board->placePiece(x,y,piece));*/
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
                                if(cnt[0][0])flag1--;
                            }
                            else{
                                flag1=cnt[0][1]-3;
                                cnt[0][1]--;
                                break;
                            }
                        }
                    }
                    else{
                        cnt[1][0]++;
                        xx1-=dir[k][0],yy1-=dir[k][1]; 
                        if(board->isInBoard(xx1,yy1)){
                            if(board->getPiece(xx1,yy1)!='.'){
                                pd2=board->getPiece(xx1,yy1);
                                cnt[0][1]++;
                                while(board->isInBoard(xx1,yy1)){
                                    xx1-=dir[k][0],yy1-=dir[k][1];
                                    if(!board->isInBoard(xx1,yy1))break;
                                    if(board->getPiece(xx1,yy1)=='.'){
                                        flag1=1;
                                        break;
                                    }
                                    else if(board->getPiece(xx1,yy1)==pd1){
                                        cnt[0][1]++;
                                        if(cnt[0][0])flag1--;
                                    }
                                    else{
                                        flag1=cnt[0][1]-3;
                                        cnt[0][1]--;
                                        break;
                                    }
                                }
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
                                if(cnt[1][0])flag2--;
                            }
                            else{
                                flag2=cnt[1][1]-3;
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
                                        if(cnt[1][0])flag2--;
                                    }
                                    else{
                                        flag2=cnt[1][1]-3;
                                        cnt[1][1]--;
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
                if(pd1!=pd2)score[i][j]=std::max(score[i][j],std::max((pow(10,cnt[0][1])*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd1==piece?1:(pd1=='.'?0:300))*pow(5,flag1),(pow(10,cnt[1][1])*pow(0.2,cnt[1][0]+cnt[0][0]))*(pd2==piece?1:(pd2=='.'?0:300))*pow(5,flag2)));
                else{
                    if(pd1==piece)score[i][j]=std::max(score[i][j],(pow(10,cnt[0][1]+cnt[1][1])*pow(0.8,cnt[0][0]+cnt[1][0]>1?1:cnt[0][0]+cnt[1][0]))*(pd2==piece?1:(pd2=='.'?0:300))*pow(5,flag1+flag2));
                    else score[i][j]=std::max(score[i][j],(pow(10,cnt[0][1])*pow(0.2,cnt[0][0]))*(pd1==piece?1:(pd1=='.'?0:300))*pow(5,flag1)+(pow(10,cnt[1][1])*pow(0.8,cnt[1][0]))*(pd2==piece?1:(pd2=='.'?0:300))*pow(5,flag2));
                }
                if(board->checkWinFrom(i,j,piece))score[i][j]=1e9;
                if(board->checkWinFrom(i,j,oppo))score[i][j]=1e9-1;
                /*if(pos){
                    if(board->getPiece(xx1,yy1)=='.'){
                        cnt[0][0]++;
                        xx1+=dir[k][0],yy1+=dir[k][1];
                        pos=board->isInBoard(xx1,yy1);
                    }
                    if(pos)pd1=board->getPiece(xx1,yy1);
                }
                if(neg){
                    if(board->getPiece(xx2,yy2)=='.'){
                        cnt[1][0]++;
                        xx2-=dir[k][0],yy2-=dir[k][1];
                        neg=board->isInBoard(xx2,yy2);
                    }
                    if(neg)pd2=board->getPiece(xx2,yy2);
                }
                if(pd1=='.')pos=false;
                if(pd2=='.')neg=false;
                while(pos||neg){
                    if(pos&&board->getPiece(xx1,yy1)==pd1)cnt[0][1]++;
                    else if(pos&&board->getPiece(xx1,yy1)=='O')cnt[0][0]++;
                    else{
                        cnt[0][1]--;
                        pos=false;
                    }
                    if(pos&&cnt[0][0]>1){
                        flag1=1;
                        pos=false;
                    }
                    if(pos)xx1+=dir[k][0],yy1+=dir[k][1];
                    if(!board->isInBoard(xx1,yy1))pos=false;//下面是反向
                    if(neg&&board->getPiece(xx2,yy2)==pd2)cnt[1][1]++;
                    else if(neg&&board->getPiece(xx2,yy2)=='O')cnt[1][0]++;
                    else{
                        cnt[1][1]--;
                        neg=false;
                    }
                    if(neg&&cnt[1][0]>1){
                        flag2=1;
                        neg=false;
                    }
                    if(neg)xx2-=dir[k][0],yy2-=dir[k][1];
                    if(!board->isInBoard(xx2,yy2))neg=false;
                    if(cnt[1][0]+cnt[0][0]>1)break;
                }
                if(pd1==pd2&&pd1!='.')score[i][j]+=(pow(10,cnt[0][1]+cnt[1][1])*pow(0.3,cnt[0][0]+cnt[1][0]))*(pd1==piece?1:1.2)*pow(10,flag1+flag2);
                else if(pd1!=pd2)score[i][j]+=(pow(10,cnt[0][1])*pow(0.3,cnt[0][0]))*(pd1==piece?1:1.2)*pow(10,flag1)+(pow(10,cnt[1][1])*pow(0.3,cnt[1][0]))*(pd2==piece?1:1.2)*pow(10,flag2);*/
                /*if(board->isInBoard(xx1,yy1)){
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
                }*/
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

void AIPlayer::iniscore(){
    memset(score,0,sizeof(score));
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