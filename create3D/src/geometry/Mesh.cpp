#include "Mesh.h"

#include <glad/glad.h>
#include <cmath>

namespace Create3D {

Mesh::Mesh() = default;

Mesh::~Mesh()
{
    Shutdown();
}

void Mesh::Initialize()
{
    if (m_Initialized)
        return;

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);

    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, color));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBindVertexArray(0);

    m_Initialized = true;
}

void Mesh::Shutdown()
{
    if (m_VAO) glDeleteVertexArrays(1, &m_VAO);
    if (m_VBO) glDeleteBuffers(1, &m_VBO);
    if (m_EBO) glDeleteBuffers(1, &m_EBO);
    m_VAO = m_VBO = m_EBO = 0;
    m_Initialized = false;
}

void Mesh::SetVertices(const std::vector<Vertex>& vertices)
{
    m_Vertices = vertices;
    m_Dirty = true;
}

void Mesh::SetIndices(const std::vector<u32>& indices)
{
    m_Indices = indices;
    m_Dirty = true;
}

void Mesh::SetVerticesAndIndices(const std::vector<Vertex>& vertices, const std::vector<u32>& indices)
{
    m_Vertices = vertices;
    m_Indices = indices;
    m_Dirty = true;
}

void Mesh::AddVertex(const Vertex& vertex)
{
    m_Vertices.push_back(vertex);
    m_Dirty = true;
}

void Mesh::AddIndex(u32 index)
{
    m_Indices.push_back(index);
    m_Dirty = true;
}

void Mesh::Clear()
{
    m_Vertices.clear();
    m_Indices.clear();
    m_Dirty = true;
}

void Mesh::Upload()
{
    if (!m_Initialized)
        Initialize();

    if (!m_Dirty)
        return;

    glBindVertexArray(m_VAO);

    if (!m_Vertices.empty())
    {
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferData(GL_ARRAY_BUFFER, m_Vertices.size() * sizeof(Vertex), m_Vertices.data(), GL_DYNAMIC_DRAW);
    }

    if (!m_Indices.empty())
    {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_Indices.size() * sizeof(u32), m_Indices.data(), GL_DYNAMIC_DRAW);
    }

    glBindVertexArray(0);
    m_Dirty = false;
}

void Mesh::Bind() const
{
    if (m_Dirty)
        const_cast<Mesh*>(this)->Upload();
    glBindVertexArray(m_VAO);
}

void Mesh::Unbind() const
{
    glBindVertexArray(0);
}

BoundingBox Mesh::GetBoundingBox() const
{
    BoundingBox bbox;
    for (const auto& v : m_Vertices)
        bbox.Expand(v.position);
    return bbox;
}

void Mesh::ComputeNormals()
{
    for (auto& v : m_Vertices)
        v.normal = Vec3(0);

    for (size_t i = 0; i < m_Indices.size(); i += 3)
    {
        u32 i0 = m_Indices[i];
        u32 i1 = m_Indices[i + 1];
        u32 i2 = m_Indices[i + 2];

        Vec3 v0 = m_Vertices[i0].position;
        Vec3 v1 = m_Vertices[i1].position;
        Vec3 v2 = m_Vertices[i2].position;

        Vec3 e1 = v1 - v0;
        Vec3 e2 = v2 - v0;
        Vec3 n = glm::normalize(glm::cross(e1, e2));

        m_Vertices[i0].normal += n;
        m_Vertices[i1].normal += n;
        m_Vertices[i2].normal += n;
    }

    for (auto& v : m_Vertices)
        if (glm::length(v.normal) > 0.0001f)
            v.normal = glm::normalize(v.normal);

    m_Dirty = true;
}

void Mesh::InvertNormals()
{
    for (auto& v : m_Vertices)
        v.normal = -v.normal;

    for (size_t i = 0; i < m_Indices.size(); i += 3)
        std::swap(m_Indices[i + 1], m_Indices[i + 2]);

    m_Dirty = true;
}

void Mesh::Scale(const Vec3& scale)
{
    for (auto& v : m_Vertices)
        v.position *= scale;
    m_Dirty = true;
}

void Mesh::Translate(const Vec3& translation)
{
    for (auto& v : m_Vertices)
        v.position += translation;
    m_Dirty = true;
}

void Mesh::Transform(const Mat4& transform)
{
    Mat3 normalTransform = glm::mat3(glm::transpose(glm::inverse(transform)));
    for (auto& v : m_Vertices)
    {
        v.position = Vec3(transform * Vec4(v.position, 1.0f));
        v.normal = glm::normalize(normalTransform * v.normal);
    }
    m_Dirty = true;
}

SharedPtr<Mesh> Mesh::CreateCube(f32 size)
{
    auto mesh = MakeShared<Mesh>();
    f32 s = size * 0.5f;

    std::vector<Vertex> vertices = {
        { Vec3(-s, -s,  s), Vec3( 0,  0,  1), Vec2(0, 0), Color(0.8f, 0.8f, 0.8f) },
        { Vec3( s, -s,  s), Vec3( 0,  0,  1), Vec2(1, 0), Color(0.8f, 0.8f, 0.8f) },
        { Vec3( s,  s,  s), Vec3( 0,  0,  1), Vec2(1, 1), Color(0.8f, 0.8f, 0.8f) },
        { Vec3(-s,  s,  s), Vec3( 0,  0,  1), Vec2(0, 1), Color(0.8f, 0.8f, 0.8f) },

        { Vec3(-s, -s, -s), Vec3( 0,  0, -1), Vec2(0, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3(-s,  s, -s), Vec3( 0,  0, -1), Vec2(1, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3( s,  s, -s), Vec3( 0,  0, -1), Vec2(1, 1), Color(0.6f, 0.6f, 0.6f) },
        { Vec3( s, -s, -s), Vec3( 0,  0, -1), Vec2(0, 1), Color(0.6f, 0.6f, 0.6f) },

        { Vec3(-s,  s, -s), Vec3( 0,  1,  0), Vec2(0, 0), Color(0.7f, 0.7f, 0.7f) },
        { Vec3(-s,  s,  s), Vec3( 0,  1,  0), Vec2(1, 0), Color(0.7f, 0.7f, 0.7f) },
        { Vec3( s,  s,  s), Vec3( 0,  1,  0), Vec2(1, 1), Color(0.7f, 0.7f, 0.7f) },
        { Vec3( s,  s, -s), Vec3( 0,  1,  0), Vec2(0, 1), Color(0.7f, 0.7f, 0.7f) },

        { Vec3(-s, -s, -s), Vec3( 0, -1,  0), Vec2(0, 0), Color(0.65f, 0.65f, 0.65f) },
        { Vec3( s, -s, -s), Vec3( 0, -1,  0), Vec2(1, 0), Color(0.65f, 0.65f, 0.65f) },
        { Vec3( s, -s,  s), Vec3( 0, -1,  0), Vec2(1, 1), Color(0.65f, 0.65f, 0.65f) },
        { Vec3(-s, -s,  s), Vec3( 0, -1,  0), Vec2(0, 1), Color(0.65f, 0.65f, 0.65f) },

        { Vec3( s, -s, -s), Vec3( 1,  0,  0), Vec2(0, 0), Color(0.75f, 0.75f, 0.75f) },
        { Vec3( s,  s, -s), Vec3( 1,  0,  0), Vec2(1, 0), Color(0.75f, 0.75f, 0.75f) },
        { Vec3( s,  s,  s), Vec3( 1,  0,  0), Vec2(1, 1), Color(0.75f, 0.75f, 0.75f) },
        { Vec3( s, -s,  s), Vec3( 1,  0,  0), Vec2(0, 1), Color(0.75f, 0.75f, 0.75f) },

        { Vec3(-s, -s, -s), Vec3(-1,  0,  0), Vec2(0, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3(-s, -s,  s), Vec3(-1,  0,  0), Vec2(1, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3(-s,  s,  s), Vec3(-1,  0,  0), Vec2(1, 1), Color(0.6f, 0.6f, 0.6f) },
        { Vec3(-s,  s, -s), Vec3(-1,  0,  0), Vec2(0, 1), Color(0.6f, 0.6f, 0.6f) },
    };

    std::vector<u32> indices = {
        0, 1, 2, 0, 2, 3,
        4, 5, 6, 4, 6, 7,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
        16, 17, 18, 16, 18, 19,
        20, 21, 22, 20, 22, 23
    };

    mesh->SetVerticesAndIndices(vertices, indices);
    return mesh;
}

SharedPtr<Mesh> Mesh::CreateSphere(f32 radius, u32 segments)
{
    auto mesh = MakeShared<Mesh>();
    u32 stacks = segments / 2;

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (u32 i = 0; i <= stacks; i++)
    {
        f32 stackAngle = (f32)i / stacks * glm::pi<f32>();
        f32 sinStack = sin(stackAngle);
        f32 cosStack = cos(stackAngle);

        for (u32 j = 0; j <= segments; j++)
        {
            f32 sectorAngle = (f32)j / segments * 2.0f * glm::pi<f32>();
            f32 sinSector = sin(sectorAngle);
            f32 cosSector = cos(sectorAngle);

            Vertex v;
            v.position = Vec3(
                cosSector * sinStack * radius,
                cosStack * radius,
                sinSector * sinStack * radius
            );
            v.normal = glm::normalize(v.position);
            v.texCoord = Vec2((f32)j / segments, (f32)i / stacks);
            v.color = Color(0.8f, 0.6f, 0.6f);
            vertices.push_back(v);
        }
    }

    for (u32 i = 0; i < stacks; i++)
    {
        for (u32 j = 0; j < segments; j++)
        {
            u32 first = i * (segments + 1) + j;
            u32 second = first + segments + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    return mesh;
}

SharedPtr<Mesh> Mesh::CreateCylinder(f32 radius, f32 height, u32 segments)
{
    auto mesh = MakeShared<Mesh>();
    f32 halfHeight = height * 0.5f;

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (u32 i = 0; i <= segments; i++)
    {
        f32 angle = (f32)i / segments * 2.0f * glm::pi<f32>();
        f32 c = cos(angle);
        f32 s = sin(angle);

        Vertex vTop, vBottom, vTopOuter, vBottomOuter;
        
        vTop.position = Vec3(c * radius, halfHeight, s * radius);
        vTop.normal = Vec3(c, 0, s);
        vTop.color = Color(0.7f, 0.7f, 0.8f);

        vBottom.position = Vec3(c * radius, -halfHeight, s * radius);
        vBottom.normal = Vec3(c, 0, s);
        vBottom.color = Color(0.7f, 0.7f, 0.8f);

        vTopOuter = vTop;
        vTopOuter.normal = Vec3(0, 1, 0);

        vBottomOuter = vBottom;
        vBottomOuter.normal = Vec3(0, -1, 0);

        vertices.push_back(vTop);
        vertices.push_back(vBottom);
        vertices.push_back(vTopOuter);
        vertices.push_back(vBottomOuter);
    }

    for (u32 i = 0; i < segments; i++)
    {
        u32 idx = i * 4;
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(idx + 5);
        indices.push_back(idx);
        indices.push_back(idx + 5);
        indices.push_back(idx + 4);
    }

    u32 topCenterIdx = static_cast<u32>(vertices.size());
    vertices.push_back(Vertex(Vec3(0, halfHeight, 0), Vec3(0, 1, 0), Color(0.75f, 0.75f, 0.85f)));

    u32 bottomCenterIdx = static_cast<u32>(vertices.size());
    vertices.push_back(Vertex(Vec3(0, -halfHeight, 0), Vec3(0, -1, 0), Color(0.65f, 0.65f, 0.75f)));

    for (u32 i = 0; i < segments; i++)
    {
        u32 idx = i * 4 + 2;
        u32 nextIdx = ((i + 1) % segments) * 4 + 2;
        indices.push_back(topCenterIdx);
        indices.push_back(idx);
        indices.push_back(nextIdx);

        idx = i * 4 + 3;
        nextIdx = ((i + 1) % segments) * 4 + 3;
        indices.push_back(bottomCenterIdx);
        indices.push_back(nextIdx);
        indices.push_back(idx);
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    return mesh;
}

SharedPtr<Mesh> Mesh::CreateCone(f32 radius, f32 height, u32 segments)
{
    auto mesh = MakeShared<Mesh>();

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (u32 i = 0; i <= segments; i++)
    {
        f32 angle = (f32)i / segments * 2.0f * glm::pi<f32>();
        f32 c = cos(angle);
        f32 s = sin(angle);

        Vertex v;
        v.position = Vec3(c * radius, 0, s * radius);
        v.normal = Vec3(c, 0, s);
        v.color = Color(0.7f, 0.6f, 0.5f);
        vertices.push_back(v);
    }

    for (u32 i = 0; i < segments; i++)
    {
        u32 idx = i;
        indices.push_back(idx);
        indices.push_back(idx + 1);
        indices.push_back(static_cast<u32>(vertices.size()));
    }

    Vertex apex;
    apex.position = Vec3(0, height, 0);
    apex.normal = Vec3(0, 1, 0);
    apex.color = Color(0.8f, 0.7f, 0.6f);
    vertices.push_back(apex);

    u32 bottomCenterIdx = static_cast<u32>(vertices.size());
    Vertex bottomCenter;
    bottomCenter.position = Vec3(0, 0, 0);
    bottomCenter.normal = Vec3(0, -1, 0);
    bottomCenter.color = Color(0.65f, 0.55f, 0.45f);
    vertices.push_back(bottomCenter);

    for (u32 i = 0; i < segments; i++)
    {
        indices.push_back(bottomCenterIdx);
        indices.push_back(i + 1);
        indices.push_back(i);
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    mesh->ComputeNormals();
    return mesh;
}

SharedPtr<Mesh> Mesh::CreateTorus(f32 outerRadius, f32 innerRadius, u32 outerSegments, u32 innerSegments)
{
    auto mesh = MakeShared<Mesh>();

    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    for (u32 i = 0; i <= outerSegments; i++)
    {
        f32 u = (f32)i / outerSegments * 2.0f * glm::pi<f32>();
        f32 cu = cos(u);
        f32 su = sin(u);

        for (u32 j = 0; j <= innerSegments; j++)
        {
            f32 v = (f32)j / innerSegments * 2.0f * glm::pi<f32>();
            f32 cv = cos(v);
            f32 sv = sin(v);

            Vertex vertex;
            vertex.position = Vec3(
                (outerRadius + innerRadius * cv) * cu,
                innerRadius * sv,
                (outerRadius + innerRadius * cv) * su
            );

            Vec3 center(outerRadius * cu, 0, outerRadius * su);
            vertex.normal = glm::normalize(vertex.position - center);
            vertex.texCoord = Vec2((f32)i / outerSegments, (f32)j / innerSegments);
            vertex.color = Color(0.7f, 0.6f, 0.8f);
            vertices.push_back(vertex);
        }
    }

    for (u32 i = 0; i < outerSegments; i++)
    {
        for (u32 j = 0; j < innerSegments; j++)
        {
            u32 a = i * (innerSegments + 1) + j;
            u32 b = a + 1;
            u32 c = a + innerSegments + 1;
            u32 d = c + 1;

            indices.push_back(a);
            indices.push_back(c);
            indices.push_back(b);

            indices.push_back(b);
            indices.push_back(c);
            indices.push_back(d);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    return mesh;
}

SharedPtr<Mesh> Mesh::CreatePlane(f32 size)
{
    auto mesh = MakeShared<Mesh>();
    f32 s = size * 0.5f;

    std::vector<Vertex> vertices = {
        { Vec3(-s, 0, -s), Vec3(0, 1, 0), Vec2(0, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3( s, 0, -s), Vec3(0, 1, 0), Vec2(1, 0), Color(0.6f, 0.6f, 0.6f) },
        { Vec3( s, 0,  s), Vec3(0, 1, 0), Vec2(1, 1), Color(0.6f, 0.6f, 0.6f) },
        { Vec3(-s, 0,  s), Vec3(0, 1, 0), Vec2(0, 1), Color(0.6f, 0.6f, 0.6f) }
    };

    std::vector<u32> indices = { 0, 1, 2, 0, 2, 3 };

    mesh->SetVerticesAndIndices(vertices, indices);
    return mesh;
}

} 
