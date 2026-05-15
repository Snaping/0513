#pragma once
#include <windows.h>

class Input {
public:
    Input();
    ~Input();

    void Update();

    bool IsKeyDown(int keyCode) const;
    bool IsKeyPressed(int keyCode) const;
    bool IsKeyReleased(int keyCode) const;

    bool IsMouseButtonDown(int button) const;
    bool IsMouseButtonPressed(int button) const;
    bool IsMouseButtonReleased(int button) const;

    int GetMouseX() const;
    int GetMouseY() const;

    static const int KEY_A = 0x41;
    static const int KEY_B = 0x42;
    static const int KEY_D = 0x44;
    static const int KEY_F = 0x46;
    static const int KEY_G = 0x47;
    static const int KEY_H = 0x48;
    static const int KEY_J = 0x4A;
    static const int KEY_K = 0x4B;
    static const int KEY_L = 0x4C;
    static const int KEY_S = 0x53;
    static const int KEY_W = 0x57;
    static const int KEY_SPACE = 0x20;
    static const int KEY_ENTER = 0x0D;
    static const int KEY_ESCAPE = 0x1B;
    static const int KEY_SHIFT = 0x10;
    static const int KEY_CTRL = 0x11;
    static const int KEY_LEFT = 0x25;
    static const int KEY_UP = 0x26;
    static const int KEY_RIGHT = 0x27;
    static const int KEY_DOWN = 0x28;

    static const int MOUSE_LEFT = 0;
    static const int MOUSE_RIGHT = 1;
    static const int MOUSE_MIDDLE = 2;

private:
    bool m_currentKeys[256];
    bool m_previousKeys[256];
    bool m_currentMouse[3];
    bool m_previousMouse[3];
    int m_mouseX;
    int m_mouseY;
};
