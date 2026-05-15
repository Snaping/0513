#pragma once
#include <windows.h>
#include <string>

class Window {
public:
    Window();
    ~Window();

    bool Create(const std::wstring& title, int width, int height);
    void Close();
    void Show();
    void Hide();

    bool ProcessMessages();

    HWND GetHandle() const;
    int GetWidth() const;
    int GetHeight() const;

    void SetTitle(const std::wstring& title);

private:
    static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    HWND m_hwnd;
    int m_width;
    int m_height;
    bool m_running;
};
