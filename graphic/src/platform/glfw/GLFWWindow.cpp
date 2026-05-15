#include "platform/Window.h"

#ifdef USE_GLFW
#include <GLFW/glfw3.h>
#endif

namespace GEngine {

Window::Window()
    : m_width(1200)
    , m_height(800)
#ifdef USE_GLFW
    , m_window(nullptr)
#endif
{
}

Window::~Window() {
    destroy();
}

bool Window::create(int width, int height, const std::string& title) {
    m_width = width;
    m_height = height;
    m_title = title;

#ifdef USE_GLFW
    if (!glfwInit()) {
        return false;
    }
    
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
    
    m_window = glfwCreateWindow(m_width, m_height, m_title.c_str(), nullptr, nullptr);
    if (!m_window) {
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);
    glfwSwapInterval(1);
    
    setupCallbacks();
    return true;
#else
    return true;
#endif
}

void Window::destroy() {
#ifdef USE_GLFW
    if (m_window) {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }
    glfwTerminate();
#endif
}

void Window::show() {
#ifdef USE_GLFW
    if (m_window) {
        glfwShowWindow(m_window);
    }
#endif
}

void Window::hide() {
#ifdef USE_GLFW
    if (m_window) {
        glfwHideWindow(m_window);
    }
#endif
}

void Window::setTitle(const std::string& title) {
    m_title = title;
#ifdef USE_GLFW
    if (m_window) {
        glfwSetWindowTitle(m_window, m_title.c_str());
    }
#endif
}

void Window::setSize(int width, int height) {
    m_width = width;
    m_height = height;
#ifdef USE_GLFW
    if (m_window) {
        glfwSetWindowSize(m_window, width, height);
    }
#endif
}

bool Window::shouldClose() const {
#ifdef USE_GLFW
    if (m_window) {
        return glfwWindowShouldClose(m_window) != 0;
    }
#endif
    return false;
}

void Window::pollEvents() {
#ifdef USE_GLFW
    glfwPollEvents();
#endif
}

void Window::swapBuffers() {
#ifdef USE_GLFW
    if (m_window) {
        glfwSwapBuffers(m_window);
    }
#endif
}

void Window::setupCallbacks() {
#ifdef USE_GLFW
    if (!m_window) return;
    
    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetCursorPosCallback(m_window, cursorPosCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    glfwSetKeyCallback(m_window, keyCallback);
#endif
}

#ifdef USE_GLFW
void Window::framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (self && self->m_application) {
        self->m_application->onResize(width, height);
    }
}

void Window::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->m_application) return;
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    int x = static_cast<int>(xpos);
    int y = static_cast<int>(ypos);
    
    int mappedButton = 0;
    if (button == GLFW_MOUSE_BUTTON_LEFT) mappedButton = 0;
    else if (button == GLFW_MOUSE_BUTTON_RIGHT) mappedButton = 1;
    else if (button == GLFW_MOUSE_BUTTON_MIDDLE) mappedButton = 2;
    
    int mappedMods = 0;
    if (mods & GLFW_MOD_SHIFT) mappedMods |= 0x0001;
    if (mods & GLFW_MOD_CONTROL) mappedMods |= 0x0002;
    if (mods & GLFW_MOD_ALT) mappedMods |= 0x0004;
    if (mods & GLFW_MOD_SUPER) mappedMods |= 0x0008;
    
    if (action == GLFW_PRESS) {
        self->m_application->onMouseDown(x, y, mappedButton, mappedMods);
    } else if (action == GLFW_RELEASE) {
        self->m_application->onMouseUp(x, y, mappedButton, mappedMods);
    }
}

void Window::cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->m_application) return;
    
    int x = static_cast<int>(xpos);
    int y = static_cast<int>(ypos);
    
    int mods = 0;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) mods |= 0x0001;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_CONTROL) == GLFW_PRESS) mods |= 0x0002;
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_ALT) == GLFW_PRESS) mods |= 0x0004;
    
    self->m_application->onMouseMove(x, y, mods);
}

void Window::scrollCallback(GLFWwindow* window, double xoffset, double yoffset) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->m_application) return;
    
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    
    int x = static_cast<int>(xpos);
    int y = static_cast<int>(ypos);
    
    self->m_application->onMouseWheel(yoffset, x, y, 0);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Window* self = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if (!self || !self->m_application) return;
    
    int mappedMods = 0;
    if (mods & GLFW_MOD_SHIFT) mappedMods |= 0x0001;
    if (mods & GLFW_MOD_CONTROL) mappedMods |= 0x0002;
    if (mods & GLFW_MOD_ALT) mappedMods |= 0x0004;
    if (mods & GLFW_MOD_SUPER) mappedMods |= 0x0008;
    
    if (action == GLFW_PRESS) {
        self->m_application->onKeyDown(key, mappedMods, false);
    } else if (action == GLFW_RELEASE) {
        self->m_application->onKeyUp(key, mappedMods);
    } else if (action == GLFW_REPEAT) {
        self->m_application->onKeyDown(key, mappedMods, true);
    }
}
#endif

} 
