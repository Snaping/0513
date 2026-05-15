#define NOMINMAX
#include "Renderer.h"
#include "Window.h"
#include <algorithm>

Renderer::Renderer()
    : m_hwnd(nullptr), m_hdc(nullptr), m_bitmap(nullptr), m_oldBitmap(nullptr),
      m_buffer(nullptr), m_width(0), m_height(0), m_pitch(0) {}

Renderer::Renderer(Window* window)
    : m_hwnd(nullptr), m_hdc(nullptr), m_bitmap(nullptr), m_oldBitmap(nullptr),
      m_buffer(nullptr), m_width(0), m_height(0), m_pitch(0) {}

Renderer::~Renderer() {
    Shutdown();
}

bool Renderer::Initialize(HWND hwnd) {
    m_hwnd = hwnd;

    RECT rect;
    GetClientRect(hwnd, &rect);
    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

    HDC hdcWindow = GetDC(hwnd);
    m_hdc = CreateCompatibleDC(hdcWindow);

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = m_width;
    bmi.bmiHeader.biHeight = -m_height;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    m_bitmap = CreateDIBSection(m_hdc, &bmi, DIB_RGB_COLORS, (void**)&m_buffer, nullptr, 0);
    if (!m_bitmap) {
        ReleaseDC(hwnd, hdcWindow);
        DeleteDC(m_hdc);
        return false;
    }

    m_oldBitmap = (HBITMAP)SelectObject(m_hdc, m_bitmap);
    m_pitch = m_width * 4;

    ReleaseDC(hwnd, hdcWindow);
    return true;
}

void Renderer::Shutdown() {
    if (m_hdc && m_oldBitmap) {
        SelectObject(m_hdc, m_oldBitmap);
    }
    if (m_bitmap) {
        DeleteObject(m_bitmap);
        m_bitmap = nullptr;
    }
    if (m_hdc) {
        DeleteDC(m_hdc);
        m_hdc = nullptr;
    }
    m_buffer = nullptr;
}

void Renderer::BeginFrame() {
    Clear(Color::Black());
}

void Renderer::EndFrame() {
    if (m_hwnd && m_hdc) {
        HDC hdcWindow = GetDC(m_hwnd);
        BitBlt(hdcWindow, 0, 0, m_width, m_height, m_hdc, 0, 0, SRCCOPY);
        ReleaseDC(m_hwnd, hdcWindow);
    }
}

void Renderer::Clear(const Color& color) {
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            SetPixelInternal(x, y, color);
        }
    }
}

void Renderer::SetPixelInternal(int x, int y, const Color& color) {
    if (x >= 0 && x < m_width && y >= 0 && y < m_height) {
        int index = y * m_pitch + x * 4;
        m_buffer[index + 0] = color.b;
        m_buffer[index + 1] = color.g;
        m_buffer[index + 2] = color.r;
        m_buffer[index + 3] = color.a;
    }
}

void Renderer::DrawPixel(int x, int y, const Color& color) {
    SetPixelInternal(x, y, color);
}

void Renderer::DrawLine(int x1, int y1, int x2, int y2, const Color& color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = x1 < x2 ? 1 : -1;
    int sy = y1 < y2 ? 1 : -1;
    int err = dx - dy;

    while (true) {
        SetPixelInternal(x1, y1, color);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
}

void Renderer::DrawRect(const Rect& rect, const Color& color) {
    int x = (int)rect.x;
    int y = (int)rect.y;
    int w = (int)rect.width;
    int h = (int)rect.height;

    DrawLine(x, y, x + w, y, color);
    DrawLine(x + w, y, x + w, y + h, color);
    DrawLine(x + w, y + h, x, y + h, color);
    DrawLine(x, y + h, x, y, color);
}

void Renderer::FillRect(const Rect& rect, const Color& color) {
    int startX = std::max(0, (int)rect.x);
    int endX = std::min(m_width, (int)(rect.x + rect.width));
    int startY = std::max(0, (int)rect.y);
    int endY = std::min(m_height, (int)(rect.y + rect.height));

    for (int y = startY; y < endY; y++) {
        for (int x = startX; x < endX; x++) {
            SetPixelInternal(x, y, color);
        }
    }
}

void Renderer::DrawCircle(int cx, int cy, int radius, const Color& color) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y) {
        SetPixelInternal(cx + x, cy + y, color);
        SetPixelInternal(cx + y, cy + x, color);
        SetPixelInternal(cx - y, cy + x, color);
        SetPixelInternal(cx - x, cy + y, color);
        SetPixelInternal(cx - x, cy - y, color);
        SetPixelInternal(cx - y, cy - x, color);
        SetPixelInternal(cx + y, cy - x, color);
        SetPixelInternal(cx + x, cy - y, color);

        if (err <= 0) {
            y++;
            err += 2 * y + 1;
        } else {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void Renderer::FillCircle(int cx, int cy, int radius, const Color& color) {
    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            if (x * x + y * y <= radius * radius) {
                SetPixelInternal(cx + x, cy + y, color);
            }
        }
    }
}

void Renderer::DrawText(int x, int y, const wchar_t* text, const Color& color) {
    HDC hdcWindow = GetDC(m_hwnd);
    HDC hdcText = CreateCompatibleDC(hdcWindow);

    HBITMAP hBitmapText = CreateCompatibleBitmap(hdcWindow, 400, 50);
    HBITMAP hOldBitmapText = (HBITMAP)SelectObject(hdcText, hBitmapText);

    SetBkMode(hdcText, TRANSPARENT);
    SetTextColor(hdcText, RGB(color.r, color.g, color.b));

    RECT textRect = {0, 0, 400, 50};
    ::DrawTextW(hdcText, text, -1, &textRect, DT_LEFT | DT_TOP);

    for (int ty = 0; ty < 50; ty++) {
        for (int tx = 0; tx < 400; tx++) {
            COLORREF pixel = GetPixel(hdcText, tx, ty);
            if (pixel != RGB(0, 0, 0)) {
                SetPixelInternal(x + tx, y + ty, color);
            }
        }
    }

    SelectObject(hdcText, hOldBitmapText);
    DeleteObject(hBitmapText);
    DeleteDC(hdcText);
    ReleaseDC(m_hwnd, hdcWindow);
}

void Renderer::SetViewport(int width, int height) {
    m_width = width;
    m_height = height;
}
