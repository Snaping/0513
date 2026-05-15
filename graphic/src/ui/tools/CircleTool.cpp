#include "ui/tools/CircleTool.h"
#include "graphics/Circle.h"
#include "core/Color.h"

namespace GEngine {

CircleTool::CircleTool()
    : m_state(CircleState::Idle)
    , m_radius(0.0)
{
}

void CircleTool::activate() {
    Tool::activate();
    m_state = CircleState::Idle;
}

void CircleTool::deactivate() {
    Tool::deactivate();
    m_state = CircleState::Idle;
}

void CircleTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                            MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == CircleState::Idle) {
        m_center = worldPos;
        m_radius = 0.0;
        m_state = CircleState::SettingRadius;
    }
}

void CircleTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                            Modifiers mods) {
    if (m_state == CircleState::SettingRadius) {
        m_radius = m_center.distanceTo(worldPos);
    }
}

void CircleTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                          MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == CircleState::SettingRadius && m_radius > 0.5) {
        m_radius = m_center.distanceTo(worldPos);
        auto circle = std::make_unique<Circle>(m_center, m_radius);
        circle->setColor(Color::Black);
        circle->setLineWidth(2.0);
        m_document->addShape(std::move(circle));
        m_state = CircleState::Idle;
    }
}

void CircleTool::onKeyDown(Key key, Modifiers mods) {
    if (key == Key::Escape) {
        m_state = CircleState::Idle;
    }
}

void CircleTool::renderPreview(Renderer* renderer) {
    if (!renderer || m_state != CircleState::SettingRadius || !m_canvas) return;
    
    renderer->pushTransform();
    renderer->scale(m_canvas->getZoom(), m_canvas->getZoom());
    renderer->translate(m_canvas->getPan().x, m_canvas->getPan().y);
    
    renderer->drawCircle(m_center, m_radius, Color::Red, 2.0);
    renderer->drawPoint(m_center, Color::Blue, 5.0);
    
    renderer->popTransform();
}

} 
