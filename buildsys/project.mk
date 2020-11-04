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

LOG_LEVEL ?=
PKG_CONFIG ?= pkg-config
DOXYGEN_FILE ?= doxygen.cfg

.PHONY: clean all install docs

docs:
	doxygen $(DOXYGEN_FILE)

cppcheck:
	@cppcheck --force \
	    --enable=warning,style,performance,portability \
	    --suppress=information \
	    -q -I ./src \
	    --error-exitcode=1 src
	@echo "[DONE]"

clean:
	rm -rf $(STAGING_DIR)

%.o: %.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $(BS_EXTRA_CPPFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) $(BS_EXTRA_CPPFLAGS) -o $@ $<

###################################################################

# $1 binary name
define inner-project-binary

.PHONY: $(1)_clean $(1)

$(1)_DEPENDENCIES ?=
$(1)_NAME ?= $(1)
$(1)_SOURCES ?=
$(1)_MAKE_ENV ?=
$(1)_LIBRARIES ?=
$(1)_CPPFLAGS ?=
$(1)_LDFLAGS ?=
$(1)_CFLAGS ?=
$(1)_CXXFLAGS ?=
$(1)_EXTRA_CPPFLAGS ?=
$(1)_EXTRA_LDFLAGS ?=
$(1)_PKGCONFIG ?=
$(1)_CMAKE_MODULE ?=
$(1)_INSTALL_TARGET ?= YES
$(1)_OBJS = $(addsuffix .o, $(basename $($(1)_SOURCES)))

ifneq ($$($(1)_LIBRARIES),)
    $(1)_EXTRA_CPPFLAGS += $$(shell $(PKG_CONFIG_CROSS_ENV) $(PKG_CONFIG) --cflags $($(1)_LIBRARIES))
    $(1)_EXTRA_LDFLAGS += $$(shell $(PKG_CONFIG_CROSS_ENV) $(PKG_CONFIG) --libs $($(1)_LIBRARIES))
endif

ifndef $(1)_BUILD_CMDS
define $(1)_BUILD_CMDS
	$$(CROSS_ENV) $$($(1)_MAKE_ENV) $$(CXX) $$(CPPFLAGS) -o $$($(1)_NAME) $$($(1)_OBJS) $$(LDFLAGS) $$(BS_EXTRA_LDFLAGS)
endef
endif

ifndef $(1)_INSTALL_CMDS
define $(1)_INSTALL_CMDS
	@mkdir -p $(DESTDIR)/usr/bin && \
	cp -f $$($(1)_NAME) $(DESTDIR)/usr/bin/
endef
endif

ifndef $(1)_CLEAN_CMDS
define $(1)_CLEAN_CMDS
	rm -rf $$($(1)_OBJS) $$($(1)_OBJS:.o=.gcno) $$($(1)_OBJS:.o=.gcda) && \
	rm -rf gmon.out $$($(1)_NAME) && \
	rm -rf .ts_$(1)_*
endef
endif

.ts_$(1)_deps: | $($(1)_DEPENDENCIES)
	@echo "=== Building $(1) ==="
	@touch $$@

$$($(1)_OBJS): .ts_$(1)_deps
$$($(1)_OBJS): CPPFLAGS += $$($(1)_CPPFLAGS)
$$($(1)_OBJS): CFLAGS += $$($(1)_CFLAGS)
$$($(1)_OBJS): CXXFLAGS += $$($(1)_CXXFLAGS)
$$($(1)_OBJS): BS_EXTRA_CPPFLAGS = $$($(1)_EXTRA_CPPFLAGS)

.ts_$(1)_build: LDFLAGS += $$($(1)_LDFLAGS)
.ts_$(1)_build: CPPFLAGS += $$($(1)_CPPFLAGS)
.ts_$(1)_build: CFLAGS += $$($(1)_CFLAGS)
.ts_$(1)_build: CXXFLAGS += $$($(1)_CXXFLAGS)
.ts_$(1)_build: BS_EXTRA_CPPFLAGS = $$($(1)_EXTRA_CPPFLAGS)
.ts_$(1)_build: BS_EXTRA_LDFLAGS = $$($(1)_EXTRA_LDFLAGS)
.ts_$(1)_build: $$($(1)_OBJS)
	$$($(1)_BUILD_CMDS)
	@touch $$@

.ts_$(1)_install: .ts_$(1)_build
	$$($(1)_INSTALL_CMDS)
	@touch $$@

$(1)_clean:
	$$($(1)_CLEAN_CMDS)
	@echo "=== $(1): clean OK ==="

$(1): .ts_$(1)_build
	@echo "=== $(1): OK ==="

install: .ts_$(1)_install

clean: $(1)_clean

all: $(1)

endef

project-binary = $(call inner-project-binary,$(PKG))

###################################################################

# $1 library name
define inner-project-static-lib

$(1)_NAME ?= $(1).a

ifndef $(1)_BUILD_CMDS
define $(1)_BUILD_CMDS
	$$(CROSS_ENV) $$($(1)_MAKE_ENV) $$(AR) rcs $$($(1)_NAME) $$($(1)_OBJS)
endef
endif

ifndef $(1)_INSTALL_CMDS
define $(1)_INSTALL_CMDS
	@mkdir -p $(DESTDIR)/usr/lib/
	@cp -f $$($(1)_NAME) $(DESTDIR)/usr/lib/
	@mkdir -p $(DESTDIR)/usr/lib/pkgconfig $(DESTDIR)/usr/share/cmake $(DESTDIR)/usr/include
	@for i in $$($(1)_PKGCONFIG); do \
	    cp -f $$$${i} $(DESTDIR)/usr/lib/pkgconfig/; \
	done
	@for i in $$($(1)_CMAKE_MODULE); do \
	    cp -f $$$${i} $(DESTDIR)/usr/share/cmake/; \
	done
	DST=`realpath $(DESTDIR)` && cd src && find ./ -name \*.h -exec cp --parents {} $$$${DST}/usr/include/ \; && cd ..
endef
endif

$(call inner-project-binary,$(1))

endef

project-static-lib = $(call inner-project-static-lib,$(PKG))

###################################################################

