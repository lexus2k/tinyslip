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


macro(register_static_library)

#    set(options OPTIONAL FAST)
    set(options)
    set(oneValueArgs NAME INCLUDE_DIR)
    set(multiValueArgs SOURCES HEADERS PKGCONFIG CMAKE_MODULE LINK_EXTERNAL)
    cmake_parse_arguments(PKG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    if (NOT DEFINED COMPONENT_DIR)
        set(CMAKE_CXX_STANDARD 11)
        set(CMAKE_CXX_STANDARD_REQUIRED ON)

        set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/buildsys)

        include(ExternalProject)

        project(${PKG_NAME})

        include_directories(${PKG_INCLUDE_DIR})

        add_library(${PKG_NAME} STATIC ${PKG_SOURCES} ${PKG_HEADERS})

        foreach(LLIB ${PKG_LINK_EXTERNAL})
            link_external_package(${LLIB} ${CMAKE_CURRENT_LIST_DIR}/deps/${LLIB})
        endforeach()

        install(TARGETS ${PKG_NAME}
            ARCHIVE DESTINATION usr/lib
        )

        foreach(HFILE ${PKG_HEADERS})
            string(LENGTH ${PKG_INCLUDE_DIR} SLEN)
            get_filename_component(DIR ${HFILE} DIRECTORY ${PKG_INCLUDE_DIR} ${PKG_INCLUDE_DIR})
            string(SUBSTRING ${DIR} ${SLEN} -1 DIR)
            install(FILES ${HFILE} DESTINATION usr/include/${DIR})
        endforeach()

        foreach(PCFILE ${PKG_PKGCONFIG})
            install(FILES ${PCFILE} DESTINATION usr/lib/pkgconfig/)
        endforeach()

        foreach(MODULEFILE ${PKG_CMAKE_MODULE})
            install(FILES ${MODULEFILE} DESTINATION usr/share/cmake/)
        endforeach()

    else()
        idf_component_register(SRCS ${PKG_SOURCES}}
                               INCLUDE_DIRS ${PKG_HEADERS})

    endif()
endmacro()
