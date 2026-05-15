#include "ui/tools/LineTool.h"
#include "graphics/Line.h"
#include "core/Color.h"
#include <cmath>

namespace GEngine {

LineTool::LineTool()
    : m_state(LineState::Idle)
{
}

void LineTool::activate() {
    Tool::activate();
    m_state = LineState::Idle;
}

void LineTool::deactivate() {
    Tool::deactivate();
    m_state = LineState::Idle;
}

void LineTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                          MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == LineState::Idle) {
        m_startPoint = worldPos;
        m_currentPoint = worldPos;
        m_state = LineState::Drawing;
    } else if (m_state == LineState::Drawing) {
        auto line = std::make_unique<Line>(m_startPoint, worldPos);
        line->setColor(Color::Black);
        line->setLineWidth(2.0);
        m_document->addShape(std::move(line));
        
        if (mods.control) {
            m_startPoint = worldPos;
            m_currentPoint = worldPos;
        } else {
            m_state = LineState::Idle;
        }
    }
}

void LineTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                          Modifiers mods) {
    if (m_state == LineState::Drawing) {
        if (mods.shift) {
            double dx = worldPos.x - m_startPoint.x;
            double dy = worldPos.y - m_startPoint.y;
            double angle = std::atan2(dy, dx);
            double step = 3.14159265358979323846 / 12.0;
            double snappedAngle = std::round(angle / step) * step;
            double length = std::sqrt(dx * dx + dy * dy);
            
            m_currentPoint = Point2D(
                m_startPoint.x + length * std::cos(snappedAngle),
                m_startPoint.y + length * std::sin(snappedAngle)
            );
        } else {
            m_currentPoint = worldPos;
        }
    }
}

void LineTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                        MouseButton button, Modifiers mods) {
}

void LineTool::onKeyDown(Key key, Modifiers mods) {
    if (key == Key::Escape && m_state == LineState::Drawing) {
        m_state = LineState::Idle;
    }
}

void LineTool::renderPreview(Renderer* renderer) {
    if (!renderer || m_state != LineState::Drawing || !m_canvas) return;
    
    renderer->pushTransform();
    renderer->scale(m_canvas->getZoom(), m_canvas->getZoom());
    renderer->translate(m_canvas->getPan().x, m_canvas->getPan().y);
    
    renderer->drawLine(m_startPoint, m_currentPoint, Color::Red, 2.0);
    renderer->drawPoint(m_startPoint, Color::Blue, 5.0);
    renderer->drawPoint(m_currentPoint, Color::Green, 5.0);
    
    renderer->popTransform();
}

} 
