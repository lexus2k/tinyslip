# BuildSys

[![Build Status](https://travis-ci.com/lexus2k/buildsys.svg?branch=main)](https://travis-ci.com/lexus2k/buildsys)

## About

This is small build system for libraries and the projects.
It supports building and searching for dependencies, GNU Make and
CMake projects.

It can perform cross-compilation for different platforms.

## How to use

 1. Download all content of buildsys and place it to buildsys folder inside your project
 2. Create Makefile or CMakeLists.txt file (refer to examples)

## Usage examples

Static library from single file (Makefile):

```.make
default: mylibrary

include buildsys/toolchain.mk
include buildsys/project.mk

PKG = mylibrary
$(PKG)_SOURCES = src/mylib.c
$(PKG)_CPPFLAGS += -I./src
$(eval $(project-static-lib))
```

Static library from single file (CMake)

```.cmake
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

```

## License

This program is distributed under GPLv2 license.
