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

const int TOOLBAR_HEIGHT = 56;
const int BOTTOMBAR_HEIGHT = 50;
const int BUTTON_WIDTH = 95;
const int BUTTON_HEIGHT = 36;
const int BUTTON_SPACING = 8;

struct ToolButton {
    int commandId;
    const wchar_t* text;
    const wchar_t* icon;
    COLORREF color;
};

ToolButton toolButtons[] = {
    { IDM_DRAW_LINE, L"Line", L"|", RGB(65, 105, 225) },
    { IDM_DRAW_CIRCLE, L"Circle", L"O", RGB(255, 107, 107) },
    { IDM_DRAW_RECTANGLE, L"Rect", L"#", RGB(78, 205, 196) },
    { IDM_DRAW_TRIANGLE, L"Tri", L"A", RGB(199, 125, 255) },
    { IDM_STYLE_FILL, L"Fill", L"F", RGB(120, 144, 156) },
    { IDM_EDIT_CLIP, L"Clip", L"C", RGB(255, 152, 0) },
    { IDM_EDIT_MIRROR, L"Mirror", L"M", RGB(0, 188, 212) },
    { IDM_VIEW_NAVIGATION, L"Nav", L"N", RGB(96, 125, 139) },
    { IDM_EDIT_CLEAR, L"Clear", L"X", RGB(244, 67, 54) },
    { IDM_VIEW_RESET, L"Reset", L"R", RGB(158, 158, 158) }
};
const int TOOLBAR_BUTTON_COUNT = sizeof(toolButtons) / sizeof(toolButtons[0]);

TriangleType currentTriangleType = TRIANGLE_EQUILATERAL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
void DrawToolBar(HDC hdc, RECT* rect);
void DrawBottomBar(HDC hdc, RECT* rect);
void DrawButton(HDC hdc, int x, int y, int width, int height, const wchar_t* text, const wchar_t* icon, COLORREF color, bool selected);
int GetButtonAtPosition(int x, int y);
int GetTriangleButtonAtPosition(int x, int y, RECT* rect);
int GetShapeAtPosition(const POINT& mousePos, const RECT& rect);

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
        int canvasHeight = rect.bottom - TOOLBAR_HEIGHT - BOTTOMBAR_HEIGHT;
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

        case IDM_EDIT_CLIP:
            canvas.startClipMode();
            InvalidateRect(hWnd, nullptr, FALSE);
            break;

        case IDM_EDIT_MIRROR:
            canvas.startMirrorMode();
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
                else if (canvas.getEditMode() == EditMode::SelectBoundary) {
                    int shapeIndex = GetShapeAtPosition(mousePos, rect);
                    if (shapeIndex >= 0) {
                        const auto& shapes = canvas.getShapes();
                        if (shapes[shapeIndex]->getType() == ShapeType::Line) {
                            bool alreadySelected = false;
                            for (int idx : canvas.boundaryShapeIndices) {
                                if (idx == shapeIndex) {
                                    alreadySelected = true;
                                    break;
                                }
                            }
                            if (!alreadySelected) {
                                canvas.boundaryShapeIndices.push_back(shapeIndex);
                                shapes[shapeIndex]->selected = true;
                                InvalidateRect(hWnd, nullptr, FALSE);
                            }
                        }
                    }
                }
                else if (canvas.getEditMode() == EditMode::SelectShapeToClip) {
                    int shapeIndex = GetShapeAtPosition(mousePos, rect);
                    if (shapeIndex >= 0) {
                        canvas.performClip(shapeIndex, worldPos);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                else if (canvas.getEditMode() == EditMode::SelectMirrorShape) {
                    int shapeIndex = GetShapeAtPosition(mousePos, rect);
                    if (shapeIndex >= 0) {
                        canvas.mirrorShapeIndex = shapeIndex;
                        canvas.setSelectedShapeIndex(shapeIndex);
                        canvas.setEditMode(EditMode::SelectMirrorLine);
                        InvalidateRect(hWnd, nullptr, FALSE);
                    }
                }
                else if (canvas.getEditMode() == EditMode::SelectMirrorLine) {
                    int shapeIndex = GetShapeAtPosition(mousePos, rect);
                    if (shapeIndex >= 0) {
                        const auto& shapes = canvas.getShapes();
                        if (shapes[shapeIndex]->getType() == ShapeType::Line) {
                            canvas.mirrorLineIndex = shapeIndex;
                            canvas.setSelectedShapeIndex(shapeIndex);
                            canvas.performMirror();
                            InvalidateRect(hWnd, nullptr, FALSE);
                        }
                    }
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
        else if (canvas.getEditMode() == EditMode::SelectBoundary) {
            if (!canvas.boundaryShapeIndices.empty()) {
                canvas.setEditMode(EditMode::SelectShapeToClip);
                InvalidateRect(hWnd, nullptr, FALSE);
            } else {
                canvas.exitClipMode();
                InvalidateRect(hWnd, nullptr, FALSE);
            }
        }
        else if (canvas.getEditMode() == EditMode::SelectShapeToClip) {
            canvas.exitClipMode();
            InvalidateRect(hWnd, nullptr, FALSE);
        }
        else if (canvas.getEditMode() == EditMode::SelectMirrorShape || canvas.getEditMode() == EditMode::SelectMirrorLine) {
            canvas.exitMirrorMode();
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
        canvasRect.bottom = rect.bottom - BOTTOMBAR_HEIGHT;

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

        DrawBottomBar(hMemDC, &rect);

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

void DrawButton(HDC hdc, int x, int y, int width, int height, const wchar_t* text, const wchar_t* icon, COLORREF color, bool selected) {
    RECT btnRect = { x, y, x + width, y + height };
    
    HBRUSH hBtnBrush;
    if (selected) {
        hBtnBrush = CreateSolidBrush(color);
    } else {
        hBtnBrush = CreateSolidBrush(RGB(55, 55, 60));
    }
    
    HPEN hShadowPen = CreatePen(PS_SOLID, 1, RGB(30, 30, 35));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hShadowPen);
    
    RoundRect(hdc, btnRect.left + 1, btnRect.top + 1, btnRect.right + 1, btnRect.bottom + 1, 10, 10);
    
    HPEN hBorderPen = CreatePen(PS_SOLID, 1, selected ? color : RGB(75, 75, 80));
    SelectObject(hdc, hBorderPen);
    SelectObject(hdc, hBtnBrush);
    
    RoundRect(hdc, btnRect.left, btnRect.top, btnRect.right, btnRect.bottom, 10, 10);
    
    SelectObject(hdc, hOldPen);
    DeleteObject(hBtnBrush);
    DeleteObject(hShadowPen);
    DeleteObject(hBorderPen);

    SetBkMode(hdc, TRANSPARENT);
    
    RECT iconRect = { x + 8, y + 5, x + 32, y + height - 5 };
    SetTextColor(hdc, selected ? RGB(255, 255, 255) : color);
    HFONT hIconFont = CreateFontW(22, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Segoe UI");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hIconFont);
    DrawTextW(hdc, icon, -1, &iconRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    
    RECT textRect = { x + 34, y, x + width - 6, y + height };
    SetTextColor(hdc, RGB(240, 240, 245));
    HFONT hTextFont = CreateFontW(14, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
    SelectObject(hdc, hTextFont);
    DrawTextW(hdc, text, -1, &textRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hIconFont);
    DeleteObject(hTextFont);
}

void DrawToolBar(HDC hdc, RECT* rect) {
    HBRUSH hToolbarBrush = CreateSolidBrush(RGB(45, 45, 52));
    RECT toolbarRect = { 0, 0, rect->right, TOOLBAR_HEIGHT };
    FillRect(hdc, &toolbarRect, hToolbarBrush);
    DeleteObject(hToolbarBrush);

    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(65, 65, 75));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    MoveToEx(hdc, 0, TOOLBAR_HEIGHT - 1, NULL);
    LineTo(hdc, rect->right, TOOLBAR_HEIGHT - 1);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);

    int startX = 16;
    int startY = (TOOLBAR_HEIGHT - BUTTON_HEIGHT) / 2;

    for (int i = 0; i < TOOLBAR_BUTTON_COUNT; i++) {
        bool selected = false;
        if (toolButtons[i].commandId == IDM_DRAW_LINE && canvas.getCreatingShapeType() == ShapeType::Line) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_CIRCLE && canvas.getCreatingShapeType() == ShapeType::Circle) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_RECTANGLE && canvas.getCreatingShapeType() == ShapeType::Rectangle) selected = true;
        if (toolButtons[i].commandId == IDM_DRAW_TRIANGLE && canvas.getCreatingShapeType() == ShapeType::Triangle) selected = true;
        if (toolButtons[i].commandId == IDM_STYLE_FILL && canvas.isFillEnabled()) selected = true;
        if (toolButtons[i].commandId == IDM_VIEW_NAVIGATION && canvas.isNavigationMode()) selected = true;
        if (toolButtons[i].commandId == IDM_EDIT_CLIP && (canvas.getEditMode() == EditMode::SelectBoundary || canvas.getEditMode() == EditMode::SelectShapeToClip)) selected = true;
        if (toolButtons[i].commandId == IDM_EDIT_MIRROR && (canvas.getEditMode() == EditMode::SelectMirrorShape || canvas.getEditMode() == EditMode::SelectMirrorLine)) selected = true;

        DrawButton(hdc, startX + i * (BUTTON_WIDTH + BUTTON_SPACING), startY, BUTTON_WIDTH, BUTTON_HEIGHT,
            toolButtons[i].text, toolButtons[i].icon, toolButtons[i].color, selected);
    }
}

void DrawBottomBar(HDC hdc, RECT* rect) {
    HBRUSH hBarBrush = CreateSolidBrush(RGB(45, 45, 52));
    RECT barRect = { 0, rect->bottom - BOTTOMBAR_HEIGHT, rect->right, rect->bottom };
    FillRect(hdc, &barRect, hBarBrush);
    DeleteObject(hBarBrush);

    HPEN hBorderPen = CreatePen(PS_SOLID, 1, RGB(65, 65, 75));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hBorderPen);
    MoveToEx(hdc, 0, rect->bottom - BOTTOMBAR_HEIGHT, NULL);
    LineTo(hdc, rect->right, rect->bottom - BOTTOMBAR_HEIGHT);
    SelectObject(hdc, hOldPen);
    DeleteObject(hBorderPen);

    SetBkMode(hdc, TRANSPARENT);
    
    std::wstring statusText = canvas.getStatusText();
    
    HFONT hStatusFont = CreateFontW(15, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Microsoft YaHei");
    HFONT hOldFont = (HFONT)SelectObject(hdc, hStatusFont);
    
    SetTextColor(hdc, RGB(220, 220, 230));
    TextOutW(hdc, 24, rect->bottom - BOTTOMBAR_HEIGHT / 2 - 8, 
        statusText.c_str(), (int)statusText.length());

    if (canvas.getCreatingShapeType() == ShapeType::Triangle) {
        const wchar_t* triangleTypes[] = { L"Equilateral", L"Isosceles", L"Right Angle" };
        TriangleType types[] = { TRIANGLE_EQUILATERAL, TRIANGLE_ISOSCELES, TRIANGLE_RIGHT };

        int btnWidth = 110;
        int btnHeight = 32;
        int startX = 320;
        int startY = rect->bottom - BOTTOMBAR_HEIGHT + (BOTTOMBAR_HEIGHT - btnHeight) / 2;

        for (int i = 0; i < 3; i++) {
            bool selected = (currentTriangleType == types[i]);
            DrawButton(hdc, startX + i * (btnWidth + BUTTON_SPACING), startY, btnWidth, btnHeight,
                triangleTypes[i], L"", RGB(199, 125, 255), selected);
        }
    }
    
    SelectObject(hdc, hOldFont);
    DeleteObject(hStatusFont);
}

int GetButtonAtPosition(int x, int y) {
    if (y >= 0 && y < TOOLBAR_HEIGHT) {
        int startX = 16;
        for (int i = 0; i < TOOLBAR_BUTTON_COUNT; i++) {
            if (x >= startX + i * (BUTTON_WIDTH + BUTTON_SPACING) && x < startX + i * (BUTTON_WIDTH + BUTTON_SPACING) + BUTTON_WIDTH) {
                return i;
            }
        }
    }
    return -1;
}

int GetTriangleButtonAtPosition(int x, int y, RECT* rect) {
    int bottomBarY = rect->bottom - BOTTOMBAR_HEIGHT;
    if (y >= bottomBarY && y < rect->bottom) {
        int btnWidth = 110;
        int startX = 320;
        for (int i = 0; i < 3; i++) {
            if (x >= startX + i * (btnWidth + BUTTON_SPACING) && x < startX + i * (btnWidth + BUTTON_SPACING) + btnWidth) {
                return i;
            }
        }
    }
    return -1;
}

int GetShapeAtPosition(const POINT& mousePos, const RECT& rect) {
    if (mousePos.y <= TOOLBAR_HEIGHT) return -1;
    
    int canvasY = mousePos.y - TOOLBAR_HEIGHT;
    Point2D worldPos = canvas.screenToWorld(POINT{ mousePos.x, canvasY });
    double threshold = 10.0 / canvas.getScale();
    
    const auto& shapes = canvas.getShapes();
    for (int i = (int)shapes.size() - 1; i >= 0; i--) {
        if (shapes[i]->hitTest(worldPos, threshold)) {
            return i;
        }
    }
    return -1;
}
