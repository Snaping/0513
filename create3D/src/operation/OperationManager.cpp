#include "OperationManager.h"
#include "Application.h"
#include "Scene.h"

#include <algorithm>
#include <cmath>

namespace Create3D {

ExtrudeOperation::ExtrudeOperation() = default;

bool ExtrudeOperation::IsValid() const
{
    return m_Params.sketch != nullptr && !m_Params.sketch->GetEntities().empty();
}

void ExtrudeOperation::UpdatePreview()
{
}

SharedPtr<Mesh> ExtrudeOperation::Execute()
{
    if (!IsValid())
        return nullptr;

    return ExtrudeSketch(
        m_Params.sketch,
        m_Params.depth,
        m_Params.direction,
        m_Params.taperAngle,
        m_Params.segments
    );
}

SharedPtr<Mesh> ExtrudeOperation::ExtrudeSketch(
    SharedPtr<Sketch> sketch,
    f32 depth,
    const Vec3& direction,
    f32 taperAngle,
    int segments
)
{
    if (!sketch)
        return nullptr;

    std::vector<Vec3> outline = sketch->GetOutline();
    if (outline.empty())
        return nullptr;

    auto mesh = MakeShared<Mesh>();
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    Vec3 dir = glm::normalize(direction);
    f32 tanTaper = tan(glm::radians(taperAngle));
    int nSegments = std::max(1, segments);

    for (int i = 0; i <= nSegments; i++)
    {
        f32 t = (f32)i / nSegments;
        f32 scale = 1.0f - tanTaper * t * depth;
        Vec3 offset = dir * t * depth;

        for (const auto& p : outline)
        {
            Vec3 scaled = p * scale + offset;
            Vertex v;
            v.position = scaled;
            v.color = Color(0.7f, 0.75f, 0.85f);
            vertices.push_back(v);
        }
    }

    u32 outlineSize = static_cast<u32>(outline.size());
    for (int i = 0; i < nSegments; i++)
    {
        for (u32 j = 0; j < outlineSize - 1; j++)
        {
            u32 curr = static_cast<u32>(i) * outlineSize + j;
            u32 next = curr + outlineSize;

            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);

            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    mesh->ComputeNormals();
    return mesh;
}

RevolveOperation::RevolveOperation() = default;

bool RevolveOperation::IsValid() const
{
    return m_Params.sketch != nullptr && !m_Params.sketch->GetEntities().empty();
}

void RevolveOperation::UpdatePreview()
{
}

SharedPtr<Mesh> RevolveOperation::Execute()
{
    if (!IsValid())
        return nullptr;

    return RevolveSketch(
        m_Params.sketch,
        m_Params.axisOrigin,
        m_Params.axisDirection,
        m_Params.angle,
        m_Params.segments
    );
}

SharedPtr<Mesh> RevolveOperation::RevolveSketch(
    SharedPtr<Sketch> sketch,
    const Vec3& axisOrigin,
    const Vec3& axisDirection,
    f32 angle,
    int segments
)
{
    if (!sketch)
        return nullptr;

    std::vector<Vec3> outline = sketch->GetOutline();
    if (outline.empty())
        return nullptr;

    auto mesh = MakeShared<Mesh>();
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    Vec3 axis = glm::normalize(axisDirection);
    f32 angleRad = glm::radians(angle);
    int nSegments = std::max(4, segments);

    for (int i = 0; i <= nSegments; i++)
    {
        f32 t = (f32)i / nSegments;
        f32 currentAngle = t * angleRad;

        Quat rot = glm::angleAxis(currentAngle, axis);
        Mat4 rotation = glm::toMat4(rot);
        Mat4 transform = glm::translate(Mat4(1.0f), axisOrigin)
            * rotation
            * glm::translate(Mat4(1.0f), -axisOrigin);

        for (const auto& p : outline)
        {
            Vec3 rotated = Vec3(transform * Vec4(p, 1.0f));
            Vertex v;
            v.position = rotated;
            v.color = Color(0.75f, 0.7f, 0.85f);
            vertices.push_back(v);
        }
    }

    u32 outlineSize = static_cast<u32>(outline.size());
    for (int i = 0; i < nSegments; i++)
    {
        for (u32 j = 0; j < outlineSize - 1; j++)
        {
            u32 curr = static_cast<u32>(i) * outlineSize + j;
            u32 next = curr + outlineSize;

            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);

            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    mesh->ComputeNormals();
    return mesh;
}

SweepOperation::SweepOperation() = default;

bool SweepOperation::IsValid() const
{
    return m_Params.profile != nullptr && m_Params.path != nullptr
        && !m_Params.profile->GetEntities().empty()
        && !m_Params.path->GetEntities().empty();
}

void SweepOperation::UpdatePreview()
{
}

SharedPtr<Mesh> SweepOperation::Execute()
{
    if (!IsValid())
        return nullptr;

    return SweepSketch(
        m_Params.profile,
        m_Params.path,
        m_Params.segments,
        m_Params.maintainOrientation
    );
}

SharedPtr<Mesh> SweepOperation::SweepSketch(
    SharedPtr<Sketch> profile,
    SharedPtr<Sketch> path,
    int segments,
    bool maintainOrientation
)
{
    if (!profile || !path)
        return nullptr;

    std::vector<Vec3> profileOutline = profile->GetOutline();
    std::vector<Vec3> pathOutline = path->GetOutline();

    if (profileOutline.empty() || pathOutline.empty())
        return nullptr;

    auto mesh = MakeShared<Mesh>();
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    int nSegments = std::max(2, segments);
    int pathPointCount = static_cast<int>(pathOutline.size());

    for (int seg = 0; seg <= nSegments; seg++)
    {
        f32 t = (f32)seg / nSegments;
        f32 pathIdx = t * (f32)(pathPointCount - 1);
        int idx0 = (int)pathIdx;
        int idx1 = std::min(idx0 + 1, pathPointCount - 1);
        f32 blend = pathIdx - (f32)idx0;

        Vec3 pathPos = pathOutline[idx0] * (1.0f - blend) + pathOutline[idx1] * blend;

        Vec3 pathDir;
        if (pathPointCount >= 2)
        {
            pathDir = glm::normalize(pathOutline[std::min(idx0 + 1, pathPointCount - 1)] - pathOutline[idx0]);
        }
        else
        {
            pathDir = Vec3(0, 0, 1);
        }

        Vec3 up = Vec3(0, 1, 0);
        if (std::abs(glm::dot(pathDir, up)) > 0.99f)
            up = Vec3(1, 0, 0);

        Vec3 right = glm::normalize(glm::cross(pathDir, up));
        Vec3 actualUp = glm::normalize(glm::cross(right, pathDir));

        Mat4 orientation = glm::mat4(
            Vec4(right, 0),
            Vec4(actualUp, 0),
            Vec4(pathDir, 0),
            Vec4(0, 0, 0, 1)
        );

        for (const auto& p : profileOutline)
        {
            Vec3 profilePos = Vec3(orientation * Vec4(p, 1.0f)) + pathPos;
            Vertex v;
            v.position = profilePos;
            v.color = Color(0.7f, 0.85f, 0.7f);
            vertices.push_back(v);
        }
    }

    u32 profileSize = static_cast<u32>(profileOutline.size());
    for (int i = 0; i < nSegments; i++)
    {
        for (u32 j = 0; j < profileSize - 1; j++)
        {
            u32 curr = static_cast<u32>(i) * profileSize + j;
            u32 next = curr + profileSize;

            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);

            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    mesh->ComputeNormals();
    return mesh;
}

LoftOperation::LoftOperation() = default;

bool LoftOperation::IsValid() const
{
    return m_Params.sections.size() >= 2;
}

void LoftOperation::UpdatePreview()
{
}

SharedPtr<Mesh> LoftOperation::Execute()
{
    if (!IsValid())
        return nullptr;

    return LoftSketches(m_Params.sections, m_Params.segments, m_Params.closed);
}

SharedPtr<Mesh> LoftOperation::LofSketches(
    const std::vector<SharedPtr<Sketch>>& sections,
    int segments,
    bool closed
)
{
    if (sections.size() < 2)
        return nullptr;

    auto mesh = MakeShared<Mesh>();
    std::vector<Vertex> vertices;
    std::vector<u32> indices;

    std::vector<std::vector<Vec3>> outlines;
    for (const auto& section : sections)
    {
        if (section)
        {
            auto outline = section->GetOutline();
            if (!outline.empty())
                outlines.push_back(outline);
        }
    }

    if (outlines.size() < 2)
        return nullptr;

    size_t maxPoints = 0;
    for (const auto& outline : outlines)
        maxPoints = std::max(maxPoints, outline.size());

    int nSegments = std::max(2, segments);
    int nSections = static_cast<int>(outlines.size());

    for (int seg = 0; seg <= nSegments * (nSections - 1); seg++)
    {
        f32 totalT = (f32)seg / (f32)(nSegments * (nSections - 1));
        f32 sectionT = totalT * (f32)(nSections - 1);
        int sectionIdx0 = (int)sectionT;
        int sectionIdx1 = std::min(sectionIdx0 + 1, nSections - 1);
        f32 blend = sectionT - (f32)sectionIdx0;

        const auto& outline0 = outlines[sectionIdx0];
        const auto& outline1 = outlines[sectionIdx1];

        for (size_t pt = 0; pt < maxPoints; pt++)
        {
            f32 ptT0 = maxPoints > 1 ? (f32)pt / (f32)(maxPoints - 1) : 0.5f;
            f32 ptT1 = ptT0;

            Vec3 p0, p1;

            size_t idx0 = (size_t)(ptT0 * (f32)(outline0.size() - 1));
            size_t idx1 = (size_t)(ptT1 * (f32)(outline1.size() - 1));
            idx0 = std::min(idx0, outline0.size() - 1);
            idx1 = std::min(idx1, outline1.size() - 1);

            p0 = outline0[idx0];
            p1 = outline1[idx1];

            Vec3 interpolated = p0 * (1.0f - blend) + p1 * blend;

            Vertex v;
            v.position = interpolated;
            v.color = Color(0.85f, 0.75f, 0.7f);
            vertices.push_back(v);
        }
    }

    u32 pointsPerSection = static_cast<u32>(maxPoints);
    u32 totalSections = static_cast<u32>(nSegments * (nSections - 1) + 1);

    for (u32 s = 0; s < totalSections - 1; s++)
    {
        for (u32 p = 0; p < pointsPerSection - 1; p++)
        {
            u32 curr = s * pointsPerSection + p;
            u32 next = curr + pointsPerSection;

            indices.push_back(curr);
            indices.push_back(next);
            indices.push_back(curr + 1);

            indices.push_back(curr + 1);
            indices.push_back(next);
            indices.push_back(next + 1);
        }
    }

    mesh->SetVerticesAndIndices(vertices, indices);
    mesh->ComputeNormals();
    return mesh;
}

SharedPtr<Mesh> LoftOperation::LoftSketches(
    const std::vector<SharedPtr<Sketch>>& sections,
    int segments,
    bool closed
)
{
    return LofSketches(sections, segments, closed);
}

OperationManager::OperationManager()
{
    m_ExtrudeOp = MakeShared<ExtrudeOperation>();
    m_RevolveOp = MakeShared<RevolveOperation>();
    m_SweepOp = MakeShared<SweepOperation>();
    m_LoftOp = MakeShared<LoftOperation>();
}

OperationManager::~OperationManager() = default;

bool OperationManager::Initialize()
{
    return true;
}

void OperationManager::Shutdown()
{
    m_ExtrudeOp.reset();
    m_RevolveOp.reset();
    m_SweepOp.reset();
    m_LoftOp.reset();
    m_CurrentOperation.reset();
    m_PreviewMesh.reset();
}

void OperationManager::Update(f64 deltaTime)
{
    (void)deltaTime;
}

void OperationManager::SetCurrentOperation(OperationType type)
{
    m_CurrentOperationType = type;

    switch (type)
    {
    case OperationType::Extrude:
        m_CurrentOperation = m_ExtrudeOp;
        break;
    case OperationType::Revolve:
        m_CurrentOperation = m_RevolveOp;
        break;
    case OperationType::Sweep:
        m_CurrentOperation = m_SweepOp;
        break;
    case OperationType::Loft:
        m_CurrentOperation = m_LoftOp;
        break;
    default:
        m_CurrentOperation = nullptr;
        break;
    }
}

SharedPtr<ExtrudeOperation> OperationManager::GetExtrudeOperation() const
{
    return m_ExtrudeOp;
}

SharedPtr<RevolveOperation> OperationManager::GetRevolveOperation() const
{
    return m_RevolveOp;
}

SharedPtr<SweepOperation> OperationManager::GetSweepOperation() const
{
    return m_SweepOp;
}

SharedPtr<LoftOperation> OperationManager::GetLoftOperation() const
{
    return m_LoftOp;
}

void OperationManager::ExecuteCurrentOperation()
{
    if (!m_CurrentOperation || !m_CurrentOperation->IsValid())
        return;

    m_PreviewMesh = m_CurrentOperation->Execute();

    if (m_PreviewMesh)
    {
        Application* app = Application::GetInstance();
        Scene* scene = app->GetScene();

        auto obj = scene->CreateObject("Operation_Result");
        obj->SetMesh(m_PreviewMesh);
    }
}

void OperationManager::CancelCurrentOperation()
{
    m_CurrentOperationType = OperationType::None;
    m_CurrentOperation = nullptr;
    m_PreviewMesh = nullptr;
}

SharedPtr<Mesh> OperationManager::Extrude(const ExtrudeParams& params)
{
    m_ExtrudeOp->SetParams(params);
    auto mesh = m_ExtrudeOp->Execute();
    m_PreviewMesh = mesh;

    if (mesh)
    {
        Application* app = Application::GetInstance();
        Scene* scene = app->GetScene();

        auto obj = scene->CreateObject("Extruded");
        obj->SetMesh(mesh);
    }

    return mesh;
}

SharedPtr<Mesh> OperationManager::Revolve(const RevolveParams& params)
{
    m_RevolveOp->SetParams(params);
    auto mesh = m_RevolveOp->Execute();
    m_PreviewMesh = mesh;

    if (mesh)
    {
        Application* app = Application::GetInstance();
        Scene* scene = app->GetScene();

        auto obj = scene->CreateObject("Revolved");
        obj->SetMesh(mesh);
    }

    return mesh;
}

SharedPtr<Mesh> OperationManager::Sweep(const SweepParams& params)
{
    m_SweepOp->SetParams(params);
    auto mesh = m_SweepOp->Execute();
    m_PreviewMesh = mesh;

    if (mesh)
    {
        Application* app = Application::GetInstance();
        Scene* scene = app->GetScene();

        auto obj = scene->CreateObject("Swept");
        obj->SetMesh(mesh);
    }

    return mesh;
}

SharedPtr<Mesh> OperationManager::Loft(const LoftParams& params)
{
    m_LoftOp->SetParams(params);
    auto mesh = m_LoftOp->Execute();
    m_PreviewMesh = mesh;

    if (mesh)
    {
        Application* app = Application::GetInstance();
        Scene* scene = app->GetScene();

        auto obj = scene->CreateObject("Lofted");
        obj->SetMesh(mesh);
    }

    return mesh;
}

} 
