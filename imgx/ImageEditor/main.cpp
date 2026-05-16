#include "ImageEditor.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ImageEditor editor;
    if (!editor.RegisterClass(hInstance))
    {
        MessageBoxW(nullptr, L"窗口类注册失败！", L"错误", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    if (!editor.InitInstance(hInstance, nCmdShow))
    {
        MessageBoxW(nullptr, L"窗口初始化失败！", L"错误", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}
