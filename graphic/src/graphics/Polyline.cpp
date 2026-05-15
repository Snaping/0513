#include "Polyline.h"
#include "renderer/Renderer.h"
#include "graphics/Line.h"
#include <cmath>
#include <limits>

namespace GEngine {

Polyline::Polyline()
    : m_closed(false)
{
}

Polyline::Polyline(const std::vector<Point2D>& points)
    : m_points(points)
    , m_closed(false)
{
}

void Polyline::draw(Renderer* renderer) const {
    if (!m_isVisible || !renderer || m_points.size() < 2) return;

    Color drawColor = m_isSelected ? Color::Red : m_color;
    
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        renderer->drawLine(m_points[i], m_points[i + 1], drawColor, m_lineWidth);
    }
    
    if (m_closed && m_points.size() > 2) {
        renderer->drawLine(m_points.back(), m_points.front(), drawColor, m_lineWidth);
    }

    if (m_isSelected) {
        for (const auto& pt : m_points) {
            renderer->drawPoint(pt, Color::Blue, 5.0);
        }
    }
}

bool Polyline::contains(const Point2D& point, double tolerance) const {
    if (m_points.size() < 2) return false;

    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        Line segment(m_points[i], m_points[i + 1]);
        if (segment.contains(point, tolerance)) {
            return true;
        }
    }
    
    if (m_closed && m_points.size() > 2) {
        Line segment(m_points.back(), m_points.front());
        if (segment.contains(point, tolerance)) {
            return true;
        }
    }
    
    return false;
}

bool Polyline::intersects(const Point2D& p1, const Point2D& p2) const {
    if (m_points.size() < 2) return false;

    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        Line segment(m_points[i], m_points[i + 1]);
        if (segment.intersects(p1, p2)) {
            return true;
        }
    }
    
    if (m_closed && m_points.size() > 2) {
        Line segment(m_points.back(), m_points.front());
        if (segment.intersects(p1, p2)) {
            return true;
        }
    }
    
    return false;
}

void Polyline::translate(double dx, double dy) {
    for (auto& pt : m_points) {
        pt = Point2D(pt.x + dx, pt.y + dy);
    }
}

void Polyline::rotate(const Point2D& center, double angle) {
    Matrix3x3 rot = Matrix3x3::translation(-center.x, -center.y) *
                    Matrix3x3::rotation(angle) *
                    Matrix3x3::translation(center.x, center.y);
    
    for (auto& pt : m_points) {
        pt = rot.transform(pt);
    }
}

void Polyline::scale(const Point2D& center, double sx, double sy) {
    Matrix3x3 scale = Matrix3x3::translation(-center.x, -center.y) *
                       Matrix3x3::scaling(sx, sy) *
                       Matrix3x3::translation(center.x, center.y);
    
    for (auto& pt : m_points) {
        pt = scale.transform(pt);
    }
}

Point2D Polyline::getCenter() const {
    if (m_points.empty()) {
        return Point2D(0, 0);
    }
    
    double sumX = 0.0, sumY = 0.0;
    for (const auto& pt : m_points) {
        sumX += pt.x;
        sumY += pt.y;
    }
    
    return Point2D(sumX / m_points.size(), sumY / m_points.size());
}

double Polyline::getBoundingRadius() const {
    if (m_points.empty()) return 0.0;
    
    Point2D center = getCenter();
    double maxDist = 0.0;
    
    for (const auto& pt : m_points) {
        double dist = center.distanceTo(pt);
        if (dist > maxDist) {
            maxDist = dist;
        }
    }
    
    return maxDist;
}

std::unique_ptr<Shape> Polyline::clone() const {
    auto poly = std::make_unique<Polyline>(m_points);
    poly->setColor(m_color);
    poly->setLineWidth(m_lineWidth);
    poly->setSelected(m_isSelected);
    poly->setVisible(m_isVisible);
    poly->setId(m_id);
    poly->setClosed(m_closed);
    return poly;
}

void Polyline::addPoint(const Point2D& point) {
    m_points.push_back(point);
}

void Polyline::insertPoint(size_t index, const Point2D& point) {
    if (index <= m_points.size()) {
        m_points.insert(m_points.begin() + index, point);
    }
}

void Polyline::removePoint(size_t index) {
    if (index < m_points.size()) {
        m_points.erase(m_points.begin() + index);
    }
}

void Polyline::clearPoints() {
    m_points.clear();
}

void Polyline::setPoint(size_t index, const Point2D& point) {
    if (index < m_points.size()) {
        m_points[index] = point;
    }
}

Point2D Polyline::getPoint(size_t index) const {
    if (index < m_points.size()) {
        return m_points[index];
    }
    return Point2D(0, 0);
}

double Polyline::getLength() const {
    if (m_points.size() < 2) return 0.0;
    
    double length = 0.0;
    for (size_t i = 0; i < m_points.size() - 1; ++i) {
        length += m_points[i].distanceTo(m_points[i + 1]);
    }
    
    if (m_closed && m_points.size() > 2) {
        length += m_points.back().distanceTo(m_points.front());
    }
    
    return length;
}

} 
