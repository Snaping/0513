cmake_minimum_required(VERSION 3.15)
project(Create3D VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(MSVC)
    add_compile_options(/W3)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
else()
    add_compile_options(-Wall -Wextra)
endif()

set(VENDOR_DIR "${CMAKE_SOURCE_DIR}/vendor")

message(STATUS "Checking for local dependencies in: ${VENDOR_DIR}")

set(USE_LOCAL_DEPS OFF)

if(EXISTS "${VENDOR_DIR}/glfw" AND 
   EXISTS "${VENDOR_DIR}/glad" AND 
   EXISTS "${VENDOR_DIR}/imgui" AND 
   EXISTS "${VENDOR_DIR}/glm")
    set(USE_LOCAL_DEPS ON)
    message(STATUS "Using local dependencies from vendor directory")
else()
    message(STATUS "Local dependencies not found, will try to fetch from GitHub")
    message(STATUS "Run download_deps.bat to download dependencies locally")
endif()

if(USE_LOCAL_DEPS)
    set(GLFW_SOURCE_DIR "${VENDOR_DIR}/glfw")
    set(GLAD_SOURCE_DIR "${VENDOR_DIR}/glad")
    set(IMGUI_SOURCE_DIR "${VENDOR_DIR}/imgui")
    set(GLM_SOURCE_DIR "${VENDOR_DIR}/glm")
    
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    
    add_subdirectory(${GLFW_SOURCE_DIR} ${CMAKE_BINARY_DIR}/glfw_build)
    
    add_library(glad STATIC
        ${GLAD_SOURCE_DIR}/src/glad.c
    )
    target_include_directories(glad PUBLIC ${GLAD_SOURCE_DIR}/include)
    
    add_library(imgui STATIC
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC
        ${IMGUI_SOURCE_DIR}
        ${IMGUI_SOURCE_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw)
    
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${GLM_SOURCE_DIR})
    
else()
    include(FetchContent)
    set(FETCHCONTENT_QUIET OFF)
    
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    
    FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(glfw)
    
    FetchContent_Declare(
        glad
        GIT_REPOSITORY https://github.com/Dav1dde/glad.git
        GIT_TAG v0.1.36
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(glad)
    if(NOT glad_POPULATED)
        FetchContent_Populate(glad)
    endif()
    set(GLAD_SOURCE_DIR ${glad_SOURCE_DIR})
    
    FetchContent_Declare(
        imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui.git
        GIT_TAG v1.91.0
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(imgui)
    if(NOT imgui_POPULATED)
        FetchContent_Populate(imgui)
    endif()
    set(IMGUI_SOURCE_DIR ${imgui_SOURCE_DIR})
    
    FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG 1.0.1
        GIT_SHALLOW TRUE
    )
    FetchContent_GetProperties(glm)
    if(NOT glm_POPULATED)
        FetchContent_Populate(glm)
    endif()
    set(GLM_SOURCE_DIR ${glm_SOURCE_DIR})
    
    add_library(glad STATIC
        ${GLAD_SOURCE_DIR}/src/glad.c
    )
    target_include_directories(glad PUBLIC ${GLAD_SOURCE_DIR}/include)
    
    add_library(imgui STATIC
        ${IMGUI_SOURCE_DIR}/imgui.cpp
        ${IMGUI_SOURCE_DIR}/imgui_draw.cpp
        ${IMGUI_SOURCE_DIR}/imgui_tables.cpp
        ${IMGUI_SOURCE_DIR}/imgui_widgets.cpp
        ${IMGUI_SOURCE_DIR}/imgui_demo.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
    )
    target_include_directories(imgui PUBLIC
        ${IMGUI_SOURCE_DIR}
        ${IMGUI_SOURCE_DIR}/backends
    )
    target_link_libraries(imgui PUBLIC glfw)
    
    add_library(glm INTERFACE)
    target_include_directories(glm INTERFACE ${GLM_SOURCE_DIR})
endif()

file(GLOB_RECURSE SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "src/*.h" "src/*.hpp")

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/core
    ${CMAKE_SOURCE_DIR}/src/renderer
    ${CMAKE_SOURCE_DIR}/src/geometry
    ${CMAKE_SOURCE_DIR}/src/sketch
    ${CMAKE_SOURCE_DIR}/src/operation
    ${CMAKE_SOURCE_DIR}/src/ui
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    imgui
    glad
    glm
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE opengl32 gdi32)
endif()

source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Source Files" FILES ${SOURCES})
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX "Header Files" FILES ${HEADERS})

message(STATUS "========================================")
message(STATUS "Create3D Configuration Complete")
message(STATUS "========================================")
message(STATUS "Use Local Deps: ${USE_LOCAL_DEPS}")
message(STATUS "GLFW: ${GLFW_SOURCE_DIR}")
message(STATUS "GLAD: ${GLAD_SOURCE_DIR}")
message(STATUS "ImGui: ${IMGUI_SOURCE_DIR}")
message(STATUS "GLM: ${GLM_SOURCE_DIR}")
message(STATUS "========================================")
