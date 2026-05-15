#pragma once

#include <cmath>

namespace GEngine {

class Point2D {
public:
    double x;
    double y;

    Point2D() : x(0.0), y(0.0) {}
    Point2D(double xCoord, double yCoord) : x(xCoord), y(yCoord) {}

    Point2D operator+(const Point2D& other) const {
        return Point2D(x + other.x, y + other.y);
    }

    Point2D operator-(const Point2D& other) const {
        return Point2D(x - other.x, y - other.y);
    }

    Point2D operator*(double scalar) const {
        return Point2D(x * scalar, y * scalar);
    }

    Point2D operator/(double scalar) const {
        return Point2D(x / scalar, y / scalar);
    }

    bool operator==(const Point2D& other) const {
        return std::abs(x - other.x) < 1e-6 && std::abs(y - other.y) < 1e-6;
    }

    bool operator!=(const Point2D& other) const {
        return !(*this == other);
    }

    double distanceTo(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return std::sqrt(dx * dx + dy * dy);
    }

    double distanceSquaredTo(const Point2D& other) const {
        double dx = x - other.x;
        double dy = y - other.y;
        return dx * dx + dy * dy;
    }
};

} 
