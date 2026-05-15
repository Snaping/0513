#include "Physics.h"

PhysicsBody::PhysicsBody()
    : m_mass(1.0f), m_gravityScale(1.0f), m_useGravity(true) {}

void PhysicsBody::SetMass(float mass) {
    m_mass = mass;
}

float PhysicsBody::GetMass() const {
    return m_mass;
}

void PhysicsBody::SetPosition(const Vector2& position) {
    m_position = position;
}

Vector2 PhysicsBody::GetPosition() const {
    return m_position;
}

void PhysicsBody::SetVelocity(const Vector2& velocity) {
    m_velocity = velocity;
}

Vector2 PhysicsBody::GetVelocity() const {
    return m_velocity;
}

void PhysicsBody::SetAcceleration(const Vector2& acceleration) {
    m_acceleration = acceleration;
}

Vector2 PhysicsBody::GetAcceleration() const {
    return m_acceleration;
}

void PhysicsBody::SetGravityScale(float scale) {
    m_gravityScale = scale;
}

float PhysicsBody::GetGravityScale() const {
    return m_gravityScale;
}

void PhysicsBody::SetUseGravity(bool use) {
    m_useGravity = use;
}

bool PhysicsBody::GetUseGravity() const {
    return m_useGravity;
}

void PhysicsBody::ApplyForce(const Vector2& force) {
    m_acceleration += force / m_mass;
}

void PhysicsBody::ApplyImpulse(const Vector2& impulse) {
    m_velocity += impulse / m_mass;
}

void PhysicsBody::Update(float deltaTime) {
    m_velocity += m_acceleration * deltaTime;
    m_position += m_velocity * deltaTime;
    m_acceleration = Vector2(0.0f, 0.0f);
}

PhysicsWorld::PhysicsWorld() : m_gravity(Vector2(0.0f, 980.0f)) {}

void PhysicsWorld::SetGravity(const Vector2& gravity) {
    m_gravity = gravity;
}

Vector2 PhysicsWorld::GetGravity() const {
    return m_gravity;
}

void PhysicsWorld::Update(float deltaTime) {
}
