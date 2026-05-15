#pragma once

#include "Types.h"

namespace Create3D {

struct Vertex
{
    Vec3 position;
    Vec3 normal;
    Vec2 texCoord;
    Color color;

    Vertex() : position(0), normal(0, 0, 1), texCoord(0), color(Color::White()) {}
    Vertex(const Vec3& pos) : position(pos), normal(0, 0, 1), texCoord(0), color(Color::White()) {}
    Vertex(const Vec3& pos, const Color& col) : position(pos), normal(0, 0, 1), texCoord(0), color(col) {}
    Vertex(const Vec3& pos, const Vec3& norm) : position(pos), normal(norm), texCoord(0), color(Color::White()) {}
    Vertex(const Vec3& pos, const Vec3& norm, const Color& col) : position(pos), normal(norm), texCoord(0), color(col) {}
};

struct LineVertex
{
    Vec3 position;
    Color color;

    LineVertex() : position(0), color(Color::White()) {}
    LineVertex(const Vec3& pos) : position(pos), color(Color::White()) {}
    LineVertex(const Vec3& pos, const Color& col) : position(pos), color(col) {}
};

class VertexBuffer
{
public:
    VertexBuffer();
    ~VertexBuffer();

    void Initialize();
    void Shutdown();

    void Bind() const;
    void Unbind() const;

    void SetVertices(const Vertex* vertices, u32 count);
    void SetIndices(const u32* indices, u32 count);

    void SetLineVertices(const LineVertex* vertices, u32 count);

    u32 GetVertexCount() const { return m_VertexCount; }
    u32 GetIndexCount() const { return m_IndexCount; }
    u32 GetLineVertexCount() const { return m_LineVertexCount; }

    u32 GetVAO() const { return m_VAO; }
    u32 GetLineVAO() const { return m_LineVAO; }

private:
    u32 m_VAO = 0;
    u32 m_VBO = 0;
    u32 m_EBO = 0;

    u32 m_LineVAO = 0;
    u32 m_LineVBO = 0;

    u32 m_VertexCount = 0;
    u32 m_IndexCount = 0;
    u32 m_LineVertexCount = 0;
};

} 
