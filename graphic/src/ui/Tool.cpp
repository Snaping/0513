#include "ui/Tool.h"

namespace GEngine {

Tool::Tool()
    : m_manager(nullptr)
    , m_isDragging(false)
{
}

void Tool::activate() {
    m_isDragging = false;
}

void Tool::deactivate() {
    m_isDragging = false;
}

void Tool::onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                      MouseButton button, Modifiers mods) {
    m_lastScreenPos = screenPos;
    m_lastWorldPos = worldPos;
    m_isDragging = true;
}

void Tool::onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                    MouseButton button, Modifiers mods) {
    m_lastScreenPos = screenPos;
    m_lastWorldPos = worldPos;
    m_isDragging = false;
}

void Tool::onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                      Modifiers mods) {
    m_lastScreenPos = screenPos;
    m_lastWorldPos = worldPos;
}

void Tool::onMouseWheel(double delta, const Point2D& screenPos, 
                       const Point2D& worldPos, Modifiers mods) {
}

void Tool::onKeyDown(Key key, Modifiers mods) {
}

void Tool::onKeyUp(Key key, Modifiers mods) {
}

void Tool::renderPreview(Renderer* renderer) {
}

void Tool::update() {
}

} 
