#include "Rectangle.h"
#include "renderer/Renderer.h"
#include "graphics/Line.h"
#include <cmath>

namespace GEngine {

Rectangle::Rectangle()
    : m_topLeft(Point2D(0, 0))
    , m_bottomRight(Point2D(100, 100))
{
}

Rectangle::Rectangle(const Point2D& p1, const Point2D& p2) {
    setCorners(p1, p2);
}

Rectangle::Rectangle(const Point2D& topLeft, double width, double height) {
    m_topLeft = topLeft;
    m_bottomRight = Point2D(topLeft.x + width, topLeft.y + height);
}

void Rectangle::draw(Renderer* renderer) const {
    if (!m_isVisible || !renderer) return;

    Point2D p2(m_bottomRight.x, m_topLeft.y);
    Point2D p3(m_bottomRight.x, m_bottomRight.y);
    Point2D p4(m_topLeft.x, m_bottomRight.y);

    Color drawColor = m_isSelected ? Color::Red : m_color;
    renderer->drawLine(m_topLeft, p2, drawColor, m_lineWidth);
    renderer->drawLine(p2, p3, drawColor, m_lineWidth);
    renderer->drawLine(p3, p4, drawColor, m_lineWidth);
    renderer->drawLine(p4, m_topLeft, drawColor, m_lineWidth);

    if (m_isSelected) {
        renderer->drawPoint(m_topLeft, Color::Blue, 5.0);
        renderer->drawPoint(p2, Color::Blue, 5.0);
        renderer->drawPoint(p3, Color::Blue, 5.0);
        renderer->drawPoint(p4, Color::Blue, 5.0);
    }
}

bool Rectangle::contains(const Point2D& point, double tolerance) const {
    Point2D p2(m_bottomRight.x, m_topLeft.y);
    Point2D p3(m_bottomRight.x, m_bottomRight.y);
    Point2D p4(m_topLeft.x, m_bottomRight.y);

    Line edges[] = {
        Line(m_topLeft, p2),
        Line(p2, p3),
        Line(p3, p4),
        Line(p4, m_topLeft)
    };

    for (const auto& edge : edges) {
        if (edge.contains(point, tolerance)) {
            return true;
        }
    }
    return false;
}

bool Rectangle::intersects(const Point2D& p1, const Point2D& p2) const {
    Point2D rectP2(m_bottomRight.x, m_topLeft.y);
    Point2D rectP3(m_bottomRight.x, m_bottomRight.y);
    Point2D rectP4(m_topLeft.x, m_bottomRight.y);

    Line edges[] = {
        Line(m_topLeft, rectP2),
        Line(rectP2, rectP3),
        Line(rectP3, rectP4),
        Line(rectP4, m_topLeft)
    };

    for (const auto& edge : edges) {
        if (edge.intersects(p1, p2)) {
            return true;
        }
    }
    return false;
}

void Rectangle::translate(double dx, double dy) {
    m_topLeft = Point2D(m_topLeft.x + dx, m_topLeft.y + dy);
    m_bottomRight = Point2D(m_bottomRight.x + dx, m_bottomRight.y + dy);
}

void Rectangle::rotate(const Point2D& center, double angle) {
    Matrix3x3 rot = Matrix3x3::translation(-center.x, -center.y) *
                    Matrix3x3::rotation(angle) *
                    Matrix3x3::translation(center.x, center.y);
    
    Point2D p2(m_bottomRight.x, m_topLeft.y);
    Point2D p3(m_bottomRight.x, m_bottomRight.y);
    Point2D p4(m_topLeft.x, m_bottomRight.y);

    Point2D newP1 = rot.transform(m_topLeft);
    Point2D newP2 = rot.transform(p2);
    Point2D newP3 = rot.transform(p3);
    Point2D newP4 = rot.transform(p4);

    double minX = std::min({newP1.x, newP2.x, newP3.x, newP4.x});
    double maxX = std::max({newP1.x, newP2.x, newP3.x, newP4.x});
    double minY = std::min({newP1.y, newP2.y, newP3.y, newP4.y});
    double maxY = std::max({newP1.y, newP2.y, newP3.y, newP4.y});

    m_topLeft = Point2D(minX, minY);
    m_bottomRight = Point2D(maxX, maxY);
}

void Rectangle::scale(const Point2D& center, double sx, double sy) {
    Matrix3x3 scale = Matrix3x3::translation(-center.x, -center.y) *
                       Matrix3x3::scaling(sx, sy) *
                       Matrix3x3::translation(center.x, center.y);
    
    m_topLeft = scale.transform(m_topLeft);
    m_bottomRight = scale.transform(m_bottomRight);
    setCorners(m_topLeft, m_bottomRight);
}

Point2D Rectangle::getCenter() const {
    return Point2D(
        (m_topLeft.x + m_bottomRight.x) / 2,
        (m_topLeft.y + m_bottomRight.y) / 2
    );
}

double Rectangle::getBoundingRadius() const {
    Point2D center = getCenter();
    return center.distanceTo(m_topLeft);
}

std::unique_ptr<Shape> Rectangle::clone() const {
    auto rect = std::make_unique<Rectangle>(m_topLeft, m_bottomRight);
    rect->setColor(m_color);
    rect->setLineWidth(m_lineWidth);
    rect->setSelected(m_isSelected);
    rect->setVisible(m_isVisible);
    rect->setId(m_id);
    return rect;
}

void Rectangle::setCorners(const Point2D& p1, const Point2D& p2) {
    m_topLeft = Point2D(std::min(p1.x, p2.x), std::min(p1.y, p2.y));
    m_bottomRight = Point2D(std::max(p1.x, p2.x), std::max(p1.y, p2.y));
}

double Rectangle::getArea() const {
    return getWidth() * getHeight();
}

double Rectangle::getPerimeter() const {
    return 2 * (getWidth() + getHeight());
}

} 
