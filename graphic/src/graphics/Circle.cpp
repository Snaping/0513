#include "Circle.h"
#include "renderer/Renderer.h"
#include <cmath>

namespace GEngine {

Circle::Circle()
    : m_center(Point2D(0, 0))
    , m_radius(50.0)
{
}

Circle::Circle(const Point2D& center, double radius)
    : m_center(center)
    , m_radius(std::abs(radius))
{
}

Circle::Circle(const Point2D& center, const Point2D& pointOnCircle)
    : m_center(center)
    , m_radius(center.distanceTo(pointOnCircle))
{
}

void Circle::draw(Renderer* renderer) const {
    if (!m_isVisible || !renderer) return;

    Color drawColor = m_isSelected ? Color::Red : m_color;
    renderer->drawCircle(m_center, m_radius, drawColor, m_lineWidth);

    if (m_isSelected) {
        renderer->drawPoint(m_center, Color::Blue, 5.0);
        renderer->drawPoint(Point2D(m_center.x + m_radius, m_center.y), Color::Green, 5.0);
    }
}

bool Circle::contains(const Point2D& point, double tolerance) const {
    double dist = m_center.distanceTo(point);
    return std::abs(dist - m_radius) <= tolerance;
}

bool Circle::intersects(const Point2D& p1, const Point2D& p2) const {
    Vector2D d(p1, p2);
    Vector2D f(p1, m_center);
    
    double a = d.dot(d);
    double b = 2 * f.dot(d);
    double c = f.dot(f) - m_radius * m_radius;
    
    double discriminant = b * b - 4 * a * c;
    if (discriminant < 0) return false;
    
    discriminant = std::sqrt(discriminant);
    double t1 = (-b - discriminant) / (2 * a);
    double t2 = (-b + discriminant) / (2 * a);
    
    return (t1 >= 0 && t1 <= 1) || (t2 >= 0 && t2 <= 1);
}

void Circle::translate(double dx, double dy) {
    m_center = Point2D(m_center.x + dx, m_center.y + dy);
}

void Circle::rotate(const Point2D& center, double angle) {
    Matrix3x3 rot = Matrix3x3::translation(-center.x, -center.y) *
                    Matrix3x3::rotation(angle) *
                    Matrix3x3::translation(center.x, center.y);
    m_center = rot.transform(m_center);
}

void Circle::scale(const Point2D& center, double sx, double sy) {
    Matrix3x3 scale = Matrix3x3::translation(-center.x, -center.y) *
                       Matrix3x3::scaling(sx, sy) *
                       Matrix3x3::translation(center.x, center.y);
    m_center = scale.transform(m_center);
    m_radius *= std::max(std::abs(sx), std::abs(sy));
}

std::unique_ptr<Shape> Circle::clone() const {
    auto circle = std::make_unique<Circle>(m_center, m_radius);
    circle->setColor(m_color);
    circle->setLineWidth(m_lineWidth);
    circle->setSelected(m_isSelected);
    circle->setVisible(m_isVisible);
    circle->setId(m_id);
    return circle;
}

double Circle::getCircumference() const {
    return 2 * 3.14159265358979323846 * m_radius;
}

double Circle::getArea() const {
    return 3.14159265358979323846 * m_radius * m_radius;
}

} 
