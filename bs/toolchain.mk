BLDDIR ?= $(shell pwd)/bld
PKG_CONFIG ?= pkg-config

CPPFLAGS += -I$(BLDDIR)/include -I$(BLDDIR)/usr/include
LDFLAGS += -L$(BLDDIR)/lib -L$(BLDDIR)/usr/lib

CROSS_ENV = \
	PKG_CONFIG_PATH="$(BLDDIR)/usr/lib/pkgconfig" \
	CFLAGS="$(CFLAGS)" \
	CXXFLAGS="$(CXXFLAGS)" \
	CPPFLAGS="$(CPPFLAGS)" \
	LDFLAGS="$(LDFLAGS)"

