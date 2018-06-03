include_directories(${PYTHON_INCLUDE_DIRS})
include_directories(${EXTERNAL_INCLUDE_DIRS})

set(CMAKE_SHARED_MODULE_PREFIX)

set(modules_SRC "")
file(GLOB_RECURSE modules_SRC "*.cpp")

foreach(module_file ${modules_SRC})
    get_filename_component (name_without_extension ${module_file} NAME_WE)
    add_library(${name_without_extension} MODULE
            ${module_file})

    target_link_libraries(${name_without_extension}
            ${PYTHON_LIBRARIES}
            ${CMAKE_THREAD_LIBS_INIT}
            ${Boost_LIBRARIES}
            ${RT_LIBRARY})

    add_dependencies(${name_without_extension}
            pybind11)

    list(APPEND modules ${name_without_extension})
endforeach()

add_test(NAME run-python-unit-tests COMMAND ${PYTHON_EXECUTABLE} -m pytest ${CMAKE_CURRENT_SOURCE_DIR})

add_custom_target(install-python-requirements
        COMMAND ${PYTHON_EXECUTABLE} -m pip install --user --upgrade pip wheel
        COMMAND ${PYTHON_EXECUTABLE} -m pip install -r ${CMAKE_CURRENT_SOURCE_DIR}/requirements.txt)