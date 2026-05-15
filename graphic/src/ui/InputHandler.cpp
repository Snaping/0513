#include "InputHandler.h"
#include <algorithm>

namespace GEngine {

InputHandler::InputHandler()
    : m_mousePosition(Point2D(0, 0))
    , m_mouseDelta(Point2D(0, 0))
    , m_lastMousePosition(Point2D(0, 0))
{
    std::fill(m_mouseButtons, m_mouseButtons + 3, false);
    std::fill(m_keys, m_keys + 512, false);
}

bool InputHandler::isMouseButtonDown(MouseButton button) const {
    int btn = static_cast<int>(button);
    if (btn >= 0 && btn < 3) {
        return m_mouseButtons[btn];
    }
    return false;
}

bool InputHandler::isKeyDown(Key key) const {
    int k = static_cast<int>(key);
    if (k >= 0 && k < 512) {
        return m_keys[k];
    }
    return false;
}

Modifiers InputHandler::getCurrentModifiers() const {
    return m_currentModifiers;
}

void InputHandler::handleMouseDown(int x, int y, int button, int mods) {
    MouseButton mb = parseMouseButton(button);
    int btn = static_cast<int>(mb);
    if (btn >= 0 && btn < 3) {
        m_mouseButtons[btn] = true;
    }
    
    m_currentModifiers = parseModifiers(mods);
    m_mousePosition = Point2D(x, y);
    
    if (m_eventHandler) {
        m_eventHandler->onMouseDown(m_mousePosition, mb, m_currentModifiers);
    }
}

void InputHandler::handleMouseUp(int x, int y, int button, int mods) {
    MouseButton mb = parseMouseButton(button);
    int btn = static_cast<int>(mb);
    if (btn >= 0 && btn < 3) {
        m_mouseButtons[btn] = false;
    }
    
    m_currentModifiers = parseModifiers(mods);
    m_mousePosition = Point2D(x, y);
    
    if (m_eventHandler) {
        m_eventHandler->onMouseUp(m_mousePosition, mb, m_currentModifiers);
    }
}

void InputHandler::handleMouseMove(int x, int y, int mods) {
    Point2D newPos(x, y);
    m_mouseDelta = Point2D(newPos.x - m_lastMousePosition.x, newPos.y - m_lastMousePosition.y);
    m_mousePosition = newPos;
    m_lastMousePosition = newPos;
    
    m_currentModifiers = parseModifiers(mods);
    
    if (m_eventHandler) {
        m_eventHandler->onMouseMove(m_mousePosition, m_currentModifiers);
    }
}

void InputHandler::handleMouseWheel(double delta, int x, int y, int mods) {
    m_currentModifiers = parseModifiers(mods);
    
    if (m_eventHandler) {
        m_eventHandler->onMouseWheel(delta, Point2D(x, y), m_currentModifiers);
    }
}

void InputHandler::handleKeyDown(int key, int mods, bool isRepeat) {
    Key k = parseKey(key);
    int keyCode = static_cast<int>(k);
    if (keyCode >= 0 && keyCode < 512) {
        m_keys[keyCode] = true;
    }
    
    m_currentModifiers = parseModifiers(mods);
    
    if (m_eventHandler) {
        if (isRepeat) {
            m_eventHandler->onKeyRepeat(k, m_currentModifiers);
        } else {
            m_eventHandler->onKeyDown(k, m_currentModifiers);
        }
    }
}

void InputHandler::handleKeyUp(int key, int mods) {
    Key k = parseKey(key);
    int keyCode = static_cast<int>(k);
    if (keyCode >= 0 && keyCode < 512) {
        m_keys[keyCode] = false;
    }
    
    m_currentModifiers = parseModifiers(mods);
    
    if (m_eventHandler) {
        m_eventHandler->onKeyUp(k, m_currentModifiers);
    }
}

Modifiers InputHandler::parseModifiers(int mods) {
    Modifiers m;
    m.shift = (mods & 0x0001) != 0;
    m.control = (mods & 0x0002) != 0;
    m.alt = (mods & 0x0004) != 0;
    m.super = (mods & 0x0008) != 0;
    return m;
}

MouseButton InputHandler::parseMouseButton(int button) {
    switch (button) {
        case 0: return MouseButton::Left;
        case 1: return MouseButton::Right;
        case 2: return MouseButton::Middle;
        default: return MouseButton::None;
    }
}

Key InputHandler::parseKey(int key) {
    if (key >= 0 && key < 512) {
        return static_cast<Key>(key);
    }
    return Key::Unknown;
}

} 
