PLATFORM ?= linux
STAGING_DIR ?= $(shell pwd)/bld
PKG_CONFIG ?= pkg-config

include bs/platform-$(PLATFORM).mk

CROSS_COMPILE ?=

ifneq ($(CROSS_COMPILE),)
CXX= $(CROSS_COMPILE)g++
CC= $(CROSS_COMPILE)gcc
AR= $(CROSS_COMPILE)ar
HOST_SYSROOT ?= n
else
HOST_SYSROOT ?= y
endif


PKG_CONFIG_CROSS_ENV = \
	PKG_CONFIG_PATH="$(STAGING_DIR)/usr/lib/pkgconfig" \

ifeq ($(HOST_SYSROOT),y)
CPPFLAGS += -I$(STAGING_DIR)/include -I$(STAGING_DIR)/usr/include
LDFLAGS += -L$(STAGING_DIR)/lib -L$(STAGING_DIR)/usr/lib
else
PKG_CONFIG_CROSS_ENV += \
	PKG_CONFIG_SYSROOT_DIR="$(STAGING_DIR)"
endif

CROSS_ENV = \
        CXX=$(CXX) \
        CC=$(CC) \
        AR=$(AR) \
	$(PKG_CONFIG_CROSS_ENV) \
	CFLAGS="$(CFLAGS)" \
	CXXFLAGS="$(CXXFLAGS)" \
	CPPFLAGS="$(CPPFLAGS)" \
	LDFLAGS="$(LDFLAGS)"

