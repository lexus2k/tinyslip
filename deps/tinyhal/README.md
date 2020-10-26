![TinyHal](.travis/tinylogo.svg)<br>
[![Build Status](https://travis-ci.com/lexus2k/tinyhal.svg?branch=main)](https://travis-ci.com/lexus2k/tinyhal)
[![Coverage Status](https://coveralls.io/repos/github/lexus2k/tinyhal/badge.svg?branch=main)](https://coveralls.io/github/lexus2k/tinyhal?branch=main)
[![Documentation](https://codedocs.xyz/lexus2k/tinyhal.svg)](https://codedocs.xyz/lexus2k/tinyhal/)

[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Key Features](#key-features)
  * [Supported platforms](#supported-platforms)
  * [Easy to use](#easy-to-use)
  * [Setting up](#setting-up)
  * [How to buid](#how-to-build)
  * [License](#license)

[tocend]: # (toc end)

## Introduction

Tinyhal is platform / hardware abstraction layer library that implements synchronization objects, time count functions, some hardware related API
for different platforms in C-style API, making available to write platform independent code. It is intended to be used for the systems with low resources.
It is also can be compiled for desktop Linux system, and it can be built it for Windows.

## Key Features

Main features:
 * Serial port support (Windows & Linux)
 * Mutex
 * Event Groups
 * Time tick functions

## Supported platforms

 * Any platform, where C/C++ compiler is available (C99, C++11)
 * If platform is not supported, the library will use default implementation (no_platform)

## Easy to use

Usage of light TinyHal in C can look like this:
```.c
#include "hal/tiny_types.h"

tiny_mutex_t mutex;
...
    tiny_mutex_create(&mutex);
    tiny_mutex_lock(&mutex);
    tiny_mutex_unlock(&mutex);
    tiny_mutex_destroy(&mutex);
```

## How to build

### Linux
```.txt
make
# === OR ===
mkdir build
cd build
cmake ..
make
```

### Windows
```.txt
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
```

## Setting up

 * Arduino Option 1 (with docs and tools)
   * Download source from https://github.com/lexus2k/tinyhal
   * Put the downloaded library content to Arduino/libraries/tinyhal folder
   * Restart the Arduino IDE

 * Arduino Option 2 (only library without docs)
   * Go to Arduino Library manager
   * Find and install tinyhal library
   * Restart the Arduino IDE

 * ESP32 IDF
   * Download sources from https://github.com/lexus2k/tinyhal and put to components
     folder of your project
   * Run `make` for your project

 * Linux
   * Download sources from https://github.com/lexus2k/tinyhal
   * Run `make` command from tinyhal folder, and it will build library

 * Plain AVR
   * Download sources from https://github.com/lexus2k/tinyhal
   * Install avr gcc compilers
   * Run `make ARCH=avr`

For more information about this library, please, visit https://github.com/lexus2k/tinyhal.
Doxygen documentation can be found at [codedocs.xyz site](https://codedocs.xyz/lexus2k/tinyhal).
If you found any problem or have any idea, please, report to Issues section.

## License

Copyright 2016-2020 (C) Alexey Dynda

This file is part of Tiny HAL Library.

Tiny HAL Library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Tiny HAL Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Tiny HAL Library.  If not, see <http://www.gnu.org/licenses/>.

