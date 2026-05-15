#pragma once

#include "renderer/Renderer.h"
#include <memory>
#include <vector>

#ifdef USE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace GEngine {

class SimpleRenderer : public Renderer {
public:
    SimpleRenderer();
    ~SimpleRenderer() override;

    void initialize() override;
    void shutdown() override;
    
    void beginFrame() override;
    void endFrame() override;
    
    void setViewport(int width, int height) override;
    void setBackgroundColor(const Color& color) override;
    
    void setTransform(const Matrix3x3& transform) override;
    Matrix3x3 getTransform() const override;
    
    void drawPoint(const Point2D& point, const Color& color, double size = 1.0) override;
    void drawLine(const Point2D& start, const Point2D& end, const Color& color, double width = 1.0) override;
    void drawCircle(const Point2D& center, double radius, const Color& color, double width = 1.0) override;
    void drawArc(const Point2D& center, double radius, double startAngle, double endAngle, 
                bool clockwise, const Color& color, double width = 1.0) override;
    void drawRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                       const Color& color, double width = 1.0) override;
    void drawPolyline(const std::vector<Point2D>& points, bool closed, 
                      const Color& color, double width = 1.0) override;
    
    void fillCircle(const Point2D& center, double radius, const Color& color) override;
    void fillRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                       const Color& color) override;

private:
    int m_viewportWidth;
    int m_viewportHeight;
    Color m_backgroundColor;
    Matrix3x3 m_currentTransform;
    std::vector<Point2D> m_vertexBuffer;
    
    Point2D transformPoint(const Point2D& p);
    void setColor(const Color& color);
    void drawCircleInternal(const Point2D& center, double radius, const Color& color, double width, bool filled);
};

} 
