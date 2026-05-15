#pragma once

#include "ui/Tool.h"

namespace GEngine {

class LineTool : public Tool {
public:
    LineTool();

    ToolType getType() const override { return ToolType::Line; }
    std::string getName() const override { return "Line"; }

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
    enum class LineState {
        Idle,
        Drawing
    };

    LineState m_state;
    Point2D m_startPoint;
    Point2D m_currentPoint;
};

} 
