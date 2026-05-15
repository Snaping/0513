#include "SketchManager.h"
#include "Application.h"
#include "Renderer.h"

#include <imgui.h>

namespace Create3D {

Sketch::Sketch()
{
    m_Plane = Plane(Vec3(0, 0, 1), 0);
}

Sketch::~Sketch() = default;

SharedPtr<SketchLine> Sketch::AddLine(const Vec3& start, const Vec3& end)
{
    auto line = MakeShared<SketchLine>();
    line->id = m_NextEntityId++;
    line->start = FindOrAddPoint(start);
    line->end = FindOrAddPoint(end);
    m_Entities.push_back(line);
    return line;
}

SharedPtr<SketchCircle> Sketch::AddCircle(const Vec3& center, f32 radius)
{
    auto circle = MakeShared<SketchCircle>();
    circle->id = m_NextEntityId++;
    circle->center = FindOrAddPoint(center);
    circle->radius = radius;
    m_Entities.push_back(circle);
    return circle;
}

SharedPtr<SketchArc> Sketch::AddArc(const Vec3& center, const Vec3& start, const Vec3& end)
{
    auto arc = MakeShared<SketchArc>();
    arc->id = m_NextEntityId++;
    arc->center = FindOrAddPoint(center);
    arc->start = FindOrAddPoint(start);
    arc->end = FindOrAddPoint(end);
    m_Entities.push_back(arc);
    return arc;
}

SharedPtr<SketchRectangle> Sketch::AddRectangle(const Vec3& min, const Vec3& max)
{
    auto rect = MakeShared<SketchRectangle>();
    rect->id = m_NextEntityId++;
    rect->minPoint = FindOrAddPoint(min);
    rect->maxPoint = FindOrAddPoint(max);
    m_Entities.push_back(rect);
    return rect;
}

SharedPtr<SketchPolygon> Sketch::AddPolygon(const std::vector<Vec3>& vertices)
{
    auto poly = MakeShared<SketchPolygon>();
    poly->id = m_NextEntityId++;
    for (const auto& v : vertices)
        poly->vertices.push_back(FindOrAddPoint(v));
    m_Entities.push_back(poly);
    return poly;
}

void Sketch::AddEntity(SharedPtr<SketchEntity> entity)
{
    entity->id = m_NextEntityId++;
    m_Entities.push_back(entity);
}

void Sketch::RemoveEntity(SharedPtr<SketchEntity> entity)
{
    auto it = std::find(m_Entities.begin(), m_Entities.end(), entity);
    if (it != m_Entities.end())
        m_Entities.erase(it);
}

void Sketch::Clear()
{
    m_Entities.clear();
    m_Points.clear();
}

SharedPtr<SketchPoint> Sketch::AddPoint(const Vec3& position)
{
    auto pt = MakeShared<SketchPoint>(position);
    m_Points.push_back(pt);
    return pt;
}

SharedPtr<SketchPoint> Sketch::FindOrAddPoint(const Vec3& position, f32 tolerance)
{
    for (const auto& pt : m_Points)
    {
        if (glm::distance(pt->position, position) < tolerance)
            return pt;
    }
    return AddPoint(position);
}

SharedPtr<SketchEntity> Sketch::GetEntity(u32 id) const
{
    for (const auto& e : m_Entities)
        if (e->id == id)
            return e;
    return nullptr;
}

SharedPtr<SketchEntity> Sketch::FindEntityAt(const Vec3& worldPos, f32 tolerance)
{
    for (auto it = m_Entities.rbegin(); it != m_Entities.rend(); ++it)
    {
        const auto& entity = *it;
        bool hit = false;

        switch (entity->type)
        {
        case SketchEntity::Type::Line:
        {
            auto line = std::static_pointer_cast<SketchLine>(entity);
            Vec3 start = line->start->position;
            Vec3 end = line->end->position;
            Vec3 ab = end - start;
            Vec3 ap = worldPos - start;
            f32 lenSq = glm::dot(ab, ab);
            if (lenSq < 0.0001f)
            {
                hit = glm::distance(worldPos, start) < tolerance;
            }
            else
            {
                f32 t = glm::clamp(glm::dot(ap, ab) / lenSq, 0.0f, 1.0f);
                Vec3 closest = start + ab * t;
                hit = glm::distance(worldPos, closest) < tolerance;
            }
            break;
        }
        case SketchEntity::Type::Circle:
        {
            auto circle = std::static_pointer_cast<SketchCircle>(entity);
            f32 dist = glm::distance(worldPos, circle->center->position);
            hit = std::abs(dist - circle->radius) < tolerance;
            break;
        }
        case SketchEntity::Type::Rectangle:
        {
            auto rect = std::static_pointer_cast<SketchRectangle>(entity);
            Vec3 min = rect->minPoint->position;
            Vec3 max = rect->maxPoint->position;
            Vec3 edges[8] = {
                min, Vec3(max.x, min.y, min.z),
                Vec3(max.x, min.y, min.z), Vec3(max.x, max.y, min.z),
                Vec3(max.x, max.y, min.z), Vec3(min.x, max.y, min.z),
                Vec3(min.x, max.y, min.z), min
            };
            for (int i = 0; i < 8; i += 2)
            {
                Vec3 a = edges[i];
                Vec3 b = edges[i + 1];
                Vec3 ab = b - a;
                Vec3 ap = worldPos - a;
                f32 lenSq = glm::dot(ab, ab);
                f32 t = glm::clamp(glm::dot(ap, ab) / lenSq, 0.0f, 1.0f);
                Vec3 closest = a + ab * t;
                if (glm::distance(worldPos, closest) < tolerance)
                {
                    hit = true;
                    break;
                }
            }
            break;
        }
        default:
            break;
        }

        if (hit)
            return entity;
    }
    return nullptr;
}

SharedPtr<SketchPoint> Sketch::FindPointAt(const Vec3& worldPos, f32 tolerance)
{
    for (auto it = m_Points.rbegin(); it != m_Points.rend(); ++it)
    {
        if (glm::distance(worldPos, (*it)->position) < tolerance)
            return *it;
    }
    return nullptr;
}

void Sketch::SelectAll()
{
    for (auto& e : m_Entities)
        e->isSelected = true;
    for (auto& p : m_Points)
        p->isSelected = true;
}

void Sketch::DeselectAll()
{
    for (auto& e : m_Entities)
        e->isSelected = false;
    for (auto& p : m_Points)
        p->isSelected = false;
}

void Sketch::DeleteSelected()
{
    std::vector<SharedPtr<SketchEntity>> toRemove;
    for (auto& e : m_Entities)
        if (e->isSelected)
            toRemove.push_back(e);

    for (auto& e : toRemove)
        RemoveEntity(e);
}

std::vector<SharedPtr<SketchEntity>> Sketch::GetSelectedEntities() const
{
    std::vector<SharedPtr<SketchEntity>> selected;
    for (const auto& e : m_Entities)
        if (e->isSelected)
            selected.push_back(e);
    return selected;
}

std::vector<Vec3> Sketch::GetOutline() const
{
    std::vector<Vec3> outline;
    for (const auto& entity : m_Entities)
    {
        switch (entity->type)
        {
        case SketchEntity::Type::Line:
        {
            auto line = std::static_pointer_cast<SketchLine>(entity);
            outline.push_back(line->start->position);
            outline.push_back(line->end->position);
            break;
        }
        case SketchEntity::Type::Rectangle:
        {
            auto rect = std::static_pointer_cast<SketchRectangle>(entity);
            Vec3 min = rect->minPoint->position;
            Vec3 max = rect->maxPoint->position;
            outline.push_back(Vec3(min.x, min.y, min.z));
            outline.push_back(Vec3(max.x, min.y, min.z));
            outline.push_back(Vec3(max.x, max.y, min.z));
            outline.push_back(Vec3(min.x, max.y, min.z));
            outline.push_back(Vec3(min.x, min.y, min.z));
            break;
        }
        case SketchEntity::Type::Circle:
        {
            auto circle = std::static_pointer_cast<SketchCircle>(entity);
            const int segments = 64;
            for (int i = 0; i <= segments; i++)
            {
                f32 angle = (f32)i / segments * 2.0f * glm::pi<f32>();
                Vec3 p = circle->center->position + Vec3(cos(angle) * circle->radius, sin(angle) * circle->radius, 0);
                outline.push_back(p);
            }
            break;
        }
        default:
            break;
        }
    }
    return outline;
}

SketchManager::SketchManager()
{
    SetPlane(PlaneType::XY);
}

SketchManager::~SketchManager() = default;

bool SketchManager::Initialize()
{
    return true;
}

void SketchManager::Shutdown()
{
    m_Sketches.clear();
    m_ActiveSketch = nullptr;
}

void SketchManager::Update(f64 deltaTime)
{
    (void)deltaTime;
}

void SketchManager::SetPlane(PlaneType type)
{
    m_PlaneType = type;
    switch (type)
    {
    case PlaneType::XY:
        m_CurrentPlane = Plane::FromPointNormal(Vec3(0), Vec3(0, 0, 1));
        break;
    case PlaneType::YZ:
        m_CurrentPlane = Plane::FromPointNormal(Vec3(0), Vec3(1, 0, 0));
        break;
    case PlaneType::XZ:
        m_CurrentPlane = Plane::FromPointNormal(Vec3(0), Vec3(0, 1, 0));
        break;
    default:
        break;
    }
}

void SketchManager::SetCustomPlane(const Plane& plane)
{
    m_PlaneType = PlaneType::Custom;
    m_CurrentPlane = plane;
}

SharedPtr<Sketch> SketchManager::CreateSketch(const std::string& name, PlaneType planeType)
{
    auto sketch = MakeShared<Sketch>();
    sketch->SetName(name);
    
    Plane plane;
    switch (planeType)
    {
    case PlaneType::XY: plane = Plane::FromPointNormal(Vec3(0), Vec3(0, 0, 1)); break;
    case PlaneType::YZ: plane = Plane::FromPointNormal(Vec3(0), Vec3(1, 0, 0)); break;
    case PlaneType::XZ: plane = Plane::FromPointNormal(Vec3(0), Vec3(0, 1, 0)); break;
    default: plane = Plane::FromPointNormal(Vec3(0), Vec3(0, 0, 1)); break;
    }
    sketch->SetPlane(plane);
    sketch->SetActive(true);
    
    if (m_ActiveSketch)
        m_ActiveSketch->SetActive(false);
    m_ActiveSketch = sketch;
    m_Sketches.push_back(sketch);
    
    return sketch;
}

SharedPtr<Sketch> SketchManager::CreateSketchOnPlane(const std::string& name, const Plane& plane)
{
    auto sketch = MakeShared<Sketch>();
    sketch->SetName(name);
    sketch->SetPlane(plane);
    sketch->SetActive(true);
    
    if (m_ActiveSketch)
        m_ActiveSketch->SetActive(false);
    m_ActiveSketch = sketch;
    m_Sketches.push_back(sketch);
    
    return sketch;
}

void SketchManager::SetActiveSketch(SharedPtr<Sketch> sketch)
{
    if (m_ActiveSketch)
        m_ActiveSketch->SetActive(false);
    m_ActiveSketch = sketch;
    if (sketch)
    {
        sketch->SetActive(true);
        m_CurrentPlane = sketch->GetPlane();
    }
}

SharedPtr<Sketch> SketchManager::GetSketch(size_t index) const
{
    if (index < m_Sketches.size())
        return m_Sketches[index];
    return nullptr;
}

void SketchManager::DeleteSketch(SharedPtr<Sketch> sketch)
{
    auto it = std::find(m_Sketches.begin(), m_Sketches.end(), sketch);
    if (it != m_Sketches.end())
    {
        if (m_ActiveSketch == sketch)
            m_ActiveSketch = nullptr;
        m_Sketches.erase(it);
    }
}

void SketchManager::Clear()
{
    m_Sketches.clear();
    m_ActiveSketch = nullptr;
}

void SketchManager::HandleMouseClick(const Vec3& worldPos)
{
    if (!m_ActiveSketch)
        return;

    switch (m_CurrentTool)
    {
    case SketchTool::Select:
    {
        auto entity = m_ActiveSketch->FindEntityAt(worldPos);
        auto point = m_ActiveSketch->FindPointAt(worldPos);
        
        if (entity)
            entity->isSelected = !entity->isSelected;
        else if (point)
            point->isSelected = !point->isSelected;
        else
            m_ActiveSketch->DeselectAll();
        break;
    }
    case SketchTool::Line:
    {
        if (!m_IsDrawing)
        {
            m_DrawStartPos = worldPos;
            m_IsDrawing = true;
        }
        else
        {
            if (glm::distance(worldPos, m_DrawStartPos) > 0.01f)
            {
                m_ActiveSketch->AddLine(m_DrawStartPos, worldPos);
            }
            m_IsDrawing = false;
        }
        break;
    }
    case SketchTool::Circle:
    {
        if (!m_IsDrawing)
        {
            m_DrawStartPos = worldPos;
            m_IsDrawing = true;
        }
        else
        {
            f32 radius = glm::distance(worldPos, m_DrawStartPos);
            if (radius > 0.01f)
            {
                m_ActiveSketch->AddCircle(m_DrawStartPos, radius);
            }
            m_IsDrawing = false;
        }
        break;
    }
    case SketchTool::Rectangle:
    {
        if (!m_IsDrawing)
        {
            m_DrawStartPos = worldPos;
            m_IsDrawing = true;
        }
        else
        {
            Vec3 min(
                std::min(m_DrawStartPos.x, worldPos.x),
                std::min(m_DrawStartPos.y, worldPos.y),
                std::min(m_DrawStartPos.z, worldPos.z)
            );
            Vec3 max(
                std::max(m_DrawStartPos.x, worldPos.x),
                std::max(m_DrawStartPos.y, worldPos.y),
                std::max(m_DrawStartPos.z, worldPos.z)
            );
            m_ActiveSketch->AddRectangle(min, max);
            m_IsDrawing = false;
        }
        break;
    }
    case SketchTool::Polygon:
    {
        if (!m_IsDrawing)
        {
            m_PolygonPoints.clear();
            m_PolygonPoints.push_back(worldPos);
            m_IsDrawing = true;
            m_PolygonVertexCount = 1;
        }
        else
        {
            m_PolygonPoints.push_back(worldPos);
            m_PolygonVertexCount++;
        }
        break;
    }
    default:
        break;
    }
}

void SketchManager::HandleMouseMove(const Vec3& worldPos)
{
    m_DrawCurrentPos = worldPos;
}

void SketchManager::HandleMouseRelease(const Vec3& worldPos)
{
    (void)worldPos;
}

void SketchManager::HandleKeyPress(int key)
{
    if (key == GLFW_KEY_ESCAPE)
    {
        CancelDrawing();
    }
    if (key == GLFW_KEY_DELETE && m_ActiveSketch)
    {
        m_ActiveSketch->DeleteSelected();
    }
}

void SketchManager::CancelDrawing()
{
    m_IsDrawing = false;
    m_PolygonPoints.clear();
    m_PolygonVertexCount = 0;
}

void SketchManager::Render()
{
    for (const auto& sketch : m_Sketches)
    {
        if (sketch->IsVisible())
            RenderSketch(sketch.get());
    }

    if (m_IsDrawing)
        RenderPreview();
}

void SketchManager::RenderSketch(const Sketch* sketch)
{
    Renderer* renderer = Application::GetInstance()->GetRenderer();
    if (!renderer || !sketch) return;

    Color entityColor = sketch->IsActive() ? Color::Yellow() : Color::Cyan();
    Color selectedColor = Color::Orange();
    Color pointColor = Color::Green();
    Color selectedPointColor = Color::Red();

    for (const auto& entity : sketch->GetEntities())
    {
        Color color = entity->isSelected ? selectedColor : entityColor;

        switch (entity->type)
        {
        case SketchEntity::Type::Line:
        {
            auto line = std::static_pointer_cast<SketchLine>(entity);
            renderer->DrawLine(line->start->position, line->end->position, color, 2.0f);
            break;
        }
        case SketchEntity::Type::Circle:
        {
            auto circle = std::static_pointer_cast<SketchCircle>(entity);
            const int segments = 64;
            Vec3 center = circle->center->position;
            for (int i = 0; i < segments; i++)
            {
                f32 a1 = (f32)i / segments * 2.0f * glm::pi<f32>();
                f32 a2 = (f32)(i + 1) / segments * 2.0f * glm::pi<f32>();
                Vec3 p1 = center + Vec3(cos(a1) * circle->radius, sin(a1) * circle->radius, 0);
                Vec3 p2 = center + Vec3(cos(a2) * circle->radius, sin(a2) * circle->radius, 0);
                renderer->DrawLine(p1, p2, color, 2.0f);
            }
            break;
        }
        case SketchEntity::Type::Rectangle:
        {
            auto rect = std::static_pointer_cast<SketchRectangle>(entity);
            Vec3 min = rect->minPoint->position;
            Vec3 max = rect->maxPoint->position;
            Vec3 p0(min.x, min.y, min.z);
            Vec3 p1(max.x, min.y, min.z);
            Vec3 p2(max.x, max.y, min.z);
            Vec3 p3(min.x, max.y, min.z);
            renderer->DrawLine(p0, p1, color, 2.0f);
            renderer->DrawLine(p1, p2, color, 2.0f);
            renderer->DrawLine(p2, p3, color, 2.0f);
            renderer->DrawLine(p3, p0, color, 2.0f);
            break;
        }
        case SketchEntity::Type::Arc:
        {
            auto arc = std::static_pointer_cast<SketchArc>(entity);
            const int segments = 32;
            Vec3 center = arc->center->position;
            f32 radius = arc->Radius();
            for (int i = 0; i < segments; i++)
            {
                f32 t1 = (f32)i / segments;
                f32 t2 = (f32)(i + 1) / segments;
                f32 a1 = arc->startAngle + (arc->endAngle - arc->startAngle) * t1;
                f32 a2 = arc->startAngle + (arc->endAngle - arc->startAngle) * t2;
                Vec3 p1 = center + Vec3(cos(a1) * radius, sin(a1) * radius, 0);
                Vec3 p2 = center + Vec3(cos(a2) * radius, sin(a2) * radius, 0);
                renderer->DrawLine(p1, p2, color, 2.0f);
            }
            break;
        }
        case SketchEntity::Type::Polygon:
        {
            auto poly = std::static_pointer_cast<SketchPolygon>(entity);
            for (size_t i = 0; i < poly->vertices.size(); i++)
            {
                size_t next = (i + 1) % poly->vertices.size();
                if (!poly->closed && i == poly->vertices.size() - 1)
                    break;
                renderer->DrawLine(
                    poly->vertices[i]->position,
                    poly->vertices[next]->position,
                    color, 2.0f
                );
            }
            break;
        }
        default:
            break;
        }
    }

    for (const auto& point : sketch->GetPoints())
    {
        Color color = point->isSelected ? selectedPointColor : pointColor;
        renderer->DrawPoint(point->position, color, 8.0f);
    }
}

void SketchManager::RenderPreview()
{
    Renderer* renderer = Application::GetInstance()->GetRenderer();
    if (!renderer) return;

    Color previewColor(0.8f, 0.8f, 0.0f, 0.7f);

    switch (m_CurrentTool)
    {
    case SketchTool::Line:
        if (m_IsDrawing)
        {
            renderer->DrawLine(m_DrawStartPos, m_DrawCurrentPos, previewColor, 2.0f);
        }
        break;
    case SketchTool::Circle:
        if (m_IsDrawing)
        {
            f32 radius = glm::distance(m_DrawCurrentPos, m_DrawStartPos);
            const int segments = 48;
            for (int i = 0; i < segments; i++)
            {
                f32 a1 = (f32)i / segments * 2.0f * glm::pi<f32>();
                f32 a2 = (f32)(i + 1) / segments * 2.0f * glm::pi<f32>();
                Vec3 p1 = m_DrawStartPos + Vec3(cos(a1) * radius, sin(a1) * radius, 0);
                Vec3 p2 = m_DrawStartPos + Vec3(cos(a2) * radius, sin(a2) * radius, 0);
                renderer->DrawLine(p1, p2, previewColor, 2.0f);
            }
        }
        break;
    case SketchTool::Rectangle:
        if (m_IsDrawing)
        {
            Vec3 min(
                std::min(m_DrawStartPos.x, m_DrawCurrentPos.x),
                std::min(m_DrawStartPos.y, m_DrawCurrentPos.y),
                std::min(m_DrawStartPos.z, m_DrawCurrentPos.z)
            );
            Vec3 max(
                std::max(m_DrawStartPos.x, m_DrawCurrentPos.x),
                std::max(m_DrawStartPos.y, m_DrawCurrentPos.y),
                std::max(m_DrawStartPos.z, m_DrawCurrentPos.z)
            );
            Vec3 p0(min.x, min.y, min.z);
            Vec3 p1(max.x, min.y, min.z);
            Vec3 p2(max.x, max.y, min.z);
            Vec3 p3(min.x, max.y, min.z);
            renderer->DrawLine(p0, p1, previewColor, 2.0f);
            renderer->DrawLine(p1, p2, previewColor, 2.0f);
            renderer->DrawLine(p2, p3, previewColor, 2.0f);
            renderer->DrawLine(p3, p0, previewColor, 2.0f);
        }
        break;
    case SketchTool::Polygon:
        if (m_IsDrawing && !m_PolygonPoints.empty())
        {
            for (size_t i = 0; i < m_PolygonPoints.size() - 1; i++)
            {
                renderer->DrawLine(m_PolygonPoints[i], m_PolygonPoints[i + 1], previewColor, 2.0f);
            }
            renderer->DrawLine(m_PolygonPoints.back(), m_DrawCurrentPos, previewColor, 2.0f);
        }
        break;
    default:
        break;
    }
}

} 
