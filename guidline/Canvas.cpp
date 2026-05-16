#include "Canvas.h"
#include <algorithm>

Canvas::Canvas()
    : windowWidth(800), windowHeight(600)
    , scale(1.0)
    , offset(Point2D(400, 300))
    , navigationMode(true)
    , selectedFeaturePoint(-1)
    , creatingShapeType(ShapeType::None)
    , creatingShape(false)
    , createStep(0)
    , angleSnapped(false)
    , triangleType(TRIANGLE_EQUILATERAL)
    , fillEnabled(false)
    , editMode(EditMode::None)
    , selectedShapeIndex(-1)
    , mirrorShapeIndex(-1)
    , mirrorLineIndex(-1) {
}

Canvas::~Canvas() {
}

void Canvas::setWindowSize(int width, int height) {
    windowWidth = width;
    windowHeight = height;
}

Point2D Canvas::screenToWorld(const POINT& screenPos) const {
    double worldX = (screenPos.x / scale) - offset.x;
    double worldY = -((screenPos.y / scale) - offset.y);
    return Point2D(worldX, worldY);
}

POINT Canvas::worldToScreen(const Point2D& worldPos) const {
    POINT screenPos;
    screenPos.x = (LONG)((worldPos.x + offset.x) * scale);
    screenPos.y = (LONG)((-worldPos.y + offset.y) * scale);
    return screenPos;
}

void Canvas::pan(int dx, int dy) {
    offset.x += dx / scale;
    offset.y += dy / scale;
}

void Canvas::zoom(double factor, const POINT& center) {
    Point2D worldCenter = screenToWorld(center);
    scale *= factor;
    Point2D newWorldCenter = screenToWorld(center);
    offset.x += (newWorldCenter.x - worldCenter.x);
    offset.y += (worldCenter.y - newWorldCenter.y);
}

void Canvas::resetView() {
    scale = 1.0;
    offset = Point2D(windowWidth / 2.0, windowHeight / 2.0);
}

void Canvas::addShape(std::shared_ptr<Shape> shape) {
    shapes.push_back(shape);
}

void Canvas::removeShape(int index) {
    if (index >= 0 && index < (int)shapes.size()) {
        shapes.erase(shapes.begin() + index);
    }
}

void Canvas::clearShapes() {
    shapes.clear();
    foundFeaturePoints.clear();
    selectedFeaturePoint = -1;
}

void Canvas::setNavigationMode(bool enabled) {
    navigationMode = enabled;
    if (!enabled) {
        foundFeaturePoints.clear();
        selectedFeaturePoint = -1;
    }
}

void Canvas::findFeaturePoints(const Point2D& worldPos, double threshold) {
    foundFeaturePoints.clear();
    selectedFeaturePoint = -1;

    if (!navigationMode) return;

    for (size_t i = 0; i < shapes.size(); ++i) {
        auto featurePoints = shapes[i]->getFeaturePoints();
        for (auto& fp : featurePoints) {
            if (worldPos.distanceTo(fp.position) < threshold) {
                fp.shapeIndex = (int)i;
                foundFeaturePoints.push_back(fp);
            }
        }
    }
}

void Canvas::setSelectedFeaturePoint(int index) {
    selectedFeaturePoint = index;
}

void Canvas::setCreatingShapeType(ShapeType type) {
    creatingShapeType = type;
    creatingShape = (type == ShapeType::Triangle);
    createStep = 0;
}

Point2D Canvas::snapToAngle(const Point2D& start, const Point2D& current) {
    double dx = current.x - start.x;
    double dy = current.y - start.y;
    double angle = atan2(dy, dx) * 180.0 / PI;
    double length = sqrt(dx * dx + dy * dy);

    double snapAngles[] = {0, 45, 90, 135, 180, 225, 270, 315};
    double threshold = 5.0;

    angleSnapped = false;
    for (double snapAngle : snapAngles) {
        double diff = fabs(angle - snapAngle);
        if (diff > 180) diff = 360 - diff;
        if (diff < threshold) {
            angle = snapAngle;
            angleSnapped = true;
            break;
        }
    }

    if (angleSnapped) {
        double rad = angle * PI / 180.0;
        return Point2D(start.x + length * cos(rad), start.y + length * sin(rad));
    }
    return current;
}

void Canvas::startCreatingShape(const Point2D& worldPos) {
    if (creatingShapeType == ShapeType::None) return;

    createStartPoint = worldPos;
    createCurrentPoint = worldPos;
    creatingShape = true;
}

void Canvas::updateCreatingShape(const Point2D& worldPos) {
    createCurrentPoint = worldPos;
    if (creatingShapeType == ShapeType::Triangle) {
        createStartPoint = worldPos;
    }
}

void Canvas::finishCreatingShape() {
    if (!creatingShape || creatingShapeType == ShapeType::None) return;

    std::shared_ptr<Shape> shape = nullptr;

    Point2D endPoint = createCurrentPoint;
    if (creatingShapeType == ShapeType::Line && navigationMode) {
        Point2D snappedPoint = snapToAngle(createStartPoint, createCurrentPoint);
        if (angleSnapped) {
            endPoint = snappedPoint;
        }
    }

    switch (creatingShapeType) {
    case ShapeType::Line:
        shape = std::make_shared<LineShape>(createStartPoint, endPoint);
        break;
    case ShapeType::Circle: {
        double radius = createStartPoint.distanceTo(createCurrentPoint);
        auto circle = std::make_shared<CircleShape>(createStartPoint, radius);
        circle->filled = fillEnabled;
        shape = circle;
        break;
    }
    case ShapeType::Rectangle: {
        auto rect = std::make_shared<RectangleShape>(createStartPoint, createCurrentPoint);
        rect->filled = fillEnabled;
        shape = rect;
        break;
    }
    case ShapeType::Triangle: {
        double size = 50.0;
        Point2D p1, p2, p3;

        switch (triangleType) {
        case TRIANGLE_EQUILATERAL: {
            double h = size * sqrt(3.0) / 2.0;
            p1 = Point2D(createCurrentPoint.x, createCurrentPoint.y + h * 2 / 3);
            p2 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y - h / 3);
            p3 = Point2D(createCurrentPoint.x + size / 2, createCurrentPoint.y - h / 3);
            break;
        }
        case TRIANGLE_ISOSCELES: {
            double baseWidth = size;
            double height = size * 1.2;
            p1 = Point2D(createCurrentPoint.x, createCurrentPoint.y + height / 2);
            p2 = Point2D(createCurrentPoint.x - baseWidth / 2, createCurrentPoint.y - height / 2);
            p3 = Point2D(createCurrentPoint.x + baseWidth / 2, createCurrentPoint.y - height / 2);
            break;
        }
        case TRIANGLE_RIGHT: {
            p1 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y + size / 2);
            p2 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y - size / 2);
            p3 = Point2D(createCurrentPoint.x + size / 2, createCurrentPoint.y - size / 2);
            break;
        }
        }
        auto triangle = std::make_shared<TriangleShape>(p1, p2, p3);
        triangle->filled = fillEnabled;
        shape = triangle;
        break;
    }
    default:
        break;
    }

    if (shape) {
        addShape(shape);
    }

    creatingShape = false;
    createStep = 0;
}

void Canvas::cancelCreatingShape() {
    creatingShape = false;
    createStep = 0;
}

void Canvas::drawCoordinateSystem(HDC hdc) {
    HPEN hAxisPen = CreatePen(PS_SOLID, 1, RGB(80, 80, 90));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hAxisPen);

    POINT origin = worldToScreen(Point2D(0, 0));

    MoveToEx(hdc, 0, origin.y, NULL);
    LineTo(hdc, windowWidth, origin.y);

    MoveToEx(hdc, origin.x, 0, NULL);
    LineTo(hdc, origin.x, windowHeight);

    SelectObject(hdc, hOldPen);
    DeleteObject(hAxisPen);

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(120, 120, 130));
    TextOutA(hdc, windowWidth - 20, origin.y - 20, "X", 1);
    TextOutA(hdc, origin.x + 5, 5, "Y", 1);

    HPEN hGridPen = CreatePen(PS_SOLID, 1, RGB(50, 50, 55));
    hOldPen = (HPEN)SelectObject(hdc, hGridPen);

    double gridSpacing = 50.0;
    if (scale < 0.5) gridSpacing = 100.0;
    if (scale > 2.0) gridSpacing = 25.0;

    Point2D worldTopLeft = screenToWorld(POINT{ 0, 0 });
    Point2D worldBottomRight = screenToWorld(POINT{ windowWidth, windowHeight });

    double startX = floor(worldTopLeft.x / gridSpacing) * gridSpacing;
    for (double x = startX; x < worldBottomRight.x; x += gridSpacing) {
        POINT p = worldToScreen(Point2D(x, 0));
        MoveToEx(hdc, p.x, 0, NULL);
        LineTo(hdc, p.x, windowHeight);
    }

    double startY = floor(worldBottomRight.y / gridSpacing) * gridSpacing;
    for (double y = startY; y < worldTopLeft.y; y += gridSpacing) {
        POINT p = worldToScreen(Point2D(0, y));
        MoveToEx(hdc, 0, p.y, NULL);
        LineTo(hdc, windowWidth, p.y);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hGridPen);
}

void Canvas::drawFeaturePoints(HDC hdc) {
    for (size_t i = 0; i < foundFeaturePoints.size(); ++i) {
        const auto& fp = foundFeaturePoints[i];
        POINT screenPos = worldToScreen(fp.position);

        COLORREF color = (i == selectedFeaturePoint) ? RGB(255, 140, 0) : RGB(100, 200, 150);

        HPEN hPen = CreatePen(PS_SOLID, 2, color);
        HBRUSH hBrush = CreateSolidBrush(color);
        HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

        Ellipse(hdc, screenPos.x - 5, screenPos.y - 5, screenPos.x + 5, screenPos.y + 5);

        SelectObject(hdc, hOldPen);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hPen);
        DeleteObject(hBrush);

        if (i == selectedFeaturePoint) {
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 140, 0));
            TextOutW(hdc, screenPos.x + 10, screenPos.y - 20,
                fp.description.c_str(), (int)fp.description.length());
        }
    }
}

void Canvas::drawCreatingShape(HDC hdc) {
    if (!creatingShape || creatingShapeType == ShapeType::None) return;

    HPEN hPen = CreatePen(PS_SOLID, 2, RGB(100, 150, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    POINT start = worldToScreen(createStartPoint);
    POINT current = worldToScreen(createCurrentPoint);

    switch (creatingShapeType) {
    case ShapeType::Line: {
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, current.x, current.y);
        
        if (navigationMode) {
            Point2D snappedPoint = snapToAngle(createStartPoint, createCurrentPoint);
            if (angleSnapped) {
                HPEN hSnapPen = CreatePen(PS_DOT, 2, RGB(255, 180, 0));
                HPEN hOldSnapPen = (HPEN)SelectObject(hdc, hSnapPen);
                
                POINT snappedScreen = worldToScreen(snappedPoint);
                MoveToEx(hdc, start.x, start.y, NULL);
                LineTo(hdc, snappedScreen.x, snappedScreen.y);
                
                SelectObject(hdc, hOldSnapPen);
                DeleteObject(hSnapPen);
                
                double dx = snappedPoint.x - createStartPoint.x;
                double dy = snappedPoint.y - createStartPoint.y;
                double angle = atan2(dy, dx) * 180.0 / PI;
                if (angle < 0) angle += 360;
                SetBkMode(hdc, TRANSPARENT);
                SetTextColor(hdc, RGB(255, 180, 0));
                WCHAR angleText[32];
                swprintf_s(angleText, 32, L"%.0f°", angle);
                TextOutW(hdc, snappedScreen.x + 10, snappedScreen.y - 20, angleText, (int)wcslen(angleText));
                
                HPEN hIndicatorPen = CreatePen(PS_SOLID, 3, RGB(255, 180, 0));
                HPEN hOldIndPen = (HPEN)SelectObject(hdc, hIndicatorPen);
                Ellipse(hdc, snappedScreen.x - 5, snappedScreen.y - 5, snappedScreen.x + 5, snappedScreen.y + 5);
                SelectObject(hdc, hOldIndPen);
                DeleteObject(hIndicatorPen);
            }
        }
        break;
    }
    case ShapeType::Circle: {
        double radius = createStartPoint.distanceTo(createCurrentPoint);
        LONG r = (LONG)(radius * scale);
        Ellipse(hdc, start.x - r, start.y - r, start.x + r, start.y + r);
        break;
    }
    case ShapeType::Rectangle:
        Rectangle(hdc, start.x, start.y, current.x, current.y);
        break;
    case ShapeType::Triangle: {
        double size = 50.0;
        Point2D p1, p2, p3;

        switch (triangleType) {
        case TRIANGLE_EQUILATERAL: {
            double h = size * sqrt(3.0) / 2.0;
            p1 = Point2D(createCurrentPoint.x, createCurrentPoint.y + h * 2 / 3);
            p2 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y - h / 3);
            p3 = Point2D(createCurrentPoint.x + size / 2, createCurrentPoint.y - h / 3);
            break;
        }
        case TRIANGLE_ISOSCELES: {
            double baseWidth = size;
            double height = size * 1.2;
            p1 = Point2D(createCurrentPoint.x, createCurrentPoint.y + height / 2);
            p2 = Point2D(createCurrentPoint.x - baseWidth / 2, createCurrentPoint.y - height / 2);
            p3 = Point2D(createCurrentPoint.x + baseWidth / 2, createCurrentPoint.y - height / 2);
            break;
        }
        case TRIANGLE_RIGHT: {
            p1 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y + size / 2);
            p2 = Point2D(createCurrentPoint.x - size / 2, createCurrentPoint.y - size / 2);
            p3 = Point2D(createCurrentPoint.x + size / 2, createCurrentPoint.y - size / 2);
            break;
        }
        }
        POINT sp1 = worldToScreen(p1);
        POINT sp2 = worldToScreen(p2);
        POINT sp3 = worldToScreen(p3);
        MoveToEx(hdc, sp1.x, sp1.y, NULL);
        LineTo(hdc, sp2.x, sp2.y);
        LineTo(hdc, sp3.x, sp3.y);
        LineTo(hdc, sp1.x, sp1.y);
        break;
    }
    default:
        break;
    }

    SelectObject(hdc, hOldPen);
    SelectObject(hdc, hOldBrush);
    DeleteObject(hPen);
}

void Canvas::drawAngleGuide(HDC hdc) {
    if (!creatingShape || creatingShapeType != ShapeType::Line) return;
    if (!navigationMode) return;

    POINT start = worldToScreen(createStartPoint);
    HPEN hGuidePen = CreatePen(PS_DOT, 1, RGB(100, 120, 180));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hGuidePen);

    double angles[] = {0, 90, 180, 270, 45, 135, 225, 315};
    for (double angle : angles) {
        double rad = angle * PI / 180.0;
        double length = 1000.0;
        POINT end;
        end.x = start.x + (LONG)(length * cos(rad));
        end.y = start.y - (LONG)(length * sin(rad));
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, end.x, end.y);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hGuidePen);
}

void Canvas::draw(HDC hdc) {
    RECT rect = { 0, 0, windowWidth, windowHeight };
    HBRUSH hBgBrush = CreateSolidBrush(RGB(35, 35, 40));
    FillRect(hdc, &rect, hBgBrush);
    DeleteObject(hBgBrush);

    drawCoordinateSystem(hdc);

    for (const auto& shape : shapes) {
        shape->draw(hdc, offset, scale);
    }

    drawAngleGuide(hdc);
    drawCreatingShape(hdc);

    if (navigationMode) {
        drawFeaturePoints(hdc);
    }

    SetBkMode(hdc, TRANSPARENT);
    SetTextColor(hdc, RGB(180, 180, 190));

    WCHAR statusText[128];
    swprintf_s(statusText, 128, L"Scale: %.1fx | Nav: %s | Shapes: %d",
        scale, navigationMode ? L"ON" : L"OFF", (int)shapes.size());
    TextOutW(hdc, 10, 10, statusText, (int)wcslen(statusText));
}

void Canvas::startClipMode() {
    editMode = EditMode::SelectBoundary;
    creatingShapeType = ShapeType::None;
    selectedShapeIndex = -1;
    boundaryShapeIndices.clear();
}

void Canvas::startMirrorMode() {
    editMode = EditMode::SelectMirrorShape;
    creatingShapeType = ShapeType::None;
    selectedShapeIndex = -1;
    mirrorShapeIndex = -1;
    mirrorLineIndex = -1;
}

void Canvas::exitClipMode() {
    for (int idx : boundaryShapeIndices) {
        if (idx >= 0 && idx < (int)shapes.size()) {
            shapes[idx]->selected = false;
        }
    }
    boundaryShapeIndices.clear();
    editMode = EditMode::None;
    selectedShapeIndex = -1;
}

void Canvas::exitMirrorMode() {
    if (mirrorShapeIndex >= 0 && mirrorShapeIndex < (int)shapes.size()) {
        shapes[mirrorShapeIndex]->selected = false;
    }
    if (mirrorLineIndex >= 0 && mirrorLineIndex < (int)shapes.size()) {
        shapes[mirrorLineIndex]->selected = false;
    }
    editMode = EditMode::None;
    mirrorShapeIndex = -1;
    mirrorLineIndex = -1;
    selectedShapeIndex = -1;
}

void Canvas::setSelectedShapeIndex(int index) {
    selectedShapeIndex = index;
    if (index >= 0 && index < (int)shapes.size()) {
        shapes[index]->selected = true;
    }
}

bool segmentsIntersect(const Point2D& p1, const Point2D& p2, const Point2D& p3, const Point2D& p4, Point2D& outIntersection) {
    double d1 = pointToLineSide(p3, p1, p2);
    double d2 = pointToLineSide(p4, p1, p2);
    double d3 = pointToLineSide(p1, p3, p4);
    double d4 = pointToLineSide(p2, p3, p4);
    
    if (((d1 > 0 && d2 < 0) || (d1 < 0 && d2 > 0)) && 
        ((d3 > 0 && d4 < 0) || (d3 < 0 && d4 > 0))) {
        outIntersection = lineIntersection(p1, p2, p3, p4);
        return true;
    }
    return false;
}

bool isPointInsideCircle(const Point2D& p, const Point2D& center, double radius) {
    double dx = p.x - center.x;
    double dy = p.y - center.y;
    return (dx * dx + dy * dy) <= (radius * radius + 1e-8);
}

bool isPointInsideRectangle(const Point2D& p, const Point2D& topLeft, const Point2D& bottomRight) {
    double minX = std::min(topLeft.x, bottomRight.x);
    double maxX = std::max(topLeft.x, bottomRight.x);
    double minY = std::min(topLeft.y, bottomRight.y);
    double maxY = std::max(topLeft.y, bottomRight.y);
    return (p.x >= minX - 1e-8 && p.x <= maxX + 1e-8 && p.y >= minY - 1e-8 && p.y <= maxY + 1e-8);
}

double cross(const Point2D& a, const Point2D& b) {
    return a.x * b.y - a.y * b.x;
}

bool isPointInsideTriangle(const Point2D& p, const Point2D& a, const Point2D& b, const Point2D& c) {
    Point2D v0 = c - a;
    Point2D v1 = b - a;
    Point2D v2 = p - a;
    
    double dot00 = v0.x * v0.x + v0.y * v0.y;
    double dot01 = v0.x * v1.x + v0.y * v1.y;
    double dot02 = v0.x * v2.x + v0.y * v2.y;
    double dot11 = v1.x * v1.x + v1.y * v1.y;
    double dot12 = v1.x * v2.x + v1.y * v2.y;
    
    double invDenom = 1.0 / (dot00 * dot11 - dot01 * dot01);
    double u = (dot11 * dot02 - dot01 * dot12) * invDenom;
    double v = (dot00 * dot12 - dot01 * dot02) * invDenom;
    
    return (u >= -1e-8) && (v >= -1e-8) && (u + v <= 1.0 + 1e-8);
}

bool isPointInDeleteRegion(const Point2D& p, const std::vector<std::shared_ptr<Shape>>& boundaries, const Point2D& clickPos) {
    if (boundaries.empty()) return false;
    
    bool firstBoundaryIsLine = (boundaries[0]->getType() == ShapeType::Line);
    
    if (firstBoundaryIsLine) {
        for (auto boundary : boundaries) {
            if (boundary->getType() == ShapeType::Line) {
                auto line = std::dynamic_pointer_cast<LineShape>(boundary);
                double clickSide = pointToLineSide(clickPos, line->start, line->end);
                double side = pointToLineSide(p, line->start, line->end);
                bool onDeleteSide = (clickSide < 0 && side < 0) || (clickSide > 0 && side > 0);
                if (!onDeleteSide) {
                    return false;
                }
            }
        }
        return true;
    }
    else {
        for (auto boundary : boundaries) {
            if (boundary->getType() == ShapeType::Circle) {
                auto circle = std::dynamic_pointer_cast<CircleShape>(boundary);
                bool clickInside = isPointInsideCircle(clickPos, circle->center, circle->radius);
                bool pointInside = isPointInsideCircle(p, circle->center, circle->radius);
                if (clickInside != pointInside) {
                    return false;
                }
            }
            else if (boundary->getType() == ShapeType::Rectangle) {
                auto rect = std::dynamic_pointer_cast<RectangleShape>(boundary);
                bool clickInside = isPointInsideRectangle(clickPos, rect->topLeft, rect->bottomRight);
                bool pointInside = isPointInsideRectangle(p, rect->topLeft, rect->bottomRight);
                if (clickInside != pointInside) {
                    return false;
                }
            }
            else if (boundary->getType() == ShapeType::Triangle) {
                auto tri = std::dynamic_pointer_cast<TriangleShape>(boundary);
                bool clickInside = isPointInsideTriangle(clickPos, tri->p1, tri->p2, tri->p3);
                bool pointInside = isPointInsideTriangle(p, tri->p1, tri->p2, tri->p3);
                if (clickInside != pointInside) {
                    return false;
                }
            }
        }
        return true;
    }
}

void findIntersectionsWithBoundary(const Point2D& lineStart, const Point2D& lineEnd, 
                                   std::shared_ptr<Shape> boundary, 
                                   std::vector<Point2D>& intersections) {
    if (boundary->getType() == ShapeType::Line) {
        auto boundaryLine = std::dynamic_pointer_cast<LineShape>(boundary);
        Point2D intersection;
        if (segmentsIntersect(lineStart, lineEnd, boundaryLine->start, boundaryLine->end, intersection)) {
            intersections.push_back(intersection);
        }
    }
    else if (boundary->getType() == ShapeType::Circle) {
        auto circle = std::dynamic_pointer_cast<CircleShape>(boundary);
        Point2D dir = lineEnd - lineStart;
        Point2D f = lineStart - circle->center;
        
        double a = dir.x * dir.x + dir.y * dir.y;
        double b = 2 * (f.x * dir.x + f.y * dir.y);
        double c = (f.x * f.x + f.y * f.y) - circle->radius * circle->radius;
        
        double discriminant = b * b - 4 * a * c;
        if (discriminant >= 0) {
            discriminant = sqrt(discriminant);
            double t1 = (-b - discriminant) / (2 * a);
            double t2 = (-b + discriminant) / (2 * a);
            
            if (t1 >= 0 && t1 <= 1) {
                intersections.push_back(Point2D(lineStart.x + t1 * dir.x, lineStart.y + t1 * dir.y));
            }
            if (t2 >= 0 && t2 <= 1 && fabs(t2 - t1) > 1e-6) {
                intersections.push_back(Point2D(lineStart.x + t2 * dir.x, lineStart.y + t2 * dir.y));
            }
        }
    }
    else if (boundary->getType() == ShapeType::Rectangle) {
        auto rect = std::dynamic_pointer_cast<RectangleShape>(boundary);
        Point2D corners[4] = {
            rect->topLeft,
            Point2D(rect->bottomRight.x, rect->topLeft.y),
            rect->bottomRight,
            Point2D(rect->topLeft.x, rect->bottomRight.y)
        };
        
        for (int i = 0; i < 4; i++) {
            Point2D intersection;
            if (segmentsIntersect(lineStart, lineEnd, corners[i], corners[(i+1)%4], intersection)) {
                intersections.push_back(intersection);
            }
        }
    }
    else if (boundary->getType() == ShapeType::Triangle) {
        auto tri = std::dynamic_pointer_cast<TriangleShape>(boundary);
        Point2D corners[3] = { tri->p1, tri->p2, tri->p3 };
        
        for (int i = 0; i < 3; i++) {
            Point2D intersection;
            if (segmentsIntersect(lineStart, lineEnd, corners[i], corners[(i+1)%3], intersection)) {
                intersections.push_back(intersection);
            }
        }
    }
}

void Canvas::performClip(int shapeIndex, const Point2D& clickPos) {
    if (boundaryShapeIndices.empty()) return;
    if (shapeIndex < 0 || shapeIndex >= (int)shapes.size()) return;
    
    for (int boundaryIdx : boundaryShapeIndices) {
        if (boundaryIdx == shapeIndex) return;
    }

    std::vector<std::shared_ptr<Shape>> boundaries;
    for (int idx : boundaryShapeIndices) {
        if (idx >= 0 && idx < (int)shapes.size()) {
            boundaries.push_back(shapes[idx]);
        }
    }
    
    if (boundaries.empty()) return;

    auto shape = shapes[shapeIndex];
    
    if (shape->getType() == ShapeType::Line) {
        auto line = std::dynamic_pointer_cast<LineShape>(shape);
        
        std::vector<Point2D> allPoints;
        allPoints.push_back(line->start);
        allPoints.push_back(line->end);
        
        for (auto boundary : boundaries) {
            findIntersectionsWithBoundary(line->start, line->end, boundary, allPoints);
        }
        
        std::sort(allPoints.begin(), allPoints.end(), [&](const Point2D& a, const Point2D& b) {
            double distA = (a.x - line->start.x) * (a.x - line->start.x) + (a.y - line->start.y) * (a.y - line->start.y);
            double distB = (b.x - line->start.x) * (b.x - line->start.x) + (b.y - line->start.y) * (b.y - line->start.y);
            return distA < distB;
        });
        
        if (allPoints.size() <= 2) {
            bool startInDelete = isPointInDeleteRegion(line->start, boundaries, clickPos);
            bool endInDelete = isPointInDeleteRegion(line->end, boundaries, clickPos);
            
            if (startInDelete && endInDelete) {
                shapes.erase(shapes.begin() + shapeIndex);
                return;
            }
            else if (startInDelete || endInDelete) {
                for (auto boundary : boundaries) {
                    std::vector<Point2D> intersections;
                    findIntersectionsWithBoundary(line->start, line->end, boundary, intersections);
                    if (!intersections.empty()) {
                        if (startInDelete) {
                            line->start = intersections[0];
                        } else {
                            line->end = intersections[0];
                        }
                    }
                }
            }
        }
        else {
            std::vector<std::pair<Point2D, Point2D>> keepSegments;
            
            for (size_t i = 0; i < allPoints.size() - 1; i++) {
                Point2D midPt((allPoints[i].x + allPoints[i+1].x) / 2, (allPoints[i].y + allPoints[i+1].y) / 2);
                if (!isPointInDeleteRegion(midPt, boundaries, clickPos)) {
                    keepSegments.push_back({allPoints[i], allPoints[i+1]});
                }
            }
            
            if (keepSegments.empty()) {
                shapes.erase(shapes.begin() + shapeIndex);
                return;
            }
            else {
                line->start = keepSegments[0].first;
                line->end = keepSegments[0].second;
                
                for (size_t i = 1; i < keepSegments.size(); i++) {
                    auto newLine = std::make_shared<LineShape>(keepSegments[i].first, keepSegments[i].second);
                    shapes.push_back(newLine);
                }
            }
        }
    }
    else if (shape->getType() == ShapeType::Circle) {
        auto circle = std::dynamic_pointer_cast<CircleShape>(shape);
        
        bool centerInDelete = isPointInDeleteRegion(circle->center, boundaries, clickPos);
        
        if (centerInDelete) {
            shapes.erase(shapes.begin() + shapeIndex);
            return;
        }
        
        double minRadius = circle->radius;
        for (auto boundary : boundaries) {
            if (boundary->getType() == ShapeType::Line) {
                auto boundaryLine = std::dynamic_pointer_cast<LineShape>(boundary);
                double dist = fabs(pointToLineSide(circle->center, boundaryLine->start, boundaryLine->end));
                double clickSide = pointToLineSide(clickPos, boundaryLine->start, boundaryLine->end);
                double side = pointToLineSide(circle->center, boundaryLine->start, boundaryLine->end);
                bool centerOnKeepSide = !((clickSide < 0 && side < 0) || (clickSide > 0 && side > 0));
                
                if (centerOnKeepSide && dist < circle->radius) {
                    minRadius = std::min(minRadius, dist);
                }
            }
        }
        
        if (minRadius < circle->radius - 1e-6) {
            circle->radius = minRadius;
        }
    }
    else if (shape->getType() == ShapeType::Rectangle) {
        auto rect = std::dynamic_pointer_cast<RectangleShape>(shape);
        Point2D corners[4] = {
            rect->topLeft,
            Point2D(rect->bottomRight.x, rect->topLeft.y),
            rect->bottomRight,
            Point2D(rect->topLeft.x, rect->bottomRight.y)
        };
        
        std::vector<Point2D> keepPoints;
        std::vector<Point2D> intersectionPoints;
        
        for (int i = 0; i < 4; i++) {
            if (!isPointInDeleteRegion(corners[i], boundaries, clickPos)) {
                keepPoints.push_back(corners[i]);
            }
        }
        
        for (auto boundary : boundaries) {
            for (int i = 0; i < 4; i++) {
                Point2D intersection;
                std::vector<Point2D> inters;
                findIntersectionsWithBoundary(corners[i], corners[(i+1)%4], boundary, inters);
                for (auto& pt : inters) {
                    intersectionPoints.push_back(pt);
                }
            }
        }
        
        for (auto& pt : intersectionPoints) {
            if (!isPointInDeleteRegion(pt, boundaries, clickPos)) {
                keepPoints.push_back(pt);
            }
        }
        
        if (keepPoints.size() == 0) {
            shapes.erase(shapes.begin() + shapeIndex);
            return;
        }
        else if (keepPoints.size() >= 2) {
            double minX = keepPoints[0].x, maxX = keepPoints[0].x;
            double minY = keepPoints[0].y, maxY = keepPoints[0].y;
            for (auto& p : keepPoints) {
                minX = std::min(minX, p.x);
                maxX = std::max(maxX, p.x);
                minY = std::min(minY, p.y);
                maxY = std::max(maxY, p.y);
            }
            rect->topLeft = Point2D(minX, minY);
            rect->bottomRight = Point2D(maxX, maxY);
        }
    }
    else if (shape->getType() == ShapeType::Triangle) {
        auto tri = std::dynamic_pointer_cast<TriangleShape>(shape);
        Point2D corners[3] = { tri->p1, tri->p2, tri->p3 };
        
        std::vector<Point2D> keepPoints;
        for (int i = 0; i < 3; i++) {
            if (!isPointInDeleteRegion(corners[i], boundaries, clickPos)) {
                keepPoints.push_back(corners[i]);
            }
        }
        
        if (keepPoints.size() == 0) {
            shapes.erase(shapes.begin() + shapeIndex);
            return;
        }
        else if (keepPoints.size() >= 2) {
            tri->p1 = keepPoints[0];
            tri->p2 = keepPoints[1];
            if (keepPoints.size() >= 3) {
                tri->p3 = keepPoints[2];
            }
        }
    }
}

void Canvas::performMirror() {
    if (mirrorShapeIndex < 0 || mirrorShapeIndex >= (int)shapes.size()) return;
    if (mirrorLineIndex < 0 || mirrorLineIndex >= (int)shapes.size()) return;
    
    auto mirrorLine = std::dynamic_pointer_cast<LineShape>(shapes[mirrorLineIndex]);
    if (!mirrorLine) return;

    auto mirroredShape = shapes[mirrorShapeIndex]->mirror(mirrorLine->start, mirrorLine->end);
    if (mirroredShape) {
        shapes.push_back(mirroredShape);
    }
    exitMirrorMode();
}

std::wstring Canvas::getStatusText() const {
    switch (editMode) {
    case EditMode::SelectBoundary:
        return L"Clip Mode: Click to select boundary lines (multi-select), right-click to confirm";
    case EditMode::SelectShapeToClip:
        return L"Clip Mode: Click shapes to clip, right-click to finish";
    case EditMode::SelectMirrorShape:
        return L"Mirror Mode: Select shape to mirror";
    case EditMode::SelectMirrorLine:
        return L"Mirror Mode: Select line as mirror axis";
    default:
        if (creatingShapeType == ShapeType::Line)
            return L"Drawing Line: Click to set start point, click again to set end (hold Shift to pan)";
        else if (creatingShapeType == ShapeType::Circle)
            return L"Drawing Circle: Click to set center, drag to set radius";
        else if (creatingShapeType == ShapeType::Rectangle)
            return L"Drawing Rectangle: Click to set corner, drag to set opposite";
        else if (creatingShapeType == ShapeType::Triangle)
            return L"Drawing Triangle: Click to set center position";
        else
            return L"Ready - Select tool from toolbar, scroll to zoom, Shift+drag to pan";
    }
}
