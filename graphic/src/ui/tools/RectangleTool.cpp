#include "ui/tools/RectangleTool.h"
#include "graphics/Rectangle.h"
#include "core/Color.h"
#include <algorithm>

namespace GEngine {

RectangleTool::RectangleTool()
    : m_state(RectangleState::Idle)
{
}

void RectangleTool::activate() {
    Tool::activate();
    m_state = RectangleState::Idle;
}

void RectangleTool::deactivate() {
    Tool::deactivate();
    m_state = RectangleState::Idle;
}

void RectangleTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                               MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == RectangleState::Idle) {
        m_startPoint = worldPos;
        m_currentPoint = worldPos;
        m_state = RectangleState::Drawing;
    }
}

void RectangleTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                               Modifiers mods) {
    if (m_state == RectangleState::Drawing) {
        if (mods.shift) {
            double dx = worldPos.x - m_startPoint.x;
            double dy = worldPos.y - m_startPoint.y;
            double size = std::max(std::abs(dx), std::abs(dy));
            double signX = (dx >= 0) ? 1.0 : -1.0;
            double signY = (dy >= 0) ? 1.0 : -1.0;
            
            m_currentPoint = Point2D(
                m_startPoint.x + size * signX,
                m_startPoint.y + size * signY
            );
        } else {
            m_currentPoint = worldPos;
        }
    }
}

void RectangleTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                             MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == RectangleState::Drawing) {
        double width = std::abs(m_currentPoint.x - m_startPoint.x);
        double height = std::abs(m_currentPoint.y - m_startPoint.y);
        
        if (width > 1.0 && height > 1.0) {
            auto rect = std::make_unique<Rectangle>(m_startPoint, m_currentPoint);
            rect->setColor(Color::Black);
            rect->setLineWidth(2.0);
            m_document->addShape(std::move(rect));
        }
        m_state = RectangleState::Idle;
    }
}

void RectangleTool::onKeyDown(Key key, Modifiers mods) {
    if (key == Key::Escape) {
        m_state = RectangleState::Idle;
    }
}

void RectangleTool::renderPreview(Renderer* renderer) {
    if (!renderer || m_state != RectangleState::Drawing || !m_canvas) return;
    
    renderer->pushTransform();
    renderer->scale(m_canvas->getZoom(), m_canvas->getZoom());
    renderer->translate(m_canvas->getPan().x, m_canvas->getPan().y);
    
    double minX = std::min(m_startPoint.x, m_currentPoint.x);
    double minY = std::min(m_startPoint.y, m_currentPoint.y);
    double maxX = std::max(m_startPoint.x, m_currentPoint.x);
    double maxY = std::max(m_startPoint.y, m_currentPoint.y);
    
    renderer->drawRectangle(
        Point2D(minX, minY),
        Point2D(maxX, maxY),
        Color::Red,
        2.0
    );
    
    renderer->drawPoint(Point2D(minX, minY), Color::Blue, 5.0);
    renderer->drawPoint(Point2D(maxX, maxY), Color::Green, 5.0);
    
    renderer->popTransform();
}

} 
