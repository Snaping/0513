#pragma once

#include "graphics/Shape.h"
#include <memory>
#include <vector>

namespace GEngine {

class Document {
public:
    Document();
    ~Document() = default;

    void addShape(std::unique_ptr<Shape> shape);
    void removeShape(Shape* shape);
    void removeShapeAt(size_t index);
    void clear();
    
    size_t getShapeCount() const { return m_shapes.size(); }
    Shape* getShape(size_t index) const;
    
    std::vector<Shape*> getAllShapes() const;
    std::vector<Shape*> getSelectedShapes() const;
    
    void selectShape(Shape* shape);
    void deselectShape(Shape* shape);
    void selectAll();
    void deselectAll();
    
    void deleteSelected();
    
    void translateSelected(double dx, double dy);
    
    Shape* findShapeAt(const Point2D& point, double tolerance = 5.0);
    std::vector<Shape*> findShapesInRect(const Point2D& p1, const Point2D& p2);

private:
    std::vector<std::unique_ptr<Shape>> m_shapes;
    int m_nextId;
};

} 
