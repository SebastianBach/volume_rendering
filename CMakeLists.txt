cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project(volume_rendering)
enable_testing()


find_package(OpenGL REQUIRED)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_library(GLAD_LIB  glad  ${CONAN_LIB_DIRS_GLAD})
find_library(GTEST_LIB gtest ${CONAN_LIB_DIRS_GTEST})

add_subdirectory(source)