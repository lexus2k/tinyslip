![Tiny SLIP Protocol](.travis/tinylogo.svg)<br>
[![Build Status](https://travis-ci.com/lexus2k/tinyslip.svg?branch=main)](https://travis-ci.com/lexus2k/tinyslip)
[![Coverage Status](https://coveralls.io/repos/github/lexus2k/tinyslip/badge.svg)](https://coveralls.io/github/lexus2k/tinyslip)
[![Documentation](https://codedocs.xyz/lexus2k/tinyslip.svg)](https://codedocs.xyz/lexus2k/tinyslip/)

[tocstart]: # (toc start)

  * [Introduction](#introduction)
  * [Key Features](#key-features)
  * [Supported platforms](#supported-platforms)
  * [Easy to use](#easy-to-use)
  * [Setting up](#setting-up)
  * [How to buid](#how-to-build)
  * [Using tiny_loopback tool](#using-tiny_loopback-tool)
  * [License](#license)

[tocend]: # (toc end)

## Introduction

Tiny SLIP Protocol is layer 2 protocol. It is intended to be used for the systems with low resources.
You don't need to think about data synchronization between points. The library use no dynamic allocation of memory.
It is also can be compiled for desktop Linux system, and it can be built it for Windows.
Using this library you can easy implement data transfer between 2 microcontrollers or between microcontroller and pc via UART, SPI,
I2C or any other communication channels.
Tiny SLIP protocol is based on RFC 1055.
This protocol doesn't implement any error detection! If you need error detection and frames acknowledgment,
please refer to [TinyProto](https://github.com/lexus2k/tinyproto) library.

## Dependencies

 * [buildsys](https://github.com/lexus2k/buildsys) integrated to this library
 * [tinyhal](https://github.com/lexus2k/tinyhal) as submodule

## Key Features

Main features:
 * Frames of maximum 32K or 2G size (limit depends on platform).
 * Low SRAM consumption (starts at 50 bytes).
 * Low Flash consumption (starts at 1KiB)
 * No dynamic memory allocation
 * Serial loopback tool for debug purposes and performance testing

## Supported platforms

 * Any platform, where C/C++ compiler is available (C99, C++11)

## Easy to use

Tiny SLIP Library is very easy to use:
```.cpp
SLIP::SLIP  proto;

void onReceive(SLIP::IPacket &pkt)
{
    // Process incoming packets here

    /* Send message back */
    proto.write( pkt );
}

...
...
    proto.setReceiveCallback( onReceive );
    proto.begin();
...
...
    if (Serial.available()) {
        uint8_t byte = Serial.read();
        proto.run_rx(&byte, 1);
    }
    uint8_t byte;
    if ( proto.run_tx(&byte, 1) == 1)
    {
        Serial.write( byte );
    }
```

## How to build

### Linux

1. Getting source code

If you wish to build the library with all dependencies you need to clone project using command
```.txt
git clone --recursive https://github.com/lexus2k/tinyslip
```
If you have all dependencies installed in the system, use the following command for cloning
```.txt
git clone https://github.com/lexus2k/tinyslip
```

2. Building
```.txt
make
# === OR ===
mkdir build
cd build
cmake -DEXAMPLES=ON ..
make
```

### Windows

1. Getting source code

If you wish to build the library with all dependencies you need to clone project using command
```.txt
git clone --recursive https://github.com/lexus2k/tinyslip
```
If you have all dependencies installed in the system, use the following command for cloning
```.txt
git clone https://github.com/lexus2k/tinyslip
```

2. Building
```.txt
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -DEXAMPLES=ON ..
```

### ESP32

1. Getting source code

TODO

2. Building

Just build your project as regular ESP32 IDF project.

### Cross-compilation

1. Getting source code

If you wish to build the library with all dependencies you need to clone project using command
```.txt
git clone --recursive https://github.com/lexus2k/tinyslip
```
If you have all dependencies installed in the system, use the following command for cloning
```.txt
git clone https://github.com/lexus2k/tinyslip
```

2. Building
```.txt
make CROSS_COMPILE=<prefix> STAGING_DIR=<your_staging_dir>
# === OR ===
mkdir build
cd build
cmake -DEXAMPLES=ON ..
make
```


## Setting up

 * Arduino Option 1 (with docs and tools)
   * Download source from https://github.com/lexus2k/tinyslip
   * Put the downloaded library content to Arduino/libraries/tinyslip folder
   * Restart the Arduino IDE
   * You will find the examples in the Arduino IDE under File->Examples->tinyslip

 * Arduino Option 2 (only library without docs)
   * Go to Arduino Library manager
   * Find and install tinyslip library
   * Restart the Arduino IDE
   * You will find the examples in the Arduino IDE under File->Examples->tinyslip

 * ESP32 IDF
   * Download sources from https://github.com/lexus2k/tinyslip and put to components
     folder of your project
   * Run `make` for your project

 * Linux
   * Download sources from https://github.com/lexus2k/tinyslip
   * Run `make` command from tinyslip folder, and it will build library and tools for you

 * Plain AVR
   * Download sources from https://github.com/lexus2k/tinyslip
   * Install avr gcc compilers
   * Run `make PLATFORM=avr`

## Using tiny_loopback tool

 * Connect your Arduino board to PC
 * Run your sketch or tinyslip_loopback
 * Compile tinyslip_loopback tool
 * Run tinyslip_loopback tool: `./bld/tinyslip_loopback -p /dev/ttyUSB0 -g -a -r`

For more information about this library, please, visit https://github.com/lexus2k/tinyslip.
Doxygen documentation can be found at [Codedocs xyz site](http://codedocs.xyz/tinyslip).
If you found any problem or have any idea, please, report to Issues section.

## License

Copyright 2020 (C) Alexey Dynda

This file is part of Tiny SLIP Library.

Tiny SLIP Library is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Tiny SLIP Library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Tiny SLIP Library.  If not, see <http://www.gnu.org/licenses/>.

