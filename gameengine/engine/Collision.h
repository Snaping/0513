#pragma once
#include "Rect.h"
#include "Vector2.h"

struct CollisionResult {
    bool collided;
    Vector2 normal;
    float penetration;
    Rect colliderA;
    Rect colliderB;
};

class Collider {
public:
    Collider();
    Collider(const Rect& bounds);

    void SetBounds(const Rect& bounds);
    Rect GetBounds() const;

    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetSize(const Vector2& size);
    Vector2 GetSize() const;

    bool Intersects(const Collider& other) const;
    CollisionResult Collide(const Collider& other) const;

private:
    Rect m_bounds;
};
