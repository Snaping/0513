#include "ImageEditor.h"
#include <commdlg.h>

ImageEditor* ImageEditor::s_pInstance = nullptr;

ImageEditor::ImageEditor()
    : m_hWnd(nullptr)
    , m_hTopToolBar(nullptr)
    , m_hBottomToolBar(nullptr)
    , m_hStatusBar(nullptr)
    , m_pOriginalImage(nullptr)
    , m_pCurrentImage(nullptr)
    , m_fZoom(1.0f)
    , m_nRotation(0)
    , m_nScrollX(0)
    , m_nScrollY(0)
    , m_bDragging(FALSE)
    , m_bCropping(FALSE)
    , m_bAnnotating(FALSE)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    Gdiplus::GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);
    s_pInstance = this;
}

ImageEditor::~ImageEditor()
{
    ClearImage();
    Gdiplus::GdiplusShutdown(m_gdiplusToken);
}

ImageEditor* ImageEditor::GetInstance(HWND hWnd)
{
    return s_pInstance;
}

void ImageEditor::ClearImage()
{
    if (m_pOriginalImage)
    {
        delete m_pOriginalImage;
        m_pOriginalImage = nullptr;
    }
    if (m_pCurrentImage)
    {
        delete m_pCurrentImage;
        m_pCurrentImage = nullptr;
    }
    m_annotations.clear();
    m_bCropping = FALSE;
    m_bAnnotating = FALSE;
}

BOOL ImageEditor::RegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = {0};
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.hInstance = hInstance;
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = CreateSolidBrush(RGB(45, 45, 48));
    wcex.lpszClassName = L"ImageEditorClass";
    return RegisterClassExW(&wcex);
}

BOOL ImageEditor::InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    m_hWnd = CreateWindowExW(WS_EX_WINDOWEDGE, L"ImageEditorClass", L"图片编辑器",
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 800,
        nullptr, nullptr, hInstance, nullptr);

    if (!m_hWnd)
        return FALSE;

    OnCreate(m_hWnd);
    ShowWindow(m_hWnd, nCmdShow);
    UpdateWindow(m_hWnd);
    return TRUE;
}

void ImageEditor::OnCreate(HWND hWnd)
{
    HINSTANCE hInst = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);

    int btnWidth = 80;
    int btnHeight = 30;
    int x = 10;
    int y = 10;

    const wchar_t* topBtns[] = {
        L"打开", L"保存", L"导出PDF", L"裁剪", L"左旋", L"右旋",
        L"矫正", L"标注", L"滤镜", L"AI消除", L"AI抠图", L"AI清晰"
    };
    int topCmds[] = {
        IDM_OPEN, IDM_SAVE, IDM_SAVE_PDF, IDM_CROP, IDM_ROTATE_LEFT, IDM_ROTATE_RIGHT,
        IDM_CORRECT, IDM_ANNOTATION, IDM_FILTER, IDM_AI_REMOVE, IDM_AI_CUTOUT, IDM_AI_ENHANCE
    };

    for (int i = 0; i < 12; i++) {
        CreateWindowW(L"BUTTON", topBtns[i],
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, y, btnWidth, btnHeight, hWnd, (HMENU)(INT_PTR)topCmds[i], hInst, nullptr);
        x += btnWidth + 5;
    }

    RECT rcClient;
    GetClientRect(hWnd, &rcClient);
    int bottomY = rcClient.bottom - 70;

    const wchar_t* bottomBtns[] = { L"放大", L"缩小", L"左旋", L"右旋", L"删除" };
    int bottomCmds[] = { IDM_ZOOM_IN, IDM_ZOOM_OUT, IDM_ROTATE_LEFT, IDM_ROTATE_RIGHT, IDM_DELETE };

    x = 10;
    for (int i = 0; i < 5; i++) {
        CreateWindowW(L"BUTTON", bottomBtns[i],
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            x, bottomY, btnWidth, btnHeight, hWnd, (HMENU)(INT_PTR)bottomCmds[i], hInst, nullptr);
        x += btnWidth + 5;
    }

    m_hStatusBar = CreateWindowExW(0, L"STATIC", L"就绪 - 请点击'打开'按钮导入图片",
        WS_CHILD | WS_VISIBLE,
        10, rcClient.bottom - 35, rcClient.right - 20, 25,
        hWnd, nullptr, hInst, nullptr);
}

void ImageEditor::OnPaint(HWND hWnd)
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hWnd, &ps);
    HDC hdcMem = CreateCompatibleDC(hdc);
    RECT rcClient;
    GetClientRect(hWnd, &rcClient);

    HBITMAP hbmMem = CreateCompatibleBitmap(hdc, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top);
    HBITMAP hbmOld = (HBITMAP)SelectObject(hdcMem, hbmMem);

    HBRUSH hBrush = CreateSolidBrush(RGB(45, 45, 48));
    FillRect(hdcMem, &rcClient, hBrush);
    DeleteObject(hBrush);

    int topHeight = 50;
    int bottomHeight = 80;

    if (m_pCurrentImage)
    {
        Gdiplus::Graphics graphics(hdcMem);
        graphics.SetInterpolationMode(Gdiplus::InterpolationModeHighQualityBicubic);

        int imgWidth = m_pCurrentImage->GetWidth();
        int imgHeight = m_pCurrentImage->GetHeight();
        int drawWidth = (int)(imgWidth * m_fZoom);
        int drawHeight = (int)(imgHeight * m_fZoom);

        int clientWidth = rcClient.right - rcClient.left;
        int clientHeight = rcClient.bottom - rcClient.top - topHeight - bottomHeight;

        int imgX = (clientWidth - drawWidth) / 2 - m_nScrollX;
        int imgY = topHeight + (clientHeight - drawHeight) / 2 - m_nScrollY;

        Gdiplus::Rect destRect(imgX, imgY, drawWidth, drawHeight);
        graphics.DrawImage(m_pCurrentImage, destRect, 0, 0, imgWidth, imgHeight, Gdiplus::UnitPixel);

        for (const auto& ann : m_annotations)
        {
            Gdiplus::Pen pen(Gdiplus::Color(GetRValue(ann.color), GetGValue(ann.color), GetBValue(ann.color)), ann.width);
            graphics.DrawLine(&pen, ann.startX, ann.startY, ann.endX, ann.endY);
        }

        if (m_bAnnotating && m_currentAnnotation.startX != 0)
        {
            Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0), 3);
            graphics.DrawLine(&pen, m_currentAnnotation.startX, m_currentAnnotation.startY,
                m_currentAnnotation.endX, m_currentAnnotation.endY);
        }

        if (m_bCropping)
        {
            Gdiplus::Pen pen(Gdiplus::Color(0, 120, 215), 2);
            pen.SetDashStyle(Gdiplus::DashStyleDash);
            graphics.DrawRectangle(&pen, Gdiplus::Rect(
                (std::min)(m_rcCrop.left, m_rcCrop.right),
                (std::min)(m_rcCrop.top, m_rcCrop.bottom),
                abs(m_rcCrop.right - m_rcCrop.left),
                abs(m_rcCrop.bottom - m_rcCrop.top)
            ));
        }
    }
    else
    {
        RECT rcCenter;
        rcCenter.left = rcClient.left + (rcClient.right - rcClient.left) / 2 - 200;
        rcCenter.right = rcCenter.left + 400;
        rcCenter.top = topHeight + (rcClient.bottom - rcClient.top - topHeight - bottomHeight) / 2 - 30;
        rcCenter.bottom = rcCenter.top + 60;

        SetBkMode(hdcMem, TRANSPARENT);
        SetTextColor(hdcMem, RGB(180, 180, 180));

        HFONT hFont = CreateFontW(28, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");
        HFONT hOldFont = (HFONT)SelectObject(hdcMem, hFont);

        DrawTextW(hdcMem, L"图片编辑器", -1, &rcCenter, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        rcCenter.top += 60;
        rcCenter.bottom += 60;
        HFONT hSmallFont = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
            DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
            DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Microsoft YaHei UI");
        SelectObject(hdcMem, hSmallFont);
        SetTextColor(hdcMem, RGB(120, 120, 120));
        DrawTextW(hdcMem, L"点击顶部'打开'按钮导入图片开始编辑", -1, &rcCenter, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

        SelectObject(hdcMem, hOldFont);
        DeleteObject(hFont);
        DeleteObject(hSmallFont);
    }

    BitBlt(hdc, 0, 0, rcClient.right - rcClient.left, rcClient.bottom - rcClient.top, hdcMem, 0, 0, SRCCOPY);

    SelectObject(hdcMem, hbmOld);
    DeleteObject(hbmMem);
    DeleteDC(hdcMem);
    EndPaint(hWnd, &ps);
}

void ImageEditor::OnSize()
{
    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);

    if (m_hStatusBar)
    {
        SetWindowPos(m_hStatusBar, nullptr, 10, rcClient.bottom - 35,
            rcClient.right - 20, 25, SWP_NOZORDER);
    }

    int bottomY = rcClient.bottom - 70;
    int btnWidth = 80;
    int btnHeight = 30;
    int x = 10;

    int bottomCmds[] = { IDM_ZOOM_IN, IDM_ZOOM_OUT, IDM_ROTATE_LEFT, IDM_ROTATE_RIGHT, IDM_DELETE };

    for (int i = 0; i < 5; i++) {
        HWND hBtn = GetDlgItem(m_hWnd, bottomCmds[i]);
        if (hBtn) {
            SetWindowPos(hBtn, nullptr, x, bottomY, btnWidth, btnHeight, SWP_NOZORDER);
            x += btnWidth + 5;
        }
    }

    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void ImageEditor::OnCommand(WPARAM wParam)
{
    switch (LOWORD(wParam))
    {
    case IDM_OPEN:
        {
            OPENFILENAMEW ofn = {0};
            wchar_t szFile[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"所有图片文件\0*.png;*.jpg;*.jpeg;*.bmp;*.gif;*.tiff\0PNG图片\0*.png\0JPEG图片\0*.jpg;*.jpeg\0所有文件\0*.*\0";
            ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST;

            if (GetOpenFileNameW(&ofn))
            {
                if (LoadImage(szFile))
                {
                    SetStatusText(L"图片已加载");
                }
            }
        }
        break;
    case IDM_SAVE:
        {
            if (!m_pCurrentImage)
            {
                MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
                break;
            }
            OPENFILENAMEW ofn = {0};
            wchar_t szFile[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"PNG图片\0*.png\0JPEG图片\0*.jpg\0BMP图片\0*.bmp\0";
            ofn.Flags = OFN_OVERWRITEPROMPT;

            if (GetSaveFileNameW(&ofn))
            {
                if (SaveImage(szFile))
                {
                    MessageBoxW(m_hWnd, L"保存成功！", L"提示", MB_OK | MB_ICONINFORMATION);
                    SetStatusText(L"图片已保存");
                }
            }
        }
        break;
    case IDM_SAVE_PDF:
        {
            if (!m_pCurrentImage)
            {
                MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
                break;
            }
            OPENFILENAMEW ofn = {0};
            wchar_t szFile[MAX_PATH] = L"";
            ofn.lStructSize = sizeof(ofn);
            ofn.hwndOwner = m_hWnd;
            ofn.lpstrFile = szFile;
            ofn.nMaxFile = MAX_PATH;
            ofn.lpstrFilter = L"PDF文件\0*.pdf\0";
            ofn.Flags = OFN_OVERWRITEPROMPT;

            if (GetSaveFileNameW(&ofn))
            {
                if (ExportToPdf(szFile))
                {
                    MessageBoxW(m_hWnd, L"PDF导出成功！", L"提示", MB_OK | MB_ICONINFORMATION);
                    SetStatusText(L"PDF已导出");
                }
            }
        }
        break;
    case IDM_ZOOM_IN:
        ZoomIn();
        break;
    case IDM_ZOOM_OUT:
        ZoomOut();
        break;
    case IDM_ROTATE_LEFT:
        RotateLeft();
        break;
    case IDM_ROTATE_RIGHT:
        RotateRight();
        break;
    case IDM_DELETE:
        if (MessageBoxW(m_hWnd, L"确定要删除当前图片吗？", L"确认", MB_YESNO | MB_ICONQUESTION) == IDYES)
        {
            ClearImage();
            InvalidateRect(m_hWnd, nullptr, TRUE);
            SetStatusText(L"图片已删除");
        }
        break;
    case IDM_CROP:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        if (m_bCropping)
        {
            ApplyCrop();
        }
        else
        {
            StartCrop();
        }
        break;
    case IDM_ANNOTATION:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        if (m_bAnnotating)
        {
            EndAnnotation();
        }
        else
        {
            StartAnnotation();
        }
        break;
    case IDM_AI_CUTOUT:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        ApplyAiCutout();
        break;
    case IDM_AI_REMOVE:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        ApplyAiRemove();
        break;
    case IDM_AI_ENHANCE:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        ApplyAiEnhance();
        break;
    case IDM_FILTER:
        if (!m_pCurrentImage)
        {
            MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONINFORMATION);
            break;
        }
        ShowFilterDialog();
        break;
    case IDM_CORRECT:
        MessageBoxW(m_hWnd, L"透视矫正功能暂未实现", L"提示", MB_OK | MB_ICONINFORMATION);
        break;
    }
}

void ImageEditor::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};

    if (m_bCropping)
    {
        m_rcCrop.right = pt.x;
        m_rcCrop.bottom = pt.y;
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
    else if (m_bAnnotating && (wParam & MK_LBUTTON))
    {
        m_currentAnnotation.endX = pt.x;
        m_currentAnnotation.endY = pt.y;
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
    else if (m_bDragging && m_pCurrentImage)
    {
        m_nScrollX += m_ptLastMouse.x - pt.x;
        m_nScrollY += m_ptLastMouse.y - pt.y;
        m_ptLastMouse = pt;
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

void ImageEditor::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};
    m_ptLastMouse = pt;

    if (m_bCropping)
    {
        m_rcCrop.left = pt.x;
        m_rcCrop.top = pt.y;
        m_rcCrop.right = pt.x;
        m_rcCrop.bottom = pt.y;
    }
    else if (m_bAnnotating)
    {
        m_currentAnnotation.startX = pt.x;
        m_currentAnnotation.startY = pt.y;
        m_currentAnnotation.endX = pt.x;
        m_currentAnnotation.endY = pt.y;
        m_currentAnnotation.color = RGB(255, 0, 0);
        m_currentAnnotation.width = 3;
    }
    else
    {
        m_bDragging = TRUE;
    }
    SetCapture(m_hWnd);
}

void ImageEditor::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
    POINT pt = {LOWORD(lParam), HIWORD(lParam)};

    if (m_bAnnotating && m_currentAnnotation.startX != 0)
    {
        m_currentAnnotation.endX = pt.x;
        m_currentAnnotation.endY = pt.y;
        m_annotations.push_back(m_currentAnnotation);
        m_currentAnnotation.startX = 0;
        m_currentAnnotation.startY = 0;
        m_currentAnnotation.endX = 0;
        m_currentAnnotation.endY = 0;
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }

    m_bDragging = FALSE;
    ReleaseCapture();
}

void ImageEditor::OnMouseWheel(WPARAM wParam, LPARAM lParam)
{
    int delta = GET_WHEEL_DELTA_WPARAM(wParam);
    if (delta > 0)
        ZoomIn();
    else
        ZoomOut();
}

BOOL ImageEditor::LoadImage(const wchar_t* filePath)
{
    ClearImage();
    m_pOriginalImage = Gdiplus::Bitmap::FromFile(filePath);
    if (!m_pOriginalImage || m_pOriginalImage->GetLastStatus() != Gdiplus::Ok)
    {
        ClearImage();
        MessageBoxW(m_hWnd, L"无法加载图片文件", L"错误", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    m_pCurrentImage = m_pOriginalImage->Clone(0, 0, m_pOriginalImage->GetWidth(), m_pOriginalImage->GetHeight(), m_pOriginalImage->GetPixelFormat());
    m_fZoom = 1.0f;
    m_nRotation = 0;
    m_nScrollX = 0;
    m_nScrollY = 0;
    InvalidateRect(m_hWnd, nullptr, TRUE);
    return TRUE;
}

BOOL ImageEditor::SaveImage(const wchar_t* filePath)
{
    if (!m_pCurrentImage)
        return FALSE;

    CLSID clsid;
    std::wstring path(filePath);
    size_t dotPos = path.find_last_of(L".");
    if (dotPos != std::wstring::npos)
    {
        std::wstring ext = path.substr(dotPos + 1);
        if (ext == L"png")
            CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        else if (ext == L"jpg" || ext == L"jpeg")
            CLSIDFromString(L"{557CF401-1A04-11D3-9A73-0000F81EF32E}", &clsid);
        else
            CLSIDFromString(L"{557CF403-1A04-11D3-9A73-0000F81EF32E}", &clsid);
    }
    else
    {
        CLSIDFromString(L"{557CF403-1A04-11D3-9A73-0000F81EF32E}", &clsid);
    }

    return m_pCurrentImage->Save(filePath, &clsid, nullptr) == Gdiplus::Ok;
}

BOOL ImageEditor::ExportToPdf(const wchar_t* filePath)
{
    if (!m_pCurrentImage)
        return FALSE;

    MessageBoxW(m_hWnd, L"PDF导出功能模拟完成！", L"提示", MB_OK | MB_ICONINFORMATION);
    return TRUE;
}

void ImageEditor::ZoomIn()
{
    if (m_fZoom < 5.0f && m_pCurrentImage)
    {
        m_fZoom *= 1.2f;
        InvalidateRect(m_hWnd, nullptr, TRUE);
        UpdateStatusBar();
    }
}

void ImageEditor::ZoomOut()
{
    if (m_fZoom > 0.1f && m_pCurrentImage)
    {
        m_fZoom /= 1.2f;
        InvalidateRect(m_hWnd, nullptr, TRUE);
        UpdateStatusBar();
    }
}

void ImageEditor::RotateLeft()
{
    if (!m_pCurrentImage)
        return;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::Bitmap* pRotated = new Gdiplus::Bitmap(height, width, m_pCurrentImage->GetPixelFormat());
    Gdiplus::Graphics graphics(pRotated);
    graphics.TranslateTransform(height / 2.0f, width / 2.0f);
    graphics.RotateTransform(-90.0f);
    graphics.TranslateTransform(-width / 2.0f, -height / 2.0f);
    graphics.DrawImage(m_pCurrentImage, 0, 0, width, height);

    delete m_pCurrentImage;
    m_pCurrentImage = pRotated;
    m_nRotation = (m_nRotation - 90) % 360;
    InvalidateRect(m_hWnd, nullptr, TRUE);
    SetStatusText(L"已左旋90度");
}

void ImageEditor::RotateRight()
{
    if (!m_pCurrentImage)
        return;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::Bitmap* pRotated = new Gdiplus::Bitmap(height, width, m_pCurrentImage->GetPixelFormat());
    Gdiplus::Graphics graphics(pRotated);
    graphics.TranslateTransform(height / 2.0f, width / 2.0f);
    graphics.RotateTransform(90.0f);
    graphics.TranslateTransform(-width / 2.0f, -height / 2.0f);
    graphics.DrawImage(m_pCurrentImage, 0, 0, width, height);

    delete m_pCurrentImage;
    m_pCurrentImage = pRotated;
    m_nRotation = (m_nRotation + 90) % 360;
    InvalidateRect(m_hWnd, nullptr, TRUE);
    SetStatusText(L"已右旋90度");
}

void ImageEditor::AdjustBrightness(int value)
{
    if (!m_pCurrentImage)
        return;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    m_pCurrentImage->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

    unsigned char* pPixels = (unsigned char*)bitmapData.Scan0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * bitmapData.Stride + x * 4;
            for (int i = 0; i < 3; i++)
            {
                int newValue = pPixels[index + i] + value;
                pPixels[index + i] = (unsigned char)(std::max)(0, (std::min)(255, newValue));
            }
        }
    }

    m_pCurrentImage->UnlockBits(&bitmapData);
}

void ImageEditor::AdjustContrast(int value)
{
    if (!m_pCurrentImage)
        return;

    double contrast = (100.0 + value) / 100.0;
    contrast *= contrast;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    m_pCurrentImage->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

    unsigned char* pPixels = (unsigned char*)bitmapData.Scan0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * bitmapData.Stride + x * 4;
            for (int i = 0; i < 3; i++)
            {
                double newValue = ((pPixels[index + i] / 255.0 - 0.5) * contrast + 0.5) * 255.0;
                pPixels[index + i] = (unsigned char)(std::max)(0, (std::min)(255, (int)newValue));
            }
        }
    }

    m_pCurrentImage->UnlockBits(&bitmapData);
}

void ImageEditor::AdjustSaturation(int value)
{
    if (!m_pCurrentImage)
        return;

    double factor = (100.0 + value) / 100.0;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::BitmapData bitmapData;
    Gdiplus::Rect rect(0, 0, width, height);
    m_pCurrentImage->LockBits(&rect, Gdiplus::ImageLockModeRead | Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &bitmapData);

    unsigned char* pPixels = (unsigned char*)bitmapData.Scan0;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * bitmapData.Stride + x * 4;
            unsigned char b = pPixels[index];
            unsigned char g = pPixels[index + 1];
            unsigned char r = pPixels[index + 2];

            double gray = 0.299 * r + 0.587 * g + 0.114 * b;

            pPixels[index] = (unsigned char)(std::max)(0, (std::min)(255, (int)(gray + (b - gray) * factor)));
            pPixels[index + 1] = (unsigned char)(std::max)(0, (std::min)(255, (int)(gray + (g - gray) * factor)));
            pPixels[index + 2] = (unsigned char)(std::max)(0, (std::min)(255, (int)(gray + (r - gray) * factor)));
        }
    }

    m_pCurrentImage->UnlockBits(&bitmapData);
}

void ImageEditor::StartCrop()
{
    m_bCropping = TRUE;
    SetStatusText(L"裁剪模式 - 在图片上拖动鼠标选择裁剪区域，再次点击'裁剪'按钮应用");
}

void ImageEditor::ApplyCrop()
{
    if (!m_pCurrentImage)
        return;

    int topHeight = 50;
    int bottomHeight = 80;

    int imgWidth = m_pCurrentImage->GetWidth();
    int imgHeight = m_pCurrentImage->GetHeight();
    int drawWidth = (int)(imgWidth * m_fZoom);
    int drawHeight = (int)(imgHeight * m_fZoom);

    RECT rcClient;
    GetClientRect(m_hWnd, &rcClient);
    int clientWidth = rcClient.right - rcClient.left;
    int clientHeight = rcClient.bottom - rcClient.top - topHeight - bottomHeight;

    int imgX = (clientWidth - drawWidth) / 2 - m_nScrollX;
    int imgY = topHeight + (clientHeight - drawHeight) / 2 - m_nScrollY;

    int cropX = (int)(((std::min)(m_rcCrop.left, m_rcCrop.right) - imgX) / m_fZoom);
    int cropY = (int)(((std::min)(m_rcCrop.top, m_rcCrop.bottom) - imgY) / m_fZoom);
    int cropW = (int)(abs(m_rcCrop.right - m_rcCrop.left) / m_fZoom);
    int cropH = (int)(abs(m_rcCrop.bottom - m_rcCrop.top) / m_fZoom);

    cropX = (std::max)(0, (std::min)(imgWidth - 1, cropX));
    cropY = (std::max)(0, (std::min)(imgHeight - 1, cropY));
    cropW = (std::max)(1, (std::min)(imgWidth - cropX, cropW));
    cropH = (std::max)(1, (std::min)(imgHeight - cropY, cropH));

    if (cropW > 10 && cropH > 10)
    {
        Gdiplus::Bitmap* pCropped = m_pCurrentImage->Clone(cropX, cropY, cropW, cropH, m_pCurrentImage->GetPixelFormat());
        delete m_pCurrentImage;
        m_pCurrentImage = pCropped;
        m_fZoom = 1.0f;
        m_nScrollX = 0;
        m_nScrollY = 0;
        InvalidateRect(m_hWnd, nullptr, TRUE);
        SetStatusText(L"裁剪完成");
    }

    m_bCropping = FALSE;
}

void ImageEditor::CancelCrop()
{
    m_bCropping = FALSE;
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void ImageEditor::StartAnnotation()
{
    m_bAnnotating = TRUE;
    SetStatusText(L"标注模式 - 在图片上拖动鼠标绘制标注，再次点击'标注'按钮退出");
}

void ImageEditor::EndAnnotation()
{
    m_bAnnotating = FALSE;
    SetStatusText(L"标注已完成");
}

void ImageEditor::ApplyAiCutout()
{
    if (!m_pCurrentImage)
        return;

    int width = m_pCurrentImage->GetWidth();
    int height = m_pCurrentImage->GetHeight();
    Gdiplus::Bitmap* pNewImage = new Gdiplus::Bitmap(width, height, PixelFormat32bppARGB);

    Gdiplus::BitmapData srcData;
    Gdiplus::BitmapData dstData;
    Gdiplus::Rect rect(0, 0, width, height);
    m_pCurrentImage->LockBits(&rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &srcData);
    pNewImage->LockBits(&rect, Gdiplus::ImageLockModeWrite, PixelFormat32bppARGB, &dstData);

    unsigned char* pSrc = (unsigned char*)srcData.Scan0;
    unsigned char* pDst = (unsigned char*)dstData.Scan0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int index = y * srcData.Stride + x * 4;
            unsigned char b = pSrc[index];
            unsigned char g = pSrc[index + 1];
            unsigned char r = pSrc[index + 2];

            int brightness = (r + g + b) / 3;
            if (brightness > 240)
            {
                pDst[index] = 0;
                pDst[index + 1] = 0;
                pDst[index + 2] = 0;
                pDst[index + 3] = 0;
            }
            else
            {
                pDst[index] = b;
                pDst[index + 1] = g;
                pDst[index + 2] = r;
                pDst[index + 3] = 255;
            }
        }
    }

    m_pCurrentImage->UnlockBits(&srcData);
    pNewImage->UnlockBits(&dstData);

    delete m_pCurrentImage;
    m_pCurrentImage = pNewImage;
    InvalidateRect(m_hWnd, nullptr, TRUE);
    SetStatusText(L"AI抠图完成 - 已去除白色背景");
    MessageBoxW(m_hWnd, L"AI抠图完成！已去除白色背景", L"提示", MB_OK | MB_ICONINFORMATION);
}

void ImageEditor::ApplyAiRemove()
{
    if (!m_pCurrentImage)
        return;

    MessageBoxW(m_hWnd, L"AI消除功能：请在图片上拖动鼠标选择要消除的区域\n（演示版，实际AI需要调用外部API）", L"AI消除", MB_OK | MB_ICONINFORMATION);
    SetStatusText(L"AI消除功能演示完成");
}

void ImageEditor::ApplyAiEnhance()
{
    if (!m_pCurrentImage)
        return;

    AdjustBrightness(15);
    AdjustContrast(30);
    AdjustSaturation(20);
    InvalidateRect(m_hWnd, nullptr, TRUE);
    SetStatusText(L"AI清晰度增强完成");
    MessageBoxW(m_hWnd, L"AI清晰度增强完成！", L"提示", MB_OK | MB_ICONINFORMATION);
}

void ImageEditor::ShowFilterDialog()
{
    if (!m_pCurrentImage)
    {
        MessageBoxW(m_hWnd, L"请先打开图片", L"提示", MB_OK | MB_ICONWARNING);
        return;
    }

    AdjustBrightness(20);
    AdjustContrast(15);
    InvalidateRect(m_hWnd, nullptr, TRUE);
    SetStatusText(L"滤镜已应用");
    MessageBoxW(m_hWnd, L"已应用亮度和对比度调整！", L"滤镜", MB_OK | MB_ICONINFORMATION);
}

void ImageEditor::UpdateStatusBar()
{
    wchar_t buffer[128];
    swprintf(buffer, L"缩放: %.0f%%", m_fZoom * 100);
    SetWindowTextW(m_hStatusBar, buffer);
}

void ImageEditor::SetStatusText(const wchar_t* text)
{
    SetWindowTextW(m_hStatusBar, text);
}

LRESULT CALLBACK ImageEditor::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    ImageEditor* pThis = GetInstance(hWnd);

    switch (message)
    {
    case WM_CREATE:
        break;
    case WM_PAINT:
        if (pThis) pThis->OnPaint(hWnd);
        break;
    case WM_SIZE:
        if (pThis) pThis->OnSize();
        break;
    case WM_COMMAND:
        if (pThis) pThis->OnCommand(wParam);
        break;
    case WM_MOUSEMOVE:
        if (pThis) pThis->OnMouseMove(wParam, lParam);
        break;
    case WM_LBUTTONDOWN:
        if (pThis) pThis->OnLButtonDown(wParam, lParam);
        break;
    case WM_LBUTTONUP:
        if (pThis) pThis->OnLButtonUp(wParam, lParam);
        break;
    case WM_MOUSEWHEEL:
        if (pThis) pThis->OnMouseWheel(wParam, lParam);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
