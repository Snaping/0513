#include "Collision.h"
#include <algorithm>

Collider::Collider() {}

Collider::Collider(const Rect& bounds) : m_bounds(bounds) {}

void Collider::SetBounds(const Rect& bounds) {
    m_bounds = bounds;
}

Rect Collider::GetBounds() const {
    return m_bounds;
}

void Collider::SetPosition(const Vector2& position) {
    m_bounds.SetPosition(position);
}

Vector2 Collider::GetPosition() const {
    return m_bounds.GetPosition();
}

void Collider::SetSize(const Vector2& size) {
    m_bounds.SetSize(size);
}

Vector2 Collider::GetSize() const {
    return m_bounds.GetSize();
}

bool Collider::Intersects(const Collider& other) const {
    return m_bounds.Intersects(other.m_bounds);
}

CollisionResult Collider::Collide(const Collider& other) const {
    CollisionResult result;
    result.collided = false;
    result.colliderA = m_bounds;
    result.colliderB = other.m_bounds;

    if (!m_bounds.Intersects(other.m_bounds)) {
        return result;
    }

    Rect intersection = m_bounds.GetIntersection(other.m_bounds);
    result.collided = true;

    float overlapLeft = m_bounds.Right() - other.m_bounds.Left();
    float overlapRight = other.m_bounds.Right() - m_bounds.Left();
    float overlapTop = m_bounds.Bottom() - other.m_bounds.Top();
    float overlapBottom = other.m_bounds.Bottom() - m_bounds.Top();

    float minOverlapX = std::min(overlapLeft, overlapRight);
    float minOverlapY = std::min(overlapTop, overlapBottom);

    if (minOverlapX < minOverlapY) {
        result.penetration = minOverlapX;
        if (overlapLeft < overlapRight) {
            result.normal = Vector2(-1.0f, 0.0f);
        } else {
            result.normal = Vector2(1.0f, 0.0f);
        }
    } else {
        result.penetration = minOverlapY;
        if (overlapTop < overlapBottom) {
            result.normal = Vector2(0.0f, -1.0f);
        } else {
            result.normal = Vector2(0.0f, 1.0f);
        }
    }

    return result;
}
