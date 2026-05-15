#pragma once

#include "Shape.h"
#include "core/Point2D.h"

namespace GEngine {

class Rectangle : public Shape {
public:
    Rectangle();
    Rectangle(const Point2D& p1, const Point2D& p2);
    Rectangle(const Point2D& topLeft, double width, double height);

    ShapeType getType() const override { return ShapeType::Rectangle; }
    std::string getTypeName() const override { return "Rectangle"; }

    void draw(Renderer* renderer) const override;
    
    bool contains(const Point2D& point, double tolerance = 5.0) const override;
    bool intersects(const Point2D& p1, const Point2D& p2) const override;
    
    void translate(double dx, double dy) override;
    void rotate(const Point2D& center, double angle) override;
    void scale(const Point2D& center, double sx, double sy) override;
    
    Point2D getCenter() const override;
    double getBoundingRadius() const override;
    
    std::unique_ptr<Shape> clone() const override;

    void setCorners(const Point2D& p1, const Point2D& p2);
    
    Point2D getTopLeft() const { return m_topLeft; }
    Point2D getBottomRight() const { return m_bottomRight; }

    double getWidth() const { return m_bottomRight.x - m_topLeft.x; }
    double getHeight() const { return m_bottomRight.y - m_topLeft.y; }

    double getArea() const;
    double getPerimeter() const;

private:
    Point2D m_topLeft;
    Point2D m_bottomRight;
};

} 
