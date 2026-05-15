#pragma once

#include "ui/Tool.h"

namespace GEngine {

class SelectTool : public Tool {
public:
    SelectTool();

    ToolType getType() const override { return ToolType::Select; }
    std::string getName() const override { return "Select"; }

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
    enum class SelectionMode {
        None,
        Click,
        DragSelect,
        Move
    };

    SelectionMode m_selectionMode;
    Point2D m_dragStartScreen;
    Point2D m_dragStartWorld;
    Point2D m_dragCurrentScreen;
    Point2D m_dragCurrentWorld;
    std::vector<Shape*> m_initialSelection;
    Point2D m_lastMovePos;
    bool m_moved;
};

} 
