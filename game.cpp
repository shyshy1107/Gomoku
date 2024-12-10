#include "game.h"
#include <iostream>
#include <windows.h>

Game::Game(int boardSize) : board(boardSize), gameOver(false) {
    player1 = new HumanPlayer(&board, 'X');
    player2 = new AIPlayer(&board, 'O');
    currentPlayer = player1;
}

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == player1) ? player2 : player1;
    while(!isHuman()){
        currentPlayer->makeMove();
        refresh();
        if(board.checkWin(currentPlayer->getPiece()))over(false);
        else{
            if(board.isFull())over(true);
            currentPlayer = (currentPlayer == player1) ? player2 : player1;
        }
    }
}

bool Game::isGameOver() const {
    return gameOver;
}

char Game::getPiece(int x,int y)const{
    return board.getPiece(x,y);
}

bool Game::placePiece(int x,int y){
    return board.placePiece(x,y,currentPlayer->getPiece());
}

void Game::initial(){
    board.clearBoard();
    delete player1;
    player1=new HumanPlayer(&board,'X');
    delete player2;
    player2=new AIPlayer(&board,'O');
    gameOver=false;
}

bool Game::isHuman()const{
    return dynamic_cast<HumanPlayer*>(currentPlayer);
}

char Game::getCurrentPiece()const{
    return currentPlayer->getPiece();
}

void Game::over(bool isTie){
    gameOver=true;
    if(isTie)MessageBoxW(NULL,L"平局",L"平局",MB_OK | MB_ICONINFORMATION);
    else if(isHuman())MessageBoxW(NULL,L"人类胜利",L"胜利",MB_OK | MB_ICONINFORMATION);
    else MessageBoxW(NULL,L"AI胜利",L"失败",MB_OK | MB_ICONINFORMATION);
}

void Game::refresh(){
    InvalidateRect(hwnd, NULL, TRUE);
}

void Game::sethwnd(HWND hwnd){
    this->hwnd=hwnd;
}