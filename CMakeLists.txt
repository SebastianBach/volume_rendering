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
    source/eventloop.h
    source/modeling.cpp
    source/modeling.h
    source/window.cpp
    source/window.h
    source/polygonobject.cpp
    source/polygonobject.h
    source/program.cpp
    source/program.h
    source/renderengine.cpp
    source/renderengine.h
    source/log.cpp
    source/log.h
    source/volumedemo.cpp)

target_include_directories(volumedemo PUBLIC source)
include_directories(${CONAN_INCLUDE_DIRS})
include_directories( ${OPENGL_INCLUDE_DIRS})

target_link_libraries(volumedemo ${OPENGL_LIBRARIES}  )
target_link_libraries(volumedemo ${CONAN_LIBS})

set_property(TARGET volumedemo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")

file(MAKE_DIRECTORY  ${CMAKE_BINARY_DIR}/product)
install (TARGETS volumedemo RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/product)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/shader DESTINATION ${CMAKE_BINARY_DIR}/product)