#pragma once
#include <windows.h>
#include "Vector2.h"
#include "Color.h"
#include "Rect.h"

class Window;

class Renderer {
public:
    Renderer();
    Renderer(Window* window);
    ~Renderer();

    bool Initialize(HWND hwnd);
    void Shutdown();

    void BeginFrame();
    void EndFrame();

    void Clear(const Color& color);
    void DrawPixel(int x, int y, const Color& color);
    void DrawLine(int x1, int y1, int x2, int y2, const Color& color);
    void DrawRect(const Rect& rect, const Color& color);
    void FillRect(const Rect& rect, const Color& color);
    void DrawCircle(int cx, int cy, int radius, const Color& color);
    void FillCircle(int cx, int cy, int radius, const Color& color);
    void DrawText(int x, int y, const wchar_t* text, const Color& color);

    void SetViewport(int width, int height);

private:
    HWND m_hwnd;
    HDC m_hdc;
    HBITMAP m_bitmap;
    HBITMAP m_oldBitmap;
    unsigned char* m_buffer;
    int m_width;
    int m_height;
    int m_pitch;

    void SetPixelInternal(int x, int y, const Color& color);
};
