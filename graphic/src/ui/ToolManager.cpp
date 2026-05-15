#include "ui/ToolManager.h"

namespace GEngine {

ToolManager::ToolManager()
    : m_activeTool(nullptr)
    , m_activeToolType(ToolType::Select)
{
}

ToolManager::~ToolManager() = default;

void ToolManager::registerTool(std::unique_ptr<Tool> tool) {
    if (tool) {
        ToolType type = tool->getType();
        tool->setManager(this);
        tool->setCanvas(m_canvas);
        tool->setDocument(m_document);
        m_tools[type] = std::move(tool);
    }
}

void ToolManager::setActiveTool(ToolType type) {
    if (m_activeTool) {
        m_activeTool->deactivate();
    }
    
    auto it = m_tools.find(type);
    if (it != m_tools.end()) {
        m_activeTool = it->second.get();
        m_activeToolType = type;
        m_activeTool->activate();
    }
}

Tool* ToolManager::getTool(ToolType type) {
    auto it = m_tools.find(type);
    if (it != m_tools.end()) {
        return it->second.get();
    }
    return nullptr;
}

void ToolManager::setCanvas(std::shared_ptr<Canvas> canvas) {
    m_canvas = canvas;
    for (auto& [type, tool] : m_tools) {
        tool->setCanvas(canvas);
    }
}

void ToolManager::setDocument(std::shared_ptr<Document> doc) {
    m_document = doc;
    for (auto& [type, tool] : m_tools) {
        tool->setDocument(doc);
    }
}

void ToolManager::onMouseDown(const Point2D& screenPos, MouseButton button, Modifiers mods) {
    if (m_activeTool && m_canvas) {
        Point2D worldPos = m_canvas->screenToWorld(screenPos);
        m_activeTool->onMouseDown(screenPos, worldPos, button, mods);
    }
}

void ToolManager::onMouseUp(const Point2D& screenPos, MouseButton button, Modifiers mods) {
    if (m_activeTool && m_canvas) {
        Point2D worldPos = m_canvas->screenToWorld(screenPos);
        m_activeTool->onMouseUp(screenPos, worldPos, button, mods);
    }
}

void ToolManager::onMouseMove(const Point2D& screenPos, Modifiers mods) {
    if (m_activeTool && m_canvas) {
        Point2D worldPos = m_canvas->screenToWorld(screenPos);
        m_activeTool->onMouseMove(screenPos, worldPos, mods);
    }
}

void ToolManager::onMouseWheel(double delta, const Point2D& screenPos, Modifiers mods) {
    if (m_activeTool && m_canvas) {
        Point2D worldPos = m_canvas->screenToWorld(screenPos);
        m_activeTool->onMouseWheel(delta, screenPos, worldPos, mods);
    }
}

void ToolManager::onKeyDown(Key key, Modifiers mods) {
    if (m_activeTool) {
        m_activeTool->onKeyDown(key, mods);
    }
}

void ToolManager::onKeyUp(Key key, Modifiers mods) {
    if (m_activeTool) {
        m_activeTool->onKeyUp(key, mods);
    }
}

void ToolManager::renderPreview(Renderer* renderer) {
    if (m_activeTool) {
        m_activeTool->renderPreview(renderer);
    }
}

void ToolManager::update() {
    if (m_activeTool) {
        m_activeTool->update();
    }
}

} 
