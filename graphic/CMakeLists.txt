cmake_minimum_required(VERSION 3.15)
project(CADGraphicsEngine CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

option(USE_GLFW "Use GLFW for window management (requires OpenGL)" OFF)
option(USE_GDI "Use Windows GDI for rendering (no external dependencies)" ON)

if(USE_GLFW)
    add_definitions(-DUSE_GLFW)
endif()

if(USE_GDI)
    add_definitions(-DUSE_GDI)
endif()

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)

include_directories(${INCLUDE_DIR})

set(CORE_SOURCES
    ${SOURCE_DIR}/core/Point2D.cpp
    ${SOURCE_DIR}/core/Vector2D.cpp
    ${SOURCE_DIR}/core/Matrix3x3.cpp
    ${SOURCE_DIR}/core/Color.cpp
)

set(GRAPHICS_SOURCES
    ${SOURCE_DIR}/graphics/Shape.cpp
    ${SOURCE_DIR}/graphics/Line.cpp
    ${SOURCE_DIR}/graphics/Circle.cpp
    ${SOURCE_DIR}/graphics/Rectangle.cpp
    ${SOURCE_DIR}/graphics/Arc.cpp
    ${SOURCE_DIR}/graphics/Polyline.cpp
)

set(RENDERER_SOURCES
    ${SOURCE_DIR}/renderer/Renderer.cpp
)

set(UI_SOURCES
    ${SOURCE_DIR}/ui/InputHandler.cpp
    ${SOURCE_DIR}/ui/Tool.cpp
    ${SOURCE_DIR}/ui/ToolManager.cpp
    ${SOURCE_DIR}/ui/tools/SelectTool.cpp
    ${SOURCE_DIR}/ui/tools/LineTool.cpp
    ${SOURCE_DIR}/ui/tools/CircleTool.cpp
    ${SOURCE_DIR}/ui/tools/RectangleTool.cpp
    ${SOURCE_DIR}/ui/tools/ArcTool.cpp
    ${SOURCE_DIR}/ui/tools/PolylineTool.cpp
)

set(MAIN_SOURCES
    ${SOURCE_DIR}/main.cpp
    ${SOURCE_DIR}/Application.cpp
    ${SOURCE_DIR}/Document.cpp
    ${SOURCE_DIR}/Canvas.cpp
)

if(USE_GLFW)
    message(STATUS "Using GLFW backend")
    list(APPEND RENDERER_SOURCES ${SOURCE_DIR}/renderer/SimpleRenderer.cpp)
    set(PLATFORM_SOURCES
        ${SOURCE_DIR}/platform/glfw/GLFWWindow.cpp
    )
elseif(USE_GDI AND WIN32)
    message(STATUS "Using Windows GDI backend")
    list(APPEND RENDERER_SOURCES ${SOURCE_DIR}/renderer/GDIRenderer.cpp)
    set(PLATFORM_SOURCES
        ${SOURCE_DIR}/platform/win32/Win32Window.cpp
    )
else()
    message(FATAL_ERROR "No valid backend selected. Set USE_GDI=ON for Windows or USE_GLFW=ON for cross-platform.")
endif()

add_executable(CADGraphicsEngine
    ${CORE_SOURCES}
    ${GRAPHICS_SOURCES}
    ${RENDERER_SOURCES}
    ${UI_SOURCES}
    ${MAIN_SOURCES}
    ${PLATFORM_SOURCES}
)

if(USE_GLFW)
    message(STATUS "Looking for GLFW...")
    
    find_package(glfw3 CONFIG QUIET)
    
    if(NOT glfw3_FOUND)
        message(STATUS "GLFW not found via CONFIG, trying module mode...")
        find_package(glfw3 QUIET)
    endif()
    
    if(NOT glfw3_FOUND)
        message(STATUS "Trying to find GLFW3 manually...")
        
        if(DEFINED ENV{VCPKG_ROOT})
            set(VCPKG_INCLUDE "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/include")
            set(VCPKG_LIB "$ENV{VCPKG_ROOT}/installed/${VCPKG_TARGET_TRIPLET}/lib")
            
            if(EXISTS "${VCPKG_INCLUDE}/GLFW/glfw3.h")
                message(STATUS "Found GLFW in vcpkg: ${VCPKG_INCLUDE}")
                include_directories(${VCPKG_INCLUDE})
                target_link_directories(CADGraphicsEngine PRIVATE ${VCPKG_LIB})
                target_link_libraries(CADGraphicsEngine PRIVATE glfw3)
                set(glfw3_FOUND TRUE)
            endif()
        endif()
    endif()
    
    if(glfw3_FOUND)
        message(STATUS "GLFW found!")
        if(TARGET glfw)
            target_link_libraries(CADGraphicsEngine PRIVATE glfw)
        elseif(TARGET glfw3)
            target_link_libraries(CADGraphicsEngine PRIVATE glfw3)
        endif()
    else()
        message(WARNING "GLFW not found. You may need to:")
        message(WARNING "  1. Install GLFW via vcpkg: vcpkg install glfw3")
        message(WARNING "  2. Or set VCPKG_ROOT environment variable")
        message(WARNING "  3. Or use GDI backend: cmake .. -DUSE_GDI=ON")
        message(WARNING "")
        message(WARNING "Falling back to system GLFW (if available)...")
        find_package(PkgConfig QUIET)
        if(PkgConfig_FOUND)
            pkg_check_modules(GLFW3 QUIET glfw3)
            if(GLFW3_FOUND)
                include_directories(${GLFW3_INCLUDE_DIRS})
                target_link_libraries(CADGraphicsEngine PRIVATE ${GLFW3_LIBRARIES})
                set(glfw3_FOUND TRUE)
            endif()
        endif()
    endif()
    
    if(WIN32)
        find_package(OpenGL QUIET)
        if(OpenGL_FOUND)
            target_link_libraries(CADGraphicsEngine PRIVATE OpenGL::GL)
        else()
            target_link_libraries(CADGraphicsEngine PRIVATE opengl32)
        endif()
    else()
        find_package(OpenGL REQUIRED)
        target_link_libraries(CADGraphicsEngine PRIVATE OpenGL::GL)
    endif()
endif()

if(WIN32)
    target_compile_definitions(CADGraphicsEngine PRIVATE _CRT_SECURE_NO_WARNINGS)
    target_link_libraries(CADGraphicsEngine PRIVATE gdi32 user32 comctl32)
    
    if(MSVC)
        target_link_options(CADGraphicsEngine PRIVATE "/SUBSYSTEM:WINDOWS" "/ENTRY:mainCRTStartup")
    endif()
endif()

target_include_directories(CADGraphicsEngine PRIVATE
    ${INCLUDE_DIR}
    ${INCLUDE_DIR}/core
    ${INCLUDE_DIR}/graphics
    ${INCLUDE_DIR}/renderer
    ${INCLUDE_DIR}/ui
    ${INCLUDE_DIR}/platform
)

if(MSVC)
    target_compile_options(CADGraphicsEngine PRIVATE /W3)
else()
    target_compile_options(CADGraphicsEngine PRIVATE -Wall -Wextra)
endif()

message(STATUS "")
message(STATUS "========================================")
message(STATUS "CAD Graphics Engine Configuration")
message(STATUS "========================================")
message(STATUS "  USE_GLFW: ${USE_GLFW}")
message(STATUS "  USE_GDI: ${USE_GDI}")
if(USE_GLFW)
    message(STATUS "  GLFW3 found: ${glfw3_FOUND}")
endif()
message(STATUS "========================================")
message(STATUS "")
