cmake_minimum_required (VERSION 3.5)

file(GLOB_RECURSE SOURCE_FILES *.cpp *.c)

if (NOT DEFINED COMPONENT_DIR)

    project (unit_test)

    add_executable(unit_test ${SOURCE_FILES})

    include_directories(cpputils)

    target_link_libraries(unit_test tinyslip)

    find_package(Threads REQUIRED)
    target_link_libraries(${PROJECT_NAME} Threads::Threads)
    find_package(CppUTest QUIET)
    if (NOT CppUTest_FOUND)
        find_package(PkgConfig REQUIRED)
        pkg_search_module(CPPUTEST REQUIRED cpputest)
        link_directories(${PROJECT_NAME} ${CPPUTEST_LIBRARY_DIRS})
        target_link_libraries(${PROJECT_NAME} ${CPPUTEST_LIBRARIES})
    else()
        target_link_libraries(${PROJECT_NAME} CppUTest::CppUTest)
    endif()

    target_link_libraries(${PROJECT_NAME} tinyhal)

    add_custom_target(check unit_test)

else()

    idf_component_register(SRCS ${SOURCE_FILES}
                           INCLUDE_DIRS ".")

endif()
