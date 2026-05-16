cmake_minimum_required(VERSION 3.16)
project(ImageEditor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets PrintSupport)

set(SOURCES
    ImageEditor/main.cpp
    ImageEditor/MainWindow.cpp
    ImageEditor/ImageCanvas.cpp
    ImageEditor/ImageProcessor.cpp
    ImageEditor/AnnotationTool.cpp
    ImageEditor/FilterDialog.cpp
    ImageEditor/PdfExporter.cpp
    ImageEditor/AIFeatures.cpp
)

set(HEADERS
    ImageEditor/MainWindow.h
    ImageEditor/ImageCanvas.h
    ImageEditor/ImageProcessor.h
    ImageEditor/AnnotationTool.h
    ImageEditor/FilterDialog.h
    ImageEditor/PdfExporter.h
    ImageEditor/AIFeatures.h
)

add_executable(${PROJECT_NAME} WIN32
    ${SOURCES}
    ${HEADERS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt5::Core
    Qt5::Gui
    Qt5::Widgets
    Qt5::PrintSupport
)

if(WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
