#!/bin/sh

# test checks install target

if [ "$2" = "" ]; then
    echo "check_install.sh SRC_DIR DEST_DIR"
    exit 1
fi

if [ ! -f "$2/usr/lib/libtinyslip.a" ]; then
    echo "Library is not copied"
    exit 1
fi

rootpath=`pwd`

# Find all header files, but exclude internal headers
for i in `cd $1/src && find . -name "*.h" ! -iname "*_int.h" && cd $rootpath`; do
    if [ ! -f "$2/usr/include/$i" ]; then
        echo "Headers are not copied: $2/usr/include/$i"
        exit 1
    fi
done
