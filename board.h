#ifndef BOARD_H
#define BOARD_H

#include <vector>

class Board {
public:
    Board(int size = 15);  // 构造函数，初始化棋盘
    void display() const;  // 显示棋盘
    bool placePiece(int x, int y, char piece);  // 放置棋子
    char getPiece(int x, int y) const;  // 获取某个位置的棋子
    bool isFull() const;  // 检查棋盘是否已满
    bool checkWin(char piece);  // 检查胜利条件
    bool isValidMove(int x, int y,char piece) const;  // 判断是否是有效落子
    int getSize()const;
    void clearBoard();
    bool checkWinFrom(int x, int y, char piece)const;
    bool isInBoard(int x,int y)const;
    struct js
    {
        int cl[8],sl[2];
        int CL;     
    }cnt[25][25];
    
private:
    int size;  // 棋盘大小
    int lastX,lastY;
    std::vector<std::vector<char>> grid;  // 棋盘存储
    bool checkHorizontal(int x, int y, char piece)const;
    bool checkVertical(int x, int y, char piece)const;
    bool checkDiagonal1(int x, int y, char piece)const;
    bool checkDiagonal2(int x, int y, char piece)const;
    void updatecnt(int x,int y);
    bool checkjs(int x,int y,int type)const;
};

#endif // BOARD_H
