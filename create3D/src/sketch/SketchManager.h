#pragma once

#include "Types.h"

namespace Create3D {

enum class SketchTool
{
    None,
    Select,
    Line,
    Rectangle,
    Circle,
    Arc,
    Polygon,
    Spline
};

enum class SketchConstraint
{
    None,
    Horizontal,
    Vertical,
    Perpendicular,
    Parallel,
    Tangent,
    EqualLength,
    Concentric,
    Coincident,
    Symmetric
};

struct SketchPoint
{
    Vec3 position;
    bool isSelected = false;
    bool isFixed = false;

    SketchPoint() = default;
    SketchPoint(const Vec3& p) : position(p) {}
};

struct SketchEntity
{
    virtual ~SketchEntity() = default;

    enum class Type
    {
        Line,
        Arc,
        Circle,
        Rectangle,
        Polygon,
        Spline
    };

    Type type;
    Color color = Color::Cyan();
    bool isSelected = false;
    bool isConstruction = false;
    u32 id = 0;

    virtual BoundingBox GetBoundingBox() const = 0;
    virtual void Translate(const Vec3& delta) = 0;
    virtual std::vector<SketchPoint*> GetControlPoints() = 0;
};

struct SketchLine : public SketchEntity
{
    SharedPtr<SketchPoint> start;
    SharedPtr<SketchPoint> end;

    SketchLine() { type = Type::Line; }
    
    BoundingBox GetBoundingBox() const override
    {
        BoundingBox bbox;
        bbox.Expand(start->position);
        bbox.Expand(end->position);
        return bbox;
    }

    void Translate(const Vec3& delta) override
    {
        start->position += delta;
        end->position += delta;
    }

    std::vector<SketchPoint*> GetControlPoints() override
    {
        return { start.get(), end.get() };
    }

    f32 Length() const
    {
        return glm::distance(start->position, end->position);
    }

    Vec3 Direction() const
    {
        return glm::normalize(end->position - start->position);
    }
};

struct SketchCircle : public SketchEntity
{
    SharedPtr<SketchPoint> center;
    f32 radius = 1.0f;

    SketchCircle() { type = Type::Circle; }

    BoundingBox GetBoundingBox() const override
    {
        return BoundingBox(
            center->position - Vec3(radius, radius, radius),
            center->position + Vec3(radius, radius, radius)
        );
    }

    void Translate(const Vec3& delta) override
    {
        center->position += delta;
    }

    std::vector<SketchPoint*> GetControlPoints() override
    {
        return { center.get() };
    }
};

struct SketchArc : public SketchEntity
{
    SharedPtr<SketchPoint> center;
    SharedPtr<SketchPoint> start;
    SharedPtr<SketchPoint> end;
    f32 startAngle = 0;
    f32 endAngle = glm::half_pi<f32>();

    SketchArc() { type = Type::Arc; }

    BoundingBox GetBoundingBox() const override
    {
        BoundingBox bbox;
        bbox.Expand(center->position);
        bbox.Expand(start->position);
        bbox.Expand(end->position);
        return bbox;
    }

    void Translate(const Vec3& delta) override
    {
        center->position += delta;
        start->position += delta;
        end->position += delta;
    }

    std::vector<SketchPoint*> GetControlPoints() override
    {
        return { center.get(), start.get(), end.get() };
    }

    f32 Radius() const
    {
        return glm::distance(center->position, start->position);
    }
};

struct SketchRectangle : public SketchEntity
{
    SharedPtr<SketchPoint> minPoint;
    SharedPtr<SketchPoint> maxPoint;

    SketchRectangle() { type = Type::Rectangle; }

    BoundingBox GetBoundingBox() const override
    {
        return BoundingBox(minPoint->position, maxPoint->position);
    }

    void Translate(const Vec3& delta) override
    {
        minPoint->position += delta;
        maxPoint->position += delta;
    }

    std::vector<SketchPoint*> GetControlPoints() override
    {
        return { minPoint.get(), maxPoint.get() };
    }
};

struct SketchPolygon : public SketchEntity
{
    std::vector<SharedPtr<SketchPoint>> vertices;
    bool closed = true;

    SketchPolygon() { type = Type::Polygon; }

    BoundingBox GetBoundingBox() const override
    {
        BoundingBox bbox;
        for (const auto& v : vertices)
            bbox.Expand(v->position);
        return bbox;
    }

    void Translate(const Vec3& delta) override
    {
        for (auto& v : vertices)
            v->position += delta;
    }

    std::vector<SketchPoint*> GetControlPoints() override
    {
        std::vector<SketchPoint*> pts;
        for (auto& v : vertices)
            pts.push_back(v.get());
        return pts;
    }
};

class Sketch
{
public:
    Sketch();
    ~Sketch();

    void SetName(const std::string& name) { m_Name = name; }
    const std::string& GetName() const { return m_Name; }

    void SetPlane(const Plane& plane) { m_Plane = plane; }
    const Plane& GetPlane() const { return m_Plane; }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    void SetActive(bool active) { m_Active = active; }
    bool IsActive() const { return m_Active; }

    SharedPtr<SketchLine> AddLine(const Vec3& start, const Vec3& end);
    SharedPtr<SketchCircle> AddCircle(const Vec3& center, f32 radius);
    SharedPtr<SketchArc> AddArc(const Vec3& center, const Vec3& start, const Vec3& end);
    SharedPtr<SketchRectangle> AddRectangle(const Vec3& min, const Vec3& max);
    SharedPtr<SketchPolygon> AddPolygon(const std::vector<Vec3>& vertices);

    void AddEntity(SharedPtr<SketchEntity> entity);
    void RemoveEntity(SharedPtr<SketchEntity> entity);
    void Clear();

    SharedPtr<SketchPoint> AddPoint(const Vec3& position);
    SharedPtr<SketchPoint> FindOrAddPoint(const Vec3& position, f32 tolerance = 0.01f);

    SharedPtr<SketchEntity> GetEntity(u32 id) const;
    SharedPtr<SketchEntity> FindEntityAt(const Vec3& worldPos, f32 tolerance = 0.1f);
    SharedPtr<SketchPoint> FindPointAt(const Vec3& worldPos, f32 tolerance = 0.1f);

    const std::vector<SharedPtr<SketchEntity>>& GetEntities() const { return m_Entities; }
    const std::vector<SharedPtr<SketchPoint>>& GetPoints() const { return m_Points; }

    void SelectAll();
    void DeselectAll();
    void DeleteSelected();

    std::vector<SharedPtr<SketchEntity>> GetSelectedEntities() const;

    std::vector<Vec3> GetOutline() const;

private:
    std::string m_Name = "Sketch";
    Plane m_Plane;
    std::vector<SharedPtr<SketchEntity>> m_Entities;
    std::vector<SharedPtr<SketchPoint>> m_Points;
    bool m_Visible = true;
    bool m_Active = false;
    u32 m_NextEntityId = 1;
};

class SketchManager
{
public:
    SketchManager();
    ~SketchManager();

    bool Initialize();
    void Shutdown();
    void Update(f64 deltaTime);
    void Render();

    SharedPtr<Sketch> CreateSketch(const std::string& name = "Sketch", PlaneType planeType = PlaneType::XY);
    SharedPtr<Sketch> CreateSketchOnPlane(const std::string& name, const Plane& plane);

    void SetActiveSketch(SharedPtr<Sketch> sketch);
    SharedPtr<Sketch> GetActiveSketch() const { return m_ActiveSketch; }

    void SetCurrentTool(SketchTool tool) { m_CurrentTool = tool; }
    SketchTool GetCurrentTool() const { return m_CurrentTool; }

    const std::vector<SharedPtr<Sketch>>& GetSketches() const { return m_Sketches; }
    SharedPtr<Sketch> GetSketch(size_t index) const;

    void DeleteSketch(SharedPtr<Sketch> sketch);
    void Clear();

    void HandleMouseClick(const Vec3& worldPos);
    void HandleMouseMove(const Vec3& worldPos);
    void HandleMouseRelease(const Vec3& worldPos);
    void HandleKeyPress(int key);

    void SetPlane(PlaneType type);
    void SetCustomPlane(const Plane& plane);
    Plane GetCurrentPlane() const { return m_CurrentPlane; }
    PlaneType GetPlaneType() const { return m_PlaneType; }

    bool IsDrawing() const { return m_IsDrawing; }
    void CancelDrawing();

private:
    void UpdateToolPreview(const Vec3& worldPos);
    void RenderSketch(const Sketch* sketch);
    void RenderPreview();

    std::vector<SharedPtr<Sketch>> m_Sketches;
    SharedPtr<Sketch> m_ActiveSketch;

    SketchTool m_CurrentTool = SketchTool::None;
    PlaneType m_PlaneType = PlaneType::XY;
    Plane m_CurrentPlane;

    bool m_IsDrawing = false;
    Vec3 m_DrawStartPos;
    Vec3 m_DrawCurrentPos;
    u32 m_PolygonVertexCount = 0;
    std::vector<Vec3> m_PolygonPoints;
};

} 
