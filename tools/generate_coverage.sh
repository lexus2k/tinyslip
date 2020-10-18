#!/bin/sh

make EXTRA_CPPFLAGS="--coverage" check
lcov -t "tinyslip" -o ./bld/tinyslip.info -c -d ./src
genhtml -o ./bld/report ./bld/tinyslip.info
