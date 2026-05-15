#pragma once

#include "engine/Window.h"
#include "engine/Renderer.h"
#include "engine/Input.h"
#include "game/FightSystem.h"

class GameApp {
public:
    GameApp();
    ~GameApp();

    bool Initialize();
    void Run();
    void Shutdown();

private:
    void ProcessInput();
    void Update(float deltaTime);
    void Render();

    Window* m_window;
    Renderer* m_renderer;
    Input* m_input;
    FightSystem* m_fightSystem;
    bool m_running;
    int m_windowWidth;
    int m_windowHeight;
};
