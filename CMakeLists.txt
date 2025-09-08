cmake_minimum_required(VERSION 3.16)  # Qt6 推荐 3.16+

project(USBMonitor-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")

# 尝试找 Qt6，找不到再找 Qt5
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
    message(STATUS "Using Qt6")
    set(QT_VERSION 6)
    set(QT_CORE Qt6::Core)
    set(QT_WIDGETS Qt6::Widgets)
else()
    find_package(Qt5 COMPONENTS Core Widgets REQUIRED)
    message(STATUS "Using Qt5")
    set(QT_VERSION 5)
    set(QT_CORE Qt5::Core)
    set(QT_WIDGETS Qt5::Widgets)
endif()

foreach(EXAMPLE_SOURCE IN ITEMS ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    set(EXAMPLE_EXECUTABLE example-${EXAMPLE_NAME})

    add_executable(${EXAMPLE_EXECUTABLE} ${EXAMPLE_SOURCE})

    target_sources(${EXAMPLE_EXECUTABLE} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/mac/macUSBMonitor.cpp)

    target_link_libraries(${EXAMPLE_EXECUTABLE} ${QT_CORE} ${QT_WIDGETS})

    target_include_directories(${EXAMPLE_EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endforeach()