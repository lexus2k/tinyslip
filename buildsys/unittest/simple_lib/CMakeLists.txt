cmake_minimum_required (VERSION 3.5)
include(buildsys/package-linker.cmake)

set(SOURCE_FILES src/source.c)
set(HEADER_FILES src/header.h)
set(INCLUDE_DIRS src)

register_static_library(simple SOURCE_FILES HEADER_FILES INCLUDE_DIRS)
