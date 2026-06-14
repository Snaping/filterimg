cmake_minimum_required(VERSION 3.16)
project(FilterImg VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core Gui Widgets Concurrent REQUIRED)

set(PROJECT_SOURCES
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/filters/filter.h
    src/filters/filter.cpp
    src/threading/taskmanager.h
    src/threading/taskmanager.cpp
    src/widgets/imageviewer.h
    src/widgets/imageviewer.cpp
    src/widgets/progresspanel.h
    src/widgets/progresspanel.cpp
    src/widgets/filtercontrolspanel.h
    src/widgets/filtercontrolspanel.cpp
    src/widgets/compareviewer.h
    src/widgets/compareviewer.cpp
)

add_executable(FilterImg
    ${PROJECT_SOURCES}
)

target_link_libraries(FilterImg PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Concurrent
)

if(WIN32)
    set_target_properties(FilterImg PROPERTIES
        WIN32_EXECUTABLE TRUE
    )
endif()
