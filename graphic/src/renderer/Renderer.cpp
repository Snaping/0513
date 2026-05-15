#include "Renderer.h"
#include "graphics/Shape.h"

namespace GEngine {

Renderer::Renderer()
    : m_defaultColor(Color::Black)
    , m_defaultLineWidth(1.0)
{
}

void Renderer::drawShape(const Shape* shape) {
    if (shape) {
        shape->draw(this);
    }
}

void Renderer::pushTransform() {
    if (!m_transformStack.empty()) {
        m_transformStack.push_back(m_transformStack.back());
    } else {
        m_transformStack.push_back(Matrix3x3());
    }
}

void Renderer::popTransform() {
    if (!m_transformStack.empty()) {
        m_transformStack.pop_back();
    }
}

void Renderer::translate(double dx, double dy) {
    if (m_transformStack.empty()) {
        m_transformStack.push_back(Matrix3x3());
    }
    m_transformStack.back() = Matrix3x3::translation(dx, dy) * m_transformStack.back();
}

void Renderer::rotate(double angle) {
    if (m_transformStack.empty()) {
        m_transformStack.push_back(Matrix3x3());
    }
    m_transformStack.back() = Matrix3x3::rotation(angle) * m_transformStack.back();
}

void Renderer::scale(double sx, double sy) {
    if (m_transformStack.empty()) {
        m_transformStack.push_back(Matrix3x3());
    }
    m_transformStack.back() = Matrix3x3::scaling(sx, sy) * m_transformStack.back();
}

} 
