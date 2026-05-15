#pragma once

#include "ui/Tool.h"

namespace GEngine {

class CircleTool : public Tool {
public:
    CircleTool();

    ToolType getType() const override { return ToolType::Circle; }
    std::string getName() const override { return "Circle"; }

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
    enum class CircleState {
        Idle,
        SettingCenter,
        SettingRadius
    };

    CircleState m_state;
    Point2D m_center;
    double m_radius;
};

} 
