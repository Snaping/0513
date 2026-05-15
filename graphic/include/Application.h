#pragma once

#include "Canvas.h"
#include "Document.h"
#include "ui/ToolManager.h"
#include "renderer/Renderer.h"
#include <memory>
#include <string>

namespace GEngine {

class Application : public std::enable_shared_from_this<Application> {
public:
    Application();
    ~Application();

    bool initialize(int argc, char* argv[]);
    void run();
    void shutdown();
    
    void setWindowTitle(const std::string& title);
    void setWindowSize(int width, int height);
    
    int getWindowWidth() const { return m_windowWidth; }
    int getWindowHeight() const { return m_windowHeight; }
    
    std::shared_ptr<Canvas> getCanvas() { return m_canvas; }
    std::shared_ptr<Document> getDocument() { return m_document; }
    std::shared_ptr<ToolManager> getToolManager() { return m_toolManager; }
    std::shared_ptr<Renderer> getRenderer() { return m_renderer; }
    
    void setRenderer(std::shared_ptr<Renderer> renderer) { m_renderer = renderer; }
    
    void setActiveTool(ToolType type);
    ToolType getActiveToolType() const;
    
    void onResize(int width, int height);
    void onRender();
    void onUpdate();
    
    void onMouseDown(int x, int y, int button, int mods);
    void onMouseUp(int x, int y, int button, int mods);
    void onMouseMove(int x, int y, int mods);
    void onMouseWheel(double delta, int x, int y, int mods);
    void onKeyDown(int key, int mods, bool isRepeat);
    void onKeyUp(int key, int mods);
    
    bool shouldClose() const { return m_shouldClose; }
    void requestClose() { m_shouldClose = true; }

private:
    void initializeTools();
    void createDefaultShapes();
    
    int m_windowWidth;
    int m_windowHeight;
    std::string m_windowTitle;
    bool m_shouldClose;
    bool m_isInitialized;
    
    std::shared_ptr<Canvas> m_canvas;
    std::shared_ptr<Document> m_document;
    std::shared_ptr<ToolManager> m_toolManager;
    std::shared_ptr<Renderer> m_renderer;
};

} 
