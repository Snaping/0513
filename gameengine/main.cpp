#include "GameApp.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    GameApp app;

    if (!app.Initialize()) {
        return 1;
    }

    app.Run();
    app.Shutdown();

    return 0;
}
