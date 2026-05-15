#include "Application.h"
#include "platform/Win32Window.h"
#include "renderer/GDIRenderer.h"
#include <iostream>
#include <memory>

using namespace GEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    std::cout << "CAD Graphics Engine - Starting..." << std::endl;
    std::cout << "Controls:" << std::endl;
    std::cout << "  1 - Select Tool" << std::endl;
    std::cout << "  2 - Line Tool" << std::endl;
    std::cout << "  3 - Circle Tool" << std::endl;
    std::cout << "  4 - Rectangle Tool" << std::endl;
    std::cout << "  5 - Arc Tool (3 points)" << std::endl;
    std::cout << "  6 - Polyline Tool (Left click to add point, Right click to finish)" << std::endl;
    std::cout << "  Mouse Wheel - Zoom in/out" << std::endl;
    std::cout << "  Ctrl+Z - Reset View" << std::endl;
    std::cout << "  Delete/Backspace - Delete selected shapes" << std::endl;
    std::cout << "  Ctrl+A - Select all" << std::endl;
    std::cout << "  Escape - Exit" << std::endl;
    std::cout << "  Shift + Click/Drag - Add to selection" << std::endl;
    std::cout << "  Shift - Ortho mode when drawing lines/squares" << std::endl;
    std::cout << std::endl;

    auto app = std::make_shared<Application>();
    if (!app->initialize(0, nullptr)) {
        std::cerr << "Failed to initialize application!" << std::endl;
        return -1;
    }

    auto window = std::make_shared<Win32Window>();
    if (!window->create(app->getWindowWidth(), app->getWindowHeight(), "CAD Graphics Engine - Win32 GDI")) {
        std::cerr << "Failed to create window!" << std::endl;
        return -1;
    }
    
    window->setApplication(app);
    
    auto renderer = window->getRenderer();
    if (renderer) {
        renderer->initialize();
        renderer->setViewport(app->getWindowWidth(), app->getWindowHeight());
        app->setRenderer(renderer);
    }
    
    std::cout << "Application initialized successfully!" << std::endl;
    
    int result = window->runMessageLoop();
    
    std::cout << "Shutting down..." << std::endl;
    
    if (renderer) {
        renderer->shutdown();
    }
    
    window->destroy();
    app->shutdown();
    
    return result;
}

int main(int argc, char* argv[]) {
    return WinMain(GetModuleHandle(nullptr), nullptr, nullptr, SW_SHOW);
}
