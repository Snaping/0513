#pragma once

#include <windows.h>
#include <commctrl.h>
#include <gdiplus.h>
#include <string>
#include <algorithm>
#include <vector>

#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "comctl32.lib")

#ifndef TBSTYLE_FLAT
#define TBSTYLE_FLAT 0x0800
#endif

#ifndef TBSTYLE_TOOLTIPS
#define TBSTYLE_TOOLTIPS 0x0100
#endif

#ifndef CCS_NODIVIDER
#define CCS_NODIVIDER 0x0040
#endif

#ifndef TB_SETBUTTONSIZE
#define TB_SETBUTTONSIZE (WM_USER + 32)
#endif

#ifndef TB_SETBITMAPSIZE
#define TB_SETBITMAPSIZE (WM_USER + 33)
#endif

#ifndef TB_BUTTONSTRUCTSIZE
#define TB_BUTTONSTRUCTSIZE (WM_USER + 30)
#endif

#ifndef TB_ADDBUTTONS
#define TB_ADDBUTTONS (WM_USER + 68)
#endif

#ifndef TB_AUTOSIZE
#define TB_AUTOSIZE (WM_USER + 33)
#endif

#ifndef SBARS_SIZEGRIP
#define SBARS_SIZEGRIP 0x0100
#endif

#ifndef SB_SETPARTS
#define SB_SETPARTS (WM_USER + 4)
#endif

#ifndef SB_SETTEXTW
#define SB_SETTEXTW (WM_USER + 11)
#endif

#ifndef TBM_SETRANGE
#define TBM_SETRANGE (WM_USER + 5)
#endif

#ifndef TBM_SETPOS
#define TBM_SETPOS (WM_USER + 6)
#endif

#ifndef TBM_GETPOS
#define TBM_GETPOS (WM_USER + 0)
#endif

#ifndef TOOLBARCLASSNAMEW
#define TOOLBARCLASSNAMEW L"ToolbarWindow32"
#endif

#ifndef STATUSCLASSNAMEW
#define STATUSCLASSNAMEW L"msctls_statusbar32"
#endif

struct Annotation
{
    int startX;
    int startY;
    int endX;
    int endY;
    COLORREF color;
    int width;
};

class ImageEditor
{
public:
    ImageEditor();
    ~ImageEditor();

    BOOL RegisterClass(HINSTANCE hInstance);
    BOOL InitInstance(HINSTANCE hInstance, int nCmdShow);
    static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

private:
    static ImageEditor* GetInstance(HWND hWnd);
    void OnCreate(HWND hWnd);
    void OnPaint(HWND hWnd);
    void OnSize();
    void OnCommand(WPARAM wParam);
    void OnMouseMove(WPARAM wParam, LPARAM lParam);
    void OnLButtonDown(WPARAM wParam, LPARAM lParam);
    void OnLButtonUp(WPARAM wParam, LPARAM lParam);
    void OnMouseWheel(WPARAM wParam, LPARAM lParam);

    BOOL LoadImage(const wchar_t* filePath);
    BOOL SaveImage(const wchar_t* filePath);
    BOOL ExportToPdf(const wchar_t* filePath);
    void ClearImage();

    void ZoomIn();
    void ZoomOut();
    void RotateLeft();
    void RotateRight();
    void AdjustBrightness(int value);
    void AdjustContrast(int value);
    void AdjustSaturation(int value);

    void StartCrop();
    void ApplyCrop();
    void CancelCrop();

    void StartAnnotation();
    void EndAnnotation();

    void ApplyAiCutout();
    void ApplyAiRemove();
    void ApplyAiEnhance();

    void ShowFilterDialog();
    void UpdateStatusBar();
    void SetStatusText(const wchar_t* text);

    POINT ImageToScreen(int x, int y);
    POINT ScreenToImage(int x, int y);

    HWND m_hWnd;
    HWND m_hTopToolBar;
    HWND m_hBottomToolBar;
    HWND m_hStatusBar;

    Gdiplus::Bitmap* m_pOriginalImage;
    Gdiplus::Bitmap* m_pCurrentImage;
    float m_fZoom;
    int m_nRotation;
    int m_nScrollX;
    int m_nScrollY;

    BOOL m_bDragging;
    POINT m_ptLastMouse;

    BOOL m_bCropping;
    RECT m_rcCrop;

    BOOL m_bAnnotating;
    std::vector<Annotation> m_annotations;
    Annotation m_currentAnnotation;

    ULONG_PTR m_gdiplusToken;
    static ImageEditor* s_pInstance;
};

#define IDM_OPEN        1001
#define IDM_SAVE        1002
#define IDM_SAVE_PDF    1003
#define IDM_EXIT        1004
#define IDM_CROP        1011
#define IDM_ROTATE_LEFT 1012
#define IDM_ROTATE_RIGHT 1013
#define IDM_CORRECT     1014
#define IDM_ANNOTATION  1015
#define IDM_FILTER      1016
#define IDM_AI_REMOVE   1021
#define IDM_AI_CUTOUT   1022
#define IDM_AI_ENHANCE  1023
#define IDM_ZOOM_IN     1031
#define IDM_ZOOM_OUT    1032
#define IDM_DELETE      1033

#define ID_TOP_TOOLBAR  2001
#define ID_BOTTOM_TOOLBAR 2002
#define ID_STATUSBAR    2003
#define IDD_FILTER_DIALOG 2004

#define IDC_BRIGHTNESS  3001
#define IDC_CONTRAST    3002
#define IDC_SATURATION  3003
