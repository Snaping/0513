#pragma once

#include "Types.h"
#include "Window.h"
#include "Renderer.h"
#include "Scene.h"
#include "SketchManager.h"
#include "OperationManager.h"
#include "InputHandler.h"
#include "UIManager.h"

namespace Create3D {

class Application
{
public:
    Application();
    ~Application();

    bool Initialize();
    void Run();
    void Shutdown();

    static Application* GetInstance();

    Window* GetWindow() const { return m_Window.get(); }
    Renderer* GetRenderer() const { return m_Renderer.get(); }
    Scene* GetScene() const { return m_Scene.get(); }
    SketchManager* GetSketchManager() const { return m_SketchManager.get(); }
    OperationManager* GetOperationManager() const { return m_OperationManager.get(); }
    InputHandler* GetInputHandler() const { return m_InputHandler.get(); }
    UIManager* GetUIManager() const { return m_UIManager.get(); }

    f64 GetDeltaTime() const { return m_DeltaTime; }

private:
    void Update();
    void Render();

    static Application* s_Instance;

    UniquePtr<Window> m_Window;
    UniquePtr<Renderer> m_Renderer;
    UniquePtr<Scene> m_Scene;
    UniquePtr<SketchManager> m_SketchManager;
    UniquePtr<OperationManager> m_OperationManager;
    UniquePtr<InputHandler> m_InputHandler;
    UniquePtr<UIManager> m_UIManager;

    f64 m_LastFrameTime = 0.0;
    f64 m_DeltaTime = 0.0;
    bool m_Running = false;
};

} 
