#include "Input.h"

Input::Input() : m_mouseX(0), m_mouseY(0) {
    for (int i = 0; i < 256; i++) {
        m_currentKeys[i] = false;
        m_previousKeys[i] = false;
    }
    for (int i = 0; i < 3; i++) {
        m_currentMouse[i] = false;
        m_previousMouse[i] = false;
    }
}

Input::~Input() {}

void Input::Update() {
    for (int i = 0; i < 256; i++) {
        m_previousKeys[i] = m_currentKeys[i];
        m_currentKeys[i] = (GetAsyncKeyState(i) & 0x8000) != 0;
    }

    for (int i = 0; i < 3; i++) {
        m_previousMouse[i] = m_currentMouse[i];
    }
    m_currentMouse[MOUSE_LEFT] = (GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0;
    m_currentMouse[MOUSE_RIGHT] = (GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0;
    m_currentMouse[MOUSE_MIDDLE] = (GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0;

    POINT cursorPos;
    GetCursorPos(&cursorPos);
    ScreenToClient(GetForegroundWindow(), &cursorPos);
    m_mouseX = cursorPos.x;
    m_mouseY = cursorPos.y;
}

bool Input::IsKeyDown(int keyCode) const {
    return m_currentKeys[keyCode];
}

bool Input::IsKeyPressed(int keyCode) const {
    return m_currentKeys[keyCode] && !m_previousKeys[keyCode];
}

bool Input::IsKeyReleased(int keyCode) const {
    return !m_currentKeys[keyCode] && m_previousKeys[keyCode];
}

bool Input::IsMouseButtonDown(int button) const {
    return m_currentMouse[button];
}

bool Input::IsMouseButtonPressed(int button) const {
    return m_currentMouse[button] && !m_previousMouse[button];
}

bool Input::IsMouseButtonReleased(int button) const {
    return !m_currentMouse[button] && m_previousMouse[button];
}

int Input::GetMouseX() const {
    return m_mouseX;
}

int Input::GetMouseY() const {
    return m_mouseY;
}
