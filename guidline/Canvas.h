#pragma once
#include "Geometry.h"
#include <vector>
#include <memory>

enum TriangleType {
    TRIANGLE_EQUILATERAL,
    TRIANGLE_ISOSCELES,
    TRIANGLE_RIGHT
};

class Canvas {
public:
    Canvas();
    ~Canvas();

    void setWindowSize(int width, int height);
    void draw(HDC hdc);

    Point2D screenToWorld(const POINT& screenPos) const;
    POINT worldToScreen(const Point2D& worldPos) const;

    void pan(int dx, int dy);
    void zoom(double factor, const POINT& center);
    void resetView();

    void addShape(std::shared_ptr<Shape> shape);
    void removeShape(int index);
    void clearShapes();
    const std::vector<std::shared_ptr<Shape>>& getShapes() const { return shapes; }

    void setNavigationMode(bool enabled);
    bool isNavigationMode() const { return navigationMode; }

    void findFeaturePoints(const Point2D& worldPos, double threshold);
    const std::vector<FeaturePoint>& getFoundFeaturePoints() const { return foundFeaturePoints; }
    void setSelectedFeaturePoint(int index);
    int getSelectedFeaturePoint() const { return selectedFeaturePoint; }

    void setCreatingShapeType(ShapeType type);
    ShapeType getCreatingShapeType() const { return creatingShapeType; }
    void startCreatingShape(const Point2D& worldPos);
    void updateCreatingShape(const Point2D& worldPos);
    void finishCreatingShape();
    void cancelCreatingShape();
    bool isCreatingShape() const { return creatingShape; }

    void setTriangleType(TriangleType type) { triangleType = type; }
    TriangleType getTriangleType() const { return triangleType; }

    double getScale() const { return scale; }

    Point2D snapToAngle(const Point2D& start, const Point2D& current);

    void setFillEnabled(bool enabled) { fillEnabled = enabled; }
    bool isFillEnabled() const { return fillEnabled; }

private:
    void drawCoordinateSystem(HDC hdc);
    void drawFeaturePoints(HDC hdc);
    void drawCreatingShape(HDC hdc);
    void drawAngleGuide(HDC hdc);

    bool fillEnabled;

    int windowWidth, windowHeight;
    double scale;
    Point2D offset;

    std::vector<std::shared_ptr<Shape>> shapes;
    bool navigationMode;
    std::vector<FeaturePoint> foundFeaturePoints;
    int selectedFeaturePoint;

    ShapeType creatingShapeType;
    Point2D createStartPoint;
    Point2D createCurrentPoint;
    Point2D createMidPoint;
    bool creatingShape;
    int createStep;
    bool angleSnapped;
    TriangleType triangleType;
};
