#include "Geometry.h"

void LineShape::draw(HDC hdc, const Point2D& offset, double scale) const {
    POINT s, e;
    s.x = (LONG)((start.x + offset.x) * scale);
    s.y = (LONG)((-start.y + offset.y) * scale);
    e.x = (LONG)((end.x + offset.x) * scale);
    e.y = (LONG)((-end.y + offset.y) * scale);

    HPEN hPen = CreatePen(PS_SOLID, 2, selected ? RGB(255, 0, 0) : RGB(0, 150, 255));
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    MoveToEx(hdc, s.x, s.y, NULL);
    LineTo(hdc, e.x, e.y);

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

std::vector<FeaturePoint> LineShape::getFeaturePoints() const {
    std::vector<FeaturePoint> points;
    points.push_back(FeaturePoint(start, FeaturePointType::Endpoint, ShapeType::Line, -1, L"Start"));
    points.push_back(FeaturePoint(end, FeaturePointType::Endpoint, ShapeType::Line, -1, L"End"));
    Point2D mid((start.x + end.x) / 2, (start.y + end.y) / 2);
    points.push_back(FeaturePoint(mid, FeaturePointType::Midpoint, ShapeType::Line, -1, L"Midpoint"));
    return points;
}

bool LineShape::hitTest(const Point2D& worldPos, double threshold) const {
    double A = worldPos.x - start.x;
    double B = worldPos.y - start.y;
    double C = end.x - start.x;
    double D = end.y - start.y;

    double dot = A * C + B * D;
    double lenSq = C * C + D * D;
    double param = lenSq != 0 ? dot / lenSq : -1;

    double xx, yy;

    if (param < 0) {
        xx = start.x;
        yy = start.y;
    }
    else if (param > 1) {
        xx = end.x;
        yy = end.y;
    }
    else {
        xx = start.x + param * C;
        yy = start.y + param * D;
    }

    double dx = worldPos.x - xx;
    double dy = worldPos.y - yy;
    return sqrt(dx * dx + dy * dy) < threshold;
}

void CircleShape::draw(HDC hdc, const Point2D& offset, double scale) const {
    POINT c;
    c.x = (LONG)((center.x + offset.x) * scale);
    c.y = (LONG)((-center.y + offset.y) * scale);
    LONG r = (LONG)(radius * scale);

    COLORREF lineColor = selected ? RGB(255, 0, 0) : RGB(255, 140, 0);
    HPEN hPen = CreatePen(PS_SOLID, 2, lineColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    if (filled) {
        COLORREF fillColor = RGB(255, 140, 0);
        COLORREF transparentFill = RGB(
            GetRValue(fillColor) * 0.3 + 35 * 0.7,
            GetGValue(fillColor) * 0.3 + 35 * 0.7,
            GetBValue(fillColor) * 0.3 + 35 * 0.7
        );
        HBRUSH hBrush = CreateSolidBrush(transparentFill);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Ellipse(hdc, c.x - r, c.y - r, c.x + r, c.y + r);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }
    else {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Ellipse(hdc, c.x - r, c.y - r, c.x + r, c.y + r);
        SelectObject(hdc, hOldBrush);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

std::vector<FeaturePoint> CircleShape::getFeaturePoints() const {
    std::vector<FeaturePoint> points;
    points.push_back(FeaturePoint(center, FeaturePointType::Center, ShapeType::Circle, -1, L"Center"));
    return points;
}

bool CircleShape::hitTest(const Point2D& worldPos, double threshold) const {
    double dist = worldPos.distanceTo(center);
    return fabs(dist - radius) < threshold;
}

void RectangleShape::draw(HDC hdc, const Point2D& offset, double scale) const {
    POINT tl, br;
    tl.x = (LONG)((topLeft.x + offset.x) * scale);
    tl.y = (LONG)((-topLeft.y + offset.y) * scale);
    br.x = (LONG)((bottomRight.x + offset.x) * scale);
    br.y = (LONG)((-bottomRight.y + offset.y) * scale);

    COLORREF lineColor = selected ? RGB(255, 0, 0) : RGB(0, 200, 120);
    HPEN hPen = CreatePen(PS_SOLID, 2, lineColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    if (filled) {
        COLORREF fillColor = RGB(0, 200, 120);
        COLORREF transparentFill = RGB(
            GetRValue(fillColor) * 0.3 + 35 * 0.7,
            GetGValue(fillColor) * 0.3 + 35 * 0.7,
            GetBValue(fillColor) * 0.3 + 35 * 0.7
        );
        HBRUSH hBrush = CreateSolidBrush(transparentFill);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Rectangle(hdc, tl.x, tl.y, br.x, br.y);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }
    else {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Rectangle(hdc, tl.x, tl.y, br.x, br.y);
        SelectObject(hdc, hOldBrush);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

std::vector<FeaturePoint> RectangleShape::getFeaturePoints() const {
    std::vector<FeaturePoint> points;

    Point2D tr(bottomRight.x, topLeft.y);
    Point2D bl(topLeft.x, bottomRight.y);

    points.push_back(FeaturePoint(topLeft, FeaturePointType::Endpoint, ShapeType::Rectangle, -1, L"Top Left"));
    points.push_back(FeaturePoint(tr, FeaturePointType::Endpoint, ShapeType::Rectangle, -1, L"Top Right"));
    points.push_back(FeaturePoint(bottomRight, FeaturePointType::Endpoint, ShapeType::Rectangle, -1, L"Bottom Right"));
    points.push_back(FeaturePoint(bl, FeaturePointType::Endpoint, ShapeType::Rectangle, -1, L"Bottom Left"));

    Point2D topMid((topLeft.x + tr.x) / 2, topLeft.y);
    Point2D rightMid(tr.x, (tr.y + bottomRight.y) / 2);
    Point2D bottomMid((bottomRight.x + bl.x) / 2, bottomRight.y);
    Point2D leftMid(bl.x, (bl.y + topLeft.y) / 2);

    points.push_back(FeaturePoint(topMid, FeaturePointType::Midpoint, ShapeType::Rectangle, -1, L"Top Mid"));
    points.push_back(FeaturePoint(rightMid, FeaturePointType::Midpoint, ShapeType::Rectangle, -1, L"Right Mid"));
    points.push_back(FeaturePoint(bottomMid, FeaturePointType::Midpoint, ShapeType::Rectangle, -1, L"Bottom Mid"));
    points.push_back(FeaturePoint(leftMid, FeaturePointType::Midpoint, ShapeType::Rectangle, -1, L"Left Mid"));

    Point2D center((topLeft.x + bottomRight.x) / 2, (topLeft.y + bottomRight.y) / 2);
    points.push_back(FeaturePoint(center, FeaturePointType::Center, ShapeType::Rectangle, -1, L"Center"));

    return points;
}

bool RectangleShape::hitTest(const Point2D& worldPos, double threshold) const {
    double minX = (topLeft.x < bottomRight.x) ? topLeft.x : bottomRight.x;
    double maxX = (topLeft.x > bottomRight.x) ? topLeft.x : bottomRight.x;
    double minY = (topLeft.y < bottomRight.y) ? topLeft.y : bottomRight.y;
    double maxY = (topLeft.y > bottomRight.y) ? topLeft.y : bottomRight.y;

    bool onVerticalEdge = (fabs(worldPos.x - minX) < threshold || fabs(worldPos.x - maxX) < threshold)
        && worldPos.y >= minY - threshold && worldPos.y <= maxY + threshold;

    bool onHorizontalEdge = (fabs(worldPos.y - minY) < threshold || fabs(worldPos.y - maxY) < threshold)
        && worldPos.x >= minX - threshold && worldPos.x <= maxX + threshold;

    return onVerticalEdge || onHorizontalEdge;
}

void TriangleShape::draw(HDC hdc, const Point2D& offset, double scale) const {
    POINT a, b, c;
    a.x = (LONG)((p1.x + offset.x) * scale);
    a.y = (LONG)((-p1.y + offset.y) * scale);
    b.x = (LONG)((p2.x + offset.x) * scale);
    b.y = (LONG)((-p2.y + offset.y) * scale);
    c.x = (LONG)((p3.x + offset.x) * scale);
    c.y = (LONG)((-p3.y + offset.y) * scale);

    COLORREF lineColor = selected ? RGB(255, 0, 0) : RGB(180, 80, 200);
    HPEN hPen = CreatePen(PS_SOLID, 2, lineColor);
    HPEN hOldPen = (HPEN)SelectObject(hdc, hPen);

    if (filled) {
        COLORREF fillColor = RGB(180, 80, 200);
        COLORREF transparentFill = RGB(
            GetRValue(fillColor) * 0.3 + 35 * 0.7,
            GetGValue(fillColor) * 0.3 + 35 * 0.7,
            GetBValue(fillColor) * 0.3 + 35 * 0.7
        );
        HBRUSH hBrush = CreateSolidBrush(transparentFill);
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);
        Polygon(hdc, new POINT[3]{a, b, c}, 3);
        SelectObject(hdc, hOldBrush);
        DeleteObject(hBrush);
    }
    else {
        HBRUSH hOldBrush = (HBRUSH)SelectObject(hdc, GetStockObject(NULL_BRUSH));
        Polygon(hdc, new POINT[3]{a, b, c}, 3);
        SelectObject(hdc, hOldBrush);
    }

    SelectObject(hdc, hOldPen);
    DeleteObject(hPen);
}

std::vector<FeaturePoint> TriangleShape::getFeaturePoints() const {
    std::vector<FeaturePoint> points;
    points.push_back(FeaturePoint(p1, FeaturePointType::Endpoint, ShapeType::Triangle, -1, L"Vertex 1"));
    points.push_back(FeaturePoint(p2, FeaturePointType::Endpoint, ShapeType::Triangle, -1, L"Vertex 2"));
    points.push_back(FeaturePoint(p3, FeaturePointType::Endpoint, ShapeType::Triangle, -1, L"Vertex 3"));

    Point2D center((p1.x + p2.x + p3.x) / 3, (p1.y + p2.y + p3.y) / 3);
    points.push_back(FeaturePoint(center, FeaturePointType::CenterOfGravity, ShapeType::Triangle, -1, L"Centroid"));

    return points;
}

bool TriangleShape::hitTest(const Point2D& worldPos, double threshold) const {
    LineShape l1(p1, p2), l2(p2, p3), l3(p3, p1);
    return l1.hitTest(worldPos, threshold) || l2.hitTest(worldPos, threshold) || l3.hitTest(worldPos, threshold);
}
