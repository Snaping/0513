#include "Document.h"
#include <algorithm>

namespace GEngine {

Document::Document()
    : m_nextId(0)
{
}

void Document::addShape(std::unique_ptr<Shape> shape) {
    if (shape) {
        shape->setId(m_nextId++);
        m_shapes.push_back(std::move(shape));
    }
}

void Document::removeShape(Shape* shape) {
    auto it = std::find_if(m_shapes.begin(), m_shapes.end(),
        [shape](const std::unique_ptr<Shape>& s) { return s.get() == shape; });
    if (it != m_shapes.end()) {
        m_shapes.erase(it);
    }
}

void Document::removeShapeAt(size_t index) {
    if (index < m_shapes.size()) {
        m_shapes.erase(m_shapes.begin() + index);
    }
}

void Document::clear() {
    m_shapes.clear();
    m_nextId = 0;
}

Shape* Document::getShape(size_t index) const {
    if (index < m_shapes.size()) {
        return m_shapes[index].get();
    }
    return nullptr;
}

std::vector<Shape*> Document::getAllShapes() const {
    std::vector<Shape*> shapes;
    shapes.reserve(m_shapes.size());
    for (const auto& shape : m_shapes) {
        shapes.push_back(shape.get());
    }
    return shapes;
}

std::vector<Shape*> Document::getSelectedShapes() const {
    std::vector<Shape*> selected;
    for (const auto& shape : m_shapes) {
        if (shape->isSelected()) {
            selected.push_back(shape.get());
        }
    }
    return selected;
}

void Document::selectShape(Shape* shape) {
    if (shape) {
        shape->setSelected(true);
    }
}

void Document::deselectShape(Shape* shape) {
    if (shape) {
        shape->setSelected(false);
    }
}

void Document::selectAll() {
    for (auto& shape : m_shapes) {
        shape->setSelected(true);
    }
}

void Document::deselectAll() {
    for (auto& shape : m_shapes) {
        shape->setSelected(false);
    }
}

void Document::deleteSelected() {
    m_shapes.erase(
        std::remove_if(m_shapes.begin(), m_shapes.end(),
            [](const std::unique_ptr<Shape>& shape) { return shape->isSelected(); }),
        m_shapes.end()
    );
}

void Document::translateSelected(double dx, double dy) {
    for (auto& shape : m_shapes) {
        if (shape->isSelected()) {
            shape->translate(dx, dy);
        }
    }
}

Shape* Document::findShapeAt(const Point2D& point, double tolerance) {
    for (auto it = m_shapes.rbegin(); it != m_shapes.rend(); ++it) {
        if ((*it)->isVisible() && (*it)->contains(point, tolerance)) {
            return it->get();
        }
    }
    return nullptr;
}

std::vector<Shape*> Document::findShapesInRect(const Point2D& p1, const Point2D& p2) {
    std::vector<Shape*> found;
    
    Point2D minP(
        std::min(p1.x, p2.x),
        std::min(p1.y, p2.y)
    );
    Point2D maxP(
        std::max(p1.x, p2.x),
        std::max(p1.y, p2.y)
    );
    
    for (const auto& shape : m_shapes) {
        if (!shape->isVisible()) continue;
        
        Point2D center = shape->getCenter();
        double radius = shape->getBoundingRadius();
        
        if (center.x - radius >= minP.x && center.x + radius <= maxP.x &&
            center.y - radius >= minP.y && center.y + radius <= maxP.y) {
            found.push_back(shape.get());
        }
    }
    
    return found;
}

} 
