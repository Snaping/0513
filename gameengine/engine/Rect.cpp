#include "Rect.h"
#include <algorithm>

Rect::Rect() : x(0.0f), y(0.0f), width(0.0f), height(0.0f) {}

Rect::Rect(float x, float y, float width, float height)
    : x(x), y(y), width(width), height(height) {}

Rect::Rect(const Vector2& position, const Vector2& size)
    : x(position.x), y(position.y), width(size.x), height(size.y) {}

float Rect::Left() const { return x; }
float Rect::Right() const { return x + width; }
float Rect::Top() const { return y; }
float Rect::Bottom() const { return y + height; }

Vector2 Rect::GetPosition() const { return Vector2(x, y); }
Vector2 Rect::GetSize() const { return Vector2(width, height); }

void Rect::SetPosition(const Vector2& position) {
    x = position.x;
    y = position.y;
}

void Rect::SetSize(const Vector2& size) {
    width = size.x;
    height = size.y;
}

bool Rect::Contains(const Vector2& point) const {
    return point.x >= Left() && point.x <= Right() &&
           point.y >= Top() && point.y <= Bottom();
}

bool Rect::Intersects(const Rect& other) const {
    return Left() < other.Right() && Right() > other.Left() &&
           Top() < other.Bottom() && Bottom() > other.Top();
}

Rect Rect::GetIntersection(const Rect& other) const {
    float left = std::max(Left(), other.Left());
    float right = std::min(Right(), other.Right());
    float top = std::max(Top(), other.Top());
    float bottom = std::min(Bottom(), other.Bottom());

    if (left < right && top < bottom) {
        return Rect(left, top, right - left, bottom - top);
    }
    return Rect(0, 0, 0, 0);
}
