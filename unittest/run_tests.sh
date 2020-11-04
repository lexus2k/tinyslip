#!/bin/sh

# includes_path, dest_path, binaries
check_install()
{
    for i in $3; do
        if [ ! -f "$2/$i" ]; then
            echo "[ERROR] Project artifacts are not copied: $2/$i"
            exit 1
        fi
    done

    local basedir=`pwd`

    # Find all header files, but exclude internal headers
    for i in `cd $1 && find . -name "*.h" ! -iname "*_int.h" && cd $basedir`; do
        if [ ! -f "$2/usr/include/$i" ]; then
            echo "[ERROR] Headers are not copied: $2/usr/include/$i"
            exit 1
        fi
    done
    return 0
}

# path, dest_path
build_project_make()
{
    local basedir=`pwd`
    mkdir -p $2
    cd $2
    local dest=`pwd`
    cd $basedir
    cd $1
    ln -s ../../ buildsys
    make install DESTDIR=$dest
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build $1 failed"
        exit 1
    fi
    make clean
    rm buildsys
    cd $basedir
}

# path, dest_path
build_project_cmake()
{
    local basedir=`pwd`
    mkdir -p $2
    cd $2
    local dest=`pwd`
    cd $basedir

    mkdir -p __temp
    cd $1 && ln -s ../../ buildsys && cd $basedir
    cd __temp
    cmake -DCMAKE_INSTALL_PREFIX= ../$1
    make install DESTDIR=$dest
    if [ $? -ne 0 ]; then
        echo "[ERROR] Build $1 failed"
        exit 1
    fi
    cd ..
    rm $1/buildsys
    rm -rf __temp
}

build_project_make unittest/simple_lib dest_make
check_install unittest/simple_lib/src dest_make \
    "usr/lib/libsimple.a usr/lib/pkgconfig/simple.pc usr/share/cmake/Findsimple.cmake"

build_project_cmake unittest/simple_lib dest_cmake
check_install unittest/simple_lib/src dest_cmake \
    "usr/lib/libsimple.a usr/lib/pkgconfig/simple.pc usr/share/cmake/Findsimple.cmake"


rm -rf dest_make dest_cmake
echo "[PASS]"
