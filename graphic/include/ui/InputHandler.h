#pragma once

#include "core/Point2D.h"
#include <memory>
#include <functional>
#include <vector>

namespace GEngine {

class InputHandler;

enum class MouseButton {
    Left,
    Right,
    Middle,
    None
};

enum class Key {
    Unknown = -1,
    Space = 32,
    A = 65, D = 68, E = 69, F = 70, G = 71, H = 72,
    I = 73, J = 74, K = 75, L = 76, M = 77, N = 78,
    O = 79, P = 80, Q = 81, R = 82, S = 83, T = 84,
    U = 85, V = 86, W = 87, X = 88, Y = 89, Z = 90,
    Num0 = 48, Num1 = 49, Num2 = 50, Num3 = 51, Num4 = 52,
    Num5 = 53, Num6 = 54, Num7 = 55, Num8 = 56, Num9 = 57,
    Escape = 256, Enter = 257, Tab = 258, Backspace = 259,
    Delete = 261, ArrowRight = 262, ArrowLeft = 263,
    ArrowDown = 264, ArrowUp = 265,
    LeftShift = 340, LeftControl = 341, LeftAlt = 342,
    RightShift = 344, RightControl = 345, RightAlt = 346
};

enum class Modifier {
    None = 0,
    Shift = 0x0001,
    Control = 0x0002,
    Alt = 0x0004,
    Super = 0x0008
};

struct Modifiers {
    bool shift;
    bool control;
    bool alt;
    bool super;
    
    Modifiers() : shift(false), control(false), alt(false), super(false) {}
};

class InputEvents {
public:
    virtual ~InputEvents() = default;
    
    virtual void onMouseDown(const Point2D& position, MouseButton button, Modifiers mods) {}
    virtual void onMouseUp(const Point2D& position, MouseButton button, Modifiers mods) {}
    virtual void onMouseMove(const Point2D& position, Modifiers mods) {}
    virtual void onMouseWheel(double delta, const Point2D& position, Modifiers mods) {}
    
    virtual void onKeyDown(Key key, Modifiers mods) {}
    virtual void onKeyUp(Key key, Modifiers mods) {}
    virtual void onKeyRepeat(Key key, Modifiers mods) {}
};

class InputHandler {
public:
    InputHandler();
    ~InputHandler() = default;

    void setEventHandler(std::shared_ptr<InputEvents> handler) { m_eventHandler = handler; }
    std::shared_ptr<InputEvents> getEventHandler() const { return m_eventHandler; }

    Point2D getMousePosition() const { return m_mousePosition; }
    Point2D getMouseDelta() const { return m_mouseDelta; }
    
    bool isMouseButtonDown(MouseButton button) const;
    bool isKeyDown(Key key) const;
    
    Modifiers getCurrentModifiers() const;

    void handleMouseDown(int x, int y, int button, int mods);
    void handleMouseUp(int x, int y, int button, int mods);
    void handleMouseMove(int x, int y, int mods);
    void handleMouseWheel(double delta, int x, int y, int mods);
    void handleKeyDown(int key, int mods, bool isRepeat);
    void handleKeyUp(int key, int mods);

private:
    Point2D m_mousePosition;
    Point2D m_mouseDelta;
    Point2D m_lastMousePosition;
    
    bool m_mouseButtons[3];
    bool m_keys[512];
    Modifiers m_currentModifiers;
    
    std::shared_ptr<InputEvents> m_eventHandler;
    
    Modifiers parseModifiers(int mods);
    MouseButton parseMouseButton(int button);
    Key parseKey(int key);
};

} 
