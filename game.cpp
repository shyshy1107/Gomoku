#include "game.h"
#include <iostream>
#include <windows.h>
#include <fstream>
#include "jsoncpp/json.h"

Game::Game(int boardSize) : board(boardSize), gameOver(false) {
    initial();
}

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == player1) ? player2 : player1;
    if(!isGameOver()&&!isHuman()){
        currentPlayer->makeMove();
        refresh();
        if(board.checkWin(currentPlayer->getPiece()))over(false);
        else{
            if(board.isFull())over(true);
        }
        switchPlayer();
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
    std::ifstream inFile("config.json");
    if(!inFile){
        player1=new HumanPlayer(&board,'X');
        player2=new AIPlayer(&board,'O');
    }
    else{
        Json::Value settings;
        Json::Reader reader;
        reader.parse(inFile, settings);
        if(settings.get("Player1",1).asBool())player1=new HumanPlayer(&board,'X');
        else player1=new AIPlayer(&board,'X');
        if(!settings.get("Player2",0).asBool())player2=new AIPlayer(&board,'O');
        else player2=new HumanPlayer(&board,'O');
    }
    inFile.close();
    currentPlayer=player1;
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
    else if(getCurrentPiece()=='X')MessageBoxW(NULL,L"黑棋胜利",L"结束",MB_OK | MB_ICONINFORMATION);
    else MessageBoxW(NULL,L"白棋胜利",L"结束",MB_OK | MB_ICONINFORMATION);
}

void Game::refresh(){
    InvalidateRect(hwnd, NULL, TRUE);
}

void Game::sethwnd(HWND hwnd){
    this->hwnd=hwnd;
}

bool Game::hq(){
    if(board.hq()){
        gameOver=false;
        if(dynamic_cast<HumanPlayer*>((currentPlayer == player1) ? player2 : player1))switchPlayer();
        else{
            if(board.hq())return true;
            return false;
        }
        return true;
    }
    return false;
}

void Game::xq(int x,int y,char piece){
    if(piece!='.')board.placePiece(x,y,piece);
    else board.grid[x][y]='.';
}

bool Game::checkWin(char piece){
    return board.checkWin(piece);
}

bool Game::isFull()const{
    return board.isFull();
}

bool Game::checkjs(int x,int y,int type){
    return board.checkjs(x,y,type);
}