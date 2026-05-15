#pragma once

#include "Application.h"
#include "renderer/GDIRenderer.h"
#include "ui/ToolManager.h"
#include <windows.h>
#include <commctrl.h>
#include <memory>
#include <string>
#include <vector>

namespace GEngine {

class Win32Window {
public:
    Win32Window();
    ~Win32Window();

    bool create(int width, int height, const std::string& title);
    void destroy();
    
    void show();
    void hide();
    
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    HWND getHwnd() const { return m_hwnd; }
    HDC getHDC() const { return m_hdc; }
    std::shared_ptr<GDIRenderer> getRenderer() { return m_renderer; }
    
    void setApplication(std::shared_ptr<Application> app);
    std::shared_ptr<Application> getApplication() const { return m_application; }
    
    int getToolbarHeight() const { return 40; }
    int getStatusbarHeight() const { return 28; }
    int getToolbarWidth() const { return 48; }
    
    int runMessageLoop();
    void processEvents();

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT msg, WPARAM wParam, LPARAM lParam);
    
    void registerWindowClass();
    void createToolbar();
    void createStatusbar();
    void updateStatusbar(const std::wstring& text);
    void drawToolbar(HDC hdc);
    void handleToolbarClick(int x, int y);
    void resizeWindows();
    
    HWND m_hwnd;
    HWND m_hwndToolbar;
    HWND m_hwndStatusbar;
    HDC m_hdc;
    int m_width;
    int m_height;
    int m_clientWidth;
    int m_clientHeight;
    std::string m_title;
    std::shared_ptr<Application> m_application;
    std::shared_ptr<GDIRenderer> m_renderer;
    bool m_isRunning;
    
    int m_hoveredTool;
    int m_selectedTool;
};

} 
