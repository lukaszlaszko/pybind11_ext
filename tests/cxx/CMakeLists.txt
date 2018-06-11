include_directories(${EXTERNAL_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB HEADERS ${CMAKE_SOURCE_DIR}/include/*)
file(GLOB_RECURSE TESTS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(cxx_unit_tests
        ${HEADERS}
        ${TESTS})

target_link_libraries(cxx_unit_tests
        ${PYTHON_LIBRARIES}
        ${EXTERNAL_LIBRARIES}
        ${Boost_LIBRARIES}
        ${CMAKE_THREAD_LIBS_INIT}
        ${RT_LIBRARY})

add_dependencies(cxx_unit_tests
        ${EXTERNAL_DEPENDENCIES})

add_test(NAME run-cxx-unit-tests COMMAND cxx_unit_tests)