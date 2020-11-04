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



# $1 package name
define inner-package-make

$(1)_DEPENDENCIES ?=
$(1)_SRCDIR ?=
$(1)_MAKE_ENV ?=
$(1)_MAKE_OPTS ?=
$(1)_CLEAN_OPTS ?= clean
$(1)_PKGCONFIG ?=
$(1)_INSTALL_OPTS ?= install
$(1)_INSTALL_TARGET ?= YES

ifneq ($(wildcard $($(1)_SRCDIR)/Makefile),)

.PHONY: $(1)_clean $(1)

$($(1)_SRCDIR)/.ts_deps: | $($(1)_DEPENDENCIES)
	@echo "=== Building $(1) ==="
	@touch $$@

$($(1)_SRCDIR)/.ts_build: $($(1)_SRCDIR)/.ts_deps
	$$(CROSS_ENV) $(MAKE) -C $($(1)_SRCDIR) $$($(1)_MAKE_OPTS)
	@touch $$@

$($(1)_SRCDIR)/.ts_install: $($(1)_SRCDIR)/.ts_build
	$(MAKE) -C $($(1)_SRCDIR) DESTDIR=$(STAGING_DIR) $$($(1)_INSTALL_OPTS)
	@mkdir -p $(STAGING_DIR)/usr/lib/pkgconfig
	@for i in $$($(1)_PKGCONFIG); do \
	    cp -f $$$${i} $(STAGING_DIR)/usr/lib/pkgconfig/; \
	done
	@touch $$@

ifeq ($$($(1)_INSTALL_TARGET),YES)
$(1): $($(1)_SRCDIR)/.ts_install
else
$(1): $($(1)_SRCDIR)/.ts_build
endif

$(1)_clean:
	@rm -f $($(1)_SRCDIR)/.ts_*
	$(MAKE) -C $($(1)_SRCDIR) $$($(1)_CLEAN_OPTS)
	@echo "=== $(1): clean OK ==="

clean: $(1)_clean

$(1):
	@echo "=== $(1): OK ==="

else

$(1):
	@echo "=== $(1): SKIPPED, no sources ==="
endif

endef

package-make = $(call inner-package-make,$(PKG))
