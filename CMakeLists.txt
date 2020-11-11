cmake_minimum_required (VERSION 3.5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/buildsys)
include(package-linker)

option(EXAMPLES "Build examples and tinyslip_loopback" OFF)
option(UNITTEST "Build unit tests" OFF)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB BASEDIR src)

register_static_library(NAME tinyslip
    SOURCES ${SOURCE_FILES}
    HEADERS ${HEADER_FILES}
    INCLUDE_DIR ${BASEDIR}
    PKGCONFIG tinyslip.pc
    CMAKE_MODULE Findtinyslip.cmake
    LINK_EXTERNAL tinyhal
)

if (EXAMPLES)
    add_subdirectory(examples/linux/loopback)
    add_subdirectory(examples/linux/slip_demo)
    add_subdirectory(examples/linux/slip_demo_multithread)
endif()

if (UNITTEST)
    add_subdirectory(unittest)
endif()
