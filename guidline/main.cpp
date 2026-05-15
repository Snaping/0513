#include <windows.h>
#include <commctrl.h>
#include "resource.h"
#include "Canvas.h"

#pragma comment(lib, "comctl32.lib")

HINSTANCE hInst;
HWND hMainWnd;
Canvas canvas;
bool isPanning = false;
POINT lastMousePos;

const int TOOLBAR_HEIGHT = 50;
const int BOTTOMBAR_HEIGHT = 60;
const int BUTTON_WIDTH = 110;
const int BUTTON_HEIGHT = 38;

struct ToolButton {
    int commandId;
    const wchar_t* text;
    COLORREF color;
};

ToolButton toolButtons[] = {
    { IDM_DRAW_LINE, L"Line", RGB(70, 130, 180) },
    { IDM_DRAW_CIRCLE, L"Circle", RGB(200, 100, 0) },
    { IDM_DRAW_RECTANGLE, L"Rectangle", RGB(0, 150, 100) },
    { IDM_DRAW_TRIANGLE, L"Triangle", RGB(150, 0, 150) },
    { IDM_STYLE_FILL, L"Fill", RGB(120, 120, 200) },
    { IDM_VIEW_NAVIGATION, L"Navigate", RGB(100, 100, 100) },
    { IDM_EDIT_CLEAR, L"Clear", RGB(180, 60, 60) },
    { IDM_VIEW_RESET, L"Reset", RGB(80, 80, 80) }
};
const int TOOLBAR_BUTTON_COUNT = sizeof(toolButtons) / sizeof(toolButtons[0]);

TriangleType currentTriangleType = TRIANGLE_EQUILATERAL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawToolBar(HDC hdc, RECT* rect);
void DrawBottomBar(HDC hdc, RECT* rect);
void DrawButton(HDC hdc, int x, int y, int width, int height, const wchar_t* text, COLORREF color, bool selected);
int GetButtonAtPosition(int x, int y);
int GetTriangleButtonAtPosition(int x, int y, RECT* rect);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;

    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_BAR_CLASSES;
    InitCommonControlsEx(&icex);

    WNDCLASSEXW wcex = { 0 };
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(30, 30, 35));
    wcex.lpszClassName = L"SketchToolWindow";

    if (!RegisterClassExW(&wcex)) {
        MessageBoxW(nullptr, L"RegisterClassEx failed!", L"Error", MB_ICONERROR);
        return FALSE;
    }

    hMainWnd = CreateWindowW(L"SketchToolWindow", L"Sketch Tool - Pro Version",
        WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN, CW_USEDEFAULT, 0, 1200, 800,
        nullptr, nullptr, hInstance, nullptr);

    if (!hMainWnd) {
        MessageBoxW(nullptr, L"CreateWindow failed!", L"Error", MB_ICONERROR);
        return FALSE;
    }

    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE: {
        break;
    }

    case WM_SIZE: {
        RECT rect;
        GetClientRect(hWnd, &rect);
        int canvasHeight = rect.bottom - TOOLBAR_HEIGHT - 
            (canvas.getCreatingShapeType() == ShapeType::Triangle ? BOTTOMBAR_HEIGHT : 0);
        canvas.setWindowSize(rect.right, canvasHeight);
        InvalidateRect(hWnd, nullptr, FALSE);
        break;
    }

    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDM_FILE_EXIT:
            DestroyWindow(hWnd);
            break;

        case IDM_DRAW_LINE:
            canvas.setCreatingShapeType(ShapeType::Line);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_DRAW_CIRCLE:
            canvas.setCreatingShapeType(ShapeType::Circle);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_DRAW_RECTANGLE:
            canvas.setCreatingShapeType(ShapeType::Rectangle);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_DRAW_TRIANGLE:
            canvas.setCreatingShapeType(ShapeType::Triangle);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_STYLE_FILL:
            canvas.setFillEnabled(!canvas.isFillEnabled());
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_VIEW_NAVIGATION:
            canvas.setNavigationMode(!canvas.isNavigationMode());
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_VIEW_RESET:
            canvas.resetView();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_EDIT_CLEAR:
            canvas.clearShapes();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case 7001:
            currentTriangleType = TRIANGLE_EQUILATERAL;
            canvas.setTriangleType(TRIANGLE_EQUILATERAL);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case 7002:
            currentTriangleType = TRIANGLE_ISOSCELES;
            canvas.setTriangleType(TRIANGLE_ISOSCELES);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        case 7003:
            currentTriangleType = TRIANGLE_RIGHT;
            canvas.setTriangleType(TRIANGLE_RIGHT);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    }

    case WM_LBUTTONDOWN: {
        POINT mousePos;
        mousePos.x = LOWORD(lParam);
        mousePos.y = HIWORD(lParam);

        int buttonIndex = GetButtonAtPosition(mousePos.x, mousePos.y);
        if (buttonIndex >= 0) {
            SendMessage(hWnd, WM_COMMAND, toolButtons[buttonIndex].commandId, 0);
            break;
        }

        if (canvas.getCreatingShapeType() == ShapeType::Triangle) {
            RECT rect;
            GetClientRect(hWnd, &rect);
            int triButton = GetTriangleButtonAtPosition(mousePos.x, mousePos.y, &rect);
            if (triButton >= 0) {
                SendMessage(hWnd, WM_COMMAND, 7001 + triButton, 0);
                break;
            }
        }

        if (mousePos.y > TOOLBAR_HEIGHT) {
            bool isTriangleMode = (canvas.getCreatingShapeType() == ShapeType::Triangle);
            RECT rect;
            GetClientRect(hWnd, &rect);
            int bottomBarY = rect.bottom - BOTTOMBAR_HEIGHT;
            
            if (!isTriangleMode || mousePos.y < bottomBarY) {
                int canvasY = mousePos.y - TOOLBAR_HEIGHT;
                Point2D worldPos = canvas.screenToWorld(POINT{ mousePos.x, canvasY });

                if (wParam & MK_SHIFT) {
                    isPanning = true;
                    lastMousePos = mousePos;
                    SetCursor(LoadCursor(nullptr, IDC_SIZEALL));
                }
                else if (isTriangleMode) {
                    canvas.setTriangleType(currentTriangleType);
                    canvas.startCreatingShape(worldPos);
                    canvas.finishCreatingShape();
                    canvas.setCreatingShapeType(ShapeType::None);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                else if (canvas.getCreatingShapeType() != ShapeType::None) {
                    if (canvas.isNavigationMode()) {
                        const auto& fps = canvas.getFoundFeaturePoints();
                        if (!fps.empty()) {
                            worldPos = fps[0].position;
                        }
                    }
                    if (!canvas.isCreatingShape()) {
                        canvas.startCreatingShape(worldPos);
                    }
                    else {
                        canvas.finishCreatingShape();
                        canvas.setCreatingShapeType(ShapeType::None);
                    }
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                else if (canvas.isNavigationMode() && !canvas.getFoundFeaturePoints().empty()) {
                    canvas.setSelectedFeaturePoint(0);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
        }

        break;
    }

    case WM_MOUSEMOVE: {
        POINT mousePos;
        mousePos.x = LOWORD(lParam);
        mousePos.y = HIWORD(lParam);

        if (isPanning) {
            int dx = mousePos.x - lastMousePos.x;
            int dy = mousePos.y - lastMousePos.y;
            canvas.pan(dx, dy);
            lastMousePos = mousePos;
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        else if (mousePos.y > TOOLBAR_HEIGHT) {
            bool isTriangleMode = (canvas.getCreatingShapeType() == ShapeType::Triangle);
            RECT rect;
            GetClientRect(hWnd, &rect);
            int bottomBarY = rect.bottom - BOTTOMBAR_HEIGHT;

            if (!isTriangleMode || mousePos.y < bottomBarY) {
                int canvasY = mousePos.y - TOOLBAR_HEIGHT;

                if (canvas.getCreatingShapeType() != ShapeType::None) {
                    Point2D worldPos = canvas.screenToWorld(POINT{ mousePos.x, canvasY });
                    if (canvas.isNavigationMode()) {
                        double threshold = 10.0 / canvas.getScale();
                        canvas.findFeaturePoints(worldPos, threshold);
                        const auto& fps = canvas.getFoundFeaturePoints();
                        if (!fps.empty()) {
                            worldPos = fps[0].position;
                        }
                    }
                    canvas.updateCreatingShape(worldPos);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
                else if (canvas.isNavigationMode()) {
                    Point2D worldPos = canvas.screenToWorld(POINT{ mousePos.x, canvasY });
                    double threshold = 10.0 / canvas.getScale();
                    canvas.findFeaturePoints(worldPos, threshold);
                    InvalidateRect(hWnd, nullptr, FALSE);
                }
            }
        }

        break;
    }

    case WM_LBUTTONUP: {
        if (isPanning) {
            isPanning = false;
            SetCursor(LoadCursor(nullptr, IDC_ARROW));
        }
        break;
    }

    case WM_RBUTTONDOWN: {
        if (canvas.getCreatingShapeType() != ShapeType::None) {
            canvas.cancelCreatingShape();
            canvas.setCreatingShapeType(ShapeType::None);
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;
    }

    case WM_MOUSEWHEEL: {
        POINT mousePos;
        mousePos.x = LOWORD(lParam);
        mousePos.y = HIWORD(lParam);
        ScreenToClient(hWnd, &mousePos);

        if (mousePos.y > TOOLBAR_HEIGHT) {
            int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            double zoomFactor = (delta > 0) ? 1.1 : 0.9;
            canvas.zoom(zoomFactor, POINT{ mousePos.x, mousePos.y - TOOLBAR_HEIGHT });
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        break;
    }

    case WM_KEYDOWN: {
        switch (wParam) {
        case VK_ESCAPE:
            canvas.cancelCreatingShape();
            canvas.setCreatingShapeType(ShapeType::None);
            canvas.setSelectedFeaturePoint(-1);
            InvalidateRect(hWnd, nullptr, FALSE);
            break;
        }
        break;
    }

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);

        RECT rect;
        GetClientRect(hWnd, &rect);

        HDC hMemDC = CreateCompatibleDC(hdc);
        HBITMAP hMemBitmap = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hMemBitmap);

        HBRUSH hBgBrush = CreateSolidBrush(RGB(30, 30, 35));
        FillRect(hMemDC, &rect, hBgBrush);
        DeleteObject(hBgBrush);

        DrawToolBar(hMemDC, &rect);

        RECT canvasRect;
        canvasRect.left = 0;
        canvasRect.top = TOOLBAR_HEIGHT;
        canvasRect.right = rect.right;
        canvasRect.bottom = rect.bottom - 
            (canvas.getCreatingShapeType() == ShapeType::Triangle ? BOTTOMBAR_HEIGHT : 0);

        HDC hCanvasDC = CreateCompatibleDC(hMemDC);
        HBITMAP hCanvasBitmap = CreateCompatibleBitmap(hMemDC, 
            canvasRect.right - canvasRect.left, 
            canvasRect.bottom - canvasRect.top);
        HBITMAP hOldCanvasBitmap = (HBITMAP)SelectObject(hCanvasDC, hCanvasBitmap);

        BitBlt(hCanvasDC, 0, 0, 
            canvasRect.right - canvasRect.left, 
            canvasRect.bottom - canvasRect.top, 
            hMemDC, canvasRect.left, canvasRect.top, SRCCOPY);

        canvas.draw(hCanvasDC);

        BitBlt(hMemDC, canvasRect.left, canvasRect.top, 
            canvasRect.right - canvasRect.left, 
            canvasRect.bottom - canvasRect.top, 
            hCanvasDC, 0, 0, SRCCOPY);

        SelectObject(hCanvasDC, hOldCanvasBitmap);
        DeleteObject(hCanvasBitmap);
        DeleteDC(hCanvasDC);

        if (canvas.getCreatingShapeType() == ShapeType::Triangle) {
            DrawBottomBar(hMemDC, &rect);
        }

        BitBlt(hdc, 0, 0, rect.right, rect.bottom, hMemDC, 0, 0, SRCCOPY);

        SelectObject(hMemDC, hOldBitmap);
        DeleteObject(hMemBitmap);
        DeleteDC(hMemDC);

        EndPaint(hWnd, &ps);
        break;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void DrawButton(HDC hdc, int x, int y, int width, int height, const wchar_t* text, COLORREF color, bool selected) {
    RECT btnRect = { x, y, x + width, y + height };
    
    if (selected) {
        HBRUSH hBtnBrush = CreateSolidBrush(color);
        RoundRect(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom, 8, 8);
        DeleteObject(hBtnBrush);
        
        HPEN hBorderPen = CreatePen(PS_SOLID, 2, color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
        SelectObject(hdc, GetStockObject(NULL_BRUSH));
        RoundRect(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom, 8, 8);
        SelectObject(hdc, hOldPen);
        DeleteObject(hBorderPen);
    }
    else {
        HBRUSH hBtnBrush = CreateSolidBrush(RGB(60, 60, 65));
        RoundRect(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom, 8, 8);
        DeleteObject(hBtnBrush);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(240, 240, 240));

    RECT textRect = btnRect;
    DrawTextW(hdc, text, -1, &textRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
}

void DrawToolBar(HDC hdc, RECT* rect) {
    HBRUSH hToolbarBrush = CreateSolidBrush(RGB(40, 40, 45));
    RECT toolbarRect = { 0, 0, rect->right, TOOLBAR_HEIGHT };
    FillRect(hdc, &toolbarRect, hToolbarBrush);
    DeleteObject(hToolbarBrush);

    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(60, 60, 65));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    MoveToEx(hdc, 0, TOOLBAR_HEIGHT - 1, NULL);
    LineTo(hdc, rect->right, TOOLBAR_HEIGHT - 1);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);

    int startX = 10;
    int startY = (TOOLBAR_HEIGHT - BUTTON_HEIGHT) / 2;

    for (int i = 0; i < TOOLBAR_BUTTON_COUNT; i++) {
        bool selected = false;
        if (toolButtons[i].commandId == IDM_DRAW_LINE && canvas.getCreatingShapeType() == ShapeType::Line) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_CIRCLE && canvas.getCreatingShapeType() == ShapeType::Circle) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_RECTANGLE && canvas.getCreatingShapeType() == ShapeType::Rectangle) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_TRIANGLE && canvas.getCreatingShapeType() == ShapeType::Triangle) selected = true;
        if (toolButtons[i].commandId == IDM_STYLE_FILL && canvas.isFillEnabled()) selected = true;
        if (toolButtons[i].commandId == IDM_VIEW_NAVIGATION && canvas.isNavigationMode()) selected = true;

        DrawButton(hdc, startX + i * BUTTON_WIDTH, startY, BUTTON_WIDTH - 10, BUTTON_HEIGHT,
            toolButtons[i].text, toolButtons[i].color, selected);
    }
}

void DrawBottomBar(HDC hdc, RECT* rect) {
    HBRUSH hBarBrush = CreateSolidBrush(RGB(40, 40, 45));
    RECT barRect = { 0, rect->bottom - BOTTOMBAR_HEIGHT, rect->right, rect->bottom };
    FillRect(hdc, &barRect, hBarBrush);
    DeleteObject(hBarBrush);

    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(60, 60, 65));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    MoveToEx(hdc, 0, rect->bottom - BOTTOMBAR_HEIGHT, NULL);
    LineTo(hdc, rect->right, rect->bottom - BOTTOMBAR_HEIGHT);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(200, 200, 200));
    TextOutW(hdc, 20, rect->bottom - BOTTOMBAR_HEIGHT / 2 - 10, L"Triangle Type:", 15);

    const wchar_t* triangleTypes[] = { L"Equilateral", L"Isosceles", L"Right Angle" };
    TriangleType types[] = { TRIANGLE_EQUILATERAL, TRIANGLE_ISOSCELES, TRIANGLE_RIGHT };

    int btnWidth = 130;
    int btnHeight = 35;
    int startX = 160;
    int startY = rect->bottom - BOTTOMBAR_HEIGHT + (BOTTOMBAR_HEIGHT - btnHeight) / 2;

    for (int i = 0; i < 3; i++) {
        bool selected = (currentTriangleType == types[i]);
        DrawButton(hdc, startX + i * btnWidth, startY, btnWidth - 10, btnHeight,
            triangleTypes[i], RGB(150, 0, 150), selected);
    }
}

int GetButtonAtPosition(int x, int y) {
    if (y >= 0 && y < TOOLBAR_HEIGHT) {
        int startX = 10;
        for (int i = 0; i < TOOLBAR_BUTTON_COUNT; i++) {
            if (x >= startX + i * BUTTON_WIDTH && x < startX + (i + 1) * BUTTON_WIDTH - 10) {
                return i;
            }
        }
    }
    return -1;
}

int GetTriangleButtonAtPosition(int x, int y, RECT* rect) {
    int bottomBarY = rect->bottom - BOTTOMBAR_HEIGHT;
    if (y >= bottomBarY && y < rect->bottom) {
        int btnWidth = 130;
        int startX = 160;
        for (int i = 0; i < 3; i++) {
            if (x >= startX + i * btnWidth && x < startX + (i + 1) * btnWidth - 10) {
                return i;
            }
        }
    }
    return -1;
}
