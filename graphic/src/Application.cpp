#include "Application.h"
#include "ui/tools/SelectTool.h"
#include "ui/tools/LineTool.h"
#include "ui/tools/CircleTool.h"
#include "ui/tools/RectangleTool.h"
#include "ui/tools/ArcTool.h"
#include "ui/tools/PolylineTool.h"
#include "graphics/Line.h"
#include "graphics/Circle.h"
#include "graphics/Rectangle.h"
#include "graphics/Arc.h"
#include "graphics/Polyline.h"

namespace GEngine {

namespace {
    Modifiers parseModifiers(int mods) {
        Modifiers m;
        m.shift = (mods & 0x0001) != 0;
        m.control = (mods & 0x0002) != 0;
        m.alt = (mods & 0x0004) != 0;
        m.super = (mods & 0x0008) != 0;
        return m;
    }
}

Application::Application()
    : m_windowWidth(1200)
    , m_windowHeight(800)
    , m_windowTitle("CAD Graphics Engine")
    , m_shouldClose(false)
    , m_isInitialized(false)
{
}

Application::~Application() {
    shutdown();
}

bool Application::initialize(int argc, char* argv[]) {
    if (m_isInitialized) return true;
    
    m_document = std::make_shared<Document>();
    m_canvas = std::make_shared<Canvas>();
    m_canvas->setSize(m_windowWidth, m_windowHeight);
    m_canvas->setDocument(m_document);
    
    m_toolManager = std::make_shared<ToolManager>();
    m_toolManager->setCanvas(m_canvas);
    m_toolManager->setDocument(m_document);
    
    initializeTools();
    createDefaultShapes();
    
    m_isInitialized = true;
    return true;
}

void Application::run() {
}

void Application::shutdown() {
    if (!m_isInitialized) return;
    
    m_toolManager.reset();
    m_renderer.reset();
    m_canvas.reset();
    m_document.reset();
    
    m_isInitialized = false;
}

void Application::setWindowTitle(const std::string& title) {
    m_windowTitle = title;
}

void Application::setWindowSize(int width, int height) {
    m_windowWidth = width;
    m_windowHeight = height;
    if (m_canvas) {
        m_canvas->setSize(width, height);
    }
    if (m_renderer) {
        m_renderer->setViewport(width, height);
    }
}

void Application::setActiveTool(ToolType type) {
    if (m_toolManager) {
        m_toolManager->setActiveTool(type);
    }
}

ToolType Application::getActiveToolType() const {
    if (m_toolManager) {
        return m_toolManager->getActiveToolType();
    }
    return ToolType::Select;
}

void Application::onResize(int width, int height) {
    setWindowSize(width, height);
}

void Application::onRender() {
    if (!m_renderer || !m_canvas) return;
    
    m_renderer->beginFrame();
    m_canvas->render(m_renderer.get());
    m_toolManager->renderPreview(m_renderer.get());
    m_renderer->endFrame();
}

void Application::onUpdate() {
    if (m_toolManager) {
        m_toolManager->update();
    }
}

void Application::onMouseDown(int x, int y, int button, int mods) {
    if (m_toolManager) {
        m_toolManager->onMouseDown(Point2D(x, y), static_cast<MouseButton>(button), parseModifiers(mods));
    }
}

void Application::onMouseUp(int x, int y, int button, int mods) {
    if (m_toolManager) {
        m_toolManager->onMouseUp(Point2D(x, y), static_cast<MouseButton>(button), parseModifiers(mods));
    }
}

void Application::onMouseMove(int x, int y, int mods) {
    if (m_toolManager) {
        m_toolManager->onMouseMove(Point2D(x, y), parseModifiers(mods));
    }
}

void Application::onMouseWheel(double delta, int x, int y, int mods) {
    if (m_canvas) {
        double zoomFactor = (delta > 0) ? 1.1 : 0.9;
        m_canvas->zoom(zoomFactor, Point2D(x, y));
    }
    if (m_toolManager) {
        m_toolManager->onMouseWheel(delta, Point2D(x, y), parseModifiers(mods));
    }
}

void Application::onKeyDown(int key, int mods, bool isRepeat) {
    if (key == static_cast<int>(Key::Num1)) {
        setActiveTool(ToolType::Select);
    } else if (key == static_cast<int>(Key::Num2)) {
        setActiveTool(ToolType::Line);
    } else if (key == static_cast<int>(Key::Num3)) {
        setActiveTool(ToolType::Circle);
    } else if (key == static_cast<int>(Key::Num4)) {
        setActiveTool(ToolType::Rectangle);
    } else if (key == static_cast<int>(Key::Num5)) {
        setActiveTool(ToolType::Arc);
    } else if (key == static_cast<int>(Key::Num6)) {
        setActiveTool(ToolType::Polyline);
    } else if (key == static_cast<int>(Key::Z) && (mods & 0x0002)) {
        if (m_canvas) m_canvas->resetView();
    } else if (key == static_cast<int>(Key::Escape)) {
        requestClose();
    }
    
    if (m_toolManager) {
        m_toolManager->onKeyDown(static_cast<Key>(key), parseModifiers(mods));
    }
}

void Application::onKeyUp(int key, int mods) {
    if (m_toolManager) {
        m_toolManager->onKeyUp(static_cast<Key>(key), parseModifiers(mods));
    }
}

void Application::initializeTools() {
    if (!m_toolManager) return;
    
    m_toolManager->registerTool(std::make_unique<SelectTool>());
    m_toolManager->registerTool(std::make_unique<LineTool>());
    m_toolManager->registerTool(std::make_unique<CircleTool>());
    m_toolManager->registerTool(std::make_unique<RectangleTool>());
    m_toolManager->registerTool(std::make_unique<ArcTool>());
    m_toolManager->registerTool(std::make_unique<PolylineTool>());
    
    m_toolManager->setActiveTool(ToolType::Select);
}

void Application::createDefaultShapes() {
    if (!m_document) return;
    
    auto line = std::make_unique<Line>(Point2D(100, 100), Point2D(300, 200));
    line->setColor(Color::Blue);
    line->setLineWidth(2.0);
    m_document->addShape(std::move(line));
    
    auto circle = std::make_unique<Circle>(Point2D(500, 200), 80.0);
    circle->setColor(Color::Green);
    circle->setLineWidth(2.0);
    m_document->addShape(std::move(circle));
    
    auto rect = std::make_unique<Rectangle>(Point2D(100, 300), Point2D(300, 450));
    rect->setColor(Color::Red);
    rect->setLineWidth(2.0);
    m_document->addShape(std::move(rect));
    
    auto arc = std::make_unique<Arc>(Point2D(600, 400), 100.0, 0.0, 3.14159, false);
    arc->setColor(Color::Magenta);
    arc->setLineWidth(2.0);
    m_document->addShape(std::move(arc));
    
    std::vector<Point2D> polyPoints = {
        Point2D(200, 550),
        Point2D(300, 500),
        Point2D(400, 550),
        Point2D(350, 650),
        Point2D(250, 650)
    };
    auto polyline = std::make_unique<Polyline>(polyPoints);
    polyline->setColor(Color::Cyan);
    polyline->setLineWidth(2.0);
    polyline->setClosed(true);
    m_document->addShape(std::move(polyline));
}

} 
