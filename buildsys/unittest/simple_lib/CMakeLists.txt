cmake_minimum_required (VERSION 3.5)
include(buildsys/package-linker.cmake)

register_static_library(NAME simple
    SOURCES
        src/source.c
    HEADERS
        src/header.h
    INCLUDE_DIR src
    PKGCONFIG simple.pc
    CMAKE_MODULE Findsimple.cmake
)
