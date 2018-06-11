cmake_minimum_required(VERSION 3.0)

# CMake versions < 3.4.0 do not support try_compile/pthread checks without C as active language.
if(CMAKE_VERSION VERSION_LESS 3.4.0)
    project(pybind11_ext)
else()
    project(pybind11_ext CXX)
endif()

# register options
set(PYBIND11_VERSION "2.2.3" CACHE STRING "Version of pybind11 this project will provide extension to.")
message(STATUS "pybind11 version: ${PYBIND11_VERSION}")

set(PYTHON_VERSION 3 CACHE STRING "Target version of python")
message(STATUS "Requested python version: ${PYTHON_VERSION}")

# register modules
include(CTest)
include(ExternalProject)

# set C++ standard and compiler settings
set(CMAKE_CXX_STANDARD 11)
add_definitions(-Wall -Werror -Wfatal-errors)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -pthread")
    set(RT_LIBRARY "rt")
endif()

# find python
find_package(PythonInterp ${PYTHON_VERSION})
find_package(PythonLibs ${PYTHON_VERSION})

if (PYTHONINTERP_FOUND)
    message(STATUS "Found Python interpreter: ${PYTHON_VERSION_STRING}")
else ()
    message(FATAL_ERROR "Python not found!")
endif ()

if (PYTHONLIBS_FOUND)
    message(STATUS "Found Python libs: ${PYTHONLIBS_VERSION_STRING} in ${PYTHON_LIBRARIES}")
else ()
    message(FATAL_ERROR "Python dev libs not found!")
endif ()

# find boost
set(Boost_USE_STATIC_LIBS OFF)
set(Boost_USE_MULTITHREADED ON)
set(Boost_USE_STATIC_RUNTIME ON)
find_package(Boost 1.53.0 COMPONENTS system program_options date_time filesystem regex log REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

# add src to includes
include_directories(include)
include_directories(${PYTHON_INCLUDE_PATH})

#Setup CMake to run tests
enable_testing()

# include subdirectories
add_subdirectory(external)
add_subdirectory(tests)