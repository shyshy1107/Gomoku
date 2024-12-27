#include <stack>
#include <vector>
#include <queue>
#include <sstream>
#include <iostream>
#include "jsoncpp/json.h" 
#include <string>
#include <random>
#include <limits>
#include <cmath>
#include <cstring>
using namespace std;
class Board {
public:
    Board(int size = 15);  // 构造函数，初始化棋盘
    bool placePiece(int x, int y, char piece);  // 放置棋子
    char getPiece(int x, int y) const;  // 获取某个位置的棋子
    bool isFull() const;  // 检查棋盘是否已满
    bool checkWin(char piece);  // 检查胜利条件
    bool isValidMove(int x, int y,char piece) const;  // 判断是否是有效落子
    int getSize()const;
    void clearBoard();
    bool checkWinFrom(int x, int y, char piece)const;
    bool isInBoard(int x,int y)const;
    bool checkjs(int x,int y,int type)const;
    bool hq();
    std::vector<std::vector<char>> grid;  // 棋盘存储
    bool hasneighbor(int x,int y,int dis);
    int size;  // 棋盘大小
    Board(Board& other):grid(other.grid),size(other.size){};
    int lastX,lastY;
    
private:
    std::stack<std::pair<int,int>> op;
};
class Player {
public:
    Player(Board* board, char piece);
    char getPiece() const;
    Board* board;

protected:
    char piece;
};
Board::Board(int size) : size(size), grid(size, std::vector<char>(size, '.')) {}

bool Board::placePiece(int x, int y, char piece) {
    if (isValidMove(x,y,piece)) {
        grid[x][y] = piece;
        lastX=x;
        lastY=y;
        if(piece!='.')op.push({x,y});
        else op.pop();
        return true;
    }
    else return false;
}

int Board::getSize()const{
    return size;
}

char Board::getPiece(int x, int y) const {
    if(isInBoard(x,y))return grid[x][y];
    return '.';
}

bool Board::isFull() const {
    for (const auto& row : grid) {
        for (char cell : row) {
            if (cell == '.') return false;
        }
    }
    return true;
}

bool Board::checkWin(char piece) {
    return checkWinFrom(lastX, lastY, piece);
}

bool Board::hasneighbor(int x,int y,int dis){
    for(int i=dis*-1;i<=dis;i++){
        for(int j=dis*-1;j<=dis;j++){
            if(!(i||j))continue;
            if(!isInBoard(x+i,y+j))continue;
            if(grid[i+x][j+y]!='.')return true;
        }
    }
    return false;
}

bool Board::isValidMove(int x, int y,char piece) const {
    if(!isInBoard(x,y))return false;
    if(piece!='.'&&grid[x][y]!='.')return false;
    if(piece=='X'){
        if(checkjs(x,y,2))return false;
        if(checkWinFrom(x,y,piece))return true;
        if(checkjs(x,y,1))return false;
    }
    return true;
}

bool Board::checkWinFrom(int x, int y, char piece) const{
    int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
    for(int i=0;i<4;i++){
        int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
        bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
        int cnt=1;
        while(pos||neg){
            if(pos&&getPiece(xx1,yy1)==piece)cnt++;
            else pos=false;
            if(neg&&getPiece(xx2,yy2)==piece)cnt++;
            else neg=false;
            xx1+=dir[i][0],yy1+=dir[i][1],xx2-=dir[i][0],yy2-=dir[i][1];
            pos=pos&&isInBoard(xx1,yy1),neg=neg&&isInBoard(xx2,yy2);
        }
        if(cnt>=5)return true;
    }
    return false;
}

bool Board::checkjs(int x,int y,int type)const{
    if(type==2){
        int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
        for(int i=0;i<4;i++){
            int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
            bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
            int cnt=0;
            while(pos||neg){
                if(pos&&getPiece(xx1,yy1)=='X')cnt++;
                else pos=false;
                if(neg&&getPiece(xx2,yy2)=='X')cnt++;
                else neg=false;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(neg)xx2-=dir[i][0],yy2-=dir[i][1];
                pos=pos&&isInBoard(xx1,yy1),neg=neg&&isInBoard(xx2,yy2);
            }
            if(cnt>=5)return true;
        }
        return false;
    }
    else{
        if(checkWinFrom(x,y,'X'))return false;
        int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
        int cnt33=0,cnt44=0;
        for(int i=0;i<4;i++){
            int flag33=0,flag44=0;
            int cnt01=0,cnt02=0,cnt1=1;
            int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
            bool pos=isInBoard(xx1,yy1),neg=isInBoard(xx2,yy2);
            while(cnt01<=2||cnt02<=2){
                if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='.')cnt01++;
                else if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='X')cnt1++;
                else pos=false;
                pos=pos&&isInBoard(xx1,yy1)&&cnt01<2;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<=3&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)=='.'&&getPiece(xx2,yy2)=='.'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='.')cnt02++;
                else if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='X')cnt1++;
                else neg=false;
                neg=neg&&isInBoard(xx2,yy2)&&cnt02<2;
                if(neg)xx2-=dir[i][0],yy2-=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<=3&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)=='.'&&getPiece(xx2,yy2)=='.'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(pos&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(!(pos||neg))break;
            }
            if(flag33)cnt33++;
            if(flag44)cnt44++;
        }
        if(cnt33>1||cnt44>1)return true;
    }
    return false;
}

bool Board::isInBoard(int x,int y)const{
    return x >= 0 && x < size && y >= 0 && y < size;
}
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
    std::priority_queue<move> bestmove(Board* board);
};

Player::Player(Board* board, char piece) : board(board), piece(piece) {}

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

char Player::getPiece() const {
    return piece;
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
                                if(board->getPiece(xx1-dir[k][0],yy1-dir[k][1])==pd1)cnt[0][1]--;
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
                                        if(board->getPiece(xx1-dir[k][0],yy1-dir[k][1])==pd1)cnt[0][1]--;
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
                                if(board->getPiece(xx2+dir[k][0],yy2+dir[k][1])==pd2)cnt[1][1]--;
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
                                        if(board->getPiece(xx2+dir[k][0],yy2+dir[k][1])==pd2)cnt[1][1]--;
                                        break;
                                    }
                                }
                                if(pd2!=piece)flag2-=2;
                            }
                        }
                    }
                }
                //分情况对分数进行合计
                if(pd1!=pd2)score[i][j]=std::max(score[i][j],std::max((cal[cnt[0][1]]*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd1==piece?1:(pd1=='.'?0:30))*pow(5,flag1),(cal[cnt[1][1]]*pow(0.2,cnt[1][0]+cnt[0][0]))*(pd2==piece?1:(pd2=='.'?0:30))*pow(5,flag2)));
                else score[i][j]=std::max(score[i][j],(cal[cnt[0][1]+cnt[1][1]]*pow(0.2,cnt[0][0]+cnt[1][0]))*(pd2==piece?1:(pd2=='.'?0:30))*pow(5,std::max(flag1,flag2)));
                if(board->checkWinFrom(i,j,piece))score[i][j]=1e9;
                if(board->checkWinFrom(i,j,oppo))score[i][j]=1e9-1;
            }
        }
    }
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
int main(){
    string str;
    getline(cin, str);
 	Json::Reader reader;
 	Json::Value input;
 	reader.parse(str, input);
    // 分析自己收到的输入和自己过往的输出，并恢复状态
    AIPlayer ai=AIPlayer(new Board(15),'X');
	int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++) {
		ai.board->placePiece(input["requests"][i]["x"].asInt(), input["requests"][i]["y"].asInt(), ai.getPiece()=='X'?'O':'X');
        ai.board->placePiece(input["responses"][i]["x"].asInt(), input["responses"][i]["y"].asInt(), ai.getPiece());
	}
    if(turnID==0&&input["requests"][turnID]["x"].asInt()==-1){
        Json::Value ret;
        ret["response"]["x"]=7;
        ret["response"]["y"]=7;
        Json::FastWriter writer;
        cout << writer.write(ret) << endl;
        return 0;
    }
	ai.board->placePiece(input["requests"][turnID]["x"].asInt(), input["requests"][turnID]["y"].asInt(), ai.getPiece()=='X'?'O':'X');
	// 输出决策JSON
	Json::Value ret;
    ai.calscore(ai.board,ai.getPiece());
    AIPlayer::move best=ai.bestmove(ai.board).top();
    ret["response"]["x"]=best.x;
    ret["response"]["y"]=best.y; 
	Json::FastWriter writer;
	cout << writer.write(ret) << endl;
}