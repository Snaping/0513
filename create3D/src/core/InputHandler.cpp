#include "InputHandler.h"
#include "Application.h"
#include "Window.h"

#include <GLFW/glfw3.h>

namespace Create3D {

bool InputHandler::Initialize()
{
    return true;
}

void InputHandler::Shutdown()
{
}

void InputHandler::Update(f64 deltaTime)
{
    (void)deltaTime;

    for (auto& [key, state] : m_KeyStates)
    {
        m_KeyStatesPrevious[key] = state;
        if (state == KeyState::Pressed)
            state = KeyState::Held;
        else if (state == KeyState::Released)
            state = KeyState::Released;
    }

    for (auto& [button, state] : m_MouseButtonStates)
    {
        m_MouseButtonStatesPrevious[button] = state;
        if (state == KeyState::Pressed)
            state = KeyState::Held;
        else if (state == KeyState::Released)
            state = KeyState::Released;
    }

    m_MouseDelta = m_MousePosition - m_MousePositionPrevious;
    m_MousePositionPrevious = m_MousePosition;
    m_MouseScrollDelta = 0.0f;
}

bool InputHandler::IsKeyPressed(int key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end()) return false;
    return it->second == KeyState::Pressed;
}

bool InputHandler::IsKeyHeld(int key) const
{
    auto it = m_KeyStates.find(key);
    if (it == m_KeyStates.end()) return false;
    return it->second == KeyState::Held || it->second == KeyState::Pressed;
}

bool InputHandler::IsKeyReleased(int key) const
{
    auto it = m_KeyStates.find(key);
    auto prevIt = m_KeyStatesPrevious.find(key);
    if (it == m_KeyStates.end()) return false;
    if (prevIt == m_KeyStatesPrevious.end()) return false;
    return (prevIt->second == KeyState::Held || prevIt->second == KeyState::Pressed)
        && it->second == KeyState::Released;
}

bool InputHandler::IsMouseButtonPressed(MouseButton button) const
{
    int glfwBtn = static_cast<int>(button);
    auto it = m_MouseButtonStates.find(glfwBtn);
    if (it == m_MouseButtonStates.end()) return false;
    return it->second == KeyState::Pressed;
}

bool InputHandler::IsMouseButtonHeld(MouseButton button) const
{
    int glfwBtn = static_cast<int>(button);
    auto it = m_MouseButtonStates.find(glfwBtn);
    if (it == m_MouseButtonStates.end()) return false;
    return it->second == KeyState::Held || it->second == KeyState::Pressed;
}

bool InputHandler::IsMouseButtonReleased(MouseButton button) const
{
    int glfwBtn = static_cast<int>(button);
    auto it = m_MouseButtonStates.find(glfwBtn);
    auto prevIt = m_MouseButtonStatesPrevious.find(glfwBtn);
    if (it == m_MouseButtonStates.end()) return false;
    if (prevIt == m_MouseButtonStatesPrevious.end()) return false;
    return (prevIt->second == KeyState::Held || prevIt->second == KeyState::Pressed)
        && it->second == KeyState::Released;
}

void InputHandler::OnKeyEvent(int key, int scancode, int action, int mods)
{
    (void)scancode;
    (void)mods;

    KeyState state;
    switch (action)
    {
    case GLFW_PRESS: state = KeyState::Pressed; break;
    case GLFW_RELEASE: state = KeyState::Released; break;
    case GLFW_REPEAT: state = KeyState::Repeat; break;
    default: return;
    }
    m_KeyStates[key] = state;
}

void InputHandler::OnMouseButtonEvent(int button, int action, int mods)
{
    (void)mods;

    KeyState state;
    switch (action)
    {
    case GLFW_PRESS: state = KeyState::Pressed; break;
    case GLFW_RELEASE: state = KeyState::Released; break;
    default: return;
    }
    m_MouseButtonStates[button] = state;
}

void InputHandler::OnMouseMoveEvent(f32 x, f32 y)
{
    m_MousePosition = Vec2(x, y);
}

void InputHandler::OnScrollEvent(f32 xOffset, f32 yOffset)
{
    (void)xOffset;
    m_MouseScrollDelta = yOffset;
}

void InputHandler::OnCharEvent(u32 codepoint)
{
    (void)codepoint;
}

void InputHandler::SetCursorMode(bool locked)
{
    m_CursorLocked = locked;
    GLFWwindow* window = Application::GetInstance()->GetWindow()->GetGLFWWindow();
    glfwSetInputMode(window, GLFW_CURSOR, locked ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL);
}

MouseButton InputHandler::GLFWToMouseButton(int button)
{
    switch (button)
    {
    case GLFW_MOUSE_BUTTON_LEFT: return MouseButton::Left;
    case GLFW_MOUSE_BUTTON_RIGHT: return MouseButton::Right;
    case GLFW_MOUSE_BUTTON_MIDDLE: return MouseButton::Middle;
    default: return MouseButton::Left;
    }
}

} 
