#pragma once
#include "Vector2.h"

class PhysicsBody {
public:
    PhysicsBody();

    void SetMass(float mass);
    float GetMass() const;

    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetVelocity(const Vector2& velocity);
    Vector2 GetVelocity() const;

    void SetAcceleration(const Vector2& acceleration);
    Vector2 GetAcceleration() const;

    void SetGravityScale(float scale);
    float GetGravityScale() const;

    void SetUseGravity(bool use);
    bool GetUseGravity() const;

    void ApplyForce(const Vector2& force);
    void ApplyImpulse(const Vector2& impulse);

    void Update(float deltaTime);

private:
    Vector2 m_position;
    Vector2 m_velocity;
    Vector2 m_acceleration;
    float m_mass;
    float m_gravityScale;
    bool m_useGravity;
};

class PhysicsWorld {
public:
    PhysicsWorld();

    void SetGravity(const Vector2& gravity);
    Vector2 GetGravity() const;

    void Update(float deltaTime);

private:
    Vector2 m_gravity;
};
