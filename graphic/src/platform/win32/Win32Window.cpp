#include "platform/Win32Window.h"
#include <thread>
#include <chrono>
#include <cmath>

namespace GEngine {

namespace {
    const wchar_t* WINDOW_CLASS_NAME = L"CADGraphicsEngineWindow";
    Win32Window* g_instance = nullptr;
    
    const int TOOL_COUNT = 6;
    const wchar_t* TOOL_LABELS[TOOL_COUNT] = {
        L"Sel", L"Lin", L"Cir", L"Rec", L"Arc", L"Poly"
    };
}

Win32Window::Win32Window()
    : m_hwnd(nullptr)
    , m_hwndToolbar(nullptr)
    , m_hwndStatusbar(nullptr)
    , m_hdc(nullptr)
    , m_width(1200)
    , m_height(800)
    , m_clientWidth(1200)
    , m_clientHeight(800)
    , m_isRunning(false)
    , m_hoveredTool(-1)
    , m_selectedTool(0)
{
    g_instance = this;
    m_renderer = std::make_shared<GDIRenderer>();
}

Win32Window::~Win32Window() {
    destroy();
    g_instance = nullptr;
}

void Win32Window::registerWindowClass() {
    WNDCLASSEXW wc = {0};
    wc.cbSize = sizeof(WNDCLASSEXW);
    wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(nullptr);
    wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszClassName = WINDOW_CLASS_NAME;
    wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
    
    RegisterClassExW(&wc);
}

void Win32Window::createStatusbar() {
    m_hwndStatusbar = CreateWindowW(
        L"msctls_statusbar32",
        L"CAD Graphics Engine",
        WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP,
        0, 0, 0, 0,
        m_hwnd,
        reinterpret_cast<HMENU>(1),
        GetModuleHandle(nullptr),
        nullptr
    );
    
    if (m_hwndStatusbar) {
        int parts[2] = {200, -1};
        SendMessageW(m_hwndStatusbar, SB_SETPARTS, 2, (LPARAM)parts);
        SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 0, (LPARAM)L"Select Tool");
        SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 1, (LPARAM)L"Shapes: 5");
    }
}

void Win32Window::updateStatusbar(const std::wstring& text) {
    if (m_hwndStatusbar) {
        SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 1, (LPARAM)text.c_str());
    }
}

void Win32Window::setApplication(std::shared_ptr<Application> app) {
    m_application = app;
    if (m_application && m_hwnd) {
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        m_clientWidth = rc.right - rc.left;
        m_clientHeight = rc.bottom - rc.top;
        
        int toolbarWidth = 48;
        int statusHeight = 28;
        int canvasWidth = m_clientWidth - toolbarWidth;
        int canvasHeight = m_clientHeight - statusHeight;
        
        if (m_renderer) {
            m_renderer->setViewport(canvasWidth, canvasHeight);
        }
        m_application->onResize(canvasWidth, canvasHeight);
        
        resizeWindows();
    }
}

void Win32Window::resizeWindows() {
    if (m_hwndStatusbar) {
        int statusHeight = 28;
        RECT rc;
        GetClientRect(m_hwnd, &rc);
        
        int parts[2] = {200, -1};
        SendMessageW(m_hwndStatusbar, SB_SETPARTS, 2, (LPARAM)parts);
        
        SetWindowPos(m_hwndStatusbar, nullptr, 
                     0, rc.bottom - statusHeight, 
                     rc.right - rc.left, statusHeight, 
                     SWP_NOZORDER);
    }
}

void Win32Window::drawToolbar(HDC hdc) {
    RECT rc;
    GetClientRect(m_hwnd, &rc);
    
    int toolbarWidth = 48;
    int toolHeight = 40;
    int startY = 10;
    int statusHeight = 28;
    if (m_hwndStatusbar) {
        statusHeight = 28;
    }
    
    HBRUSH hBgBrush = CreateSolidBrush(RGB(240, 240, 240));
    RECT toolbarRect = {0, 0, toolbarWidth, rc.bottom - statusHeight};
    FillRect(hdc, &toolbarRect, hBgBrush);
    DeleteObject(hBgBrush);
    
    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    MoveToEx(hdc, toolbarWidth, 0, nullptr);
    LineTo(hdc, toolbarWidth, rc.bottom - statusHeight);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);
    
    for (int i = 0; i < TOOL_COUNT; i++) {
        int x = 4;
        int y = startY + i * toolHeight;
        int w = toolbarWidth - 8;
        int h = toolHeight - 4;
        
        RECT btnRect = {x, y, x + w, y + h};
        
        HBRUSH hBrush;
        COLORREF textColor;
        
        if (i == m_selectedTool) {
            hBrush = CreateSolidBrush(RGB(0, 122, 204));
            textColor = RGB(255, 255, 255);
        } else if (i == m_hoveredTool) {
            hBrush = CreateSolidBrush(RGB(220, 235, 252));
            textColor = RGB(0, 0, 0);
        } else {
            hBrush = CreateSolidBrush(RGB(240, 240, 240));
            textColor = RGB(50, 50, 50);
        }
        
        FillRect(hdc, &btnRect, hBrush);
        DeleteObject(hBrush);
        
        if (i == m_selectedTool || i == m_hoveredTool) {
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(0, 122, 204));
            HPEN hOldPen2 = (HPEN)SelectObject(hdc, hPen);
            Rectangle(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom);
            SelectObject(hdc, hOldPen2);
            DeleteObject(hPen);
        }
        
        SetBkMode(hdc, TRANSPARENT);
        SetTextColor(hdc, textColor);
        
        RECT textRect = btnRect;
        DrawTextW(hdc, TOOL_LABELS[i], -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    
    SetBkMode(hdc, OPAQUE);
}

void Win32Window::handleToolbarClick(int x, int y) {
    int toolbarWidth = 48;
    int toolHeight = 40;
    int startY = 10;
    
    if (x < toolbarWidth) {
        int toolIndex = (y - startY) / toolHeight;
        if (toolIndex >= 0 && toolIndex < TOOL_COUNT) {
            m_selectedTool = toolIndex;
            
            ToolType toolTypes[TOOL_COUNT] = {
                ToolType::Select,
                ToolType::Line,
                ToolType::Circle,
                ToolType::Rectangle,
                ToolType::Arc,
                ToolType::Polyline
            };
            
            if (m_application) {
                m_application->setActiveTool(toolTypes[toolIndex]);
            }
            
            const wchar_t* hints[TOOL_COUNT] = {
                L"Select", L"Line", L"Circle", L"Rectangle", L"Arc", L"Polyline"
            };
            if (m_hwndStatusbar) {
                SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 0, (LPARAM)hints[toolIndex]);
            }
            
            InvalidateRect(m_hwnd, nullptr, FALSE);
        }
    }
}

bool Win32Window::create(int width, int height, const std::string& title) {
    m_width = width;
    m_height = height;
    m_title = title;
    
    registerWindowClass();
    
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_WIN95_CLASSES;
    InitCommonControlsEx(&icex);
    
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    int posX = (screenWidth - width) / 2;
    int posY = (screenHeight - height) / 2;
    
    std::wstring wTitle(title.begin(), title.end());
    
    m_hwnd = CreateWindowExW(
        0,
        WINDOW_CLASS_NAME,
        wTitle.c_str(),
        WS_OVERLAPPEDWINDOW,
        posX, posY,
        width, height,
        nullptr, nullptr,
        GetModuleHandle(nullptr),
        this
    );
    
    if (!m_hwnd) {
        return false;
    }
    
    createStatusbar();
    resizeWindows();
    
    m_hdc = GetDC(m_hwnd);
    m_renderer->setHDC(m_hdc);
    
    show();
    m_isRunning = true;
    
    return true;
}

void Win32Window::destroy() {
    if (m_hwnd) {
        if (m_hdc) {
            ReleaseDC(m_hwnd, m_hdc);
            m_hdc = nullptr;
        }
        DestroyWindow(m_hwnd);
        m_hwnd = nullptr;
    }
    m_isRunning = false;
}

void Win32Window::show() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_SHOW);
        UpdateWindow(m_hwnd);
    }
}

void Win32Window::hide() {
    if (m_hwnd) {
        ShowWindow(m_hwnd, SW_HIDE);
    }
}

void Win32Window::setTitle(const std::string& title) {
    m_title = title;
    if (m_hwnd) {
        std::wstring wTitle(title.begin(), title.end());
        SetWindowTextW(m_hwnd, wTitle.c_str());
    }
}

void Win32Window::setSize(int width, int height) {
    m_width = width;
    m_height = height;
    if (m_hwnd) {
        SetWindowPos(m_hwnd, nullptr, 0, 0, width, height,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    }
}

int Win32Window::runMessageLoop() {
    MSG msg;
    m_isRunning = true;
    
    while (m_isRunning) {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                m_isRunning = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (m_application) {
            m_application->onUpdate();
        }
        
        InvalidateRect(m_hwnd, nullptr, FALSE);
        
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }
    
    return static_cast<int>(msg.wParam);
}

void Win32Window::processEvents() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

LRESULT CALLBACK Win32Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_CREATE) {
        CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
    }
    
    Win32Window* self = reinterpret_cast<Win32Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    if (self) {
        return self->handleMessage(msg, wParam, lParam);
    }
    
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT Win32Window::handleMessage(UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_DESTROY:
            m_isRunning = false;
            PostQuitMessage(0);
            return 0;
            
        case WM_SIZE: {
            m_clientWidth = LOWORD(lParam);
            m_clientHeight = HIWORD(lParam);
            resizeWindows();
            
            int toolbarWidth = 48;
            int statusHeight = 28;
            int canvasWidth = m_clientWidth - toolbarWidth;
            int canvasHeight = m_clientHeight - statusHeight;
            
            if (m_renderer) {
                m_renderer->setViewport(canvasWidth, canvasHeight);
            }
            if (m_application) {
                m_application->onResize(canvasWidth, canvasHeight);
            }
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(m_hwnd, &ps);
            
            RECT rc;
            GetClientRect(m_hwnd, &rc);
            
            int toolbarWidth = 48;
            int statusHeight = 28;
            
            drawToolbar(hdc);
            
            HDC memDC = CreateCompatibleDC(hdc);
            int canvasW = rc.right - toolbarWidth;
            int canvasH = rc.bottom - statusHeight;
            
            if (canvasW > 0 && canvasH > 0) {
                HBITMAP hBitmap = CreateCompatibleBitmap(hdc, canvasW, canvasH);
                HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, hBitmap);
                
                HBRUSH hBg = CreateSolidBrush(RGB(245, 245, 245));
                RECT bgRect = {0, 0, canvasW, canvasH};
                FillRect(memDC, &bgRect, hBg);
                DeleteObject(hBg);
                
                if (m_application && m_renderer) {
                    m_renderer->setViewport(canvasW, canvasH);
                    m_renderer->setHDC(memDC);
                    m_application->onRender();
                } else {
                    HPEN hTestPen = CreatePen(PS_SOLID, 2, RGB(255, 0, 0));
                    HPEN hOldTestPen = (HPEN)SelectObject(memDC, hTestPen);
                    MoveToEx(memDC, 100, 100, nullptr);
                    LineTo(memDC, 300, 200);
                    Ellipse(memDC, 400, 100, 560, 260);
                    SelectObject(memDC, hOldTestPen);
                    DeleteObject(hTestPen);
                }
                
                BitBlt(hdc, toolbarWidth, 0, canvasW, canvasH, memDC, 0, 0, SRCCOPY);
                
                SelectObject(memDC, hOldBitmap);
                DeleteObject(hBitmap);
            }
            DeleteDC(memDC);
            
            if (m_renderer) {
                m_renderer->setHDC(m_hdc);
            }
            
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(200, 200, 200));
            HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
            MoveToEx(hdc, toolbarWidth, rc.bottom - statusHeight, nullptr);
            LineTo(hdc, rc.right, rc.bottom - statusHeight);
            SelectObject(hdc, hOldPen);
            DeleteObject(hPen);
            
            EndPaint(m_hwnd, &ps);
            return 0;
        }
            
        case WM_ERASEBKGND:
            return 1;
            
        case WM_MOUSEMOVE: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            int toolHeight = 40;
            int startY = 10;
            
            if (x < toolbarWidth) {
                int oldHover = m_hoveredTool;
                m_hoveredTool = -1;
                
                int toolIndex = (y - startY) / toolHeight;
                if (toolIndex >= 0 && toolIndex < TOOL_COUNT) {
                    m_hoveredTool = toolIndex;
                }
                
                if (oldHover != m_hoveredTool) {
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }
                SetCursor(LoadCursor(nullptr, IDC_HAND));
            } else {
                if (m_hoveredTool != -1) {
                    m_hoveredTool = -1;
                    InvalidateRect(m_hwnd, nullptr, FALSE);
                }
                
                int canvasX = x - toolbarWidth;
                int canvasY = y;
                
                if (m_application) {
                    m_application->onMouseMove(canvasX, canvasY, mods);
                }
                
                if (m_hwndStatusbar && m_application) {
                    wchar_t buf[256];
                    int shapeCount = 0;
                    auto doc = m_application->getDocument();
                    if (doc) {
                        shapeCount = static_cast<int>(doc->getAllShapes().size());
                    }
                    swprintf_s(buf, L"Shapes: %d | (%d, %d)", shapeCount, canvasX, canvasY);
                    SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 1, (LPARAM)buf);
                }
            }
            return 0;
        }
            
        case WM_LBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            
            if (x < toolbarWidth) {
                handleToolbarClick(x, y);
            } else {
                if (m_application) {
                    m_application->onMouseDown(x - toolbarWidth, y, 0, mods);
                }
            }
            SetCapture(m_hwnd);
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_RBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            if (x >= toolbarWidth && m_application) {
                m_application->onMouseDown(x - toolbarWidth, y, 1, mods);
            }
            SetCapture(m_hwnd);
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_MBUTTONDOWN: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            if (x >= toolbarWidth && m_application) {
                m_application->onMouseDown(x - toolbarWidth, y, 2, mods);
            }
            SetCapture(m_hwnd);
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_LBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            if (x >= toolbarWidth && m_application) {
                m_application->onMouseUp(x - toolbarWidth, y, 0, mods);
            }
            ReleaseCapture();
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_RBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            if (x >= toolbarWidth && m_application) {
                m_application->onMouseUp(x - toolbarWidth, y, 1, mods);
            }
            ReleaseCapture();
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_MBUTTONUP: {
            int x = LOWORD(lParam);
            int y = HIWORD(lParam);
            int mods = 0;
            if (wParam & MK_SHIFT) mods |= 0x0001;
            if (wParam & MK_CONTROL) mods |= 0x0002;
            
            int toolbarWidth = 48;
            if (x >= toolbarWidth && m_application) {
                m_application->onMouseUp(x - toolbarWidth, y, 2, mods);
            }
            ReleaseCapture();
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_MOUSEWHEEL: {
            POINT pt;
            pt.x = LOWORD(lParam);
            pt.y = HIWORD(lParam);
            ScreenToClient(m_hwnd, &pt);
            
            short delta = GET_WHEEL_DELTA_WPARAM(wParam);
            double scrollDelta = (delta > 0) ? 1.0 : -1.0;
            
            int toolbarWidth = 48;
            if (pt.x >= toolbarWidth && m_application) {
                m_application->onMouseWheel(scrollDelta, pt.x - toolbarWidth, pt.y, 0);
            }
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_KEYDOWN: {
            int mods = 0;
            if (GetKeyState(VK_SHIFT) < 0) mods |= 0x0001;
            if (GetKeyState(VK_CONTROL) < 0) mods |= 0x0002;
            if (GetKeyState(VK_MENU) < 0) mods |= 0x0004;
            
            int key = static_cast<int>(wParam);
            
            if (key >= '1' && key <= '6') {
                m_selectedTool = key - '1';
                ToolType toolTypes[TOOL_COUNT] = {
                    ToolType::Select, ToolType::Line, ToolType::Circle,
                    ToolType::Rectangle, ToolType::Arc, ToolType::Polyline
                };
                if (m_application) {
                    m_application->setActiveTool(toolTypes[m_selectedTool]);
                }
                const wchar_t* hints[TOOL_COUNT] = {
                    L"Select", L"Line", L"Circle", L"Rectangle", L"Arc", L"Polyline"
                };
                if (m_hwndStatusbar) {
                    SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 0, (LPARAM)hints[m_selectedTool]);
                }
            }
            
            if (key == VK_ESCAPE) {
                key = 256;
            } else if (key == VK_RETURN) {
                key = 257;
            } else if (key == VK_TAB) {
                key = 258;
            } else if (key == VK_BACK) {
                key = 259;
            } else if (key == VK_DELETE) {
                key = 261;
            } else if (key == VK_RIGHT) {
                key = 262;
            } else if (key == VK_LEFT) {
                key = 263;
            } else if (key == VK_DOWN) {
                key = 264;
            } else if (key == VK_UP) {
                key = 265;
            } else if (key == VK_LSHIFT || key == VK_RSHIFT) {
                key = 340;
            } else if (key == VK_LCONTROL || key == VK_RCONTROL) {
                key = 341;
            } else if (key == VK_LMENU || key == VK_RMENU) {
                key = 342;
            }
            
            bool isRepeat = (lParam & (1 << 30)) != 0;
            
            if (m_application) {
                m_application->onKeyDown(key, mods, isRepeat);
            }
            
            if (m_hwndStatusbar && m_application) {
                wchar_t buf[256];
                int shapeCount = 0;
                auto doc = m_application->getDocument();
                if (doc) {
                    shapeCount = static_cast<int>(doc->getAllShapes().size());
                }
                swprintf_s(buf, L"Shapes: %d", shapeCount);
                SendMessageW(m_hwndStatusbar, SB_SETTEXTW, 1, (LPARAM)buf);
            }
            
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        case WM_KEYUP: {
            int mods = 0;
            if (GetKeyState(VK_SHIFT) < 0) mods |= 0x0001;
            if (GetKeyState(VK_CONTROL) < 0) mods |= 0x0002;
            if (GetKeyState(VK_MENU) < 0) mods |= 0x0004;
            
            int key = static_cast<int>(wParam);
            
            if (key == VK_ESCAPE) {
                key = 256;
            } else if (key == VK_RETURN) {
                key = 257;
            } else if (key == VK_TAB) {
                key = 258;
            } else if (key == VK_BACK) {
                key = 259;
            } else if (key == VK_DELETE) {
                key = 261;
            } else if (key == VK_RIGHT) {
                key = 262;
            } else if (key == VK_LEFT) {
                key = 263;
            } else if (key == VK_DOWN) {
                key = 264;
            } else if (key == VK_UP) {
                key = 265;
            } else if (key == VK_LSHIFT || key == VK_RSHIFT) {
                key = 340;
            } else if (key == VK_LCONTROL || key == VK_RCONTROL) {
                key = 341;
            } else if (key == VK_LMENU || key == VK_RMENU) {
                key = 342;
            }
            
            if (m_application) {
                m_application->onKeyUp(key, mods);
            }
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;
        }
            
        default:
            return DefWindowProc(m_hwnd, msg, wParam, lParam);
    }
}

} 
