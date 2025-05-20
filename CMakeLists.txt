cmake_minimum_required(VERSION 3.10)
project(USBMonitor-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

option(USE_QT "Build Qt GUI example(s) instead of console" OFF)

if(USE_QT)
    message(STATUS "Building Qt GUI example...")

    set(CMAKE_AUTOMOC ON)
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

    file(GLOB QT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/qt-gui/*.cpp")
    foreach(SRC ${QT_SOURCES})
        get_filename_component(NAME ${SRC} NAME_WE)
        add_executable(example-qt-${NAME} ${SRC})
        target_link_libraries(example-qt-${NAME} Qt5::Core Qt5::Widgets)
    endforeach()

else()
    message(STATUS "Building Console example...")

    file(GLOB CONSOLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/console/*.cpp")
    foreach(SRC ${CONSOLE_SOURCES})
        get_filename_component(NAME ${SRC} NAME_WE)
        add_executable(example-console-${NAME} ${SRC})
    endforeach()

endif()

