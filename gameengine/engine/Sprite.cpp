#define NOMINMAX
#include "Sprite.h"
#include "Renderer.h"

Sprite::Sprite()
    : m_position(0.0f, 0.0f), m_size(100.0f, 100.0f),
      m_origin(0.0f, 0.0f), m_color(Color::White()), m_rotation(0.0f) {}

Sprite::~Sprite() {}

void Sprite::SetPosition(const Vector2& position) {
    m_position = position;
}

Vector2 Sprite::GetPosition() const {
    return m_position;
}

void Sprite::SetSize(const Vector2& size) {
    m_size = size;
}

Vector2 Sprite::GetSize() const {
    return m_size;
}

void Sprite::SetColor(const Color& color) {
    m_color = color;
}

Color Sprite::GetColor() const {
    return m_color;
}

void Sprite::SetOrigin(const Vector2& origin) {
    m_origin = origin;
}

Vector2 Sprite::GetOrigin() const {
    return m_origin;
}

void Sprite::SetRotation(float rotation) {
    m_rotation = rotation;
}

float Sprite::GetRotation() const {
    return m_rotation;
}

void Sprite::Draw(Renderer* renderer) {
}

Rect Sprite::GetBounds() const {
    return Rect(m_position.x - m_origin.x, m_position.y - m_origin.y, m_size.x, m_size.y);
}

BoxSprite::BoxSprite() : Sprite(), m_filled(true) {}

void BoxSprite::SetFilled(bool filled) {
    m_filled = filled;
}

bool BoxSprite::IsFilled() const {
    return m_filled;
}

void BoxSprite::Draw(Renderer* renderer) {
    Rect rect = GetBounds();
    if (m_filled) {
        renderer->FillRect(rect, m_color);
    } else {
        renderer->DrawRect(rect, m_color);
    }
}

CircleSprite::CircleSprite() : Sprite(), m_filled(true), m_radius(50.0f) {}

void CircleSprite::SetFilled(bool filled) {
    m_filled = filled;
}

bool CircleSprite::IsFilled() const {
    return m_filled;
}

void CircleSprite::SetRadius(float radius) {
    m_radius = radius;
}

float CircleSprite::GetRadius() const {
    return m_radius;
}

void CircleSprite::Draw(Renderer* renderer) {
    int cx = (int)(m_position.x);
    int cy = (int)(m_position.y);

    if (m_filled) {
        renderer->FillCircle(cx, cy, (int)m_radius, m_color);
    } else {
        renderer->DrawCircle(cx, cy, (int)m_radius, m_color);
    }
}
