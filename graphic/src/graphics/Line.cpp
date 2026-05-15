#include "Line.h"
#include "renderer/Renderer.h"
#include "core/Vector2D.h"
#include <cmath>

namespace GEngine {

Line::Line()
    : m_start(Point2D(0, 0))
    , m_end(Point2D(100, 100))
{
}

Line::Line(const Point2D& start, const Point2D& end)
    : m_start(start)
    , m_end(end)
{
}

void Line::draw(Renderer* renderer) const {
    if (!m_isVisible || !renderer) return;

    Color drawColor = m_isSelected ? Color::Red : m_color;
    renderer->drawLine(m_start, m_end, drawColor, m_lineWidth);

    if (m_isSelected) {
        renderer->drawPoint(m_start, Color::Blue, 5.0);
        renderer->drawPoint(m_end, Color::Blue, 5.0);
    }
}

bool Line::contains(const Point2D& point, double tolerance) const {
    double dist = distanceToPoint(point);
    return dist <= tolerance;
}

bool Line::intersects(const Point2D& p1, const Point2D& p2) const {
    Vector2D v1(m_start, m_end);
    Vector2D v2(p1, p2);
    Vector2D v3(m_start, p1);

    double cross1 = v1.cross(v2);
    if (std::abs(cross1) < 1e-10) return false;

    double t = v3.cross(v2) / cross1;
    double u = v3.cross(v1) / cross1;

    return t >= 0 && t <= 1 && u >= 0 && u <= 1;
}

void Line::translate(double dx, double dy) {
    m_start = Point2D(m_start.x + dx, m_start.y + dy);
    m_end = Point2D(m_end.x + dx, m_end.y + dy);
}

void Line::rotate(const Point2D& center, double angle) {
    Matrix3x3 rot = Matrix3x3::translation(-center.x, -center.y) *
                    Matrix3x3::rotation(angle) *
                    Matrix3x3::translation(center.x, center.y);
    m_start = rot.transform(m_start);
    m_end = rot.transform(m_end);
}

void Line::scale(const Point2D& center, double sx, double sy) {
    Matrix3x3 scale = Matrix3x3::translation(-center.x, -center.y) *
                       Matrix3x3::scaling(sx, sy) *
                       Matrix3x3::translation(center.x, center.y);
    m_start = scale.transform(m_start);
    m_end = scale.transform(m_end);
}

Point2D Line::getCenter() const {
    return Point2D((m_start.x + m_end.x) / 2, (m_start.y + m_end.y) / 2);
}

double Line::getBoundingRadius() const {
    return getLength() / 2.0;
}

std::unique_ptr<Shape> Line::clone() const {
    auto line = std::make_unique<Line>(m_start, m_end);
    line->setColor(m_color);
    line->setLineWidth(m_lineWidth);
    line->setSelected(m_isSelected);
    line->setVisible(m_isVisible);
    line->setId(m_id);
    return line;
}

double Line::getLength() const {
    return m_start.distanceTo(m_end);
}

double Line::distanceToPoint(const Point2D& p) const {
    Vector2D v(m_start, m_end);
    Vector2D w(m_start, p);

    double c1 = v.dot(w);
    if (c1 <= 0) return p.distanceTo(m_start);

    double c2 = v.dot(v);
    if (c2 <= c1) return p.distanceTo(m_end);

    double b = c1 / c2;
    Point2D proj(m_start.x + b * v.x, m_start.y + b * v.y);
    return p.distanceTo(proj);
}

} 
