#include <windows.h>
#include <gdiplus.h>
#include "game.h"
#include "ui.h"
#pragma comment(lib, "gdiplus.lib")
using namespace Gdiplus;

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    Game game(15);
    UI ui(hInstance,game);
    ui.createWindow();
    ui.runMessageLoop();
}