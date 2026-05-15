#pragma once

#include "core/Point2D.h"
#include "core/Matrix3x3.h"
#include "renderer/Renderer.h"
#include "Document.h"
#include <memory>

namespace GEngine {

class Canvas {
public:
    Canvas();
    ~Canvas() = default;

    void setSize(int width, int height);
    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

    Point2D screenToWorld(const Point2D& screenPoint) const;
    Point2D worldToScreen(const Point2D& worldPoint) const;

    void pan(double dx, double dy);
    void zoom(double factor, const Point2D& center);
    void zoomIn();
    void zoomOut();
    void resetView();

    void setDocument(std::shared_ptr<Document> doc) { m_document = doc; }
    std::shared_ptr<Document> getDocument() const { return m_document; }

    void render(Renderer* renderer);

    double getZoom() const { return m_zoom; }
    Point2D getPan() const { return m_pan; }
    
    Matrix3x3 getViewTransform() const;
    Matrix3x3 getInverseViewTransform() const;

private:
    int m_width;
    int m_height;
    double m_zoom;
    Point2D m_pan;
    std::shared_ptr<Document> m_document;
};

} 
