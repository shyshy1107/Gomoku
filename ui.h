    #ifndef UI_H
#define UI_H

#include <windows.h>
#include "game.h"

class UI {
public:
    UI(HINSTANCE hInstance,Game& game);
    ~UI();
    void createWindow();  // 创建窗口
    void runMessageLoop();  // 运行消息循环

private:
    HINSTANCE hInstance;
    HWND hwnd;
    Game& game;
    void DrawBoard(HDC hdc);
    const wchar_t* CLASS_NAME = L"GomokuWindowClass";
    const static int BOARD_SIZE = 15;  // 棋盘大小
    const static int CELL_SIZE = 40;   // 每个格子的大小
    void save1(HWND hwnd);
    // 窗口过程回调函数
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif  // UI_H