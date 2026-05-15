#pragma once

#include "core/Point2D.h"
#include "ui/InputHandler.h"
#include "Canvas.h"
#include "Document.h"
#include <memory>
#include <string>

namespace GEngine {

class Renderer;
class ToolManager;

enum class ToolType {
    Select,
    Line,
    Circle,
    Rectangle,
    Arc,
    Polyline,
    Pan,
    Zoom
};

class Tool {
public:
    Tool();
    virtual ~Tool() = default;

    virtual ToolType getType() const = 0;
    virtual std::string getName() const = 0;
    
    virtual void activate();
    virtual void deactivate();
    
    virtual void onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                            MouseButton button, Modifiers mods);
    virtual void onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                          MouseButton button, Modifiers mods);
    virtual void onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                            Modifiers mods);
    virtual void onMouseWheel(double delta, const Point2D& screenPos, 
                             const Point2D& worldPos, Modifiers mods);
    
    virtual void onKeyDown(Key key, Modifiers mods);
    virtual void onKeyUp(Key key, Modifiers mods);
    
    virtual void renderPreview(Renderer* renderer);
    virtual void update();
    
    void setManager(ToolManager* manager) { m_manager = manager; }
    ToolManager* getManager() const { return m_manager; }
    
    void setCanvas(std::shared_ptr<Canvas> canvas) { m_canvas = canvas; }
    std::shared_ptr<Canvas> getCanvas() const { return m_canvas; }
    
    void setDocument(std::shared_ptr<Document> doc) { m_document = doc; }
    std::shared_ptr<Document> getDocument() const { return m_document; }

protected:
    ToolManager* m_manager;
    std::shared_ptr<Canvas> m_canvas;
    std::shared_ptr<Document> m_document;
    
    Point2D m_lastScreenPos;
    Point2D m_lastWorldPos;
    bool m_isDragging;
};

} 
