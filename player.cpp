#include "player.h"
#include "board.h"
#include <iostream>
#include <random>
#include <limits>
#include <cmath>
#include <string.h>

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

char Player::getPiece() const {
    return piece;
}

HumanPlayer::HumanPlayer(Board* board, char piece) : Player(board, piece) {}

void HumanPlayer::makeMove() {
}

AIPlayer::AIPlayer(Board* board, char piece) : Player(board, piece) {}

std::priority_queue<AIPlayer::move> AIPlayer::bestmove(Board* board){
    std::priority_queue<move> q;
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            if(board->getPiece(i,j)!='.')continue;
            if(board->checkjs(i,j,0)||board->checkjs(i,j,1)||board->checkjs(i,j,2)){
                score[i][j]=0;
                continue;
            }
            if(board->hasneighbor(i,j,1))q.push({i,j,score[i][j]});
        }
    }
    return q;
}

void AIPlayer::calscore(Board* board, char piece){
    memset(score,0,sizeof(score));
    iniscore();
    char oppo=(piece=='X'?'O':'X');
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            if(board->getPiece(i,j)!='.')continue;
            if(!board->hasneighbor(i,j,1))continue;
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
                                flag1++;
                                if(cnt[0][0]>1){
                                    flag1=2;
                                    break;
                                }
                            }
                            else if(board->getPiece(xx1,yy1)==pd1){
                                cnt[0][1]++;
                            }
                            else{
                                if(pd1!=piece)flag1=cnt[0][1]-2;
                                else flag1=-2;
                                break;
                            }
                        }
                    }
                    else{
                        flag1--;
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
                                        if(pd1!=piece)flag1=cnt[0][1]-2;
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
                                flag2++;
                                if(cnt[1][0]>1){
                                    flag2=2;
                                    break;
                                }
                            }
                            else if(board->getPiece(xx2,yy2)==pd2){
                                cnt[1][1]++;
                            }
                            else{
                                if(pd2!=piece)flag2=cnt[1][1]-2;
                                else flag2=-2;
                                cnt[1][1]--;
                                break;
                            }
                        }
                    }
                    else{
                        flag2--;
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
                                        if(pd2!=piece)flag2=cnt[1][1]-2;
                                        else flag2=-2;
                                        cnt[1][1]--;
                                        break;
                                    }
                                }
                                if(pd2!=piece)flag2-=2;
                            }
                        }
                    }
                }
                //分情况对分数进行合计
                if(pd1!=pd2)score[i][j]=std::max(score[i][j],std::max((cal[cnt[0][1]]*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd1==piece?1:(pd1=='.'?0:0.3))*pow(5,flag1),(cal[cnt[1][1]]*pow(0.2,cnt[1][0]+cnt[0][0]))*(pd2==piece?1:(pd2=='.'?0:0.3))*pow(5,flag2)));
                else score[i][j]=std::max(score[i][j],(cal[cnt[0][1]+cnt[1][1]]*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd2==piece?1:(pd2=='.'?0:0.3))*pow(5,std::max(flag1,flag2)));
                if(board->checkWinFrom(i,j,piece))score[i][j]=1e9;
                if(board->checkWinFrom(i,j,oppo))score[i][j]=1e9-1;
            }
        }
    }
}

void AIPlayer::minmax(move& best,move m,int depth,bool isPlayer,std::priority_queue<move> q,double& minscore,double& maxscore){
    if(depth==0){
        if(isPlayer){
            if(q.top().score<minscore){
                best=m;
                minscore=q.top().score;
                return;
            }
        }
        else{
            if(q.top().score>maxscore){
                best=m;
                maxscore=q.top().score;
                return;
            }
        }
        return;
    }
    while(!q.empty()){
        move n=q.top();
        q.pop();
        if(isPlayer&&n.score==1e9)return;
        board->placePiece(n.x,n.y,isPlayer?(piece=='X'?'O':'X'):piece);
        calscore(board,isPlayer?piece:(piece=='X'?'O':'X'));
        std::priority_queue<move> p=bestmove(board);
        minmax(best,m,depth-1,!isPlayer,p,minscore,maxscore);
        board->placePiece(n.x,n.y,'.');
        if(isPlayer)return;
    }
}

void AIPlayer::makeMove() {
    /*calscore(piece);
    std::priority_queue<move> q=bestmove();
    move best=q.top();
    double minscore=1e9,maxscore=0;
    while(!q.empty()){
        move m=q.top();
        q.pop();
        board->placePiece(m.x,m.y,piece);
        calscore(piece=='X'?'O':'X');
        std::priority_queue<move> p=bestmove();
        if(p.top().score<minscore){
            minscore=p.top().score;
            best=m;
        }
        board->placePiece(m.x,m.y,'.');
    }
    while(!q.empty()){
        move m=q.top();
        q.pop();
        board->placePiece(m.x,m.y,piece);
        calscore(piece=='X'?'O':'X');
        std::priority_queue<move> p=bestmove();
        minmax(best,m,1,1,p,minscore,maxscore);
        board->placePiece(m.x,m.y,'.');
    }
    board->placePiece(best.x,best.y,piece);*/
    int cnt=0;
    for(int i=0;i<board->getSize();i++){
        for(int j=0;j<board->getSize();j++){
            if(board->getPiece(i,j)!='.')cnt++;
        }
    }
    //if(cnt>20)mtcl(10000);
   // else{
        calscore(board,piece);
        move best=bestmove(board).top();
        board->placePiece(best.x,best.y,piece);
    //}
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
    score[7][7]+=20;
}

void deleteNode(AIPlayer::Node* node) {
    for (auto child : node->children) {
        deleteNode(child);
    }
    delete node;
}

void AIPlayer::mtcl(int cnt){
    Node* root=new Node(-1,-1);
    for(int i=0;i<cnt;i++){
        Node* node=root;
        char currentPlayer=piece;
        Board* simboard=new Board(*board);
        while (!node->children.empty()) {
            Node* bestNode = nullptr;
            double bestValue = -std::numeric_limits<double>::infinity();

            for (auto child : node->children) {
                double winRate,uctValue;
                if (child->visits == 0) {
                    uctValue = std::numeric_limits<double>::infinity(); // 强制选择未访问节点
                } else {
                    double winRate = static_cast<double>(child->wins) / child->visits;
                    uctValue = winRate + std::sqrt(2 * std::log(node->visits + 1) / child->visits);
                }
                uctValue += static_cast<double>(std::rand()) / RAND_MAX * 1e-6; 
                if (uctValue > bestValue) {
                    bestValue = uctValue;
                    bestNode = child;
                }
            }
            node=bestNode;
            simboard->placePiece(node->x,node->y,currentPlayer);
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                    if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                    Node* child = new Node(i,j, node);
                    node->children.push_back(child);
                }
            }
        }
        char winner='A';
        int steps=0;
        while(1){
            if(simboard->isFull()){
                winner='.';
                break;
            }
            std::vector<std::pair<int,int>> moves;
            /*calscore(simboard,currentPlayer);
            std::priority_queue<move> q=bestmove(simboard);
            int n=5;
            while(!q.empty()&&n--){
                move m=q.top();
                q.pop();
                moves.push_back({m.x,m.y});
            }*/
            for(int i=0;i<15;i++){
                for(int j=0;j<15;j++){
                    if(simboard->getPiece(i,j)=='.'&&simboard->hasneighbor(i,j,1)){
                        if(currentPlayer=='X'&&(simboard->checkjs(i,j,1)||simboard->checkjs(i,j,2)))continue;
                        moves.push_back({i,j});
                    }
                }
            }
            if(moves.empty()){
                winner=(currentPlayer=='X'?'O':'X');
                break;
            }
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_int_distribution<int> dis(0, moves.size()-1);
            std::pair<int,int> best=moves[dis(gen)];
            simboard->placePiece(best.first,best.second, currentPlayer);
            steps++;
            currentPlayer = (currentPlayer == 'X') ? 'O' : 'X';
        }
        while (node != nullptr) {
            node->visits++;
            if (winner == piece) {
                node->wins++;
            }
            if(winner==(piece=='X'?'O':'X'))node->wins--;
            node = node->parent;
        }
        delete simboard;
    }
    Node* bestNode = nullptr;
    double bestScore = -std::numeric_limits<double>::infinity();

    for (auto child : root->children) {
        double winRate = static_cast<double>(child->wins) / (child->visits + 1e-6);
        std::cout << "Win rate for child (" << child->x << ", " << child->y << "): " << winRate << std::endl;
        if (winRate > bestScore) {
            bestScore = winRate;
            bestNode = child;
        }
    }
    board->placePiece(bestNode->x,bestNode->y,piece);
    deleteNode(root);
}