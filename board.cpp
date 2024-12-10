#include "board.h"
#include <iostream>

Board::Board(int size) : size(size), grid(size, std::vector<char>(size, '.')) {}

void Board::display() const {
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            std::cout << grid[i][j] << ' ';
        }
        std::cout << std::endl;
    }
}

bool Board::placePiece(int x, int y, char piece) {
    if (isValidMove(x,y,piece)) {
        grid[x][y] = piece;
        lastX=x;
        lastY=y;
        updatecnt(x,y);
        return true;
    }
    else return false;
}

int Board::getSize()const{
    return size;
}

void Board::clearBoard(){
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            grid[i][j] = '.';
        }
    }
}

char Board::getPiece(int x, int y) const {
    return grid[x][y];
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
    if(grid[x][y]!='.')return false;
    if(piece=='X'){
        if(checkjs(x,y,2))return false;
        if(checkWinFrom(x,y,piece))return true;
        if(checkjs(x,y,1))return false;
    }
    return true;
}

bool Board::checkWinFrom(int x, int y, char piece) const{
    // 现在可以利用传入的坐标 (x, y) 和 piece 来判断是否有五子连线
    // 例如，检查该位置的横竖斜连成五子的条件
    return checkHorizontal(x, y, piece) || checkVertical(x, y, piece) || 
checkDiagonal1(x, y, piece) || checkDiagonal2(x, y, piece);
}

bool Board::checkHorizontal(int x, int y, char piece)const {
    int count = 1;  // 包括当前的位置
    // 向右检查
    for (int i = 1; i < 5; ++i) {
        if (x + i < size && grid[x + i][y] == piece) {
            count++;
        } else {
            break;
        }
    }
    // 向左检查
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && grid[x - i][y] == piece) {
        count++;
        } else {
            break;
        }
    }
    return count >= 5;
}

bool Board::checkVertical(int x, int y, char piece)const {
    int count = 1;  // 包括当前的位置
    // 向下检查
    for (int i = 1; i < 5; ++i) {
        if (y + i < size && grid[x][y + i] == piece) {
            count++;
        } else {
            break;
        }
    }
    // 向上检查
    for (int i = 1; i < 5; ++i) {
        if (y - i >= 0 && grid[x][y - i] == piece) {
            count++;
        } else {
            break;
        }
    }
    return count >= 5;
}

bool Board::checkDiagonal1(int x, int y, char piece)const {
    int count = 1;  // 包括当前的位置
    // 向右下检查
    for (int i = 1; i < 5; ++i) {
        if (x + i < size && y + i < size && grid[x + i][y + i] == piece) {
            count++;
        } else {
            break;
        }
    }
    // 向左上检查
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && y - i >= 0 && grid[x - i][y - i] == piece) {
            count++;
        } else {
            break;
        }
    }
    return count >= 5;
}

bool Board::checkDiagonal2(int x, int y, char piece)const {
    int count = 1;  // 包括当前的位置
    // 向右上检查
    for (int i = 1; i < 5; ++i) {
        if (x + i < size && y - i >= 0 && grid[x + i][y - i] == piece) {
            count++;
        } else {
            break;
        }
    }
    // 向左下检查
    for (int i = 1; i < 5; ++i) {
        if (x - i >= 0 && y + i < size && grid[x - i][y + i] == piece) {
            count++;
        } else {
            break;
        }
    }
    return count >= 5;
}

void Board::updatecnt(int x,int y){
    if(getPiece(x,y)=='X'){
        int dir[4][2]={{0,1},{1,0},{1,-1},{1,1}};
        for(int i=0;i<4;i++){
            int xx1=x+dir[i][0],yy1=y+dir[i][1],xx2=x-dir[i][0],yy2=y-dir[i][1];
            int cnt1=0,cnt2=0;
            bool pos=(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='X'),neg=(isInBoard(xx2,yy2)&&getPiece(xx2,yy2)=='X');
            while(pos){
                cnt1++;
                xx1+=dir[i][0],yy1+=dir[i][1];
                pos=(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='X');
            }
            while(neg){
                cnt2++;
                xx2-=dir[i][0],yy2-=dir[i][1];
                neg=(isInBoard(xx2,yy2)&&getPiece(xx2,yy2)=='X');
            }
            if(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='.'){
                cnt[xx1][yy1].cl[i+4]+=1+cnt2;
            }
            if(isInBoard(xx2,yy2)&&getPiece(xx2,yy2)=='.'){
                cnt[xx2][yy2].cl[i]+=1+cnt1;
            }
        }
        for(int i=0;i<15;i++){
            for(int j=0;j<15;j++){
                for(int k=0;k<4;k++){
                    cnt[i][j].CL=std::max(cnt[i][j].CL,cnt[i][j].cl[k]+cnt[i][j].cl[k+4]);
                }
            }
        }
    }
}

bool Board::checkjs(int x,int y,int type)const{
    if(type==2){
        if(cnt[x][y].CL>=5)return true;
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
                if(isInBoard(xx1,yy1)&&pos&&getPiece(xx1,yy1)=='X')cnt1++;
                pos=isInBoard(xx1,yy1)&&cnt01<2;
                if(pos)xx1+=dir[i][0],yy1+=dir[i][1];
                if(cnt1==3&&cnt01+cnt02<2&&(isInBoard(xx1,yy1)&&isInBoard(xx2,yy2)&&getPiece(xx1,yy1)!='O'&&getPiece(xx2,yy2)!='O'))flag33=1;
                if(cnt1==4&&(cnt01==1||cnt02==1||(isInBoard(xx1,yy1)&&getPiece(xx1,yy1)=='.')||(neg&&getPiece(xx2,yy2)=='.')))flag44=1;
                if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='.')cnt02++;
                if(isInBoard(xx2,yy2)&&neg&&getPiece(xx2,yy2)=='X')cnt1++;
                neg=isInBoard(xx2,yy2)&&cnt02<2;
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