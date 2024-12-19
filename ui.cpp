#include <windows.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <iomanip>
#include "board.h"
#include "game.h"
#include "player.h"
#include "ui.h"

// 构造函数：初始化实例句柄
UI::UI(HINSTANCE hInstance,Game& game) : hInstance(hInstance), game(game), hwnd(nullptr) {}

// 析构函数：清理资源
UI::~UI() {
    // 可以在此处清理资源（如果有的话）
}

// 创建窗口
void UI::createWindow() {
    // 注册窗口类
    WNDCLASSEXW wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);  // 结构体的大小
    wc.lpfnWndProc = windowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassExW(&wc);

    HWND hwnd = CreateWindowExW(
        0, CLASS_NAME, L"五子棋", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, CELL_SIZE * BOARD_SIZE+50, CELL_SIZE * BOARD_SIZE+80,
        NULL, NULL, hInstance, this);

    if (hwnd == NULL) {
        return;
    }

    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);
}

// 运行消息循环
void UI::runMessageLoop() {
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void UI::save1(HWND hwnd){
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
            MessageBoxW(hwnd, L"存档成功", L"存档", MB_OK);
}

// 窗口过程回调函数
LRESULT CALLBACK UI::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static int x, y;
    UI* ui = reinterpret_cast<UI*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    switch (uMsg) {
    case WM_CREATE:
        // 创建菜单
        {
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            ui = reinterpret_cast<UI*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ui));
            HMENU hMenu = CreateMenu();
            AppendMenuW(hMenu, MF_STRING, 1, L"存档");
            AppendMenuW(hMenu, MF_STRING, 2, L"读档");
            AppendMenuW(hMenu, MF_STRING, 3, L"重新开始");
            AppendMenuW(hMenu, MF_STRING, 4, L"悔棋");
            SetMenu(hwnd, hMenu);
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
            case 1: // 存盘
                {
                    ui->save1(hwnd);
                }
                break;
            case 2: // 读盘
                {
                    std::ifstream inFile("board.txt");
                    if (!inFile) {
                        MessageBoxW(hwnd, L"读档失败!", L"Load", MB_OK);
                    }
                    for (int i = 0; i < BOARD_SIZE; ++i) {
                        for (int j = 0; j < BOARD_SIZE; ++j) {
                            char piece;
                            inFile >> piece;
                            ui->game.xq(j,i,piece);
                        }
                    }
                    inFile.close();
                    InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
                }
                break;
            case 3: // 重新开始
                // 清空棋盘，重新开始
                ui->game.initial();
                InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
                break;
            case 4: // 悔棋
                {
                    if (!ui->game.hq()) {
                        MessageBoxW(hwnd, L"还没有下棋", L"悔棋", MB_OK);
                    }
                    InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
                }
                break;
        }
        return 0;

    case WM_RBUTTONDOWN:{
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标

        if (x < BOARD_SIZE && y < BOARD_SIZE &&ui->game.isHuman()) {
            MessageBoxW(hwnd, std::to_wstring(dynamic_cast<AIPlayer*>(ui->game.player2)->score[x][y]).c_str(), L"查分", MB_OK);
        }
        return 0;
    }

    case WM_LBUTTONDOWN: {
        // 处理鼠标点击
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标

        if (!ui->game.isGameOver()&& x < BOARD_SIZE && y < BOARD_SIZE && ui->game.getPiece(x, y) == '.'&&ui->game.isHuman()) {
            if(ui->game.placePiece(x,y)){
                InvalidateRect(hwnd, NULL, TRUE); // 请求重绘窗口
                if(ui->game.checkWin(ui->game.getCurrentPiece()))ui->game.over(false);
                else{
                    if(ui->game.isFull())ui->game.over(true);
                }
                ui->game.switchPlayer();
            }
            else{
                if(ui->game.checkjs(x,y,2))MessageBoxW(hwnd, L"这步棋是长连禁手!", L"禁手", MB_OK);
                else if(ui->game.checkjs(x,y,1))MessageBoxW(hwnd, L"这步棋是三三/四四禁手!", L"禁手", MB_OK);
            }
        }
        return 0;
    }

    case WM_PAINT: {
        // 绘制棋盘
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        // 绘制棋盘网格
        for (int i = 0; i < BOARD_SIZE; ++i) {
            for (int j = 0; j < BOARD_SIZE; ++j) {
                // 绘制格子
                Rectangle(hdc, i * CELL_SIZE, j * CELL_SIZE, (i + 1) * CELL_SIZE, (j + 1) * CELL_SIZE);
                if (ui->game.getPiece(i, j) == 'X') {
                    MoveToEx(hdc, i * CELL_SIZE + 5, j * CELL_SIZE + 5, NULL);
                    LineTo(hdc, (i + 1) * CELL_SIZE - 5, (j + 1) * CELL_SIZE - 5);
                    MoveToEx(hdc, (i + 1) * CELL_SIZE - 5, j * CELL_SIZE + 5, NULL);
                    LineTo(hdc, i * CELL_SIZE + 5, (j + 1) * CELL_SIZE - 5);
                } else if (ui->game.getPiece(i, j) == 'O') {
                    Ellipse(hdc, i * CELL_SIZE + 5, j * CELL_SIZE + 5, (i + 1) * CELL_SIZE - 5, (j + 1) * CELL_SIZE - 5);
                }
            }
        }
        EndPaint(hwnd, &ps);
        return 0;
    }

    case WM_DESTROY:
        if(!ui->game.isGameOver())ui->save1(hwnd);
        PostQuitMessage(0);
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void UI::DrawBoard(HDC hdc) {
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