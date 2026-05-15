#pragma once

#include "Shape.h"
#include "core/Point2D.h"

namespace GEngine {

class Line : public Shape {
public:
    Line();
    Line(const Point2D& start, const Point2D& end);

    ShapeType getType() const override { return ShapeType::Line; }
    std::string getTypeName() const override { return "Line"; }

    void draw(Renderer* renderer) const override;
    
    bool contains(const Point2D& point, double tolerance = 5.0) const override;
    bool intersects(const Point2D& p1, const Point2D& p2) const override;
    
    void translate(double dx, double dy) override;
    void rotate(const Point2D& center, double angle) override;
    void scale(const Point2D& center, double sx, double sy) override;
    
    Point2D getCenter() const override;
    double getBoundingRadius() const override;
    
    std::unique_ptr<Shape> clone() const override;

    void setStartPoint(const Point2D& p) { m_start = p; }
    Point2D getStartPoint() const { return m_start; }

    void setEndPoint(const Point2D& p) { m_end = p; }
    Point2D getEndPoint() const { return m_end; }

    double getLength() const;

private:
    Point2D m_start;
    Point2D m_end;

    double distanceToPoint(const Point2D& p) const;
};

} 
