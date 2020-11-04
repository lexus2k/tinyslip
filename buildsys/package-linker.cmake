#
# This is build system for small libraries and projects.
# Copyright (C) 2020 Alexey Dynda
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

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


macro(register_static_library _NAME _SRC _HDR _INC_DIR)
    if (NOT DEFINED COMPONENT_DIR)
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)

        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/buildsys)

        include(ExternalProject)

        project(${_NAME})

        include_directories(${${_INC_DIR}})

        add_library(${_NAME} STATIC ${${_SRC}} ${${_HDR}})

        install(TARGETS ${_NAME}
            ARCHIVE DESTINATION usr/lib
        )

        foreach(HFILE ${${_HDR}})
            string(LENGTH ${${_INC_DIR}} SLEN)
            get_filename_component(DIR ${HFILE} DIRECTORY ${${_INC_DIR}} ${${_INC_DIR}}/)
            string(SUBSTRING ${DIR} ${SLEN} -1 DIR)
            install(FILES ${HFILE} DESTINATION usr/include/${DIR})
        endforeach()
        install(FILES ${_NAME}.pc DESTINATION usr/lib/pkgconfig/)
        install(FILES Find${_NAME}.cmake DESTINATION usr/share/cmake/)

    else()
        idf_component_register(SRCS ${${_SRC}}
                               INCLUDE_DIRS ${${_INC_DIR}})

    endif()
endmacro()
