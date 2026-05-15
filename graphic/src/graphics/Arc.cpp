#include "Arc.h"
#include "renderer/Renderer.h"
#include "core/Vector2D.h"
#include <cmath>

namespace GEngine {

constexpr double PI = 3.14159265358979323846;

Arc::Arc()
    : m_center(Point2D(0, 0))
    , m_radius(50.0)
    , m_startAngle(0.0)
    , m_endAngle(PI / 2.0)
    , m_clockwise(false)
{
}

Arc::Arc(const Point2D& center, double radius, double startAngle, double endAngle, bool clockwise)
    : m_center(center)
    , m_radius(std::abs(radius))
    , m_startAngle(normalizeAngle(startAngle))
    , m_endAngle(normalizeAngle(endAngle))
    , m_clockwise(clockwise)
{
}

Arc::Arc(const Point2D& start, const Point2D& mid, const Point2D& end) {
    double x1 = start.x, y1 = start.y;
    double x2 = mid.x, y2 = mid.y;
    double x3 = end.x, y3 = end.y;

    double d = 2.0 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
    
    if (std::abs(d) < 1e-10) {
        m_center = Point2D((x1 + x3) / 2.0, (y1 + y3) / 2.0);
        m_radius = m_center.distanceTo(start);
        m_startAngle = std::atan2(start.y - m_center.y, start.x - m_center.x);
        m_endAngle = std::atan2(end.y - m_center.y, end.x - m_center.x);
        m_clockwise = false;
        return;
    }

    double ux = ((x1*x1 + y1*y1) * (y2 - y3) + (x2*x2 + y2*y2) * (y3 - y1) + (x3*x3 + y3*y3) * (y1 - y2)) / d;
    double uy = ((x1*x1 + y1*y1) * (x3 - x2) + (x2*x2 + y2*y2) * (x1 - x3) + (x3*x3 + y3*y3) * (x2 - x1)) / d;

    m_center = Point2D(ux, uy);
    m_radius = m_center.distanceTo(start);
    m_startAngle = std::atan2(start.y - uy, start.x - ux);
    m_endAngle = std::atan2(end.y - uy, end.x - ux);
    
    Vector2D v1(start, mid);
    Vector2D v2(mid, end);
    m_clockwise = (v1.cross(v2) < 0);
}

void Arc::draw(Renderer* renderer) const {
    if (!m_isVisible || !renderer) return;

    Color drawColor = m_isSelected ? Color::Red : m_color;
    renderer->drawArc(m_center, m_radius, m_startAngle, m_endAngle, m_clockwise, drawColor, m_lineWidth);

    if (m_isSelected) {
        renderer->drawPoint(m_center, Color::Blue, 5.0);
        renderer->drawPoint(getStartPoint(), Color::Green, 5.0);
        renderer->drawPoint(getEndPoint(), Color::Green, 5.0);
    }
}

bool Arc::contains(const Point2D& point, double tolerance) const {
    double dist = m_center.distanceTo(point);
    if (std::abs(dist - m_radius) > tolerance) {
        return false;
    }

    double angle = std::atan2(point.y - m_center.y, point.x - m_center.x);
    return isAngleInRange(angle);
}

bool Arc::intersects(const Point2D& p1, const Point2D& p2) const {
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
    
    auto checkPoint = [&](double t) {
        if (t < 0 || t > 1) return false;
        Point2D pt(p1.x + t * d.x, p1.y + t * d.y);
        return contains(pt, 0.1);
    };
    
    return checkPoint(t1) || checkPoint(t2);
}

void Arc::translate(double dx, double dy) {
    m_center = Point2D(m_center.x + dx, m_center.y + dy);
}

void Arc::rotate(const Point2D& center, double angle) {
    Matrix3x3 rot = Matrix3x3::translation(-center.x, -center.y) *
                    Matrix3x3::rotation(angle) *
                    Matrix3x3::translation(center.x, center.y);
    m_center = rot.transform(m_center);
    m_startAngle = normalizeAngle(m_startAngle + angle);
    m_endAngle = normalizeAngle(m_endAngle + angle);
}

void Arc::scale(const Point2D& center, double sx, double sy) {
    Matrix3x3 scale = Matrix3x3::translation(-center.x, -center.y) *
                       Matrix3x3::scaling(sx, sy) *
                       Matrix3x3::translation(center.x, center.y);
    m_center = scale.transform(m_center);
    m_radius *= std::max(std::abs(sx), std::abs(sy));
}

std::unique_ptr<Shape> Arc::clone() const {
    auto arc = std::make_unique<Arc>(m_center, m_radius, m_startAngle, m_endAngle, m_clockwise);
    arc->setColor(m_color);
    arc->setLineWidth(m_lineWidth);
    arc->setSelected(m_isSelected);
    arc->setVisible(m_isVisible);
    arc->setId(m_id);
    return arc;
}

Point2D Arc::getStartPoint() const {
    return Point2D(
        m_center.x + m_radius * std::cos(m_startAngle),
        m_center.y + m_radius * std::sin(m_startAngle)
    );
}

Point2D Arc::getEndPoint() const {
    return Point2D(
        m_center.x + m_radius * std::cos(m_endAngle),
        m_center.y + m_radius * std::sin(m_endAngle)
    );
}

double Arc::getArcLength() const {
    double angleDiff;
    if (m_clockwise) {
        angleDiff = m_startAngle - m_endAngle;
        if (angleDiff < 0) angleDiff += 2 * PI;
    } else {
        angleDiff = m_endAngle - m_startAngle;
        if (angleDiff < 0) angleDiff += 2 * PI;
    }
    return m_radius * angleDiff;
}

double Arc::normalizeAngle(double angle) const {
    while (angle < 0) angle += 2 * PI;
    while (angle >= 2 * PI) angle -= 2 * PI;
    return angle;
}

bool Arc::isAngleInRange(double angle) const {
    angle = normalizeAngle(angle);
    
    double start = normalizeAngle(m_startAngle);
    double end = normalizeAngle(m_endAngle);
    
    if (m_clockwise) {
        if (start >= end) {
            return angle <= start && angle >= end;
        } else {
            return angle <= start || angle >= end;
        }
    } else {
        if (end >= start) {
            return angle >= start && angle <= end;
        } else {
            return angle >= start || angle <= end;
        }
    }
}

} 
