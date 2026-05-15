#pragma once

#include <memory>
#include <vector>
#include <array>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <optional>
#include <variant>
#include <cstdint>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Create3D {

using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using f32 = float;
using f64 = double;

template<typename T>
using UniquePtr = std::unique_ptr<T>;

template<typename T>
using SharedPtr = std::shared_ptr<T>;

template<typename T>
using WeakPtr = std::weak_ptr<T>;

template<typename T, typename... Args>
constexpr UniquePtr<T> MakeUnique(Args&&... args)
{
    return std::make_unique<T>(std::forward<Args>(args)...);
}

template<typename T, typename... Args>
constexpr SharedPtr<T> MakeShared(Args&&... args)
{
    return std::make_shared<T>(std::forward<Args>(args)...);
}

using Vec2 = glm::vec2;
using Vec3 = glm::vec3;
using Vec4 = glm::vec4;
using Mat4 = glm::mat4;
using Quat = glm::quat;

struct Color
{
    f32 r, g, b, a;

    Color() : r(1.0f), g(1.0f), b(1.0f), a(1.0f) {}
    Color(f32 r, f32 g, f32 b, f32 a = 1.0f) : r(r), g(g), b(b), a(a) {}

    static Color Red() { return Color(1.0f, 0.0f, 0.0f, 1.0f); }
    static Color Green() { return Color(0.0f, 1.0f, 0.0f, 1.0f); }
    static Color Blue() { return Color(0.0f, 0.0f, 1.0f, 1.0f); }
    static Color Yellow() { return Color(1.0f, 1.0f, 0.0f, 1.0f); }
    static Color Cyan() { return Color(0.0f, 1.0f, 1.0f, 1.0f); }
    static Color Magenta() { return Color(1.0f, 0.0f, 1.0f, 1.0f); }
    static Color White() { return Color(1.0f, 1.0f, 1.0f, 1.0f); }
    static Color Black() { return Color(0.0f, 0.0f, 0.0f, 1.0f); }
    static Color Gray() { return Color(0.5f, 0.5f, 0.5f, 1.0f); }
    static Color LightGray() { return Color(0.7f, 0.7f, 0.7f, 1.0f); }
    static Color DarkGray() { return Color(0.3f, 0.3f, 0.3f, 1.0f); }
    static Color Orange() { return Color(1.0f, 0.5f, 0.0f, 1.0f); }
};

struct BoundingBox
{
    Vec3 min;
    Vec3 max;

    BoundingBox()
        : min(std::numeric_limits<f32>::max())
        , max(-std::numeric_limits<f32>::max()) {}

    BoundingBox(const Vec3& min, const Vec3& max)
        : min(min), max(max) {}

    Vec3 Center() const { return (min + max) * 0.5f; }
    Vec3 Size() const { return max - min; }

    void Expand(const Vec3& point)
    {
        min = glm::min(min, point);
        max = glm::max(max, point);
    }

    void Expand(const BoundingBox& other)
    {
        min = glm::min(min, other.min);
        max = glm::max(max, other.max);
    }

    bool Contains(const Vec3& point) const
    {
        return point.x >= min.x && point.x <= max.x
            && point.y >= min.y && point.y <= max.y
            && point.z >= min.z && point.z <= max.z;
    }
};

enum class PlaneType
{
    XY,
    YZ,
    XZ,
    Custom
};

struct Plane
{
    Vec3 normal;
    f32 distance;

    Plane() : normal(0, 0, 1), distance(0) {}
    Plane(const Vec3& n, f32 d) : normal(glm::normalize(n)), distance(d) {}

    static Plane FromPointNormal(const Vec3& point, const Vec3& normal)
    {
        return Plane(normal, -glm::dot(normal, point));
    }

    static Plane FromThreePoints(const Vec3& p0, const Vec3& p1, const Vec3& p2)
    {
        Vec3 v1 = p1 - p0;
        Vec3 v2 = p2 - p0;
        Vec3 n = glm::normalize(glm::cross(v1, v2));
        return Plane(n, -glm::dot(n, p0));
    }

    f32 DistanceToPoint(const Vec3& p) const
    {
        return glm::dot(normal, p) + distance;
    }

    Vec3 GetPoint() const
    {
        return normal * (-distance);
    }
};

struct Ray
{
    Vec3 origin;
    Vec3 direction;

    Ray() : origin(0), direction(0, 0, 1) {}
    Ray(const Vec3& o, const Vec3& d) : origin(o), direction(glm::normalize(d)) {}

    Vec3 At(f32 t) const
    {
        return origin + direction * t;
    }

    std::optional<f32> IntersectPlane(const Plane& plane) const
    {
        f32 denom = glm::dot(direction, plane.normal);
        if (std::abs(denom) < 1e-6f)
            return std::nullopt;

        f32 t = -(glm::dot(origin, plane.normal) + plane.distance) / denom;
        if (t < 0)
            return std::nullopt;

        return t;
    }
};

template<typename T>
struct Hash
{
    size_t operator()(const T& val) const
    {
        return std::hash<T>()(val);
    }
};

template<>
struct Hash<Vec2>
{
    size_t operator()(const Vec2& v) const
    {
        size_t h1 = std::hash<f32>()(v.x);
        size_t h2 = std::hash<f32>()(v.y);
        return h1 ^ (h2 << 1);
    }
};

template<>
struct Hash<Vec3>
{
    size_t operator()(const Vec3& v) const
    {
        size_t h1 = std::hash<f32>()(v.x);
        size_t h2 = std::hash<f32>()(v.y);
        size_t h3 = std::hash<f32>()(v.z);
        return h1 ^ (h2 << 1) ^ (h3 << 2);
    }
};

} 
