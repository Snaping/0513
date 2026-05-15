#include "ui/tools/ArcTool.h"
#include "graphics/Arc.h"
#include "core/Color.h"

namespace GEngine {

constexpr double PI = 3.14159265358979323846;

ArcTool::ArcTool()
    : m_state(ArcState::Idle)
{
}

void ArcTool::activate() {
    Tool::activate();
    m_state = ArcState::Idle;
}

void ArcTool::deactivate() {
    Tool::deactivate();
    m_state = ArcState::Idle;
}

void ArcTool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                         MouseButton button, Modifiers mods) {
    if (button != MouseButton::Left || !m_document) return;
    
    if (m_state == ArcState::Idle) {
        m_startPoint = worldPos;
        m_state = ArcState::MidPoint;
    } else if (m_state == ArcState::MidPoint) {
        m_midPoint = worldPos;
        m_state = ArcState::EndPoint;
    } else if (m_state == ArcState::EndPoint) {
        auto arc = std::make_unique<Arc>(m_startPoint, m_midPoint, worldPos);
        arc->setColor(Color::Black);
        arc->setLineWidth(2.0);
        m_document->addShape(std::move(arc));
        m_state = ArcState::Idle;
    }
}

void ArcTool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                         Modifiers mods) {
    if (m_state == ArcState::MidPoint) {
        m_midPoint = worldPos;
    } else if (m_state == ArcState::EndPoint) {
        m_endPoint = worldPos;
    }
}

void ArcTool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                       MouseButton button, Modifiers mods) {
}

void ArcTool::onKeyDown(Key key, Modifiers mods) {
    if (key == Key::Escape) {
        m_state = ArcState::Idle;
    }
}

void ArcTool::renderPreview(Renderer* renderer) {
    if (!renderer || !m_canvas) return;
    
    renderer->pushTransform();
    renderer->scale(m_canvas->getZoom(), m_canvas->getZoom());
    renderer->translate(m_canvas->getPan().x, m_canvas->getPan().y);
    
    if (m_state >= ArcState::MidPoint) {
        renderer->drawPoint(m_startPoint, Color::Blue, 5.0);
        
        if (m_state >= ArcState::EndPoint) {
            renderer->drawPoint(m_midPoint, Color::Yellow, 5.0);
            renderer->drawPoint(m_endPoint, Color::Green, 5.0);
            
            Arc preview(m_startPoint, m_midPoint, m_endPoint);
            renderer->drawArc(
                preview.getCenter(),
                preview.getRadius(),
                preview.getStartAngle(),
                preview.getEndAngle(),
                preview.isClockwise(),
                Color::Red,
                2.0
            );
            renderer->drawPoint(preview.getCenter(), Color::Magenta, 5.0);
        } else {
            renderer->drawPoint(m_midPoint, Color::Yellow, 5.0);
            renderer->drawLine(m_startPoint, m_midPoint, Color::Red, 2.0);
        }
    }
    
    renderer->popTransform();
}

} 
