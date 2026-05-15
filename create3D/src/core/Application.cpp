#include "Application.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Create3D {

Application* Application::s_Instance = nullptr;

Application::Application()
{
    s_Instance = this;
}

Application::~Application() = default;

Application* Application::GetInstance()
{
    return s_Instance;
}

bool Application::Initialize()
{
    m_Window = MakeUnique<Window>("Create3D - 3D Modeling Tool", 1600, 900);
    if (!m_Window->Initialize())
        return false;

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        return false;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(m_Window->GetGLFWWindow(), true);
    ImGui_ImplOpenGL3_Init("#version 450 core");

    m_Renderer = MakeUnique<Renderer>();
    m_Scene = MakeUnique<Scene>();
    m_SketchManager = MakeUnique<SketchManager>();
    m_OperationManager = MakeUnique<OperationManager>();
    m_InputHandler = MakeUnique<InputHandler>();
    m_UIManager = MakeUnique<UIManager>();

    m_Renderer->Initialize();
    m_InputHandler->Initialize();
    m_UIManager->Initialize();

    m_Scene->Initialize();

    return true;
}

void Application::Run()
{
    m_Running = true;
    m_LastFrameTime = glfwGetTime();

    while (m_Running && !m_Window->ShouldClose())
    {
        f64 currentTime = glfwGetTime();
        m_DeltaTime = currentTime - m_LastFrameTime;
        m_LastFrameTime = currentTime;

        Update();
        Render();
    }
}

void Application::Update()
{
    glfwPollEvents();
    m_InputHandler->Update(m_DeltaTime);
    m_SketchManager->Update(m_DeltaTime);
    m_OperationManager->Update(m_DeltaTime);
    m_Scene->Update(m_DeltaTime);
}

void Application::Render()
{
    m_Renderer->BeginFrame();

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    m_UIManager->Render();

    m_Renderer->RenderScene(m_Scene.get());
    m_SketchManager->Render();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        GLFWwindow* backup_current_context = glfwGetCurrentContext();
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
        glfwMakeContextCurrent(backup_current_context);
    }

    m_Window->SwapBuffers();
}

void Application::Shutdown()
{
    m_UIManager->Shutdown();
    m_InputHandler->Shutdown();
    m_OperationManager->Shutdown();
    m_SketchManager->Shutdown();
    m_Scene->Shutdown();
    m_Renderer->Shutdown();

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    m_Window->Shutdown();
}

} 
