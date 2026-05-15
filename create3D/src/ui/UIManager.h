#pragma once

#include "Types.h"

namespace Create3D {

class UIManager
{
public:
    UIManager();
    ~UIManager();

    bool Initialize();
    void Shutdown();
    void Render();

    bool IsMouseHoveringUI() const { return m_MouseHoveringUI; }

private:
    void RenderMainMenuBar();
    void RenderToolbar();
    void RenderSketchToolbar();
    void RenderOperationToolbar();
    void RenderSceneHierarchy();
    void RenderInspector();
    void RenderViewport();
    void RenderStatusBar();

    void HandleCameraControls();
    void HandleSketchInput();
    void HandleObjectSelection();

    void CreateNewSketch();
    void CreatePrimitive(const std::string& type);

    void ShowExtrudeDialog();
    void ShowRevolveDialog();
    void ShowSweepDialog();
    void ShowLoftDialog();

    bool m_MouseHoveringUI = false;
    bool m_ShowExtrudeDialog = false;
    bool m_ShowRevolveDialog = false;
    bool m_ShowSweepDialog = false;
    bool m_ShowLoftDialog = false;

    f32 m_ExtrudeDepth = 2.0f;
    Vec3 m_ExtrudeDirection = Vec3(0, 0, 1);
    f32 m_ExtrudeTaper = 0.0f;

    f32 m_RevolveAngle = 360.0f;
    Vec3 m_RevolveAxisOrigin = Vec3(0, 0, 0);
    Vec3 m_RevolveAxisDirection = Vec3(0, 1, 0);

    int m_SweepSegments = 20;
    int m_LoftSegments = 20;
    bool m_LoftClosed = false;

    PlaneType m_SelectedPlane = PlaneType::XY;
};

} 
