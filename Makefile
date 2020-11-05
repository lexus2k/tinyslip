default: libtinyslip
UNITTEST ?= n

include buildsys/toolchain.mk
include buildsys/project.mk
include buildsys/package_make.mk

######################## Custom options #########################

ifeq ($(PLATFORM),linux)
CONFIG_ENABLE_FCS32 ?= y
CONFIG_ENABLE_FCS16 ?= y
CONFIG_ENABLE_CHECKSUM ?= y
CONFIG_ENABLE_STATS ?=y
endif

ifeq ($(PLATFORM),avr)
CONFIG_ENABLE_FCS32 ?= n
CONFIG_ENABLE_FCS16 ?= n
CONFIG_ENABLE_CHECKSUM ?= y
CONFIG_ENABLE_STATS ?= n
endif

LOG_LEVEL ?=
ifeq ($(ENABLE_LOGS),y)
    ENABLE_SLIP_LOGS ?= y
endif

ifneq ($(LOG_LEVEL),)
    CPPFLAGS += -DTINY_LOG_LEVEL_DEFAULT=$(LOG_LEVEL)
endif

ifeq ($(ENABLE_SLIP_LOGS),y)
    CPPFLAGS += -DTINY_SLIP_DEBUG=1
    ENABLE_DEBUG=y
endif

ifeq ($(ENABLE_DEBUG),y)
    CPPFLAGS += -DTINY_DEBUG=1
endif

###################### Added HAL library #########################

PKG = tinyhal
$(PKG)_SRCDIR = ./deps/tinyhal
$(PKG)_MAKE_OPTS = ARCH=$(PLATFORM)
$(eval $(package-make))

##################################################################

CFLAGS += -std=gnu99
CPPFLAGS += -Os -Wall -Werror -ffunction-sections -fdata-sections $(EXTRA_CPPFLAGS)
CXXFLAGS += -std=c++11

####################### Compiling library #########################
PKG = libtinyslip
$(PKG)_DEPENDENCIES = tinyhal
$(PKG)_LIBRARIES = tinyhal
$(PKG)_SOURCES = src/proto/slip/tiny_slip.c \
	src/TinyProtocolSlip.cpp
$(PKG)_CPPFLAGS += -I./src
$(eval $(project-static-lib))

####################### Compiling tools #########################
ifeq ($(PLATFORM),linux)
PKG = tinyslip_loopback
$(PKG)_SOURCES = examples/linux/loopback/tinyslip_loopback.cpp
$(PKG)_DEPENDENCIES = libtinyslip tinyhal
$(PKG)_LIBRARIES = tinyhal
$(PKG)_CPPFLAGS = -pthread -I./src
$(PKG)_LDFLAGS = -pthread -lm -L. -ltinyslip
$(eval $(project-binary))
endif

####################### Compiling unit tests ####################
ifeq ($(PLATFORM),linux)
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
unit_test: .ts_unittest_build
check:
	./unit_test

ifeq ($(UNITTEST),y)
check: unittest
all: unittest
endif

endif

