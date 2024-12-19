#include "board.h"

Board::Board(int size) : size(size), grid(size, std::vector<char>(size, '.')) {}

bool Board::placePiece(int x, int y, char piece) {
    if (isValidMove(x,y,piece)) {
        grid[x][y] = piece;
        lastX=x;
        lastY=y;
        if(piece!='.')op.push({x,y});
        return true;
    }
    else return false;
}

int Board::getSize()const{
    return size;
}

bool Board::hq(){
    if(!op.empty()){
        int x=op.top().first,y=op.top().second;
        placePiece(x,y,'.');
        op.pop();
    }
    else return false;
    if(!op.empty()){
        int x=op.top().first,y=op.top().second;
        placePiece(x,y,'.');
        op.pop();
        return true;
    }
    else return false;
}

void Board::clearBoard(){
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid[i][j] = '.';
        }
    }
    while(!op.empty())op.pop();
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
                xx1+=dir[i][0],yy1+=dir[i][1],xx2-=dir[i][0],yy2-=dir[i][1];
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
            while(cnt01<2||cnt02<2){
                if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='.')cnt01++;
                else if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='X')cnt1++;
                else pos=false;
                pos=pos&&isInBoard(xx1,yy1)&&cnt01<2;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<2&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)!='O'&&getPiece(xx2,yy2)!='O'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='.')cnt02++;
                else if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='X')cnt1++;
                else neg=false;
                neg=neg&&isInBoard(xx2,yy2)&&cnt02<2;
                if(neg)xx2-=dir[i][0],yy2-=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<2&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)!='O'&&getPiece(xx2,yy2)!='O'))flag33=1;
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