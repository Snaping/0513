#pragma once

#include "ui/Tool.h"
#include <vector>

namespace GEngine {

class PolylineTool : public Tool {
public:
    PolylineTool();

    ToolType getType() const override { return ToolType::Polyline; }
    std::string getName() const override { return "Polyline"; }

    void activate() override;
    void deactivate() override;

    void onMouseDown(const Point2D& screenPos, const Point2D& worldPos, 
                    MouseButton button, Modifiers mods) override;
    void onMouseUp(const Point2D& screenPos, const Point2D& worldPos, 
                  MouseButton button, Modifiers mods) override;
    void onMouseMove(const Point2D& screenPos, const Point2D& worldPos, 
                    Modifiers mods) override;
    void onKeyDown(Key key, Modifiers mods) override;

    void renderPreview(Renderer* renderer) override;

private:
    enum class PolylineState {
        Idle,
        Drawing
    };

    PolylineState m_state;
    std::vector<Point2D> m_points;
    Point2D m_currentPoint;
};

} 
