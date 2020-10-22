BLDDIR ?= $(shell pwd)/bld
PKG_CONFIG ?= pkg-config

CPPFLAGS += -I$(BLDDIR)/include
LDFLAGS += -L$(BLDDIR)/lib

CROSS_ENV = \
	PKG_CONFIG_PATH="$(BLDDIR)/lib/pkgconfig" \
	CFLAGS="$(CFLAGS)" \
	CXXFLAGS="$(CXXFLAGS)" \
	CPPFLAGS="$(CPPFLAGS)" \
	LDFLAGS="$(LDFLAGS)"

