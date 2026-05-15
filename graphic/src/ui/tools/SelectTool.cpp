#include "ui/tools/SelectTool.h"
#include "graphics/Shape.h"
#include "core/Color.h"
#include <algorithm>

namespace GEngine {

SelectTool::SelectTool()
    : m_selectionMode(SelectionMode::None)
    , m_moved(false)
{
}

void SelectTool::activate() {
    Tool::activate();
    m_selectionMode = SelectionMode::None;
}

void SelectTool::deactivate() {
    Tool::deactivate();
    m_selectionMode = SelectionMode::None;
}

void SelectTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                            MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    m_dragStartScreen = screenPos;
    m_dragStartWorld = worldPos;
    m_dragCurrentScreen = screenPos;
    m_dragCurrentWorld = worldPos;
    m_lastMovePos = worldPos;
    m_moved = false;
    
    Shape* shape = m_document->findShapeAt(worldPos, 5.0 / m_canvas->getZoom());
    
    if (mods.shift) {
        m_initialSelection = m_document->getSelectedShapes();
    } else {
        m_initialSelection.clear();
    }
    
    if (shape) {
        if (mods.shift) {
            if (shape->isSelected()) {
                m_document->deselectShape(shape);
            } else {
                m_document->selectShape(shape);
            }
        } else {
            m_document->deselectAll();
            m_document->selectShape(shape);
        }
        m_selectionMode = SelectionMode::Move;
    } else {
        if (!mods.shift) {
            m_document->deselectAll();
        }
        m_selectionMode = SelectionMode::DragSelect;
    }
}

void SelectTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                            Modifiers mods) {
    if (!m_isDragging || !m_document) return;
    
    m_dragCurrentScreen = screenPos;
    m_dragCurrentWorld = worldPos;
    
    if (m_selectionMode == SelectionMode::Move) {
        double dx = worldPos.x - m_lastMovePos.x;
        double dy = worldPos.y - m_lastMovePos.y;
        
        if (std::abs(dx) > 0.1 || std::abs(dy) > 0.1) {
            m_document->translateSelected(dx, dy);
            m_lastMovePos = worldPos;
            m_moved = true;
        }
    }
}

void SelectTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                          MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_selectionMode == SelectionMode::DragSelect) {
        Point2D p1 = m_dragStartWorld;
        Point2D p2 = worldPos;
        
        double dist = m_dragStartScreen.distanceTo(screenPos);
        if (dist > 5.0) {
            auto shapes = m_document->findShapesInRect(p1, p2);
            if (mods.shift) {
                for (auto shape : shapes) {
                    m_document->selectShape(shape);
                }
            } else {
                m_document->deselectAll();
                for (auto shape : shapes) {
                    m_document->selectShape(shape);
                }
            }
        }
    }
    
    m_selectionMode = SelectionMode::None;
}

void SelectTool::onKeyDown(Key key, Modifiers mods) {
    if (!m_document) return;
    
    if (key == Key::Delete || key == Key::Backspace) {
        m_document->deleteSelected();
    } else if (key == Key::A && mods.control) {
        m_document->selectAll();
    } else if (key == Key::Escape) {
        m_document->deselectAll();
    }
}

void SelectTool::renderPreview(Renderer* renderer) {
    if (!renderer || !m_canvas) return;
    
    if (m_selectionMode == SelectionMode::DragSelect) {
        double minX = std::min(m_dragStartScreen.x, m_dragCurrentScreen.x);
        double minY = std::min(m_dragStartScreen.y, m_dragCurrentScreen.y);
        double maxX = std::max(m_dragStartScreen.x, m_dragCurrentScreen.x);
        double maxY = std::max(m_dragStartScreen.y, m_dragCurrentScreen.y);
        
        renderer->drawRectangle(
            Point2D(minX, minY),
            Point2D(maxX, maxY),
            Color::Blue,
            1.0
        );
    }
}

} 
