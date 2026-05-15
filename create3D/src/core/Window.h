#pragma once

#include "Types.h"

struct GLFWwindow;

namespace Create3D {

class Window
{
public:
    Window(const std::string& title, u32 width, u32 height);
    ~Window();

    bool Initialize();
    void Shutdown();

    void SwapBuffers();
    bool ShouldClose() const;

    u32 GetWidth() const { return m_Width; }
    u32 GetHeight() const { return m_Height; }
    f32 GetAspectRatio() const { return static_cast<f32>(m_Width) / static_cast<f32>(m_Height); }
    const std::string& GetTitle() const { return m_Title; }
    GLFWwindow* GetGLFWWindow() const { return m_Window; }

    void SetTitle(const std::string& title);

private:
    static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
    static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void CharCallback(GLFWwindow* window, unsigned int codepoint);

    std::string m_Title;
    u32 m_Width;
    u32 m_Height;
    GLFWwindow* m_Window = nullptr;
};

} 
