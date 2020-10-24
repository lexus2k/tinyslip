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
# TODO: PKG CONFIG

ifeq ($(ARCH),linux)
PKG = tinyslip_loopback
$(PKG)_SOURCES = examples/linux/loopback/tinyslip_loopback.cpp
$(PKG)_DEPENDENCIES = libtinyslip tinyhal
$(PKG)_LIBRARIES = tinyhal
$(PKG)_CPPFLAGS = -pthread -I./src
$(PKG)_LDFLAGS = -lm -L. -ltinyslip
$(eval $(project-binary))
endif
