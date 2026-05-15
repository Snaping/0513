#pragma once
#include "Vector2.h"

struct Rect {
    float x, y, width, height;

    Rect();
    Rect(float x, float y, float width, float height);
    Rect(const Vector2& position, const Vector2& size);

    float Left() const;
    float Right() const;
    float Top() const;
    float Bottom() const;

    Vector2 GetPosition() const;
    Vector2 GetSize() const;

    void SetPosition(const Vector2& position);
    void SetSize(const Vector2& size);

    bool Contains(const Vector2& point) const;
    bool Intersects(const Rect& other) const;
    Rect GetIntersection(const Rect& other) const;
};
