cmake_minimum_required (VERSION 3.5)

option(EXAMPLES "Build examples and tinyslip_loopback" OFF)
option(UNITTEST "Build unit tests" OFF)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    project (tinyslip)

    include_directories(src)

    add_library(tinyslip STATIC ${HEADER_FILES} ${SOURCE_FILES})

    if (EXAMPLES)
        add_subdirectory(examples/linux/loopback)
        add_subdirectory(examples/linux/slip_demo)
        add_subdirectory(examples/linux/slip_demo_multithread)
    endif()

    if (UNITTEST)
        add_subdirectory(unittest)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()
