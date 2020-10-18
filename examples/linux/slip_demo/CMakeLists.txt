cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

if (NOT DEFINED COMPONENT_DIR)

    project (slip_demo)

    add_executable(slip_demo ${SOURCE_FILES})

    target_link_libraries(slip_demo tinyslip)

    if (WIN32)
        find_package(Threads REQUIRED)
        target_link_libraries(${PROJECT_NAME} Threads::Threads)

    elseif (UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(${PROJECT_NAME} Threads::Threads)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS ".")

endif()
