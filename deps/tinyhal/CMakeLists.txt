cmake_minimum_required (VERSION 3.5)

option(UNITTEST "Build unit tests" OFF)

file(GLOB_RECURSE SOURCE_FILES src/*.cpp src/*.c)
file(GLOB_RECURSE HEADER_FILES src/*.h)
file(GLOB BASEDIR src)
# file(GLOB_RECURSE HEADER_FILES src/*.h)

if (NOT DEFINED COMPONENT_DIR)

    project (tinyhal)

    include_directories(src)

    add_library(tinyhal STATIC ${HEADER_FILES} ${SOURCE_FILES})

#    set_target_properties(tinyhal PROPERTIES PUBLIC_HEADER "${HEADER_FILES}")

    install(TARGETS tinyhal
        ARCHIVE DESTINATION usr/lib
    )

    foreach(HFILE ${HEADER_FILES})
        string(LENGTH ${BASEDIR} SLEN)
        get_filename_component(DIR ${HFILE} DIRECTORY BASE_DIR src/)
        string(SUBSTRING ${DIR} ${SLEN} -1 DIR)
        install(FILES ${HFILE} DESTINATION usr/include/${DIR})
    endforeach()
    install(FILES tinyhal.pc DESTINATION usr/lib/pkgconfig/)
    install(FILES Findtinyhal.cmake DESTINATION usr/share/cmake/)

    if (UNITTEST)
        add_subdirectory(unittest)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS "src")

endif()
