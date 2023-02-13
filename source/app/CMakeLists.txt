add_executable(volumedemo)

target_sources(volumedemo PRIVATE volumedemo.cpp)

target_include_directories(volumedemo PRIVATE ${CONAN_INCLUDE_DIRS})
target_include_directories(volumedemo PRIVATE ${OPENGL_INCLUDE_DIRS})


target_link_libraries(volumedemo PRIVATE ${OPENGL_LIBRARIES})
target_link_libraries(volumedemo PRIVATE ${GLAD_LIB})
target_link_libraries(volumedemo PRIVATE volume_lib)


set_property(TARGET volumedemo PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")


file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/product)
install(TARGETS volumedemo RUNTIME DESTINATION ${CMAKE_BINARY_DIR}/product)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/shader DESTINATION ${CMAKE_BINARY_DIR}/product)

