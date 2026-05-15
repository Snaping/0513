#define NOMINMAX
#include "renderer/GDIRenderer.h"
#include <cmath>
#include <algorithm>

namespace GEngine {

constexpr double PI = 3.14159265358979323846;

namespace {
    int clampInt(int value, int minVal, int maxVal) {
        return value < minVal ? minVal : (value > maxVal ? maxVal : value);
    }
    
    int maxInt(int a, int b) {
        return a > b ? a : b;
    }
    
    double maxDouble(double a, double b) {
        return a > b ? a : b;
    }
    
    double minDouble(double a, double b) {
        return a < b ? a : b;
    }
}

GDIRenderer::GDIRenderer()
    : m_hdc(nullptr)
    , m_hBitmap(nullptr)
    , m_memDC(nullptr)
    , m_viewportWidth(800)
    , m_viewportHeight(600)
    , m_backgroundColor(Color::White)
{
}

GDIRenderer::~GDIRenderer() {
    shutdown();
}

void GDIRenderer::initialize() {
}

void GDIRenderer::shutdown() {
    if (m_memDC && m_hBitmap) {
        SelectObject(m_memDC, m_hBitmap);
        DeleteObject(m_hBitmap);
        DeleteDC(m_memDC);
        m_hBitmap = nullptr;
        m_memDC = nullptr;
    }
}

void GDIRenderer::beginFrame() {
    if (!m_hdc) return;
    
    COLORREF bgColor = colorToCOLORREF(m_backgroundColor);
    HBRUSH hBrush = CreateSolidBrush(bgColor);
    RECT fillRect = {0, 0, m_viewportWidth, m_viewportHeight};
    FillRect(m_hdc, &fillRect, hBrush);
    DeleteObject(hBrush);
    
    m_transformStack.clear();
}

void GDIRenderer::endFrame() {
}

void GDIRenderer::setViewport(int width, int height) {
    m_viewportWidth = width;
    m_viewportHeight = height;
}

void GDIRenderer::setBackgroundColor(const Color& color) {
    m_backgroundColor = color;
}

void GDIRenderer::setTransform(const Matrix3x3& transform) {
    m_currentTransform = transform;
}

Matrix3x3 GDIRenderer::getTransform() const {
    return m_currentTransform;
}

void GDIRenderer::drawPoint(const Point2D& point, const Color& color, double size) {
    if (!m_hdc) return;
    
    Point2D p = transformPoint(point);
    COLORREF col = colorToCOLORREF(color);
    
    int x = static_cast<int>(p.x);
    int y = static_cast<int>(p.y);
    int s = maxInt(1, static_cast<int>(size));
    
    HBRUSH hBrush = CreateSolidBrush(col);
    HPEN hPen = CreatePen(PS_SOLID, 1, col);
    
    HGDIOBJ oldBrush = SelectObject(m_hdc, hBrush);
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    
    Ellipse(m_hdc, x - s, y - s, x + s, y + s);
    
    SelectObject(m_hdc, oldBrush);
    SelectObject(m_hdc, oldPen);
    DeleteObject(hBrush);
    DeleteObject(hPen);
}

void GDIRenderer::drawLine(const Point2D& start, const Point2D& end, const Color& color, double width) {
    if (!m_hdc) return;
    
    Point2D s = transformPoint(start);
    Point2D e = transformPoint(end);
    COLORREF col = colorToCOLORREF(color);
    
    HPEN hPen = CreatePen(PS_SOLID, maxInt(1, static_cast<int>(width)), col);
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    
    MoveToEx(m_hdc, static_cast<int>(s.x), static_cast<int>(s.y), nullptr);
    LineTo(m_hdc, static_cast<int>(e.x), static_cast<int>(e.y));
    
    SelectObject(m_hdc, oldPen);
    DeleteObject(hPen);
}

void GDIRenderer::drawCircle(const Point2D& center, double radius, const Color& color, double width) {
    drawCircleInternal(center, radius, color, width, false);
}

void GDIRenderer::drawArc(const Point2D& center, double radius, double startAngle, double endAngle,
                         bool clockwise, const Color& color, double width) {
    if (!m_hdc) return;
    
    Point2D c = transformPoint(center);
    COLORREF col = colorToCOLORREF(color);
    
    double start = startAngle;
    double end = endAngle;
    
    if (clockwise) {
        if (start < end) {
            start += 2 * PI;
        }
    } else {
        if (end < start) {
            end += 2 * PI;
        }
    }
    
    Point2D startPt(
        c.x + radius * std::cos(start),
        c.y + radius * std::sin(start)
    );
    Point2D endPt(
        c.x + radius * std::cos(end),
        c.y + radius * std::sin(end)
    );
    
    HPEN hPen = CreatePen(PS_SOLID, maxInt(1, static_cast<int>(width)), col);
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    
    Arc(m_hdc,
        static_cast<int>(c.x - radius),
        static_cast<int>(c.y - radius),
        static_cast<int>(c.x + radius),
        static_cast<int>(c.y + radius),
        static_cast<int>(startPt.x),
        static_cast<int>(startPt.y),
        static_cast<int>(endPt.x),
        static_cast<int>(endPt.y)
    );
    
    SelectObject(m_hdc, oldPen);
    DeleteObject(hPen);
}

void GDIRenderer::drawRectangle(const Point2D& topLeft, const Point2D& bottomRight,
                               const Color& color, double width) {
    if (!m_hdc) return;
    
    Point2D p1 = transformPoint(topLeft);
    Point2D p2 = transformPoint(bottomRight);
    COLORREF col = colorToCOLORREF(color);
    
    int left = static_cast<int>(minDouble(p1.x, p2.x));
    int top = static_cast<int>(minDouble(p1.y, p2.y));
    int right = static_cast<int>(maxDouble(p1.x, p2.x));
    int bottom = static_cast<int>(maxDouble(p1.y, p2.y));
    
    HPEN hPen = CreatePen(PS_SOLID, maxInt(1, static_cast<int>(width)), col);
    HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
    
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(m_hdc, hBrush);
    
    Rectangle(m_hdc, left, top, right, bottom);
    
    SelectObject(m_hdc, oldPen);
    SelectObject(m_hdc, oldBrush);
    DeleteObject(hPen);
}

void GDIRenderer::drawPolyline(const std::vector<Point2D>& points, bool closed,
                              const Color& color, double width) {
    if (!m_hdc || points.size() < 2) return;
    
    COLORREF col = colorToCOLORREF(color);
    HPEN hPen = CreatePen(PS_SOLID, maxInt(1, static_cast<int>(width)), col);
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    
    std::vector<POINT> winPoints;
    winPoints.reserve(points.size());
    
    for (const auto& pt : points) {
        Point2D transformed = transformPoint(pt);
        winPoints.push_back({
            static_cast<LONG>(transformed.x),
            static_cast<LONG>(transformed.y)
        });
    }
    
    Polyline(m_hdc, winPoints.data(), static_cast<int>(winPoints.size()));
    
    if (closed && points.size() > 2) {
        MoveToEx(m_hdc, winPoints.back().x, winPoints.back().y, nullptr);
        LineTo(m_hdc, winPoints.front().x, winPoints.front().y);
    }
    
    SelectObject(m_hdc, oldPen);
    DeleteObject(hPen);
}

void GDIRenderer::fillCircle(const Point2D& center, double radius, const Color& color) {
    drawCircleInternal(center, radius, color, 1.0, true);
}

void GDIRenderer::fillRectangle(const Point2D& topLeft, const Point2D& bottomRight,
                               const Color& color) {
    if (!m_hdc) return;
    
    Point2D p1 = transformPoint(topLeft);
    Point2D p2 = transformPoint(bottomRight);
    COLORREF col = colorToCOLORREF(color);
    
    int left = static_cast<int>(minDouble(p1.x, p2.x));
    int top = static_cast<int>(minDouble(p1.y, p2.y));
    int right = static_cast<int>(maxDouble(p1.x, p2.x));
    int bottom = static_cast<int>(maxDouble(p1.y, p2.y));
    
    HBRUSH hBrush = CreateSolidBrush(col);
    RECT rect = {left, top, right, bottom};
    FillRect(m_hdc, &rect, hBrush);
    DeleteObject(hBrush);
}

Point2D GDIRenderer::transformPoint(const Point2D& p) {
    if (!m_transformStack.empty()) {
        return m_transformStack.back().transform(p);
    }
    return m_currentTransform.transform(p);
}

COLORREF GDIRenderer::colorToCOLORREF(const Color& color) {
    return RGB(color.r, color.g, color.b);
}

void GDIRenderer::drawCircleInternal(const Point2D& center, double radius, const Color& color, double width, bool filled) {
    if (!m_hdc) return;
    
    Point2D c = transformPoint(center);
    COLORREF col = colorToCOLORREF(color);
    
    int left = static_cast<int>(c.x - radius);
    int top = static_cast<int>(c.y - radius);
    int right = static_cast<int>(c.x + radius);
    int bottom = static_cast<int>(c.y + radius);
    
    HPEN hPen = CreatePen(PS_SOLID, maxInt(1, static_cast<int>(width)), col);
    HBRUSH hBrush = filled ? CreateSolidBrush(col) : (HBRUSH)GetStockObject(NULL_BRUSH);
    
    HGDIOBJ oldPen = SelectObject(m_hdc, hPen);
    HGDIOBJ oldBrush = SelectObject(m_hdc, hBrush);
    
    Ellipse(m_hdc, left, top, right, bottom);
    
    SelectObject(m_hdc, oldPen);
    SelectObject(m_hdc, oldBrush);
    DeleteObject(hPen);
    if (filled) DeleteObject(hBrush);
}

} 
