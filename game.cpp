#include "game.h"
#include <iostream>

Game::Game(int boardSize) : board(boardSize), gameOver(false) {
    player1 = new HumanPlayer(&board, 'X');
    player2 = new AIPlayer(&board, 'O');
    currentPlayer = player1;
}

void Game::start() {
    while (!gameOver) {
        board.display();
        currentPlayer->makeMove();
        if (board.isFull()) {
            std::cout << "It's a draw!" << std::endl;
            break;
        }
        gameOver = board.checkWin(currentPlayer->getPiece());
        switchPlayer();
    }
}

void Game::switchPlayer() {
    currentPlayer = (currentPlayer == player1) ? player2 : player1;
}

bool Game::isGameOver() const {
    return gameOver;
}
