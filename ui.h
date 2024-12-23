    #ifndef UI_H
#define UI_H

#include <windows.h>
#include <string>
#include "game.h"
#include <vector>
#include <gdiplus.h>

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

class Tool{
public:
    int pos[2][2];
    bool onTool(int x,int y)const;
    virtual void onClick(int x,int y,Game& game,HWND hwnd)=0;
};

class Button:public Tool{
public:
    std::wstring image[2];
    bool isFocused;
    void onClick(int x,int y,Game& game,HWND hwnd)override;
};

class BOARD:public Tool{
public:
    BOARD(const int arr[2][2]);
    void onClick(int x,int y,Game& game,HWND hwnd)override;
};

class UI {
public:
    UI(HINSTANCE hInstance,Game& game);
    ~UI();
    std::vector<Tool*> tools;
    void createWindow();  // 创建窗口
    void runMessageLoop();  // 运行消息循环

private:
    HINSTANCE hInstance;
    HWND hwnd;
    Game& game;
    void DrawBoard(HDC hdc, int startX, int startY, int endX, int endY);
    const wchar_t* CLASS_NAME = L"GomokuWindowClass";
    const static int BOARD_SIZE = 15;  // 棋盘大小
    const static int CELL_SIZE = 40;   // 每个格子的大小
    void save1(HWND hwnd);
    // 窗口过程回调函数
    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
};
#endif  // UI_H