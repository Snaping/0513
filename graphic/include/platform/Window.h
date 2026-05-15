#pragma once

#include "Application.h"
#include <memory>
#include <string>

#ifdef USE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace GEngine {

class Window {
public:
    Window();
    ~Window();

    bool create(int width, int height, const std::string& title);
    void destroy();
    
    void show();
    void hide();
    
    void setTitle(const std::string& title);
    void setSize(int width, int height);
    
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }
    
    bool shouldClose() const;
    void pollEvents();
    void swapBuffers();
    
    void setApplication(std::shared_ptr<Application> app) { m_application = app; }
    std::shared_ptr<Application> getApplication() const { return m_application; }

#ifdef USE_GLFW
    GLFWwindow* getGLFWWindow() const { return m_window; }
#endif

private:
    void setupCallbacks();
    
    int m_width;
    int m_height;
    std::string m_title;
    
#ifdef USE_GLFW
    GLFWwindow* m_window;
#endif
    
    std::shared_ptr<Application> m_application;
    
#ifdef USE_GLFW
    static void framebufferSizeCallback(GLFWwindow* window, int width, int height);
    static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    static void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    static void scrollCallback(GLFWwindow* window, double xoffset, double yoffset);
    static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
#endif
};

} 
