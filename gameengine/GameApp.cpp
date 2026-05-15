#include "GameApp.h"

GameApp::GameApp()
    : m_window(nullptr),
      m_renderer(nullptr),
      m_input(nullptr),
      m_fightSystem(nullptr),
      m_running(false),
      m_windowWidth(1280),
      m_windowHeight(720)
{
}

GameApp::~GameApp() {
    Shutdown();
}

bool GameApp::Initialize() {
    m_window = new Window();
    if (!m_window->Create(L"Fighter Engine", m_windowWidth, m_windowHeight)) {
        return false;
    }

    m_renderer = new Renderer();
    if (!m_renderer->Initialize(m_window->GetHandle())) {
        return false;
    }

    m_input = new Input();

    m_fightSystem = new FightSystem();
    m_fightSystem->Initialize(m_windowWidth, m_windowHeight);
    m_fightSystem->GetPlayer1()->SetInput(m_input);

    m_running = true;
    return true;
}

void GameApp::Run() {
    LARGE_INTEGER frequency;
    LARGE_INTEGER previousTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&previousTime);

    const float targetFPS = 60.0f;
    const float targetFrameTime = 1.0f / targetFPS;

    while (m_running) {
        LARGE_INTEGER currentTime;
        QueryPerformanceCounter(&currentTime);

        float deltaTime = (float)(currentTime.QuadPart - previousTime.QuadPart) / (float)frequency.QuadPart;

        if (deltaTime >= targetFrameTime) {
            previousTime = currentTime;

            const float maxDeltaTime = 0.1f;
            if (deltaTime > maxDeltaTime) {
                deltaTime = maxDeltaTime;
            }

            ProcessInput();
            Update(deltaTime);
            Render();
        } else {
            Sleep(1);
        }
    }
}

void GameApp::Shutdown() {
    delete m_fightSystem;
    delete m_input;
    delete m_renderer;
    delete m_window;
}

void GameApp::ProcessInput() {
    MSG msg;
    while (PeekMessageW(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_running = false;
        }
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }

    m_input->Update();

    if (m_input->IsKeyPressed(Input::KEY_ESCAPE)) {
        m_running = false;
    }
}

void GameApp::Update(float deltaTime) {
    m_fightSystem->Update(deltaTime);
}

void GameApp::Render() {
    m_renderer->BeginFrame();

    m_fightSystem->Draw(m_renderer);

    m_renderer->EndFrame();
}
