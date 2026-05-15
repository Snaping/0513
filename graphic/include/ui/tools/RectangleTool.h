#pragma once

#include "ui/Tool.h"

namespace GEngine {

class RectangleTool : public Tool {
public:
    RectangleTool();

    ToolType getType() const override { return ToolType::Rectangle; }
    std::string getName() const override { return "Rectangle"; }

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
    enum class RectangleState {
        Idle,
        Drawing
    };

    RectangleState m_state;
    Point2D m_startPoint;
    Point2D m_currentPoint;
};

} 
