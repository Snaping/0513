#pragma once

#include "Types.h"
#include "Mesh.h"

namespace Create3D {

class SceneObject
{
public:
    SceneObject(const std::string& name = "Object");
    ~SceneObject() = default;

    void SetName(const std::string& name) { m_Name = name; }
    const std::string& GetName() const { return m_Name; }

    void SetMesh(SharedPtr<Mesh> mesh) { m_Mesh = mesh; }
    SharedPtr<Mesh> GetMesh() const { return m_Mesh; }

    void SetPosition(const Vec3& position) { m_Position = position; UpdateTransform(); }
    void SetRotation(const Vec3& eulerAngles) { m_Rotation = eulerAngles; UpdateTransform(); }
    void SetScale(const Vec3& scale) { m_Scale = scale; UpdateTransform(); }
    void SetTransform(const Mat4& transform);

    Vec3 GetPosition() const { return m_Position; }
    Vec3 GetRotation() const { return m_Rotation; }
    Vec3 GetScale() const { return m_Scale; }
    const Mat4& GetTransform() const { return m_Transform; }

    void SetVisible(bool visible) { m_Visible = visible; }
    bool IsVisible() const { return m_Visible; }

    void SetSelected(bool selected) { m_Selected = selected; }
    bool IsSelected() const { return m_Selected; }

    void SetColor(const Color& color) { m_Color = color; }
    const Color& GetColor() const { return m_Color; }

    BoundingBox GetWorldBoundingBox() const;

    void Translate(const Vec3& delta);
    void Rotate(const Vec3& delta);

private:
    void UpdateTransform();

    std::string m_Name;
    SharedPtr<Mesh> m_Mesh;
    Vec3 m_Position = Vec3(0);
    Vec3 m_Rotation = Vec3(0);
    Vec3 m_Scale = Vec3(1);
    Mat4 m_Transform = Mat4(1);
    bool m_Visible = true;
    bool m_Selected = false;
    Color m_Color = Color(0.8f, 0.8f, 0.8f);
};

class Scene
{
public:
    Scene();
    ~Scene();

    bool Initialize();
    void Shutdown();
    void Update(f64 deltaTime);

    SharedPtr<SceneObject> CreateObject(const std::string& name = "Object");
    SharedPtr<SceneObject> CreatePrimitive(const std::string& type, const Vec3& position = Vec3(0));

    void AddObject(SharedPtr<SceneObject> object);
    void RemoveObject(SharedPtr<SceneObject> object);
    void RemoveObject(size_t index);
    void Clear();

    SharedPtr<SceneObject> GetObject(size_t index) const;
    SharedPtr<SceneObject> FindObject(const std::string& name) const;
    
    size_t GetObjectCount() const { return m_Objects.size(); }
    const std::vector<SharedPtr<SceneObject>>& GetObjects() const { return m_Objects; }

    void SelectObject(SharedPtr<SceneObject> object);
    void DeselectAll();
    SharedPtr<SceneObject> GetSelectedObject() const { return m_SelectedObject; }

    void SetDirty(bool dirty) { m_Dirty = dirty; }
    bool IsDirty() const { return m_Dirty; }

private:
    std::vector<SharedPtr<SceneObject>> m_Objects;
    SharedPtr<SceneObject> m_SelectedObject;
    bool m_Dirty = false;
};

} 
