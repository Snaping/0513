#pragma once

#include "core/Point2D.h"
#include "core/Color.h"
#include "core/Matrix3x3.h"
#include <string>
#include <memory>

namespace GEngine {

class Renderer;

enum class ShapeType {
    Unknown,
    Line,
    Circle,
    Rectangle,
    Arc,
    Polyline,
    Polygon
};

class Shape {
public:
    Shape();
    virtual ~Shape() = default;

    virtual ShapeType getType() const = 0;
    virtual std::string getTypeName() const = 0;
    
    virtual void draw(Renderer* renderer) const = 0;
    
    virtual bool contains(const Point2D& point, double tolerance = 5.0) const = 0;
    virtual bool intersects(const Point2D& p1, const Point2D& p2) const = 0;
    
    virtual void translate(double dx, double dy) = 0;
    virtual void rotate(const Point2D& center, double angle) = 0;
    virtual void scale(const Point2D& center, double sx, double sy) = 0;
    
    virtual Point2D getCenter() const = 0;
    virtual double getBoundingRadius() const = 0;
    
    virtual std::unique_ptr<Shape> clone() const = 0;

    void setColor(const Color& color) { m_color = color; }
    Color getColor() const { return m_color; }

    void setLineWidth(double width) { m_lineWidth = width; }
    double getLineWidth() const { return m_lineWidth; }

    void setSelected(bool selected) { m_isSelected = selected; }
    bool isSelected() const { return m_isSelected; }

    void setVisible(bool visible) { m_isVisible = visible; }
    bool isVisible() const { return m_isVisible; }

    void setId(int id) { m_id = id; }
    int getId() const { return m_id; }

protected:
    Color m_color;
    double m_lineWidth;
    bool m_isSelected;
    bool m_isVisible;
    int m_id;
};

} 
