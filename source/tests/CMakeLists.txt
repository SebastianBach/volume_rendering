
add_executable(unit_tests)

target_sources(unit_tests PRIVATE tests.cpp)

target_link_libraries(unit_tests PRIVATE ${CONAN_LIBS})
target_link_libraries(unit_tests PRIVATE ${CONAN_LIBS_DEBUG})
target_link_libraries(unit_tests PRIVATE volume_lib)

add_test(NAME the_test COMMAND unit_tests)
