#pragma once

#include "Types.h"

namespace Create3D {

enum class CameraMode
{
    Orbit,
    Free,
    Pan
};

class Camera
{
public:
    Camera();

    void SetPerspective(f32 fov, f32 aspect, f32 nearClip, f32 farClip);
    void SetOrthographic(f32 size, f32 aspect, f32 nearClip, f32 farClip);

    void SetPosition(const Vec3& position);
    void SetTarget(const Vec3& target);
    void SetDistance(f32 distance);
    void SetYaw(f32 yaw);
    void SetPitch(f32 pitch);

    Vec3 GetPosition() const { return m_Position; }
    Vec3 GetTarget() const { return m_Target; }
    Vec3 GetForward() const { return m_Forward; }
    Vec3 GetRight() const { return m_Right; }
    Vec3 GetUp() const { return m_Up; }

    f32 GetDistance() const { return m_Distance; }
    f32 GetYaw() const { return m_Yaw; }
    f32 GetPitch() const { return m_Pitch; }

    Mat4 GetViewMatrix() const { return m_ViewMatrix; }
    Mat4 GetProjectionMatrix() const { return m_ProjectionMatrix; }
    Mat4 GetViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

    f32 GetFOV() const { return m_FOV; }
    f32 GetAspectRatio() const { return m_AspectRatio; }
    f32 GetNearClip() const { return m_NearClip; }
    f32 GetFarClip() const { return m_FarClip; }

    bool IsPerspective() const { return m_IsPerspective; }
    void SetPerspectiveMode(bool perspective) { m_IsPerspective = perspective; UpdateProjection(); }

    void Orbit(f32 deltaYaw, f32 deltaPitch);
    void Pan(f32 deltaX, f32 deltaY);
    void Zoom(f32 delta);
    void Move(f32 deltaForward, f32 deltaRight, f32 deltaUp);

    void Update();

    Ray ScreenToRay(const Vec2& screenPos, const Vec2& screenSize) const;
    Vec3 ProjectPointToPlane(const Vec3& point, const Plane& plane) const;

    Vec2 WorldToScreen(const Vec3& worldPos, const Vec2& screenSize) const;

private:
    void UpdateView();
    void UpdateProjection();
    void UpdateVectors();

    Vec3 m_Position = Vec3(0.0f, 0.0f, 10.0f);
    Vec3 m_Target = Vec3(0.0f, 0.0f, 0.0f);
    Vec3 m_Forward = Vec3(0.0f, 0.0f, -1.0f);
    Vec3 m_Right = Vec3(1.0f, 0.0f, 0.0f);
    Vec3 m_Up = Vec3(0.0f, 1.0f, 0.0f);

    f32 m_Distance = 10.0f;
    f32 m_Yaw = -90.0f;
    f32 m_Pitch = 0.0f;

    f32 m_FOV = 45.0f;
    f32 m_AspectRatio = 16.0f / 9.0f;
    f32 m_NearClip = 0.01f;
    f32 m_FarClip = 10000.0f;
    f32 m_OrthoSize = 10.0f;

    Mat4 m_ViewMatrix = Mat4(1.0f);
    Mat4 m_ProjectionMatrix = Mat4(1.0f);
    Mat4 m_ViewProjectionMatrix = Mat4(1.0f);

    bool m_IsPerspective = true;
};

} 
