#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include <vector>
#include <cmath>
#include <string>
#include <memory>

const double PI = 3.14159265358979323846;

struct Point2D {
    double x, y;
    Point2D(double x = 0, double y = 0) : x(x), y(y) {}
    Point2D operator+(const Point2D& other) const { return Point2D(x + other.x, y + other.y); }
    Point2D operator-(const Point2D& other) const { return Point2D(x - other.x, y - other.y); }
    Point2D operator*(double scale) const { return Point2D(x * scale, y * scale); }
    double distanceTo(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return sqrt(dx * dx + dy * dy);
    }
};

enum class ShapeType {
    None,
    Line,
    Circle,
    Rectangle,
    Triangle
};

enum class FeaturePointType {
    None,
    Endpoint,
    Midpoint,
    Center,
    CenterOfGravity
};

struct FeaturePoint {
    Point2D position;
    FeaturePointType type;
    ShapeType ownerShape;
    int shapeIndex;
    std::wstring description;

    FeaturePoint(Point2D pos = Point2D(), FeaturePointType t = FeaturePointType::None,
        ShapeType owner = ShapeType::None, int idx = -1, const std::wstring& desc = L"")
        : position(pos), type(t), ownerShape(owner), shapeIndex(idx), description(desc) {}
};

Point2D mirrorPoint(const Point2D& p, const Point2D& lineStart, const Point2D& lineEnd);
double pointToLineSide(const Point2D& p, const Point2D& lineStart, const Point2D& lineEnd);
Point2D lineIntersection(const Point2D& p1, const Point2D& p2, const Point2D& p3, const Point2D& p4);

class Shape {
public:
    Shape() : selected(false), filled(false) {}
    virtual ~Shape() = default;
    virtual void draw(HDC hdc, const Point2D& offset, double scale) const = 0;
    virtual std::vector<FeaturePoint> getFeaturePoints() const = 0;
    virtual bool hitTest(const Point2D& worldPos, double threshold) const = 0;
    virtual ShapeType getType() const = 0;
    virtual std::shared_ptr<Shape> mirror(const Point2D& lineStart, const Point2D& lineEnd) const = 0;
    virtual bool shouldClip(const Point2D& lineStart, const Point2D& lineEnd, bool keepPositiveSide) const = 0;

    bool selected;
    bool filled;
};

class LineShape : public Shape {
public:
    Point2D start, end;
    LineShape(Point2D s = Point2D(), Point2D e = Point2D()) : start(s), end(e) {}

    void draw(HDC hdc, const Point2D& offset, double scale) const override;
    std::vector<FeaturePoint> getFeaturePoints() const override;
    bool hitTest(const Point2D& worldPos, double threshold) const override;
    ShapeType getType() const override { return ShapeType::Line; }
    std::shared_ptr<Shape> mirror(const Point2D& lineStart, const Point2D& lineEnd) const override;
    bool shouldClip(const Point2D& lineStart, const Point2D& lineEnd, bool keepPositiveSide) const override;
};

class CircleShape : public Shape {
public:
    Point2D center;
    double radius;
    CircleShape(Point2D c = Point2D(), double r = 0) : center(c), radius(r) {}

    void draw(HDC hdc, const Point2D& offset, double scale) const override;
    std::vector<FeaturePoint> getFeaturePoints() const override;
    bool hitTest(const Point2D& worldPos, double threshold) const override;
    ShapeType getType() const override { return ShapeType::Circle; }
    std::shared_ptr<Shape> mirror(const Point2D& lineStart, const Point2D& lineEnd) const override;
    bool shouldClip(const Point2D& lineStart, const Point2D& lineEnd, bool keepPositiveSide) const override;
};

class RectangleShape : public Shape {
public:
    Point2D topLeft, bottomRight;
    RectangleShape(Point2D tl = Point2D(), Point2D br = Point2D()) : topLeft(tl), bottomRight(br) {}

    void draw(HDC hdc, const Point2D& offset, double scale) const override;
    std::vector<FeaturePoint> getFeaturePoints() const override;
    bool hitTest(const Point2D& worldPos, double threshold) const override;
    ShapeType getType() const override { return ShapeType::Rectangle; }
    std::shared_ptr<Shape> mirror(const Point2D& lineStart, const Point2D& lineEnd) const override;
    bool shouldClip(const Point2D& lineStart, const Point2D& lineEnd, bool keepPositiveSide) const override;
};

class TriangleShape : public Shape {
public:
    Point2D p1, p2, p3;
    TriangleShape(Point2D a = Point2D(), Point2D b = Point2D(), Point2D c = Point2D()) : p1(a), p2(b), p3(c) {}

    void draw(HDC hdc, const Point2D& offset, double scale) const override;
    std::vector<FeaturePoint> getFeaturePoints() const override;
    bool hitTest(const Point2D& worldPos, double threshold) const override;
    ShapeType getType() const override { return ShapeType::Triangle; }
    std::shared_ptr<Shape> mirror(const Point2D& lineStart, const Point2D& lineEnd) const override;
    bool shouldClip(const Point2D& lineStart, const Point2D& lineEnd, bool keepPositiveSide) const override;
};
