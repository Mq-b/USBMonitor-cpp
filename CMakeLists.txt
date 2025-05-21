cmake_minimum_required(VERSION 3.10)
project(USBMonitor-cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/${CMAKE_BUILD_TYPE}/bin)

set(CMAKE_AUTOMOC ON)

file(GLOB EXAMPLE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/examples/*.cpp")

find_package(Qt5 COMPONENTS Core Widgets REQUIRED)

foreach(EXAMPLE_SOURCE IN ITEMS ${EXAMPLE_SOURCES})
    get_filename_component(EXAMPLE_NAME ${EXAMPLE_SOURCE} NAME_WE)
    set(EXAMPLE_EXECUTABLE example-${EXAMPLE_NAME})

    add_executable(${EXAMPLE_EXECUTABLE} ${EXAMPLE_SOURCE})
    
    target_link_libraries(${EXAMPLE_EXECUTABLE} Qt5::Core Qt5::Widgets)

    target_include_directories(${EXAMPLE_EXECUTABLE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
    )
endforeach()