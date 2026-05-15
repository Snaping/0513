#pragma once

#include "Types.h"

namespace Create3D {

enum class KeyState
{
    Released,
    Pressed,
    Held,
    Repeat
};

enum class MouseButton
{
    Left,
    Right,
    Middle,
    X1,
    X2
};

class InputHandler
{
public:
    InputHandler() = default;
    ~InputHandler() = default;

    bool Initialize();
    void Shutdown();
    void Update(f64 deltaTime);

    bool IsKeyPressed(int key) const;
    bool IsKeyHeld(int key) const;
    bool IsKeyReleased(int key) const;
    
    bool IsMouseButtonPressed(MouseButton button) const;
    bool IsMouseButtonHeld(MouseButton button) const;
    bool IsMouseButtonReleased(MouseButton button) const;

    Vec2 GetMousePosition() const { return m_MousePosition; }
    Vec2 GetMouseDelta() const { return m_MouseDelta; }
    f32 GetMouseScrollDelta() const { return m_MouseScrollDelta; }

    void OnKeyEvent(int key, int scancode, int action, int mods);
    void OnMouseButtonEvent(int button, int action, int mods);
    void OnMouseMoveEvent(f32 x, f32 y);
    void OnScrollEvent(f32 xOffset, f32 yOffset);
    void OnCharEvent(u32 codepoint);

    void SetCursorMode(bool locked);

private:
    static MouseButton GLFWToMouseButton(int button);

    std::unordered_map<int, KeyState> m_KeyStates;
    std::unordered_map<int, KeyState> m_KeyStatesPrevious;
    std::unordered_map<int, KeyState> m_MouseButtonStates;
    std::unordered_map<int, KeyState> m_MouseButtonStatesPrevious;

    Vec2 m_MousePosition = Vec2(0);
    Vec2 m_MousePositionPrevious = Vec2(0);
    Vec2 m_MouseDelta = Vec2(0);
    f32 m_MouseScrollDelta = 0.0f;
    bool m_CursorLocked = false;
};

} 
