find_package(PkgConfig)
pkg_check_modules(pc_simple QUIET simple)

find_path(simple_INCLUDE_DIR
    NAMES header.h
    PATHS ${pc_simple_INCLUDE_DIRS}
    PATH_SUFFIXES simple
)

find_library(simple_LIBRARY
    NAMES simple
    PATHS ${pc_simple_LIBRARY_DIRS}
)

set(simple_VERSION ${pc_simple_VERSION})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(simple
    FOUND_VAR simple_FOUND
    REQUIRED_VARS
        simple_LIBRARY
        simple_INCLUDE_DIR
    VERSION_VAR simple_VERSION
)

if(simple_FOUND)
    set(simple_LIBRARIES ${simple_LIBRARY})
    set(simple_INCLUDE_DIRS ${simple_INCLUDE_DIR})
    set(simple_DEFINITIONS ${pc_simple_CFLAGS_OTHER})
endif()

if(simple_FOUND AND NOT TARGET simple::simple)
    add_library(simple::simple UNKNOWN IMPORTED)
    set_target_properties(simple::simple PROPERTIES
        IMPORTED_LOCATION "${simple_LIBRARY}"
        INTERFACE_COMPILE_OPTIONS "${pc_simple_CFLAGS_OTHER}"
        INTERFACE_INCLUDE_DIRECTORIES "${simple_INCLUDE_DIR}"
    )
endif()
