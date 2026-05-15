#include "UIManager.h"
#include "Application.h"
#include "Window.h"
#include "Renderer.h"
#include "Scene.h"
#include "SketchManager.h"
#include "OperationManager.h"
#include "InputHandler.h"
#include "Mesh.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <GLFW/glfw3.h>

namespace Create3D {

UIManager::UIManager() = default;
UIManager::~UIManager() = default;

bool UIManager::Initialize()
{
    return true;
}

void UIManager::Shutdown()
{
}

void UIManager::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    m_MouseHoveringUI = io.WantCaptureMouse;

    ImGui::DockSpaceOverViewport(ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    RenderMainMenuBar();
    RenderToolbar();
    RenderSceneHierarchy();
    RenderInspector();
    RenderViewport();
    RenderStatusBar();

    if (!m_MouseHoveringUI)
    {
        HandleCameraControls();
        HandleSketchInput();
    }

    if (m_ShowExtrudeDialog) ShowExtrudeDialog();
    if (m_ShowRevolveDialog) ShowRevolveDialog();
    if (m_ShowSweepDialog) ShowSweepDialog();
    if (m_ShowLoftDialog) ShowLoftDialog();
}

void UIManager::RenderMainMenuBar()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("New Scene", "Ctrl+N"))
            {
                Application::GetInstance()->GetScene()->Clear();
                Application::GetInstance()->GetSketchManager()->Clear();
            }
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "Alt+F4"))
            {
                glfwSetWindowShouldClose(Application::GetInstance()->GetWindow()->GetGLFWWindow(), GLFW_TRUE);
            }
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
            if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Delete", "Delete")) {}
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View"))
        {
            Renderer* renderer = Application::GetInstance()->GetRenderer();
            if (ImGui::MenuItem("Show Grid", nullptr, renderer->GetSettings().showGrid))
                renderer->GetSettings().showGrid = !renderer->GetSettings().showGrid;
            if (ImGui::MenuItem("Show Axis", nullptr, renderer->GetSettings().showAxis))
                renderer->GetSettings().showAxis = !renderer->GetSettings().showAxis;
            if (ImGui::MenuItem("Wireframe", nullptr, renderer->GetSettings().showWireframe))
                renderer->GetSettings().showWireframe = !renderer->GetSettings().showWireframe;
            if (ImGui::MenuItem("Enable Lighting", nullptr, renderer->GetSettings().enableLighting))
                renderer->GetSettings().enableLighting = !renderer->GetSettings().enableLighting;

            ImGui::Separator();

            if (ImGui::MenuItem("Perspective"))
                renderer->GetMainCamera()->SetPerspectiveMode(true);
            if (ImGui::MenuItem("Orthographic"))
                renderer->GetMainCamera()->SetPerspectiveMode(false);

            ImGui::Separator();
            if (ImGui::MenuItem("Front View", "NumPad 1"))
            {
                renderer->GetMainCamera()->SetYaw(0.0f);
                renderer->GetMainCamera()->SetPitch(0.0f);
            }
            if (ImGui::MenuItem("Top View", "NumPad 7"))
            {
                renderer->GetMainCamera()->SetYaw(-90.0f);
                renderer->GetMainCamera()->SetPitch(90.0f);
            }
            if (ImGui::MenuItem("Right View", "NumPad 3"))
            {
                renderer->GetMainCamera()->SetYaw(90.0f);
                renderer->GetMainCamera()->SetPitch(0.0f);
            }
            if (ImGui::MenuItem("Isometric View", "NumPad 0"))
            {
                renderer->GetMainCamera()->SetYaw(-45.0f);
                renderer->GetMainCamera()->SetPitch(35.0f);
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Help"))
        {
            ImGui::MenuItem("About", nullptr, false, false);
            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void UIManager::RenderToolbar()
{
    if (ImGui::Begin("Toolbar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoDocking))
    {
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 4));

        if (ImGui::Button("Select"))
        {
            Application::GetInstance()->GetSketchManager()->SetCurrentTool(SketchTool::Select);
        }
        ImGui::SameLine();

        if (ImGui::Button("Move")) {}
        ImGui::SameLine();
        if (ImGui::Button("Rotate")) {}
        ImGui::SameLine();
        if (ImGui::Button("Scale")) {}

        ImGui::Separator();

        RenderSketchToolbar();

        ImGui::Separator();

        RenderOperationToolbar();

        ImGui::Separator();

        ImGui::Text("Primitives:");
        if (ImGui::Button("Cube")) CreatePrimitive("Cube");
        ImGui::SameLine();
        if (ImGui::Button("Sphere")) CreatePrimitive("Sphere");
        ImGui::SameLine();
        if (ImGui::Button("Cylinder")) CreatePrimitive("Cylinder");
        ImGui::SameLine();
        if (ImGui::Button("Cone")) CreatePrimitive("Cone");

        ImGui::PopStyleVar();
    }
    ImGui::End();
}

void UIManager::RenderSketchToolbar()
{
    SketchManager* sm = Application::GetInstance()->GetSketchManager();
    SketchTool currentTool = sm->GetCurrentTool();

    ImGui::Text("Sketch:");

    if (ImGui::Button("New Sketch"))
    {
        CreateNewSketch();
    }
    ImGui::SameLine();

    if (ImGui::BeginCombo("Plane",
        m_SelectedPlane == PlaneType::XY ? "XY" :
        m_SelectedPlane == PlaneType::YZ ? "YZ" :
        m_SelectedPlane == PlaneType::XZ ? "XZ" : "Custom"))
    {
        if (ImGui::Selectable("XY", m_SelectedPlane == PlaneType::XY))
            m_SelectedPlane = PlaneType::XY;
        if (ImGui::Selectable("YZ", m_SelectedPlane == PlaneType::YZ))
            m_SelectedPlane = PlaneType::YZ;
        if (ImGui::Selectable("XZ", m_SelectedPlane == PlaneType::XZ))
            m_SelectedPlane = PlaneType::XZ;
        ImGui::EndCombo();
    }

    ImGui::Separator();

    if (ImGui::Button("Select", ImVec2(60, 30)))
    {
        sm->SetCurrentTool(SketchTool::Select);
    }
    ImGui::SameLine();
    if (ImGui::Button("Line", ImVec2(60, 30)))
    {
        sm->SetCurrentTool(SketchTool::Line);
    }
    ImGui::SameLine();
    if (ImGui::Button("Rect", ImVec2(60, 30)))
    {
        sm->SetCurrentTool(SketchTool::Rectangle);
    }
    ImGui::SameLine();
    if (ImGui::Button("Circle", ImVec2(60, 30)))
    {
        sm->SetCurrentTool(SketchTool::Circle);
    }
    ImGui::SameLine();
    if (ImGui::Button("Poly", ImVec2(60, 30)))
    {
        sm->SetCurrentTool(SketchTool::Polygon);
    }

    ImGui::Separator();

    if (sm->IsDrawing())
    {
        if (ImGui::Button("Cancel (ESC)"))
        {
            sm->CancelDrawing();
        }
    }
}

void UIManager::RenderOperationToolbar()
{
    ImGui::Text("3D Ops:");

    if (ImGui::Button("Extrude", ImVec2(70, 30)))
    {
        m_ShowExtrudeDialog = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Revolve", ImVec2(70, 30)))
    {
        m_ShowRevolveDialog = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Sweep", ImVec2(70, 30)))
    {
        m_ShowSweepDialog = true;
    }
    ImGui::SameLine();

    if (ImGui::Button("Loft", ImVec2(70, 30)))
    {
        m_ShowLoftDialog = true;
    }
}

void UIManager::RenderSceneHierarchy()
{
    if (ImGui::Begin("Scene Hierarchy"))
    {
        Scene* scene = Application::GetInstance()->GetScene();
        SketchManager* sm = Application::GetInstance()->GetSketchManager();

        if (ImGui::CollapsingHeader("Sketches", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (size_t i = 0; i < sm->GetSketches().size(); i++)
            {
                auto sketch = sm->GetSketch(i);
                bool isActive = sketch == sm->GetActiveSketch();

                ImGui::PushID(static_cast<int>(i));
                if (ImGui::Selectable(sketch->GetName().c_str(), isActive))
                {
                    sm->SetActiveSketch(sketch);
                }
                ImGui::PopID();
            }
        }

        if (ImGui::CollapsingHeader("3D Objects", ImGuiTreeNodeFlags_DefaultOpen))
        {
            for (size_t i = 0; i < scene->GetObjectCount(); i++)
            {
                auto obj = scene->GetObject(i);
                bool isSelected = obj == scene->GetSelectedObject();

                ImGui::PushID(static_cast<int>(i) + 1000);
                if (ImGui::Selectable(obj->GetName().c_str(), isSelected))
                {
                    scene->SelectObject(obj);
                }
                ImGui::PopID();
            }
        }
    }
    ImGui::End();
}

void UIManager::RenderInspector()
{
    if (ImGui::Begin("Inspector"))
    {
        Scene* scene = Application::GetInstance()->GetScene();
        auto selectedObj = scene->GetSelectedObject();

        if (selectedObj)
        {
            ImGui::Text("Selected Object: %s", selectedObj->GetName().c_str());
            ImGui::Separator();

            Vec3 pos = selectedObj->GetPosition();
            if (ImGui::DragFloat3("Position", &pos.x, 0.1f))
                selectedObj->SetPosition(pos);

            Vec3 rot = selectedObj->GetRotation();
            if (ImGui::DragFloat3("Rotation (rad)", &rot.x, 0.01f))
                selectedObj->SetRotation(rot);

            Vec3 scale = selectedObj->GetScale();
            if (ImGui::DragFloat3("Scale", &scale.x, 0.01f))
                selectedObj->SetScale(scale);

            ImGui::Separator();

            float col[4] = { selectedObj->GetColor().r, selectedObj->GetColor().g,
                           selectedObj->GetColor().b, selectedObj->GetColor().a };
            if (ImGui::ColorEdit4("Color", col))
            {
                selectedObj->SetColor(Color(col[0], col[1], col[2], col[3]));
            }
        }
        else
        {
            ImGui::Text("Select an object to edit");
        }

        ImGui::Separator();
        ImGui::Text("Camera Info:");
        Camera* cam = Application::GetInstance()->GetRenderer()->GetMainCamera();
        Vec3 camPos = cam->GetPosition();
        Vec3 camTarget = cam->GetTarget();
        ImGui::Text("Position: (%.1f, %.1f, %.1f)", camPos.x, camPos.y, camPos.z);
        ImGui::Text("Target: (%.1f, %.1f, %.1f)", camTarget.x, camTarget.y, camTarget.z);
        ImGui::Text("Distance: %.1f", cam->GetDistance());
        ImGui::Text("Yaw: %.1f, Pitch: %.1f", cam->GetYaw(), cam->GetPitch());
    }
    ImGui::End();
}

void UIManager::RenderViewport()
{
    if (ImGui::Begin("Viewport", nullptr,
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse))
    {
        Renderer* renderer = Application::GetInstance()->GetRenderer();
        Scene* scene = Application::GetInstance()->GetScene();

        ImVec2 viewportSize = ImGui::GetContentRegionAvail();
        if (viewportSize.x > 0 && viewportSize.y > 0)
        {
            renderer->GetMainCamera()->SetPerspective(
                45.0f,
                viewportSize.x / viewportSize.y,
                0.1f, 1000.0f
            );
        }

        renderer->BeginFrame();
        renderer->RenderScene(scene);

        for (const auto& obj : scene->GetObjects())
        {
            if (obj->IsVisible() && obj->GetMesh())
            {
                renderer->DrawMesh(obj->GetMesh().get(), obj->GetTransform());
            }
        }

        SketchManager* sm = Application::GetInstance()->GetSketchManager();
        sm->Render();

        auto selectedObj = scene->GetSelectedObject();
        if (selectedObj && selectedObj->GetMesh())
        {
            BoundingBox bbox = selectedObj->GetWorldBoundingBox();
            renderer->DrawAABB(bbox, Color::Yellow());
        }

        Application::GetInstance()->GetRenderer()->EndFrame();
    }
    ImGui::End();
}

void UIManager::RenderStatusBar()
{
    if (ImGui::Begin("Status Bar", nullptr,
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoDocking))
    {
        SketchManager* sm = Application::GetInstance()->GetSketchManager();
        InputHandler* input = Application::GetInstance()->GetInputHandler();
        Vec2 mousePos = input->GetMousePosition();

        ImGui::Text("Mouse: (%.0f, %.0f)  |  ", mousePos.x, mousePos.y);

        SketchTool tool = sm->GetCurrentTool();
        const char* toolName = "None";
        switch (tool)
        {
        case SketchTool::Select: toolName = "Select"; break;
        case SketchTool::Line: toolName = "Line"; break;
        case SketchTool::Rectangle: toolName = "Rectangle"; break;
        case SketchTool::Circle: toolName = "Circle"; break;
        case SketchTool::Polygon: toolName = "Polygon"; break;
        default: toolName = "None"; break;
        }
        ImGui::SameLine();
        ImGui::Text("Tool: %s  |  ", toolName);

        if (sm->GetActiveSketch())
        {
            ImGui::SameLine();
            ImGui::Text("Active Sketch: %s", sm->GetActiveSketch()->GetName().c_str());
        }
    }
    ImGui::End();
}

void UIManager::HandleCameraControls()
{
    Renderer* renderer = Application::GetInstance()->GetRenderer();
    Camera* camera = renderer->GetMainCamera();
    InputHandler* input = Application::GetInstance()->GetInputHandler();

    static bool isOrbiting = false;
    static bool isPanning = false;

    if (input->IsMouseButtonPressed(MouseButton::Middle))
    {
        if (input->IsKeyHeld(GLFW_KEY_LEFT_SHIFT) || input->IsKeyHeld(GLFW_KEY_RIGHT_SHIFT))
            isPanning = true;
        else
            isOrbiting = true;
    }

    if (input->IsMouseButtonReleased(MouseButton::Middle))
    {
        isOrbiting = false;
        isPanning = false;
    }

    if (isOrbiting)
    {
        Vec2 delta = input->GetMouseDelta();
        camera->Orbit(-delta.x * 0.3f, -delta.y * 0.3f);
    }

    if (isPanning)
    {
        Vec2 delta = input->GetMouseDelta();
        f32 panSpeed = camera->GetDistance() * 0.001f;
        camera->Pan(delta.x * panSpeed, delta.y * panSpeed);
    }

    f32 scrollDelta = input->GetMouseScrollDelta();
    if (std::abs(scrollDelta) > 0.001f)
    {
        f32 zoomAmount = scrollDelta * camera->GetDistance() * 0.1f;
        camera->Zoom(zoomAmount);
    }

    f32 moveSpeed = camera->GetDistance() * 0.5f;
    if (input->IsKeyHeld(GLFW_KEY_W))
        camera->Move(moveSpeed * 0.02f, 0, 0);
    if (input->IsKeyHeld(GLFW_KEY_S))
        camera->Move(-moveSpeed * 0.02f, 0, 0);
    if (input->IsKeyHeld(GLFW_KEY_D))
        camera->Move(0, moveSpeed * 0.02f, 0);
    if (input->IsKeyHeld(GLFW_KEY_A))
        camera->Move(0, -moveSpeed * 0.02f, 0);
    if (input->IsKeyHeld(GLFW_KEY_Q))
        camera->Move(0, 0, -moveSpeed * 0.02f);
    if (input->IsKeyHeld(GLFW_KEY_E))
        camera->Move(0, 0, moveSpeed * 0.02f);

    if (input->IsKeyPressed(GLFW_KEY_1))
    {
        camera->SetYaw(0.0f);
        camera->SetPitch(0.0f);
    }
    if (input->IsKeyPressed(GLFW_KEY_3))
    {
        camera->SetYaw(90.0f);
        camera->SetPitch(0.0f);
    }
    if (input->IsKeyPressed(GLFW_KEY_7))
    {
        camera->SetYaw(-90.0f);
        camera->SetPitch(90.0f);
    }
    if (input->IsKeyPressed(GLFW_KEY_5))
    {
        camera->SetPerspectiveMode(!camera->IsPerspective());
    }
    if (input->IsKeyPressed(GLFW_KEY_0))
    {
        camera->SetYaw(-45.0f);
        camera->SetPitch(35.0f);
    }
}

void UIManager::HandleSketchInput()
{
    Application* app = Application::GetInstance();
    SketchManager* sm = app->GetSketchManager();
    InputHandler* input = app->GetInputHandler();
    Renderer* renderer = app->GetRenderer();
    Camera* camera = renderer->GetMainCamera();
    Window* window = app->GetWindow();

    if (input->IsKeyPressed(GLFW_KEY_ESCAPE))
    {
        sm->HandleKeyPress(GLFW_KEY_ESCAPE);
    }
    if (input->IsKeyPressed(GLFW_KEY_DELETE))
    {
        sm->HandleKeyPress(GLFW_KEY_DELETE);
    }

    Vec2 mousePos = input->GetMousePosition();
    Vec2 screenSize(window->GetWidth(), window->GetHeight());

    Ray ray = camera->ScreenToRay(mousePos, screenSize);
    Plane plane = sm->GetCurrentPlane();

    auto t = ray.IntersectPlane(plane);
    Vec3 worldPos(0);
    if (t.has_value())
    {
        worldPos = ray.At(*t);
    }

    sm->HandleMouseMove(worldPos);

    if (sm->GetCurrentTool() != SketchTool::None && sm->GetActiveSketch())
    {
        if (input->IsMouseButtonPressed(MouseButton::Left))
        {
            sm->HandleMouseClick(worldPos);
        }

        if (input->IsMouseButtonReleased(MouseButton::Left))
        {
            sm->HandleMouseRelease(worldPos);
        }
    }
}

void UIManager::HandleObjectSelection()
{
}

void UIManager::CreateNewSketch()
{
    static int sketchCounter = 1;
    std::string name = "Sketch_" + std::to_string(sketchCounter++);
    Application::GetInstance()->GetSketchManager()->CreateSketch(name, m_SelectedPlane);
}

void UIManager::CreatePrimitive(const std::string& type)
{
    Application* app = Application::GetInstance();
    Scene* scene = app->GetScene();

    Camera* cam = app->GetRenderer()->GetMainCamera();
    Vec3 pos = cam->GetTarget();

    scene->CreatePrimitive(type, pos);
}

void UIManager::ShowExtrudeDialog()
{
    ImGui::OpenPopup("Extrude");
    if (ImGui::BeginPopupModal("Extrude", &m_ShowExtrudeDialog))
    {
        SketchManager* sm = Application::GetInstance()->GetSketchManager();
        auto activeSketch = sm->GetActiveSketch();

        if (activeSketch && !activeSketch->GetEntities().empty())
        {
            ImGui::Text("Sketch: %s", activeSketch->GetName().c_str());
            ImGui::Text("Entities: %zu", activeSketch->GetEntities().size());

            ImGui::Separator();

            ImGui::DragFloat("Depth", &m_ExtrudeDepth, 0.1f, 0.01f, 100.0f);
            ImGui::DragFloat3("Direction", &m_ExtrudeDirection.x, 0.01f, -1.0f, 1.0f);
            ImGui::DragFloat("Taper Angle", &m_ExtrudeTaper, 0.5f, -45.0f, 45.0f);

            ImGui::Separator();

            if (ImGui::Button("Extrude", ImVec2(100, 30)))
            {
                ExtrudeParams params;
                params.sketch = activeSketch;
                params.depth = m_ExtrudeDepth;
                params.direction = m_ExtrudeDirection;
                params.taperAngle = m_ExtrudeTaper;

                Application::GetInstance()->GetOperationManager()->Extrude(params);
                m_ShowExtrudeDialog = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 30)))
            {
                m_ShowExtrudeDialog = false;
            }
        }
        else
        {
            ImGui::Text("Please create a sketch first!");
            if (ImGui::Button("Close"))
                m_ShowExtrudeDialog = false;
        }

        ImGui::EndPopup();
    }
}

void UIManager::ShowRevolveDialog()
{
    ImGui::OpenPopup("Revolve");
    if (ImGui::BeginPopupModal("Revolve", &m_ShowRevolveDialog))
    {
        SketchManager* sm = Application::GetInstance()->GetSketchManager();
        auto activeSketch = sm->GetActiveSketch();

        if (activeSketch && !activeSketch->GetEntities().empty())
        {
            ImGui::Text("Sketch: %s", activeSketch->GetName().c_str());
            ImGui::Separator();

            ImGui::DragFloat("Angle", &m_RevolveAngle, 1.0f, 0.0f, 360.0f);
            ImGui::DragFloat3("Axis Origin", &m_RevolveAxisOrigin.x, 0.1f);
            ImGui::DragFloat3("Axis Direction", &m_RevolveAxisDirection.x, 0.01f, -1.0f, 1.0f);

            ImGui::Separator();

            if (ImGui::Button("Revolve", ImVec2(100, 30)))
            {
                RevolveParams params;
                params.sketch = activeSketch;
                params.angle = m_RevolveAngle;
                params.axisOrigin = m_RevolveAxisOrigin;
                params.axisDirection = m_RevolveAxisDirection;

                Application::GetInstance()->GetOperationManager()->Revolve(params);
                m_ShowRevolveDialog = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 30)))
            {
                m_ShowRevolveDialog = false;
            }
        }
        else
        {
            ImGui::Text("Please create a sketch first!");
            if (ImGui::Button("Close"))
                m_ShowRevolveDialog = false;
        }

        ImGui::EndPopup();
    }
}

void UIManager::ShowSweepDialog()
{
    ImGui::OpenPopup("Sweep");
    if (ImGui::BeginPopupModal("Sweep", &m_ShowSweepDialog))
    {
        SketchManager* sm = Application::GetInstance()->GetSketchManager();

        if (sm->GetSketches().size() >= 2)
        {
            ImGui::Text("Need 2 sketches: Profile and Path");
            ImGui::Text("Available sketches: %zu", sm->GetSketches().size());

            ImGui::DragInt("Segments", &m_SweepSegments, 1, 2, 100);

            ImGui::Separator();

            if (ImGui::Button("Sweep", ImVec2(100, 30)))
            {
                SweepParams params;
                if (sm->GetSketches().size() >= 2)
                {
                    params.profile = sm->GetSketch(0);
                    params.path = sm->GetSketch(1);
                    params.segments = m_SweepSegments;

                    Application::GetInstance()->GetOperationManager()->Sweep(params);
                }
                m_ShowSweepDialog = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 30)))
            {
                m_ShowSweepDialog = false;
            }
        }
        else
        {
            ImGui::Text("Need at least 2 sketches (profile + path)!");
            if (ImGui::Button("Close"))
                m_ShowSweepDialog = false;
        }

        ImGui::EndPopup();
    }
}

void UIManager::ShowLoftDialog()
{
    ImGui::OpenPopup("Loft");
    if (ImGui::BeginPopupModal("Loft", &m_ShowLoftDialog))
    {
        SketchManager* sm = Application::GetInstance()->GetSketchManager();

        if (sm->GetSketches().size() >= 2)
        {
            ImGui::Text("Available sketches: %zu", sm->GetSketches().size());

            ImGui::DragInt("Segments", &m_LoftSegments, 1, 2, 100);
            ImGui::Checkbox("Closed", &m_LoftClosed);

            ImGui::Separator();

            if (ImGui::Button("Loft", ImVec2(100, 30)))
            {
                LoftParams params;
                for (const auto& s : sm->GetSketches())
                    params.sections.push_back(s);
                params.segments = m_LoftSegments;
                params.closed = m_LoftClosed;

                Application::GetInstance()->GetOperationManager()->Loft(params);
                m_ShowLoftDialog = false;
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(100, 30)))
            {
                m_ShowLoftDialog = false;
            }
        }
        else
        {
            ImGui::Text("Need at least 2 sketches!");
            if (ImGui::Button("Close"))
                m_ShowLoftDialog = false;
        }

        ImGui::EndPopup();
    }
}

} 
