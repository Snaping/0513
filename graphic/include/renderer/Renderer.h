#pragma once

#include "core/Point2D.h"
#include "core/Color.h"
#include "core/Matrix3x3.h"
#include <vector>

namespace GEngine {

class Shape;

class Renderer {
public:
    Renderer();
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void shutdown() = 0;
    
    virtual void beginFrame() = 0;
    virtual void endFrame() = 0;
    
    virtual void setViewport(int width, int height) = 0;
    virtual void setBackgroundColor(const Color& color) = 0;
    
    virtual void setTransform(const Matrix3x3& transform) = 0;
    virtual Matrix3x3 getTransform() const = 0;
    
    virtual void drawPoint(const Point2D& point, const Color& color, double size = 1.0) = 0;
    virtual void drawLine(const Point2D& start, const Point2D& end, const Color& color, double width = 1.0) = 0;
    virtual void drawCircle(const Point2D& center, double radius, const Color& color, double width = 1.0) = 0;
    virtual void drawArc(const Point2D& center, double radius, double startAngle, double endAngle, 
                        bool clockwise, const Color& color, double width = 1.0) = 0;
    virtual void drawRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                               const Color& color, double width = 1.0) = 0;
    virtual void drawPolyline(const std::vector<Point2D>& points, bool closed, 
                              const Color& color, double width = 1.0) = 0;
    
    virtual void fillCircle(const Point2D& center, double radius, const Color& color) = 0;
    virtual void fillRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                               const Color& color) = 0;
    
    virtual void drawShape(const Shape* shape);
    
    virtual void pushTransform();
    virtual void popTransform();
    
    virtual void translate(double dx, double dy);
    virtual void rotate(double angle);
    virtual void scale(double sx, double sy);
    
    void setLineWidth(double width) { m_defaultLineWidth = width; }
    double getLineWidth() const { return m_defaultLineWidth; }
    
    void setDefaultColor(const Color& color) { m_defaultColor = color; }
    Color getDefaultColor() const { return m_defaultColor; }

protected:
    Color m_defaultColor;
    double m_defaultLineWidth;
    std::vector<Matrix3x3> m_transformStack;
};

} 
