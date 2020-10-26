#!/bin/sh

make EXTRA_CPPFLAGS="--coverage" check
lcov -t "tinyhal" -o ./bld/tinyhal.info -c -d ./src
genhtml -o ./bld/report ./bld/tinyhal.info
