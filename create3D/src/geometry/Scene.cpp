#include "Scene.h"
#include "Application.h"
#include "Renderer.h"

namespace Create3D {

SceneObject::SceneObject(const std::string& name)
    : m_Name(name)
{
    UpdateTransform();
}

void SceneObject::SetTransform(const Mat4& transform)
{
    m_Transform = transform;
    Vec3 skew;
    Vec4 perspective;
    glm::decompose(transform, m_Scale, Quat(), m_Position, skew, perspective);
    m_Rotation = glm::eulerAngles(glm::quat_cast(glm::mat3(transform)));
}

void SceneObject::UpdateTransform()
{
    m_Transform = Mat4(1);
    m_Transform = glm::translate(m_Transform, m_Position);
    m_Transform = glm::rotate(m_Transform, m_Rotation.x, Vec3(1, 0, 0));
    m_Transform = glm::rotate(m_Transform, m_Rotation.y, Vec3(0, 1, 0));
    m_Transform = glm::rotate(m_Transform, m_Rotation.z, Vec3(0, 0, 1));
    m_Transform = glm::scale(m_Transform, m_Scale);
}

BoundingBox SceneObject::GetWorldBoundingBox() const
{
    BoundingBox bbox;
    if (!m_Mesh)
        return bbox;

    BoundingBox localBbox = m_Mesh->GetBoundingBox();
    
    std::array<Vec3, 8> corners = {
        Vec3(localBbox.min.x, localBbox.min.y, localBbox.min.z),
        Vec3(localBbox.max.x, localBbox.min.y, localBbox.min.z),
        Vec3(localBbox.max.x, localBbox.min.y, localBbox.max.z),
        Vec3(localBbox.min.x, localBbox.min.y, localBbox.max.z),
        Vec3(localBbox.min.x, localBbox.max.y, localBbox.min.z),
        Vec3(localBbox.max.x, localBbox.max.y, localBbox.min.z),
        Vec3(localBbox.max.x, localBbox.max.y, localBbox.max.z),
        Vec3(localBbox.min.x, localBbox.max.y, localBbox.max.z)
    };

    for (const auto& c : corners)
    {
        Vec3 worldPos = Vec3(m_Transform * Vec4(c, 1.0f));
        bbox.Expand(worldPos);
    }

    return bbox;
}

void SceneObject::Translate(const Vec3& delta)
{
    m_Position += delta;
    UpdateTransform();
}

void SceneObject::Rotate(const Vec3& delta)
{
    m_Rotation += delta;
    UpdateTransform();
}

Scene::Scene() = default;

Scene::~Scene() = default;

bool Scene::Initialize()
{
    return true;
}

void Scene::Shutdown()
{
    Clear();
}

void Scene::Update(f64 deltaTime)
{
    (void)deltaTime;
}

SharedPtr<SceneObject> Scene::CreateObject(const std::string& name)
{
    auto obj = MakeShared<SceneObject>(name);
    m_Objects.push_back(obj);
    m_Dirty = true;
    return obj;
}

SharedPtr<SceneObject> Scene::CreatePrimitive(const std::string& type, const Vec3& position)
{
    SharedPtr<Mesh> mesh;

    if (type == "Cube" || type == "Box")
        mesh = Mesh::CreateCube();
    else if (type == "Sphere")
        mesh = Mesh::CreateSphere();
    else if (type == "Cylinder")
        mesh = Mesh::CreateCylinder();
    else if (type == "Cone")
        mesh = Mesh::CreateCone();
    else if (type == "Torus")
        mesh = Mesh::CreateTorus();
    else if (type == "Plane")
        mesh = Mesh::CreatePlane();
    else
        return nullptr;

    auto obj = CreateObject(type);
    obj->SetMesh(mesh);
    obj->SetPosition(position);
    return obj;
}

void Scene::AddObject(SharedPtr<SceneObject> object)
{
    m_Objects.push_back(object);
    m_Dirty = true;
}

void Scene::RemoveObject(SharedPtr<SceneObject> object)
{
    auto it = std::find(m_Objects.begin(), m_Objects.end(), object);
    if (it != m_Objects.end())
    {
        m_Objects.erase(it);
        if (m_SelectedObject == object)
            m_SelectedObject = nullptr;
        m_Dirty = true;
    }
}

void Scene::RemoveObject(size_t index)
{
    if (index < m_Objects.size())
    {
        if (m_SelectedObject == m_Objects[index])
            m_SelectedObject = nullptr;
        m_Objects.erase(m_Objects.begin() + index);
        m_Dirty = true;
    }
}

void Scene::Clear()
{
    m_Objects.clear();
    m_SelectedObject = nullptr;
    m_Dirty = true;
}

SharedPtr<SceneObject> Scene::GetObject(size_t index) const
{
    if (index < m_Objects.size())
        return m_Objects[index];
    return nullptr;
}

SharedPtr<SceneObject> Scene::FindObject(const std::string& name) const
{
    for (const auto& obj : m_Objects)
        if (obj->GetName() == name)
            return obj;
    return nullptr;
}

void Scene::SelectObject(SharedPtr<SceneObject> object)
{
    if (m_SelectedObject)
        m_SelectedObject->SetSelected(false);
    m_SelectedObject = object;
    if (object)
        object->SetSelected(true);
}

void Scene::DeselectAll()
{
    if (m_SelectedObject)
        m_SelectedObject->SetSelected(false);
    m_SelectedObject = nullptr;
}

} 
