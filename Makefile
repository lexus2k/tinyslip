default: tinyslip
ARCH ?= linux
DESTDIR ?= .

include bs/toolchain.mk
include bs/package_make.mk

###################### Added HAL library #########################

PKG = tinyhal
$(PKG)_SRCDIR = ./tinyhal
$(eval $(package-make))

####################### Compiling library #########################

PKG = tinyslip
$(PKG)_SRCDIR = .
$(PKG)_DEPENDENCIES = tinyhal
$(PKG)_MAKE_OPTS = -f Makefile.$(ARCH)
$(PKG)_CLEAN_OPTS = -f Makefile.$(ARCH) clean
$(PKG)_INSTALL_OPTS = -f Makefile.$(ARCH) install DESTDIR=$(DESTDIR)
$(eval $(package-make))

