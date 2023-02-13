add_library(volume_lib STATIC)

target_sources(volume_lib PRIVATE 
    eventloop.cpp
    eventloop.h
    modeling.cpp
    modeling.h
    window.cpp
    window.h
    polygonobject.cpp
    polygonobject.h
    program.cpp
    program.h
    renderengine.cpp
    renderengine.h
    log.cpp
    log.h)

target_include_directories(volume_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})


