default: all
#default: libtinyslip
ARCH ?= linux
DESTDIR ?= .

include bs/toolchain.mk
include bs/package_make.mk
include bs/project.mk

###################### Added HAL library #########################

PKG = tinyhal
$(PKG)_SRCDIR = ./tinyhal
$(PKG)_PKGCONFIG = tinyhal.pc
$(eval $(package-make))

##################################################################

CFLAGS += -std=gnu99
CPPFLAGS += -Os -Wall -Werror -ffunction-sections -fdata-sections
CXXFLAGS += -std=c++11

####################### Compiling library #########################
PKG = libtinyslip
$(PKG)_DEPENDENCIES = tinyhal
$(PKG)_SOURCES = src/proto/slip/tiny_slip.c \
	src/TinyProtocolSlip.cpp
$(PKG)_CPPFLAGS += -I./src
$(eval $(project-static-lib))

####################### Compiling tools #########################
ifeq ($(ARCH),linux)
PKG = tinyslip_loopback
$(PKG)_SOURCES = examples/linux/loopback/tinyslip_loopback.cpp
$(PKG)_DEPENDENCIES = libtinyslip tinyhal
$(PKG)_LIBRARIES = tinyhal
$(PKG)_CPPFLAGS = -pthread -I./src
$(PKG)_LDFLAGS = -pthread -lm -L. -ltinyslip
$(eval $(project-binary))
endif

####################### Compiling unit tests ####################
ifeq ($(ARCH),linux)
PKG = unittest
$(PKG)_NAME = unit_test
$(PKG)_SOURCES = \
        unittest/cpputils/fake_wire.cpp \
        unittest/cpputils/fake_connection.cpp \
        unittest/cpputils/fake_endpoint.cpp \
        unittest/cpputils/tiny_base_helper.cpp \
        unittest/helpers/tiny_slip_helper.cpp \
        unittest/main.cpp \
        unittest/packet_tests.cpp \
        unittest/slip_tests.cpp
$(PKG)_DEPENDENCIES = libtinyslip tinyhal
$(PKG)_LIBRARIES = tinyhal
$(PKG)_CPPFLAGS = -pthread -I./src -I./unittest -I./unittest/cpputils
$(PKG)_LDFLAGS = -pthread -lm -L. -ltinyslip -lCppUTest -lCppUTestExt
$(eval $(project-binary))

.PHONY: check
check: unit_test
	./unit_test
endif

