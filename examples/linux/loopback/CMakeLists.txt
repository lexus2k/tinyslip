cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

if (NOT DEFINED COMPONENT_DIR)

    project (tinyslip_loopback)

    add_executable(tinyslip_loopback ${SOURCE_FILES})

    target_link_libraries(tinyslip_loopback tinyslip)

    if (WIN32)
        find_package(Threads REQUIRED)
        target_link_libraries(${PROJECT_NAME} Threads::Threads)

    elseif (UNIX)
        find_package(Threads REQUIRED)
        target_link_libraries(${PROJECT_NAME} Threads::Threads)
#        target_link_libraries(tiny_loopback pthread)
    endif()

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS ".")

endif()
