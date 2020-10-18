cmake_minimum_required (VERSION 3.5)

option(UNITTEST "Build unit tests" OFF)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    project (tinyhal)

    include_directories(src)

    add_library(tinyhal STATIC ${HEADER_FILES} ${SOURCE_FILES})

    if (UNITTEST)
        add_subdirectory(unittest)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()
