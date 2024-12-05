#ifndef SAVE_LOAD_H
#define SAVE_LOAD_H

#include "board.h"
#include <fstream>
#include <string>

class SaveLoad {
public:
    SaveLoad(Board* board);
    
    // 保存棋盘状态到文件
    void saveToFile(const std::string& filename);
    
    // 从文件加载棋盘状态
    void loadFromFile(const std::string& filename);

private:
    Board* board;
};

#endif // SAVE_LOAD_H