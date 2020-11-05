#
# This is build system for small libraries and projects.
# Copyright (C) 2020 Alexey Dynda
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
#

PLATFORM ?= linux
STAGING_DIR ?= $(shell pwd)/bld
PKG_CONFIG ?= pkg-config
DESTDIR ?=

include buildsys/platform-$(PLATFORM).mk

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

