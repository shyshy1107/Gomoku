#include <windows.h>
#include "game.h"
#include "ui.h"
int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    Game game(15);
    UI ui(hInstance,game);
    ui.createWindow();
    ui.runMessageLoop();
}