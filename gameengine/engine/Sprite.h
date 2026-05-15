#pragma once
#include "Vector2.h"
#include "Color.h"
#include "Rect.h"

class Renderer;

class Sprite {
public:
    Sprite();
    ~Sprite();

    void SetPosition(const Vector2& position);
    Vector2 GetPosition() const;

    void SetSize(const Vector2& size);
    Vector2 GetSize() const;

    void SetColor(const Color& color);
    Color GetColor() const;

    void SetOrigin(const Vector2& origin);
    Vector2 GetOrigin() const;

    void SetRotation(float rotation);
    float GetRotation() const;

    virtual void Draw(Renderer* renderer);

    Rect GetBounds() const;

protected:
    Vector2 m_position;
    Vector2 m_size;
    Vector2 m_origin;
    Color m_color;
    float m_rotation;
};

class BoxSprite : public Sprite {
public:
    BoxSprite();

    void SetFilled(bool filled);
    bool IsFilled() const;

    void Draw(Renderer* renderer) override;

private:
    bool m_filled;
};

class CircleSprite : public Sprite {
public:
    CircleSprite();

    void SetFilled(bool filled);
    bool IsFilled() const;

    void SetRadius(float radius);
    float GetRadius() const;

    void Draw(Renderer* renderer) override;

private:
    bool m_filled;
    float m_radius;
};
