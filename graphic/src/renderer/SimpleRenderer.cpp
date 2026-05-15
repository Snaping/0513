#include "SimpleRenderer.h"
#include <cmath>

namespace GEngine {

constexpr double PI = 3.14159265358979323846;

SimpleRenderer::SimpleRenderer()
    : m_viewportWidth(800)
    , m_viewportHeight(600)
    , m_backgroundColor(Color::White)
{
}

SimpleRenderer::~SimpleRenderer() {
    shutdown();
}

void SimpleRenderer::initialize() {
#ifdef USE_GLFW
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, m_viewportWidth, m_viewportHeight, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#endif
}

void SimpleRenderer::shutdown() {
}

void SimpleRenderer::beginFrame() {
#ifdef USE_GLFW
    glClearColor(
        m_backgroundColor.r / 255.0f,
        m_backgroundColor.g / 255.0f,
        m_backgroundColor.b / 255.0f,
        m_backgroundColor.a / 255.0f
    );
    glClear(GL_COLOR_BUFFER_BIT);
#endif
}

void SimpleRenderer::endFrame() {
}

void SimpleRenderer::setViewport(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
    
#ifdef USE_GLFW
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
#endif
}

void SimpleRenderer::setBackgroundColor(const Color& color) {
    m_backgroundColor = color;
}

void SimpleRenderer::setTransform(const Matrix3x3& transform) {
    m_currentTransform = transform;
}

Matrix3x3 SimpleRenderer::getTransform() const {
    return m_currentTransform;
}

void SimpleRenderer::drawPoint(const Point2D& point, const Color& color, double size) {
#ifdef USE_GLFW
    Point2D transformed = transformPoint(point);
    
    setColor(color);
    glPointSize(static_cast<float>(size));
    glBegin(GL_POINTS);
    glVertex2d(transformed.x, transformed.y);
    glEnd();
#endif
}

void SimpleRenderer::drawLine(const Point2D& start, const Point2D& end, const Color& color, double width) {
#ifdef USE_GLFW
    Point2D tStart = transformPoint(start);
    Point2D tEnd = transformPoint(end);
    
    setColor(color);
    glLineWidth(static_cast<float>(width));
    glBegin(GL_LINES);
    glVertex2d(tStart.x, tStart.y);
    glVertex2d(tEnd.x, tEnd.y);
    glEnd();
#endif
}

void SimpleRenderer::drawCircle(const Point2D& center, double radius, const Color& color, double width) {
    drawCircleInternal(center, radius, color, width, false);
}

void SimpleRenderer::drawArc(const Point2D& center, double radius, double startAngle, double endAngle,
                            bool clockwise, const Color& color, double width) {
#ifdef USE_GLFW
    Point2D tCenter = transformPoint(center);
    
    setColor(color);
    glLineWidth(static_cast<float>(width));
    
    int segments = static_cast<int>(radius * 0.5) + 10;
    if (segments < 8) segments = 8;
    if (segments > 100) segments = 100;
    
    double start = startAngle;
    double end = endAngle;
    double step;
    
    if (clockwise) {
        if (start > end) {
            end += 2 * PI;
        }
        step = (end - start) / segments;
    } else {
        if (end < start) {
            end += 2 * PI;
        }
        step = (end - start) / segments;
    }
    
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= segments; ++i) {
        double angle = start + i * step;
        double x = tCenter.x + radius * std::cos(angle);
        double y = tCenter.y + radius * std::sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
#endif
}

void SimpleRenderer::drawRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                               const Color& color, double width) {
    Point2D p2(bottomRight.x, topLeft.y);
    Point2D p3(bottomRight.x, bottomRight.y);
    Point2D p4(topLeft.x, bottomRight.y);
    
    drawLine(topLeft, p2, color, width);
    drawLine(p2, p3, color, width);
    drawLine(p3, p4, color, width);
    drawLine(p4, topLeft, color, width);
}

void SimpleRenderer::drawPolyline(const std::vector<Point2D>& points, bool closed, 
                                const Color& color, double width) {
    if (points.size() < 2) return;
    
#ifdef USE_GLFW
    setColor(color);
    glLineWidth(static_cast<float>(width));
    glBegin(GL_LINE_STRIP);
    for (const auto& pt : points) {
        Point2D transformed = transformPoint(pt);
        glVertex2d(transformed.x, transformed.y);
    }
    if (closed && points.size() > 2) {
        Point2D first = transformPoint(points.front());
        glVertex2d(first.x, first.y);
    }
    glEnd();
#endif
}

void SimpleRenderer::fillCircle(const Point2D& center, double radius, const Color& color) {
    drawCircleInternal(center, radius, color, 1.0, true);
}

void SimpleRenderer::fillRectangle(const Point2D& topLeft, const Point2D& bottomRight, 
                               const Color& color) {
#ifdef USE_GLFW
    Point2D tTopLeft = transformPoint(topLeft);
    Point2D tBottomRight = transformPoint(bottomRight);
    
    setColor(color);
    glBegin(GL_QUADS);
    glVertex2d(tTopLeft.x, tTopLeft.y);
    glVertex2d(tBottomRight.x, tTopLeft.y);
    glVertex2d(tBottomRight.x, tBottomRight.y);
    glVertex2d(tTopLeft.x, tBottomRight.y);
    glEnd();
#endif
}

Point2D SimpleRenderer::transformPoint(const Point2D& p) {
    if (!m_transformStack.empty()) {
        return m_transformStack.back().transform(p);
    }
    return m_currentTransform.transform(p);
}

void SimpleRenderer::setColor(const Color& color) {
#ifdef USE_GLFW
    glColor4ub(color.r, color.g, color.b, color.a);
#endif
}

void SimpleRenderer::drawCircleInternal(const Point2D& center, double radius, const Color& color, double width, bool filled) {
#ifdef USE_GLFW
    Point2D tCenter = transformPoint(center);
    
    setColor(color);
    
    int segments = static_cast<int>(radius * 0.5) + 10;
    if (segments < 8) segments = 8;
    if (segments > 100) segments = 100;
    
    if (filled) {
        glBegin(GL_POLYGON);
    } else {
        glLineWidth(static_cast<float>(width));
        glBegin(GL_LINE_LOOP);
    }
    
    for (int i = 0; i < segments; ++i) {
        double angle = (2.0 * PI * i) / segments;
        double x = tCenter.x + radius * std::cos(angle);
        double y = tCenter.y + radius * std::sin(angle);
        glVertex2d(x, y);
    }
    glEnd();
#endif
}

} 
