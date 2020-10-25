cmake_minimum_required (VERSION 3.5)
include(ExternalProject)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(EXAMPLES "Build examples and tinyslip_loopback" ON)
option(UNITTEST "Build unit tests" OFF)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    if (EXISTS ${CMAKE_CURRENT_LIST_DIR}/tinyhal/CMakeLists.txt)
        ExternalProject_Add(tinyhal_external
            SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tinyhal"
            CMAKE_CACHE_ARGS
                -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/tinyhal
        )
        set(USE_EXTERNAL_HAL TRUE)
        # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external-projects/installed
    else()
        set(USE_EXTERNAL_HAL FALSE)
    endif()

    project(tinyslip)

    include_directories(src)

    add_library(tinyslip STATIC ${HEADER_FILES} ${SOURCE_FILES})

    if (${USE_EXTERNAL_HAL})
        add_dependencies(${PROJECT_NAME} tinyhal_external)
        target_link_libraries(${PROJECT_NAME} PRIVATE tinyhal)
        include_directories(${CMAKE_CURRENT_BINARY_DIR}/tinyhal/usr/include)
        link_directories(${CMAKE_CURRENT_BINARY_DIR}/tinyhal/usr/lib)
    else()
        find_package(tinyhal REQUIRED)
        include_directories(${tinyhal_INCLUDE_DIR})
    endif()

    if (EXAMPLES)
        add_subdirectory(examples/linux/loopback)
#        add_subdirectory(examples/linux/slip_demo)
#        add_subdirectory(examples/linux/slip_demo_multithread)
    endif()

    if (UNITTEST)
        add_subdirectory(unittest)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()
