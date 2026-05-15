#pragma once

#include "Point2D.h"
#include "Vector2D.h"
#include <cmath>

namespace GEngine {

class Matrix3x3 {
public:
    double m[3][3];

    Matrix3x3() {
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                m[i][j] = (i == j) ? 1.0 : 0.0;
            }
        }
    }

    Matrix3x3(double m00, double m01, double m02,
             double m10, double m11, double m12,
             double m20, double m21, double m22) {
        m[0][0] = m00; m[0][1] = m01; m[0][2] = m02;
        m[1][0] = m10; m[1][1] = m11; m[1][2] = m12;
        m[2][0] = m20; m[2][1] = m21; m[2][2] = m22;
    }

    static Matrix3x3 translation(double tx, double ty) {
        return Matrix3x3(
            1.0, 0.0, tx,
            0.0, 1.0, ty,
            0.0, 0.0, 1.0
        );
    }

    static Matrix3x3 rotation(double angle) {
        double cosA = std::cos(angle);
        double sinA = std::sin(angle);
        return Matrix3x3(
            cosA, -sinA, 0.0,
            sinA, cosA, 0.0,
            0.0, 0.0, 1.0
        );
    }

    static Matrix3x3 scaling(double sx, double sy) {
        return Matrix3x3(
            sx, 0.0, 0.0,
            0.0, sy, 0.0,
            0.0, 0.0, 1.0
        );
    }

    Matrix3x3 operator*(const Matrix3x3& other) const {
        Matrix3x3 result;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                result.m[i][j] = 0.0;
                for (int k = 0; k < 3; ++k) {
                    result.m[i][j] += m[i][k] * other.m[k][j];
                }
            }
        }
        return result;
    }

    Point2D transform(const Point2D& p) const {
        double x = m[0][0] * p.x + m[0][1] * p.y + m[0][2] * 1.0;
        double y = m[1][0] * p.x + m[1][1] * p.y + m[1][2] * 1.0;
        return Point2D(x, y);
    }

    Vector2D transform(const Vector2D& v) const {
        double x = m[0][0] * v.x + m[0][1] * v.y;
        double y = m[1][0] * v.x + m[1][1] * v.y;
        return Vector2D(x, y);
    }
};

} 
