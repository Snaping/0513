#pragma once

#include "Types.h"
#include "VertexBuffer.h"

namespace Create3D {

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void Initialize();
    void Shutdown();

    void SetVertices(const std::vector<Vertex>& vertices);
    void SetIndices(const std::vector<u32>& indices);
    void SetVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<u32>& indices);

    void AddVertex(const Vertex& vertex);
    void AddIndex(u32 index);

    void Clear();
    void Upload();

    void Bind() const;
    void Unbind() const;

    u32 GetVertexCount() const { return static_cast<u32>(m_Vertices.size()); }
    u32 GetIndexCount() const { return static_cast<u32>(m_Indices.size()); }
    const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
    const std::vector<u32>& GetIndices() const { return m_Indices; }
    BoundingBox GetBoundingBox() const;

    void ComputeNormals();
    void InvertNormals();
    void Scale(const Vec3& scale);
    void Translate(const Vec3& translation);
    void Transform(const Mat4& transform);

    static SharedPtr<Mesh> CreateCube(f32 size = 1.0f);
    static SharedPtr<Mesh> CreateSphere(f32 radius = 0.5f, u32 segments = 32);
    static SharedPtr<Mesh> CreateCylinder(f32 radius = 0.5f, f32 height = 1.0f, u32 segments = 32);
    static SharedPtr<Mesh> CreateCone(f32 radius = 0.5f, f32 height = 1.0f, u32 segments = 32);
    static SharedPtr<Mesh> CreateTorus(f32 outerRadius = 0.5f, f32 innerRadius = 0.2f, u32 outerSegments = 32, u32 innerSegments = 16);
    static SharedPtr<Mesh> CreatePlane(f32 size = 10.0f);

private:
    std::vector<Vertex> m_Vertices;
    std::vector<u32> m_Indices;
    u32 m_VAO = 0;
    u32 m_VBO = 0;
    u32 m_EBO = 0;
    bool m_Dirty = false;
    bool m_Initialized = false;
};

} 
