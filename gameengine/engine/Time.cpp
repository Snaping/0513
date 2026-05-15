#include "Time.h"

Time::Time()
    : m_deltaTime(0.0f), m_totalTime(0.0f), m_fpsTimer(0.0f), m_frameCount(0), m_fps(0.0f) {
    QueryPerformanceFrequency(&m_frequency);
    QueryPerformanceCounter(&m_previousTime);
    m_currentTime = m_previousTime;
}

void Time::Update() {
    m_previousTime = m_currentTime;
    QueryPerformanceCounter(&m_currentTime);

    m_deltaTime = (float)(m_currentTime.QuadPart - m_previousTime.QuadPart) / m_frequency.QuadPart;
    m_totalTime += m_deltaTime;

    m_fpsTimer += m_deltaTime;
    m_frameCount++;

    if (m_fpsTimer >= 1.0f) {
        m_fps = (float)m_frameCount / m_fpsTimer;
        m_fpsTimer = 0.0f;
        m_frameCount = 0;
    }
}

float Time::GetDeltaTime() const {
    return m_deltaTime;
}

float Time::GetTotalTime() const {
    return m_totalTime;
}

float Time::GetFPS() const {
    return m_fps;
}
