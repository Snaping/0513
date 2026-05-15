#include "Renderer.h"
#include "Scene.h"
#include "Mesh.h"

#include <glad/glad.h>

namespace Create3D {

const char* STANDARD_VS = R"(
    #version 450 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;
    layout (location = 3) in vec4 aColor;
    
    out vec3 FragPos;
    out vec3 Normal;
    out vec2 TexCoord;
    out vec4 VertexColor;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        FragPos = vec3(model * vec4(aPos, 1.0));
        Normal = mat3(transpose(inverse(model))) * aNormal;
        TexCoord = aTexCoord;
        VertexColor = aColor;
        gl_Position = projection * view * vec4(FragPos, 1.0);
    }
)";

const char* STANDARD_FS = R"(
    #version 450 core
    
    in vec3 FragPos;
    in vec3 Normal;
    in vec2 TexCoord;
    in vec4 VertexColor;
    
    out vec4 FragColor;
    
    uniform vec3 lightPos;
    uniform vec3 viewPos;
    uniform vec3 lightColor;
    uniform bool enableLighting;
    
    void main()
    {
        vec3 result;
        
        if (enableLighting)
        {
            vec3 ambient = 0.3f * VertexColor.rgb;
            
            vec3 norm = normalize(Normal);
            vec3 lightDir = normalize(lightPos - FragPos);
            float diff = max(dot(norm, lightDir), 0.0);
            vec3 diffuse = diff * lightColor * VertexColor.rgb;
            
            vec3 viewDir = normalize(viewPos - FragPos);
            vec3 reflectDir = reflect(-lightDir, norm);
            float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
            vec3 specular = 0.5f * spec * lightColor;
            
            result = ambient + diffuse + specular;
        }
        else
        {
            result = VertexColor.rgb;
        }
        
        FragColor = vec4(result, VertexColor.a);
    }
)";

const char* LINE_VS = R"(
    #version 450 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    
    out vec4 VertexColor;
    
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        VertexColor = aColor;
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
)";

const char* LINE_FS = R"(
    #version 450 core
    
    in vec4 VertexColor;
    out vec4 FragColor;
    
    void main()
    {
        FragColor = VertexColor;
    }
)";

const char* POINT_VS = R"(
    #version 450 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec4 aColor;
    
    out vec4 VertexColor;
    
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        VertexColor = aColor;
        gl_Position = projection * view * vec4(aPos, 1.0);
    }
)";

const char* POINT_FS = R"(
    #version 450 core
    
    in vec4 VertexColor;
    out vec4 FragColor;
    
    void main()
    {
        FragColor = VertexColor;
    }
)";

const char* UNLIT_VS = R"(
    #version 450 core
    
    layout (location = 0) in vec3 aPos;
    layout (location = 1) in vec3 aNormal;
    layout (location = 2) in vec2 aTexCoord;
    layout (location = 3) in vec4 aColor;
    
    out vec4 VertexColor;
    
    uniform mat4 model;
    uniform mat4 view;
    uniform mat4 projection;
    
    void main()
    {
        VertexColor = aColor;
        gl_Position = projection * view * model * vec4(aPos, 1.0);
    }
)";

const char* UNLIT_FS = R"(
    #version 450 core
    
    in vec4 VertexColor;
    out vec4 FragColor;
    
    void main()
    {
        FragColor = VertexColor;
    }
)";

Renderer::Renderer() = default;

Renderer::~Renderer()
{
    Shutdown();
}

bool Renderer::Initialize()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);

    InitializeShaders();
    CreateGridData();
    CreateAxisData();

    m_DynamicLineBuffer.Initialize();

    m_MainCamera.SetPosition(Vec3(10.0f, 10.0f, 10.0f));
    m_MainCamera.SetTarget(Vec3(0.0f, 0.0f, 0.0f));
    m_MainCamera.SetYaw(-45.0f);
    m_MainCamera.SetPitch(35.0f);

    m_Initialized = true;
    return true;
}

void Renderer::Shutdown()
{
    m_GridBuffer.Shutdown();
    m_AxisBuffer.Shutdown();
    m_DynamicLineBuffer.Shutdown();

    m_StandardShader.reset();
    m_LineShader.reset();
    m_PointShader.reset();
    m_UnlitShader.reset();

    m_Initialized = false;
}

void Renderer::InitializeShaders()
{
    m_StandardShader = MakeUnique<Shader>();
    m_StandardShader->LoadFromSource(STANDARD_VS, STANDARD_FS);

    m_LineShader = MakeUnique<Shader>();
    m_LineShader->LoadFromSource(LINE_VS, LINE_FS);

    m_PointShader = MakeUnique<Shader>();
    m_PointShader->LoadFromSource(POINT_VS, POINT_FS);

    m_UnlitShader = MakeUnique<Shader>();
    m_UnlitShader->LoadFromSource(UNLIT_VS, UNLIT_FS);
}

void Renderer::CreateGridData()
{
    std::vector<LineVertex> gridVertices;
    const f32 gridSize = 50.0f;
    const f32 gridStep = 1.0f;
    const Color gridColor(0.3f, 0.3f, 0.3f, 0.5f);
    const Color originColor(0.4f, 0.4f, 0.4f, 0.8f);

    for (f32 i = -gridSize; i <= gridSize; i += gridStep)
    {
        bool isOrigin = std::abs(i) < 0.01f;
        Color color = isOrigin ? originColor : gridColor;
        
        gridVertices.emplace_back(Vec3(i, 0.0f, -gridSize), color);
        gridVertices.emplace_back(Vec3(i, 0.0f, gridSize), color);
        
        gridVertices.emplace_back(Vec3(-gridSize, 0.0f, i), color);
        gridVertices.emplace_back(Vec3(gridSize, 0.0f, i), color);
    }

    m_GridBuffer.Initialize();
    m_GridBuffer.SetLineVertices(gridVertices.data(), static_cast<u32>(gridVertices.size()));
}

void Renderer::CreateAxisData()
{
    std::vector<LineVertex> axisVertices;
    const f32 axisLength = 5.0f;

    axisVertices.emplace_back(Vec3(0.0f, 0.0f, 0.0f), Color::Red());
    axisVertices.emplace_back(Vec3(axisLength, 0.0f, 0.0f), Color::Red());

    axisVertices.emplace_back(Vec3(0.0f, 0.0f, 0.0f), Color::Green());
    axisVertices.emplace_back(Vec3(0.0f, axisLength, 0.0f), Color::Green());

    axisVertices.emplace_back(Vec3(0.0f, 0.0f, 0.0f), Color::Blue());
    axisVertices.emplace_back(Vec3(0.0f, 0.0f, axisLength), Color::Blue());

    m_AxisBuffer.Initialize();
    m_AxisBuffer.SetLineVertices(axisVertices.data(), static_cast<u32>(axisVertices.size()));
}

void Renderer::BeginFrame()
{
    glClearColor(
        m_Settings.backgroundColor.r,
        m_Settings.backgroundColor.g,
        m_Settings.backgroundColor.b,
        m_Settings.backgroundColor.a
    );
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_PendingLines.clear();
    m_PendingPoints.clear();
}

void Renderer::EndFrame()
{
}

void Renderer::RenderScene(Scene* scene)
{
    (void)scene;

    Window* window = Application::GetInstance()->GetWindow();
    m_MainCamera.SetPerspective(45.0f, window->GetAspectRatio(), 0.1f, 1000.0f);

    if (m_Settings.showGrid)
        RenderGrid();

    if (m_Settings.showAxis)
        RenderAxis();

    if (!m_PendingLines.empty())
    {
        m_DynamicLineBuffer.SetLineVertices(m_PendingLines.data(), static_cast<u32>(m_PendingLines.size()));
        
        m_LineShader->Use();
        m_LineShader->SetMat4("view", m_MainCamera.GetViewMatrix());
        m_LineShader->SetMat4("projection", m_MainCamera.GetProjectionMatrix());
        
        glBindVertexArray(m_DynamicLineBuffer.GetLineVAO());
        glLineWidth(2.0f);
        glDrawArrays(GL_LINES, 0, static_cast<i32>(m_PendingLines.size()));
        glBindVertexArray(0);
        glLineWidth(1.0f);
    }
}

void Renderer::RenderGrid()
{
    m_LineShader->Use();
    m_LineShader->SetMat4("view", m_MainCamera.GetViewMatrix());
    m_LineShader->SetMat4("projection", m_MainCamera.GetProjectionMatrix());

    glBindVertexArray(m_GridBuffer.GetLineVAO());
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES, 0, static_cast<i32>(m_GridBuffer.GetLineVertexCount()));
    glBindVertexArray(0);
}

void Renderer::RenderAxis()
{
    m_LineShader->Use();
    m_LineShader->SetMat4("view", m_MainCamera.GetViewMatrix());
    m_LineShader->SetMat4("projection", m_MainCamera.GetProjectionMatrix());

    glBindVertexArray(m_AxisBuffer.GetLineVAO());
    glLineWidth(3.0f);
    glDrawArrays(GL_LINES, 0, static_cast<i32>(m_AxisBuffer.GetLineVertexCount()));
    glBindVertexArray(0);
    glLineWidth(1.0f);
}

void Renderer::SetClearColor(const Color& color)
{
    m_Settings.backgroundColor = color;
}

void Renderer::SetViewport(i32 x, i32 y, i32 width, i32 height)
{
    glViewport(x, y, width, height);
}

void Renderer::DrawMesh(const Mesh* mesh, const Mat4& transform)
{
    if (!mesh || mesh->GetVertexCount() == 0)
        return;

    if (m_Settings.enableLighting)
    {
        m_StandardShader->Use();
        m_StandardShader->SetMat4("model", transform);
        m_StandardShader->SetMat4("view", m_MainCamera.GetViewMatrix());
        m_StandardShader->SetMat4("projection", m_MainCamera.GetProjectionMatrix());
        m_StandardShader->SetVec3("lightPos", Vec3(20.0f, 30.0f, 20.0f));
        m_StandardShader->SetVec3("viewPos", m_MainCamera.GetPosition());
        m_StandardShader->SetVec3("lightColor", Vec3(1.0f, 1.0f, 1.0f));
        m_StandardShader->SetInt("enableLighting", 1);
    }
    else
    {
        m_UnlitShader->Use();
        m_UnlitShader->SetMat4("model", transform);
        m_UnlitShader->SetMat4("view", m_MainCamera.GetViewMatrix());
        m_UnlitShader->SetMat4("projection", m_MainCamera.GetProjectionMatrix());
    }

    mesh->Bind();

    if (m_Settings.showWireframe)
    {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }

    if (mesh->GetIndexCount() > 0)
    {
        glDrawElements(GL_TRIANGLES, static_cast<i32>(mesh->GetIndexCount()), GL_UNSIGNED_INT, nullptr);
    }
    else
    {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<i32>(mesh->GetVertexCount()));
    }

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

    mesh->Unbind();
}

void Renderer::DrawLines(const LineVertex* vertices, u32 count, f32 lineWidth)
{
    for (u32 i = 0; i < count; i++)
    {
        m_PendingLines.push_back(vertices[i]);
    }
    (void)lineWidth;
}

void Renderer::DrawLine(const Vec3& from, const Vec3& to, const Color& color, f32 width)
{
    (void)width;
    m_PendingLines.emplace_back(from, color);
    m_PendingLines.emplace_back(to, color);
}

void Renderer::DrawAABB(const BoundingBox& box, const Color& color)
{
    Vec3 v0 = box.min;
    Vec3 v1 = Vec3(box.max.x, box.min.y, box.min.z);
    Vec3 v2 = Vec3(box.max.x, box.min.y, box.max.z);
    Vec3 v3 = Vec3(box.min.x, box.min.y, box.max.z);
    Vec3 v4 = Vec3(box.min.x, box.max.y, box.min.z);
    Vec3 v5 = Vec3(box.max.x, box.max.y, box.min.z);
    Vec3 v6 = box.max;
    Vec3 v7 = Vec3(box.min.x, box.max.y, box.max.z);

    DrawLine(v0, v1, color);
    DrawLine(v1, v2, color);
    DrawLine(v2, v3, color);
    DrawLine(v3, v0, color);

    DrawLine(v4, v5, color);
    DrawLine(v5, v6, color);
    DrawLine(v6, v7, color);
    DrawLine(v7, v4, color);

    DrawLine(v0, v4, color);
    DrawLine(v1, v5, color);
    DrawLine(v2, v6, color);
    DrawLine(v3, v7, color);
}

void Renderer::DrawPlane(const Plane& plane, f32 size, const Color& color)
{
    Vec3 center = plane.GetPoint();
    
    Vec3 tangent, bitangent;
    if (std::abs(plane.normal.y) > 0.9f)
    {
        tangent = glm::normalize(glm::cross(plane.normal, Vec3(1, 0, 0)));
    }
    else
    {
        tangent = glm::normalize(glm::cross(plane.normal, Vec3(0, 1, 0)));
    }
    bitangent = glm::normalize(glm::cross(plane.normal, tangent));

    Vec3 v0 = center - tangent * size - bitangent * size;
    Vec3 v1 = center + tangent * size - bitangent * size;
    Vec3 v2 = center + tangent * size + bitangent * size;
    Vec3 v3 = center - tangent * size + bitangent * size;

    DrawLine(v0, v1, color);
    DrawLine(v1, v2, color);
    DrawLine(v2, v3, color);
    DrawLine(v3, v0, color);
}

void Renderer::DrawPoint(const Vec3& position, const Color& color, f32 size)
{
    (void)size;
    m_PendingPoints.emplace_back(position, color);
}

} 
