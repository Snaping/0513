#pragma once

#include "Types.h"
#include "Shader.h"
#include "Camera.h"
#include "VertexBuffer.h"

namespace Create3D {

class Scene;
class Mesh;

struct RenderSettings
{
    bool showGrid = true;
    bool showAxis = true;
    bool showWireframe = false;
    bool showNormals = false;
    bool enableLighting = true;
    Color backgroundColor = Color(0.15f, 0.15f, 0.15f, 1.0f);
};

class Renderer
{
public:
    Renderer();
    ~Renderer();

    bool Initialize();
    void Shutdown();

    void BeginFrame();
    void EndFrame();
    void RenderScene(Scene* scene);

    void SetClearColor(const Color& color);
    void SetViewport(i32 x, i32 y, i32 width, i32 height);

    Camera* GetMainCamera() { return &m_MainCamera; }
    const Camera* GetMainCamera() const { return &m_MainCamera; }

    RenderSettings& GetSettings() { return m_Settings; }
    const RenderSettings& GetSettings() const { return m_Settings; }

    void DrawMesh(const Mesh* mesh, const Mat4& transform);
    void DrawLines(const LineVertex* vertices, u32 count, f32 lineWidth = 1.0f);
    void DrawPoint(const Vec3& position, const Color& color, f32 size = 5.0f);
    void DrawLine(const Vec3& from, const Vec3& to, const Color& color, f32 width = 2.0f);
    void DrawAABB(const BoundingBox& box, const Color& color);
    void DrawPlane(const Plane& plane, f32 size, const Color& color);

private:
    void InitializeShaders();
    void CreateGridData();
    void CreateAxisData();

    void RenderGrid();
    void RenderAxis();

    UniquePtr<Shader> m_StandardShader;
    UniquePtr<Shader> m_LineShader;
    UniquePtr<Shader> m_PointShader;
    UniquePtr<Shader> m_UnlitShader;

    Camera m_MainCamera;
    RenderSettings m_Settings;

    VertexBuffer m_GridBuffer;
    VertexBuffer m_AxisBuffer;
    VertexBuffer m_DynamicLineBuffer;

    std::vector<LineVertex> m_PendingLines;
    std::vector<LineVertex> m_PendingPoints;

    bool m_Initialized = false;
};

} 
