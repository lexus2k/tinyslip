find_package(PkgConfig)
pkg_check_modules(pc_tinyslip QUIET tinyslip)

find_path(tinyslip_INCLUDE_DIR
    NAMES TinyProtocolSlip.h
    PATHS ${pc_tinyslip_INCLUDE_DIRS}
    PATH_SUFFIXES tinyslip
)

find_library(tinyslip_LIBRARY
    NAMES tinyslip
    PATHS ${pc_tinyslip_LIBRARY_DIRS}
)

set(tinyslip_VERSION ${pc_tinyslip_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(tinyslip
    FOUND_VAR tinyslip_FOUND
    REQUIRED_VARS
        tinyslip_LIBRARY
        tinyslip_INCLUDE_DIR
    VERSION_VAR tinyslip_VERSION
)

if(tinyslip_FOUND)
    set(tinyslip_LIBRARIES ${tinyslip_LIBRARY})
    set(tinyslip_INCLUDE_DIRS ${tinyslip_INCLUDE_DIR})
    set(tinyslip_DEFINITIONS ${pc_tinyslip_CFLAGS_OTHER})
endif()

if(tinyslip_FOUND AND NOT TARGET tinyslip::tinyslip)
    add_library(tinyslip::tinyslip UNKNOWN IMPORTED)
    set_target_properties(tinyslip::tinyslip PROPERTIES
        IMPORTED_LOCATION "${tinyslip_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${pc_tinyslip_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${tinyslip_INCLUDE_DIR}"
    )
endif()
