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
    if (isValidMove(x,y)) {
        grid[x][y] = piece;
        lastX=x;
        lastY=y;
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

bool Board::isValidMove(int x, int y) const {
    return x >= 0 && x < size && y >= 0 && y < size && grid[x][y] == '.';
}

bool Board::checkHorizontal(int x, int y, char piece) {
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

bool Board::checkVertical(int x, int y, char piece) {
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

bool Board::checkDiagonal1(int x, int y, char piece) {
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

bool Board::checkDiagonal2(int x, int y, char piece) {
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