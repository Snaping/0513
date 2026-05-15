#include "Camera.h"

#include <glad/glad.h>
#include <cmath>
#include <limits>

namespace Create3D {

Camera::Camera()
{
    UpdateVectors();
    UpdateView();
    UpdateProjection();
}

void Camera::SetPerspective(f32 fov, f32 aspect, f32 nearClip, f32 farClip)
{
    m_FOV = fov;
    m_AspectRatio = aspect;
    m_NearClip = nearClip;
    m_FarClip = farClip;
    m_IsPerspective = true;
    UpdateProjection();
}

void Camera::SetOrthographic(f32 size, f32 aspect, f32 nearClip, f32 farClip)
{
    m_OrthoSize = size;
    m_AspectRatio = aspect;
    m_NearClip = nearClip;
    m_FarClip = farClip;
    m_IsPerspective = false;
    UpdateProjection();
}

void Camera::SetPosition(const Vec3& position)
{
    m_Position = position;
    UpdateView();
}

void Camera::SetTarget(const Vec3& target)
{
    m_Target = target;
    m_Distance = glm::distance(m_Position, m_Target);
    UpdateVectors();
    UpdateView();
}

void Camera::SetDistance(f32 distance)
{
    m_Distance = glm::max(0.1f, distance);
    UpdateVectors();
    UpdateView();
}

void Camera::SetYaw(f32 yaw)
{
    m_Yaw = yaw;
    UpdateVectors();
    UpdateView();
}

void Camera::SetPitch(f32 pitch)
{
    m_Pitch = glm::clamp(pitch, -89.0f, 89.0f);
    UpdateVectors();
    UpdateView();
}

void Camera::UpdateVectors()
{
    f32 yawRad = glm::radians(m_Yaw);
    f32 pitchRad = glm::radians(m_Pitch);

    m_Forward.x = cos(yawRad) * cos(pitchRad);
    m_Forward.y = sin(pitchRad);
    m_Forward.z = sin(yawRad) * cos(pitchRad);
    m_Forward = glm::normalize(m_Forward);

    m_Right = glm::normalize(glm::cross(m_Forward, Vec3(0.0f, 1.0f, 0.0f)));
    m_Up = glm::normalize(glm::cross(m_Right, m_Forward));

    m_Position = m_Target - m_Forward * m_Distance;
}

void Camera::UpdateView()
{
    m_ViewMatrix = glm::lookAt(m_Position, m_Target, Vec3(0.0f, 1.0f, 0.0f));
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::UpdateProjection()
{
    if (m_IsPerspective)
    {
        m_ProjectionMatrix = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
    }
    else
    {
        f32 h = m_OrthoSize * 0.5f;
        f32 w = h * m_AspectRatio;
        m_ProjectionMatrix = glm::ortho(-w, w, -h, h, m_NearClip, m_FarClip);
    }
    m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
}

void Camera::Update()
{
}

void Camera::Orbit(f32 deltaYaw, f32 deltaPitch)
{
    m_Yaw += deltaYaw;
    m_Pitch += deltaPitch;
    m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
    UpdateVectors();
    UpdateView();
}

void Camera::Pan(f32 deltaX, f32 deltaY)
{
    Vec3 offset = -m_Right * deltaX + m_Up * deltaY;
    m_Target += offset;
    UpdateVectors();
    UpdateView();
}

void Camera::Zoom(f32 delta)
{
    m_Distance = glm::max(0.1f, m_Distance + delta);
    UpdateVectors();
    UpdateView();
}

void Camera::Move(f32 deltaForward, f32 deltaRight, f32 deltaUp)
{
    Vec3 forwardFlat = glm::normalize(Vec3(m_Forward.x, 0.0f, m_Forward.z));
    m_Target += forwardFlat * deltaForward + m_Right * deltaRight + Vec3(0, 1, 0) * deltaUp;
    UpdateVectors();
    UpdateView();
}

Ray Camera::ScreenToRay(const Vec2& screenPos, const Vec2& screenSize) const
{
    Vec2 ndc = Vec2(
        (2.0f * screenPos.x) / screenSize.x - 1.0f,
        1.0f - (2.0f * screenPos.y) / screenSize.y
    );

    Mat4 invProj = glm::inverse(m_ProjectionMatrix);
    Mat4 invView = glm::inverse(m_ViewMatrix);

    Vec4 nearP = invProj * Vec4(ndc.x, ndc.y, -1.0f, 1.0f);
    nearP /= nearP.w;
    Vec3 nearWorld = Vec3(invView * nearP);

    Vec4 farP = invProj * Vec4(ndc.x, ndc.y, 1.0f, 1.0f);
    farP /= farP.w;
    Vec3 farWorld = Vec3(invView * farP);

    return Ray(m_Position, glm::normalize(farWorld - nearWorld));
}

Vec3 Camera::ProjectPointToPlane(const Vec3& point, const Plane& plane) const
{
    Ray ray(m_Position, point - m_Position);
    auto t = ray.IntersectPlane(plane);
    if (t.has_value())
    {
        return ray.At(*t);
    }
    return point;
}

Vec2 Camera::WorldToScreen(const Vec3& worldPos, const Vec2& screenSize) const
{
    Vec4 clipPos = m_ViewProjectionMatrix * Vec4(worldPos, 1.0f);
    if (clipPos.w <= 0.0f)
        return Vec2(-10000.0f, -10000.0f);

    Vec3 ndc = Vec3(clipPos) / clipPos.w;
    return Vec2(
        (ndc.x + 1.0f) * 0.5f * screenSize.x,
        (1.0f - ndc.y) * 0.5f * screenSize.y
    );
}

} 
