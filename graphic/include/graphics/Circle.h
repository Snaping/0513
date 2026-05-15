#pragma once

#include "Shape.h"
#include "core/Point2D.h"

namespace GEngine {

class Circle : public Shape {
public:
    Circle();
    Circle(const Point2D& center, double radius);
    Circle(const Point2D& center, const Point2D& pointOnCircle);

    ShapeType getType() const override { return ShapeType::Circle; }
    std::string getTypeName() const override { return "Circle"; }

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

    double getCircumference() const;
    double getArea() const;

private:
    Point2D m_center;
    double m_radius;
};

} 
