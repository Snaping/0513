#include "ui/tools/PolylineTool.h"
#include "graphics/Polyline.h"
#include "core/Color.h"

namespace GEngine {

PolylineTool::PolylineTool()
    : m_state(PolylineState::Idle)
{
}

void PolylineTool::activate() {
    Tool::activate();
    m_state = PolylineState::Idle;
    m_points.clear();
}

void PolylineTool::deactivate() {
    Tool::deactivate();
    m_state = PolylineState::Idle;
    m_points.clear();
}

void PolylineTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                              MouseButton button, Modifiers mods) {
    if (button == MouseButton::Left && m_document) {
        if (m_state == PolylineState::Idle) {
            m_points.clear();
            m_points.push_back(worldPos);
            m_currentPoint = worldPos;
            m_state = PolylineState::Drawing;
        } else if (m_state == PolylineState::Drawing) {
            if (!m_points.empty() && worldPos.distanceTo(m_points.front()) < 10.0 / m_canvas->getZoom()) {
                if (m_points.size() >= 3) {
                    auto polyline = std::make_unique<Polyline>(m_points);
                    polyline->setColor(Color::Black);
                    polyline->setLineWidth(2.0);
                    polyline->setClosed(true);
                    m_document->addShape(std::move(polyline));
                }
                m_state = PolylineState::Idle;
                m_points.clear();
            } else {
                m_points.push_back(worldPos);
            }
        }
    } else if (button == MouseButton::Right && m_document) {
        if (m_state == PolylineState::Drawing && m_points.size() >= 2) {
            auto polyline = std::make_unique<Polyline>(m_points);
            polyline->setColor(Color::Black);
            polyline->setLineWidth(2.0);
            polyline->setClosed(false);
            m_document->addShape(std::move(polyline));
        }
        m_state = PolylineState::Idle;
        m_points.clear();
    }
}

void PolylineTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                              Modifiers mods) {
    if (m_state == PolylineState::Drawing) {
        m_currentPoint = worldPos;
    }
}

void PolylineTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                            MouseButton button, Modifiers mods) {
}

void PolylineTool::onKeyDown(Key key, Modifiers mods) {
    if (key == Key::Escape) {
        m_state = PolylineState::Idle;
        m_points.clear();
    } else if (key == Key::Enter && m_document) {
        if (m_state == PolylineState::Drawing && m_points.size() >= 2) {
            auto polyline = std::make_unique<Polyline>(m_points);
            polyline->setColor(Color::Black);
            polyline->setLineWidth(2.0);
            m_document->addShape(std::move(polyline));
        }
        m_state = PolylineState::Idle;
        m_points.clear();
    }
}

void PolylineTool::renderPreview(Renderer* renderer) {
    if (!renderer || m_state != PolylineState::Drawing || !m_canvas) return;
    
    if (m_points.empty()) return;
    
    renderer->pushTransform();
    renderer->scale(m_canvas->getZoom(), m_canvas->getZoom());
    renderer->translate(m_canvas->getPan().x, m_canvas->getPan().y);
    
    for (size_t i = 0; i < m_points.size(); ++i) {
        renderer->drawPoint(m_points[i], Color::Blue, 5.0);
    }
    
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        renderer->drawLine(m_points[i], m_points[i + 1], Color::Red, 2.0);
    }
    
    if (!m_points.empty()) {
        renderer->drawLine(m_points.back(), m_currentPoint, Color::Green, 2.0);
        renderer->drawPoint(m_currentPoint, Color::Green, 5.0);
    }
    
    renderer->popTransform();
}

} 
