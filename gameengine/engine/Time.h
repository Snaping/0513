#pragma once
#include <windows.h>

class Time {
public:
    Time();

    void Update();

    float GetDeltaTime() const;
    float GetTotalTime() const;

    float GetFPS() const;

private:
    LARGE_INTEGER m_frequency;
    LARGE_INTEGER m_previousTime;
    LARGE_INTEGER m_currentTime;

    float m_deltaTime;
    float m_totalTime;

    float m_fpsTimer;
    int m_frameCount;
    float m_fps;
};
