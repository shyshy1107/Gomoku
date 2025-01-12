#include <windows.h>
#include <fstream>
#include <vector>
#include <iostream>
#include <gdiplus.h>
#include <iomanip>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "board.h"
#include "game.h"
#include "player.h"
#include "ui.h"

#pragma comment(lib, "gdiplus.lib")

using namespace Gdiplus;

ULONG_PTR gdiplusToken;
// 构造函数：初始化实例句柄
UI::UI(HINSTANCE hInstance,Game& game) : hInstance(hInstance), game(game), hwnd(nullptr) {}

BOARD::BOARD(const int arr[2][2]){
    for(int i=0;i<2;i++)for(int j=0;j<2;j++)pos[i][j]=arr[i][j];
}

Button::Button(const int arr[2][2],std::wstring Image,int ID):id(ID){
    for(int i=0;i<2;i++)for(int j=0;j<2;j++)pos[i][j]=arr[i][j];
    image[0]=L"ui\\onfocus2.png";
    image[1]=Image;
}

UI::~UI() {
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
        CW_USEDEFAULT, CW_USEDEFAULT, 1000, 750,
        NULL, NULL, hInstance, this);

    if (hwnd == NULL) {
        return;
    }
    int arr[2][2]={{40,40},{680,680}};
    tools.push_back(new BOARD(arr));
    arr[0][0]=720,arr[0][1]=620,arr[1][0]=960,arr[1][1]=660;
    tools.push_back(new Button(arr,L"ui\\save.png",1));
    arr[0][0]=720,arr[0][1]=577,arr[1][0]=960,arr[1][1]=617;
    tools.push_back(new Button(arr,L"ui\\load.png",2));
    arr[0][0]=720,arr[0][1]=534,arr[1][0]=960,arr[1][1]=574;
    tools.push_back(new Button(arr,L"ui\\newgame.png",3));
    arr[0][0]=720,arr[0][1]=491,arr[1][0]=960,arr[1][1]=531;
    tools.push_back(new Button(arr,L"ui\\retract.png",4));
    ShowWindow(hwnd, SW_SHOWNORMAL);
    UpdateWindow(hwnd);

    SDL_Init(SDL_INIT_AUDIO);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);
    Mix_Music *music = Mix_LoadMUS("music\\bgm.mp3");
    Mix_PlayMusic(music, -1);
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
            // 初始化 GDI+
            GdiplusStartupInput gdiPlusStartupInput;
            GdiplusStartup(&gdiplusToken, &gdiPlusStartupInput, NULL);
            CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
            ui = reinterpret_cast<UI*>(pCreate->lpCreateParams);
            SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(ui));
            if(!ui->game.isHuman()){
                ui->game.currentPlayer->makeMove();
                ui->game.switchPlayer();
            }
            /*HMENU hMenu = CreateMenu();
            AppendMenuW(hMenu, MF_STRING, 1, L"存档");
            AppendMenuW(hMenu, MF_STRING, 2, L"读档");
            AppendMenuW(hMenu, MF_STRING, 3, L"重新开始");
            AppendMenuW(hMenu, MF_STRING, 4, L"悔棋");
            SetMenu(hwnd, hMenu);*/
        }
        return 0;

    case WM_MOUSEMOVE: {
        // 获取鼠标的 X 和 Y 坐标
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标
        for(int i=0;i<ui->tools.size();i++){
            Tool* temp=ui->tools[i];
            if(temp->onTool(LOWORD(lParam),HIWORD(lParam))){
                temp->onFocus(LOWORD(lParam),HIWORD(lParam),ui->game,hwnd);
                return 0;
            }
            else{
                if(temp->isFocused){
                    temp->isFocused=false;
                    InvalidateRect(hwnd,NULL,TRUE);
                }
            }
        }
        return 0;
    }
    /*case WM_RBUTTONDOWN:{
        x = LOWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 x 坐标
        y = HIWORD(lParam) / CELL_SIZE; // 获取鼠标点击的 y 坐标

        if (x < BOARD_SIZE && y < BOARD_SIZE &&ui->game.isHuman()) {
            MessageBoxW(hwnd, std::to_wstring(dynamic_cast<AIPlayer*>(ui->game.player2)->score[x][y]).c_str(), L"查分", MB_OK);
        }
        return 0;
    }*/

    case WM_LBUTTONDOWN: {
        // 处理鼠标点击
        for(int i=0;i<ui->tools.size();i++){
            Tool* temp=ui->tools[i];
            if(temp->onTool(LOWORD(lParam),HIWORD(lParam))){
                temp->onClick(LOWORD(lParam),HIWORD(lParam),ui->game,hwnd);
                return 0;
            }
        }
    }

    case WM_PAINT: {
        // 绘制棋盘
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        HDC hMemDC = CreateCompatibleDC(hdc);//创建一个内存DC
        HBITMAP hBitmap = CreateCompatibleBitmap(hdc, 1000, 750);
        SelectObject(hMemDC, hBitmap);
        Gdiplus::Graphics graphics(hMemDC);//在内存DC中绘制
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBRUSH hBrush = CreateSolidBrush(RGB(220, 220, 220)); // 重绘背景
        FillRect(hMemDC, &rect, hBrush);
        DeleteObject(hBrush); 
        std::wstring filePath = L"ui\\bg3.png";
        Gdiplus::Image* image = new Gdiplus::Image(filePath.c_str());
        graphics.DrawImage(image, 0, 0, 1000, 750);
        delete image; 
        filePath = L"ui\\bg2.png";
        image = new Gdiplus::Image(filePath.c_str());
        graphics.DrawImage(image, 0, 0, 1000, 750);
        delete image; 
        ui->DrawBoard(hMemDC,ui->tools[0]->pos[0][0],ui->tools[0]->pos[0][1],ui->tools[0]->pos[1][0],ui->tools[0]->pos[1][1]);
        for(int i=1;i<ui->tools.size();i++){
            Tool* temp=ui->tools[i];
            Button* button= dynamic_cast<Button*>(temp);
            Gdiplus::Image* buttoni=new Gdiplus::Image(button->image[1].c_str());
            if(button->isFocused){
                Gdiplus::Image* focus=new Gdiplus::Image(button->image[0].c_str());
                graphics.DrawImage(focus,ui->tools[i]->pos[0][0],ui->tools[i]->pos[0][1],ui->tools[i]->pos[1][0]-ui->tools[i]->pos[0][0],ui->tools[i]->pos[1][1]-ui->tools[i]->pos[0][1]);
                delete focus;
            }
            graphics.DrawImage(buttoni,ui->tools[i]->pos[0][0],ui->tools[i]->pos[0][1],ui->tools[i]->pos[1][0]-ui->tools[i]->pos[0][0],ui->tools[i]->pos[1][1]-ui->tools[i]->pos[0][1]);
            delete buttoni;
        }
        BITMAP bm;
        GetObject(hBitmap, sizeof(bm), &bm);  // 获取位图信息

        // 使用 BitBlt 将内存中的内容复制到窗口
        BitBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
        EndPaint(hwnd, &ps); 

        return 0;
    }

    case WM_DESTROY:
        if(!ui->game.isGameOver())ui->save1(hwnd);
        // 清理 GDI+
        GdiplusShutdown(gdiplusToken);
        PostQuitMessage(0);
        Mix_HaltMusic();
        Mix_CloseAudio();
        SDL_Quit();
        return 0;

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

void UI::DrawBoard(HDC hdc, int startX, int startY, int endX, int endY) {
    int gridSize = 14;  // 五子棋的棋盘大小是 15x15
    int cellWidth = (endX - startX) / gridSize;  // 计算每个格子的宽度
    int cellHeight = (endY - startY) / gridSize; // 计算每个格子的高度

    // 绘制棋盘网格
    for (int i = 0; i <=gridSize; ++i) {
        for (int j = 0; j <=gridSize; ++j) {
            // 计算当前格子的左上角和右下角坐标
            int x1 = startX + i * cellWidth;
            int y1 = startY + j * cellHeight;
            int x2 = x1 + cellWidth;
            int y2 = y1 + cellHeight;
            // 绘制格子（用黑色绘制格线）
            if(i<gridSize&&j<gridSize){
                HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 0, 0));  // 黑色
                HGDIOBJ oldPen = SelectObject(hdc, hPen);
                MoveToEx(hdc, x1, y1, NULL);  // 起始点
                LineTo(hdc, x1, y2);  // 竖线
                LineTo(hdc, x2, y2);  // 横线
                LineTo(hdc, x2, y1);  // 竖线
                LineTo(hdc, x1, y1);  // 关闭矩形
                SelectObject(hdc, oldPen);
                DeleteObject(hPen);
            }
            if(i==game.board.lastX&&j==game.board.lastY){
                HPEN hPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));  // 红色
                HGDIOBJ oldPen = SelectObject(hdc, hPen);
                MoveToEx(hdc, x1-cellWidth/3, y1-cellHeight/3, NULL);  // 起始点
                LineTo(hdc, x1-cellWidth/3, y1+cellHeight/3);  // 竖线
                LineTo(hdc, x1+cellWidth/3, y1+cellHeight/3);  // 横线
                LineTo(hdc, x1+cellWidth/3, y1-cellHeight/3);  // 竖线
                LineTo(hdc, x1-cellWidth/3, y1-cellHeight/3);  // 关闭矩形
                SelectObject(hdc, oldPen);
                DeleteObject(hPen);
            }
            // 获取当前格子的棋子（假设 game.getPiece(i, j) 返回 'X', 'O' 或 '.'）
            char piece = game.getPiece(i, j);
            if (piece == 'X') {
                // 绘制黑色棋子（实心圆）
                HBRUSH hBrush = CreateSolidBrush(RGB(0, 0, 0));  // 黑色
                SelectObject(hdc, hBrush);

                // 确保棋子绘制在格点交点上
                int centerX = x1;
                int centerY = y1;
                int radius = std::min(cellWidth, cellHeight) / 3;  // 半径为格子宽度或高度的三分之一

                // 绘制实心圆（黑棋）
                Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
                DeleteObject(hBrush);  // 释放画刷资源
            } else if (piece == 'O') {
                // 绘制白色棋子（实心圆）
                HBRUSH hBrush = CreateSolidBrush(RGB(255, 255, 255));  // 白色
                SelectObject(hdc, hBrush);

                // 确保棋子绘制在格点交点上
                int centerX = x1;
                int centerY = y1;
                int radius = std::min(cellWidth, cellHeight) / 3;  // 半径为格子宽度或高度的三分之一

                // 绘制实心圆（白棋）
                Ellipse(hdc, centerX - radius, centerY - radius, centerX + radius, centerY + radius);
                DeleteObject(hBrush);  // 释放画刷资源
            }
        }
    }
}

void BOARD::onClick(int x,int y,Game& game,HWND hwnd){
    
    int startX=pos[0][0],startY=pos[0][1],endX=pos[1][0],endY=pos[1][1];
    int gridSize = 14;  // 五子棋的棋盘大小是 15x15
    int cellWidth = (endX - startX) / gridSize;  // 计算每个格子的宽度
    int cellHeight = (endY - startY) / gridSize; // 计算每个格子的高度
    for (int i = 0; i <=gridSize; ++i) {
        for (int j = 0; j <=gridSize; ++j) {
            // 计算当前格子的左上角和右下角坐标
            int x1 = startX + i * cellWidth;
            int y1 = startY + j * cellHeight;
            if(abs(x-x1)<=(cellWidth/2)&&abs(y-y1)<=(cellHeight/2)&&!game.isGameOver() &&game.isHuman()){
                if(game.placePiece(i,j)){// 请求重绘窗口
                    Mix_Chunk *music = Mix_LoadWAV("music\\xq.mp3");
                    Mix_Volume(Mix_PlayChannel(2, music, 0),100);
                    SDL_Delay(500);
                    Mix_FreeChunk(music);
                    InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
                    if(game.checkWin(game.getCurrentPiece()))game.over(false);
                    else{
                        if(game.isFull())game.over(true);
                    }
                    game.switchPlayer();
                }
                else{
                    if(game.checkjs(i,j,2))MessageBoxW(hwnd, L"这步棋是长连禁手!", L"禁手", MB_OK);
                    else if(game.checkjs(i,j,1))MessageBoxW(hwnd, L"这步棋是三三/四四禁手!", L"禁手", MB_OK);
            }
            }
        }
    }
}

void Button::onClick(int x,int y,Game& game,HWND hwnd){
    switch (id)
    {
        case 1: // 存盘
            {
                std::ofstream outFile("board.txt");
                for (int i = 0; i < 15; ++i) {
                    for (int j = 0; j < 15; ++j) {
                        outFile<<game.getPiece(j, i);
                    }
                    outFile << "\n";
                }
                outFile.close();
                MessageBoxW(hwnd, L"存档成功", L"存档", MB_OK);
            }
            break;
        case 2: // 读盘
            {
                std::ifstream inFile("board.txt");
                if (!inFile) {
                    MessageBoxW(hwnd, L"读档失败!", L"读档", MB_OK);
                }
                for (int i = 0; i < 15; ++i) {
                    for (int j = 0; j < 15; ++j) {
                        char piece;
                        inFile >> piece;
                        game.xq(j,i,piece);
                    }
                }
                inFile.close();
                InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
            }
            break;
        case 3: // 重新开始
            // 清空棋盘，重新开始
            game.initial();
            InvalidateRect(hwnd, NULL, TRUE);
            if(!game.isHuman()){
                game.currentPlayer->makeMove();
                game.switchPlayer();
            } // 重新绘制
            break;
        case 4: // 悔棋
            {
                if (!game.hq()) {
                    MessageBoxW(hwnd, L"还没有下棋", L"悔棋", MB_OK);
                }
                InvalidateRect(hwnd, NULL, TRUE); // 重新绘制
            }
            break;
    
        default:
            break;
    }
}

bool Tool::onTool(int x,int y)const{
    if(x<=pos[1][0]&&x>=pos[0][0]&&y<=pos[1][1]&&y>=pos[0][1])return true;
    return false;
}

void BOARD::onFocus(int x,int y,Game& game,HWND hwnd){}

void Button::onFocus(int x,int y,Game& game,HWND hwnd){
    if(!isFocused){
        isFocused=true;
        InvalidateRect(hwnd,NULL,TRUE);
    }
}