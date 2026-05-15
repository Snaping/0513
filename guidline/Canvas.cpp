#include "Canvas.h"

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
    , fillEnabled(false) {
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
    if (creatingShapeType == ShapeType::Line && navigationMode) {
        createCurrentPoint = snapToAngle(createStartPoint, worldPos);
    }
    else if (creatingShapeType == ShapeType::Triangle) {
        createCurrentPoint = worldPos;
        createStartPoint = worldPos;
    }
    else {
        createCurrentPoint = worldPos;
    }
}

void Canvas::finishCreatingShape() {
    if (!creatingShape || creatingShapeType == ShapeType::None) return;

    std::shared_ptr<Shape> shape = nullptr;

    switch (creatingShapeType) {
    case ShapeType::Line:
        shape = std::make_shared<LineShape>(createStartPoint, createCurrentPoint);
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

    HPEN hPen = CreatePen(PS_DASH, 2, RGB(100, 150, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);
    HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));

    POINT start = worldToScreen(createStartPoint);
    POINT current = worldToScreen(createCurrentPoint);

    switch (creatingShapeType) {
    case ShapeType::Line:
        MoveToEx(hdc, start.x, start.y, NULL);
        LineTo(hdc, current.x, current.y);
        if (angleSnapped) {
            double dx = createCurrentPoint.x - createStartPoint.x;
            double dy = createCurrentPoint.y - createStartPoint.y;
            double angle = atan2(dy, dx) * 180.0 / PI;
            if (angle < 0) angle += 360;
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 140, 0));
            WCHAR angleText[32];
            swprintf_s(angleText, 32, L"%.0f deg", angle);
            TextOutW(hdc, current.x + 10, current.y - 20, angleText, (int)wcslen(angleText));
        }
        break;
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
