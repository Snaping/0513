#include "Window.h"
#include "Application.h"
#include "InputHandler.h"

#include <GLFW/glfw3.h>

namespace Create3D {

Window::Window(const std::string& title, u32 width, u32 height)
    : m_Title(title), m_Width(width), m_Height(height)
{
}

Window::~Window()
{
    Shutdown();
}

bool Window::Initialize()
{
    if (!glfwInit())
        return false;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    m_Window = glfwCreateWindow(
        static_cast<int>(m_Width),
        static_cast<int>(m_Height),
        m_Title.c_str(),
        nullptr,
        nullptr
    );

    if (!m_Window)
    {
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_Window);
    glfwSetWindowUserPointer(m_Window, this);
    glfwSwapInterval(1);

    glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
    glfwSetKeyCallback(m_Window, KeyCallback);
    glfwSetMouseButtonCallback(m_Window, MouseButtonCallback);
    glfwSetCursorPosCallback(m_Window, CursorPosCallback);
    glfwSetScrollCallback(m_Window, ScrollCallback);
    glfwSetCharCallback(m_Window, CharCallback);

    int frameWidth, frameHeight;
    glfwGetFramebufferSize(m_Window, &frameWidth, &frameHeight);
    m_Width = static_cast<u32>(frameWidth);
    m_Height = static_cast<u32>(frameHeight);

    return true;
}

void Window::Shutdown()
{
    if (m_Window)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }
    glfwTerminate();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_Window);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_Window);
}

void Window::SetTitle(const std::string& title)
{
    m_Title = title;
    glfwSetWindowTitle(m_Window, title.c_str());
}

void Window::FramebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    Window* win = static_cast<Window*>(glfwGetWindowUserPointer(window));
    win->m_Width = static_cast<u32>(width);
    win->m_Height = static_cast<u32>(height);
    glViewport(0, 0, width, height);
}

void Window::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    Application* app = Application::GetInstance();
    if (app && app->GetInputHandler())
    {
        app->GetInputHandler()->OnKeyEvent(key, scancode, action, mods);
    }
}

void Window::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    Application* app = Application::GetInstance();
    if (app && app->GetInputHandler())
    {
        app->GetInputHandler()->OnMouseButtonEvent(button, action, mods);
    }
}

void Window::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    Application* app = Application::GetInstance();
    if (app && app->GetInputHandler())
    {
        app->GetInputHandler()->OnMouseMoveEvent(static_cast<f32>(xpos), static_cast<f32>(ypos));
    }
}

void Window::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
    Application* app = Application::GetInstance();
    if (app && app->GetInputHandler())
    {
        app->GetInputHandler()->OnScrollEvent(static_cast<f32>(xoffset), static_cast<f32>(yoffset));
    }
}

void Window::CharCallback(GLFWwindow* window, unsigned int codepoint)
{
    Application* app = Application::GetInstance();
    if (app && app->GetInputHandler())
    {
        app->GetInputHandler()->OnCharEvent(codepoint);
    }
}

} 
