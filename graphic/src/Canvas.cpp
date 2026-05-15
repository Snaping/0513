#include "Canvas.h"
#include <cmath>

namespace GEngine {

Canvas::Canvas()
    : m_width(800)
    , m_height(600)
    , m_zoom(1.0)
    , m_pan(Point2D(0, 0))
{
}

void Canvas::setSize(int width, int height) {
    m_width = width;
    m_height = height;
}

Point2D Canvas::screenToWorld(const Point2D& screenPoint) const {
    return Point2D(
        (screenPoint.x - m_pan.x) / m_zoom,
        (screenPoint.y - m_pan.y) / m_zoom
    );
}

Point2D Canvas::worldToScreen(const Point2D& worldPoint) const {
    return Point2D(
        worldPoint.x * m_zoom + m_pan.x,
        worldPoint.y * m_zoom + m_pan.y
    );
}

void Canvas::pan(double dx, double dy) {
    m_pan = Point2D(m_pan.x + dx, m_pan.y + dy);
}

void Canvas::zoom(double factor, const Point2D& center) {
    Point2D worldCenter = screenToWorld(center);
    
    double newZoom = m_zoom * factor;
    if (newZoom < 0.01) newZoom = 0.01;
    if (newZoom > 100.0) newZoom = 100.0;
    
    m_pan = Point2D(
        center.x - worldCenter.x * newZoom,
        center.y - worldCenter.y * newZoom
    );
    
    m_zoom = newZoom;
}

void Canvas::zoomIn() {
    zoom(1.25, Point2D(m_width / 2.0, m_height / 2.0));
}

void Canvas::zoomOut() {
    zoom(0.8, Point2D(m_width / 2.0, m_height / 2.0));
}

void Canvas::resetView() {
    m_zoom = 1.0;
    m_pan = Point2D(0, 0);
}

void Canvas::render(Renderer* renderer) {
    if (!renderer) return;
    
    renderer->pushTransform();
    renderer->scale(m_zoom, m_zoom);
    renderer->translate(m_pan.x, m_pan.y);
    
    if (m_document) {
        auto shapes = m_document->getAllShapes();
        for (const auto& shape : shapes) {
            renderer->drawShape(shape);
        }
    }
    
    renderer->popTransform();
}

Matrix3x3 Canvas::getViewTransform() const {
    return Matrix3x3::translation(m_pan.x, m_pan.y) *
           Matrix3x3::scaling(m_zoom, m_zoom);
}

Matrix3x3 Canvas::getInverseViewTransform() const {
    double invZoom = 1.0 / m_zoom;
    return Matrix3x3::scaling(invZoom, invZoom) *
           Matrix3x3::translation(-m_pan.x, -m_pan.y);
}

} 
