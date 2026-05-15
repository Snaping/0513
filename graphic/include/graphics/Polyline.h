#pragma once

#include "Shape.h"
#include "core/Point2D.h"
#include <vector>

namespace GEngine {

class Polyline : public Shape {
public:
    Polyline();
    explicit Polyline(const std::vector<Point2D>& points);

    ShapeType getType() const override { return ShapeType::Polyline; }
    std::string getTypeName() const override { return "Polyline"; }

    void draw(Renderer* renderer) const override;
    
    bool contains(const Point2D& point, double tolerance = 5.0) const override;
    bool intersects(const Point2D& p1, const Point2D& p2) const override;
    
    void translate(double dx, double dy) override;
    void rotate(const Point2D& center, double angle) override;
    void scale(const Point2D& center, double sx, double sy) override;
    
    Point2D getCenter() const override;
    double getBoundingRadius() const override;
    
    std::unique_ptr<Shape> clone() const override;

    void addPoint(const Point2D& point);
    void insertPoint(size_t index, const Point2D& point);
    void removePoint(size_t index);
    void clearPoints();
    
    void setPoint(size_t index, const Point2D& point);
    Point2D getPoint(size_t index) const;
    
    size_t getPointCount() const { return m_points.size(); }
    const std::vector<Point2D>& getPoints() const { return m_points; }

    void setClosed(bool closed) { m_closed = closed; }
    bool isClosed() const { return m_closed; }

    double getLength() const;

private:
    std::vector<Point2D> m_points;
    bool m_closed;
};

} 
