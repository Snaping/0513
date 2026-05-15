#pragma once

#include "Shape.h"
#include "core/Point2D.h"

namespace GEngine {

class Arc : public Shape {
public:
    Arc();
    Arc(const Point2D& center, double radius, double startAngle, double endAngle, bool clockwise = false);
    Arc(const Point2D& start, const Point2D& mid, const Point2D& end);

    ShapeType getType() const override { return ShapeType::Arc; }
    std::string getTypeName() const override { return "Arc"; }

    void draw(Renderer* renderer) const override;
    
    bool contains(const Point2D& point, double tolerance = 5.0) const override;
    bool intersects(const Point2D& p1, const Point2D& p2) const override;
    
    void translate(double dx, double dy) override;
    void rotate(const Point2D& center, double angle) override;
    void scale(const Point2D& center, double sx, double sy) override;
    
    Point2D getCenter() const override { return m_center; }
    double getBoundingRadius() const override { return m_radius; }
    
    std::unique_ptr<Shape> clone() const override;

    void setCenter(const Point2D& center) { m_center = center; }
    
    void setRadius(double radius) { m_radius = std::abs(radius); }
    double getRadius() const { return m_radius; }

    void setStartAngle(double angle) { m_startAngle = normalizeAngle(angle); }
    double getStartAngle() const { return m_startAngle; }

    void setEndAngle(double angle) { m_endAngle = normalizeAngle(angle); }
    double getEndAngle() const { return m_endAngle; }

    void setClockwise(bool clockwise) { m_clockwise = clockwise; }
    bool isClockwise() const { return m_clockwise; }

    Point2D getStartPoint() const;
    Point2D getEndPoint() const;
    double getArcLength() const;

private:
    Point2D m_center;
    double m_radius;
    double m_startAngle;
    double m_endAngle;
    bool m_clockwise;

    double normalizeAngle(double angle) const;
    bool isAngleInRange(double angle) const;
};

} 
