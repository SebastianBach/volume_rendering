cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(volume_rendering)

find_package(OpenGL REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(volumedemo)

target_sources(volumedemo PRIVATE 
    source/eventloop.cpp
    source/modeling.cpp
    source/window.cpp
    source/polygonobject.cpp
    source/program.cpp
    source/renderengine.cpp
    source/log.cpp
    source/volumedemo.cpp
    source/renderengine.cpp)

target_include_directories(volumedemo PUBLIC source)
include_directories(${CONAN_INCLUDE_DIRS})
include_directories( ${OPENGL_INCLUDE_DIRS})

target_link_libraries(volumedemo ${OPENGL_LIBRARIES}  )
target_link_libraries(volumedemo ${CONAN_LIBS})

set_property(TARGET volumedemo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
