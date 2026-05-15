#pragma once

#include "Point2D.h"
#include <cmath>

namespace GEngine {

class Vector2D {
public:
    double x;
    double y;

    Vector2D() : x(0.0), y(0.0) {}
    Vector2D(double xVal, double yVal) : x(xVal), y(yVal) {}
    Vector2D(const Point2D& from, const Point2D& to) 
        : x(to.x - from.x), y(to.y - from.y) {}

    Vector2D operator+(const Vector2D& other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D& other) const {
        return Vector2D(x - other.x, y - other.y);
    }

    Vector2D operator*(double scalar) const {
        return Vector2D(x * scalar, y * scalar);
    }

    Vector2D operator/(double scalar) const {
        return Vector2D(x / scalar, y / scalar);
    }

    double dot(const Vector2D& other) const {
        return x * other.x + y * other.y;
    }

    double cross(const Vector2D& other) const {
        return x * other.y - y * other.x;
    }

    double length() const {
        return std::sqrt(x * x + y * y);
    }

    double lengthSquared() const {
        return x * x + y * y;
    }

    Vector2D normalized() const {
        double len = length();
        if (len < 1e-6) {
            return Vector2D(0, 0);
        }
        return Vector2D(x / len, y / len);
    }

    Vector2D perpendicular() const {
        return Vector2D(-y, x);
    }

    double angle() const {
        return std::atan2(y, x);
    }

    double angleTo(const Vector2D& other) const {
        return std::atan2(cross(other), dot(other));
    }
};

} 
