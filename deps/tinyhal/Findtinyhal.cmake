find_package(PkgConfig)
pkg_check_modules(pc_tinyhal QUIET tinyhal)

find_path(tinyhal_INCLUDE_DIR
    NAMES hal/tinyhal.h
    PATHS ${pc_tinyhal_INCLUDE_DIRS}
    PATH_SUFFIXES tinyhal
)

find_library(tinyhal_LIBRARY
    NAMES tinyhal
    PATHS ${pc_tinyhal_LIBRARY_DIRS}
)

set(tinyhal_VERSION ${pc_tinyhal_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(tinyhal
    FOUND_VAR tinyhal_FOUND
    REQUIRED_VARS
        tinyhal_LIBRARY
        tinyhal_INCLUDE_DIR
    VERSION_VAR tinyhal_VERSION
)

if(tinyhal_FOUND)
    set(tinyhal_LIBRARIES ${tinyhal_LIBRARY})
    set(tinyhal_INCLUDE_DIRS ${tinyhal_INCLUDE_DIR})
    set(tinyhal_DEFINITIONS ${pc_tinyhal_CFLAGS_OTHER})
endif()

if(tinyhal_FOUND AND NOT TARGET tinyhal::tinyhal)
    add_library(tinyhal::tinyhal UNKNOWN IMPORTED)
    set_target_properties(tinyhal::tinyhal PROPERTIES
        IMPORTED_LOCATION "${tinyhal_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${pc_tinyhal_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${tinyhal_INCLUDE_DIR}"
    )
endif()
