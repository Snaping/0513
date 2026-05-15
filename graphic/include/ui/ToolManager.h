#pragma once

#include "ui/Tool.h"
#include "Canvas.h"
#include "Document.h"
#include "renderer/Renderer.h"
#include <memory>
#include <map>
#include <vector>

namespace GEngine {

class ToolManager {
public:
    ToolManager();
    ~ToolManager();

    void registerTool(std::unique_ptr<Tool> tool);
    
    void setActiveTool(ToolType type);
    Tool* getActiveTool() const { return m_activeTool; }
    ToolType getActiveToolType() const { return m_activeToolType; }
    
    Tool* getTool(ToolType type);
    
    void setCanvas(std::shared_ptr<Canvas> canvas);
    void setDocument(std::shared_ptr<Document> doc);
    
    void onMouseDown(const Point2D& screenPos, MouseButton button, Modifiers mods);
    void onMouseUp(const Point2D& screenPos, MouseButton button, Modifiers mods);
    void onMouseMove(const Point2D& screenPos, Modifiers mods);
    void onMouseWheel(double delta, const Point2D& screenPos, Modifiers mods);
    
    void onKeyDown(Key key, Modifiers mods);
    void onKeyUp(Key key, Modifiers mods);
    
    void renderPreview(Renderer* renderer);
    void update();

private:
    std::map<ToolType, std::unique_ptr<Tool>> m_tools;
    Tool* m_activeTool;
    ToolType m_activeToolType;
    std::shared_ptr<Canvas> m_canvas;
    std::shared_ptr<Document> m_document;
};

} 
