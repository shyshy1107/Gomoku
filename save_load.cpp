#include "save_load.h"
#include <iostream>
#include <fstream>
#include <sstream>

SaveLoad::SaveLoad(Board* board) : board(board) {}

void SaveLoad::saveToFile(const std::string& filename) {
    std::ofstream outFile(filename);

    if (outFile.is_open()) {
        for (int i = 0; i < board->getSize(); ++i) {
            for (int j = 0; j < board->getSize(); ++j) {
                outFile << board->getPiece(i, j) << " ";
            }
            outFile << std::endl;
        }
        outFile.close();
        std::cout << "Game saved to " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file for saving!" << std::endl;
    }
}

void SaveLoad::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);

    if (inFile.is_open()) {
        for (int i = 0; i < board->getSize(); ++i) {
            for (int j = 0; j < board->getSize(); ++j) {
                char piece;
                inFile >> piece;
                board->placePiece(i, j, piece);
            }
        }
        inFile.close();
        std::cout << "Game loaded from " << filename << std::endl;
    } else {
        std::cerr << "Unable to open file for loading!" << std::endl;
    }
}
