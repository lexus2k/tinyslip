
macro(link_external_package _NAME _PATH)
    if (NOT DEFINED USE_EXTERNAL_${_NAME})
        if (EXISTS ${_PATH}/CMakeLists.txt)
            ExternalProject_Add(${_NAME}_external
                SOURCE_DIR "${_PATH}"
                CMAKE_CACHE_ARGS
                    -DCMAKE_INSTALL_PREFIX:PATH=${CMAKE_CURRENT_BINARY_DIR}/${_NAME}
            )
            set(USE_EXTERNAL_${_NAME} TRUE)
            # CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/external-projects/installed
        else()
            set(USE_EXTERNAL_${_NAME} FALSE)
        endif()
    endif()
    set(__local_USE_EXTERNAL_NAME "USE_EXTERNAL_${_NAME}")

    if (${${__local_USE_EXTERNAL_NAME}})
        add_dependencies(${PROJECT_NAME} ${_NAME}_external)
        target_link_libraries(${PROJECT_NAME} PRIVATE ${_NAME})
        include_directories(${CMAKE_CURRENT_BINARY_DIR}/${_NAME}/usr/include)
        link_directories(${CMAKE_CURRENT_BINARY_DIR}/${_NAME}/usr/lib)
        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_BINARY_DIR}/${_NAME}/usr/share/cmake)
    else()
        find_package(${_NAME} REQUIRED)
        set(__local_PKG__INCLUDE_DIR "${NAME}_INCLUDE_DIRS")
        include_directories(${${__local_PKG__INCLUDE_DIR}})
    endif()

#    set(USE_EXTERNAL_${_NAME} ${USE_EXTERNAL_${_NAME}} PARENT_SCOPE)
endmacro()
