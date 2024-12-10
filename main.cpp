#include <windows.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include "board.h"
#include "game.h"
#include "player.h"

const int BOARD_SIZE = 15;  // 棋盘大小
const int CELL_SIZE = 40;   // 每个格子的大小
Game game(15);

// 绘制棋盘
void DrawBoard(HDC hdc) {
    // 绘制棋盘网格
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            // 绘制格子
            Rectangle(hdc, i * CELL_SIZE, j * CELL_SIZE, (i + 1) * CELL_SIZE, (j + 1) * CELL_SIZE);
            if (game.getPiece(i, j) == 'X') {
                MoveToEx(hdc, i * CELL_SIZE + 5, j * CELL_SIZE + 5, NULL);
                LineTo(hdc, (i + 1) * CELL_SIZE - 5, (j + 1) * CELL_SIZE - 5);
                MoveToEx(hdc, (i + 1) * CELL_SIZE - 5, j * CELL_SIZE + 5, NULL);
                LineTo(hdc, i * CELL_SIZE + 5, (j + 1) * CELL_SIZE - 5);
            } else if (game.getPiece(i, j) == 'O') {
                Ellipse(hdc, i * CELL_SIZE + 5, j * CELL_SIZE + 5, (i + 1) * CELL_SIZE - 5, (j + 1) * CELL_SIZE - 5);
            }
        }
    }
}

// 菜单命令处理
void ProcessMenuCommand(HWND hwnd, WPARAM wParam) {
    switch (LOWORD(wParam)) {
    case 1: // 存盘
        {
            std::ofstream outFile("board.txt");
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    outFile<<std::setw(7) << game.getPiece(j, i) << " ";
                }
                outFile << "\n";
            }
            outFile << "\n";
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    outFile<<std::setw(12) << int(dynamic_cast<AIPlayer*>(game.player2)->score[j][i]) << " ";
                }
                outFile << "\n\n";
            }
            outFile.close();
            MessageBoxW(hwnd, L"Game saved!", L"Save", MB_OK);
        }
        break;
    case 2: // 读盘
        {
            std::ifstream inFile("board.txt");
            if (!inFile) {
                MessageBoxW(hwnd, L"Failed to load the game!", L"Load", MB_OK);
                return;
            }
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    char piece;
                    inFile >> piece;
                    game.board.placePiece(j,i,piece);
                }
            }
            inFile.close();
            InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
        }
        break;
    case 3: // 重新开始
        // 清空棋盘，重新开始
        game.initial();
        InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
        break;
    case 4: // 退出
        PostMessage(hwnd, WM_CLOSE, 0, 0);
        break;
    }
}

// 窗口过程函数
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int x, y;

    switch (uMsg) {
    case WM_CREATE:
        // 创建菜单
        {
            HMENU hMenu = CreateMenu();
            AppendMenuW(hMenu, MF_STRING, 1, L"Save Game");
            AppendMenuW(hMenu, MF_STRING, 2, L"Load Game");
            AppendMenuW(hMenu, MF_STRING, 3, L"Restart Game");
            AppendMenuW(hMenu, MF_STRING, 4, L"Exit");
            SetMenu(hwnd, hMenu);
        }
        return 0;

    case WM_COMMAND:
        ProcessMenuCommand(hwnd, wParam);
        return 0;

    case WM_RBUTTONDOWN:{
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标

        if (x < BOARD_SIZE && y < BOARD_SIZE &&game.isHuman()) {
            MessageBoxW(hwnd, std::to_wstring(dynamic_cast<AIPlayer*>(game.player2)->score[x][y]).c_str(), L"查分", MB_OK);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        // 处理鼠标点击
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标

        if (!game.isGameOver()&& x < BOARD_SIZE && y < BOARD_SIZE && game.getPiece(x, y) == '.'&&game.isHuman()) {
            if(game.placePiece(x,y)){
                InvalidateRect(hwnd, NULL, TRUE); // 请求重绘窗口
                if(game.board.checkWin(game.getCurrentPiece()))game.over(false);
                else{
                    if(game.board.isFull())game.over(true); 
                    else game.switchPlayer();
                }
            }
            else{
                if(game.board.checkjs(x,y,2))MessageBoxW(hwnd, L"这步棋是长连禁手!", L"禁手", MB_OK);
                else if(game.board.checkjs(x,y,1))MessageBoxW(hwnd, L"这步棋是三三/四四禁手!", L"禁手", MB_OK);
            }
        }
        return 0;
    }

    case WM_PAINT: {
        // 绘制棋盘
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawBoard(hdc);
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if(!game.isGameOver()){
            std::ofstream outFile("board.txt");
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    outFile << game.getPiece(i, j) << " ";
                }
                outFile << "\n";
            }
            for (int i = 0; i < BOARD_SIZE; ++i) {
                for (int j = 0; j < BOARD_SIZE; ++j) {
                    outFile << game.board.cnt[i][j].CL << " ";
                }
                outFile << "\n";
            }
            outFile.close();
            MessageBoxW(hwnd, L"Game saved!", L"Save", MB_OK);
        }
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const wchar_t CLASS_NAME[] = L"GomokuWindowClass";

    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);  // 结构体的大小
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"Gomoku", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CELL_SIZE * BOARD_SIZE+50, CELL_SIZE * BOARD_SIZE+80,
        NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);

    // 事件循环
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
