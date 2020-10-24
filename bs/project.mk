BLD ?= bld
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
	rm -rf $(BLDDIR)

###################################################################

# $1 binary name
define inner-project-binary

.PHONY: $(1)_clean $(1)

$(1)_DEPENDENCIES ?=
$(1)_SOURCES ?=
$(1)_MAKE_ENV ?=
$(1)_LIBRARIES ?=
$(1)_CPPFLAGS ?=
$(1)_LDFLAGS ?=
$(1)_CFLAGS ?=
$(1)_CXXFLAGS ?=
$(1)_PKGCONFIG ?=
$(1)_INSTALL_TARGET ?= YES
$(1)_OBJS = $(addsuffix .o, $(basename $($(1)_SOURCES)))

ifneq ($($(1)_LIBRARIES),)
    $(1)_CPPFLAGS += $$(shell $(CROSS_ENV) $(PKG_CONFIG) --cflags $($(1)_LIBRARIES))
    $(1)_LDFLAGS += $$(shell $(CROSS_ENV) $(PKG_CONFIG) --libs $($(1)_LIBRARIES))
endif

ifndef $(1)_BUILD_CMDS
define $(1)_BUILD_CMDS
	$$(CROSS_ENV) $$($(1)_MAKE_ENV) $$(CXX) $(CPPFLAGS) -o $(1) $$($(1)_OBJS) $$(LDFLAGS)
endef
endif

ifndef $(1)_INSTALL_CMDS
define $(1)_INSTALL_CMDS
	@mkdir -p $(DESTDIR)/usr/bin && \
	cp -f $(1) $(DESTDIR)/usr/bin/
endef
endif

ifndef $(1)_CLEAN_CMDS
define $(1)_CLEAN_CMDS
	rm -rf $$($(1)_OBJS) $$($(1)_OBJS:.o=.gcno) $$($(1)_OBJS:.o=.gcda) && \
	rm -rf gmon.out $(1) $(1).a && \
	rm -rf .ts_$(1)_*
endef
endif

.ts_$(1)_deps: | $($(1)_DEPENDENCIES)
	@echo "=== Building $(1) ==="
	@touch $$@

$$($(1)_OBJS): .ts_$(1)_deps

#.ts_$(1)_build: LDFLAGS = $(LDFLAGS) $$($(1)_LDFLAGS)
#.ts_$(1)_build: CPPFLAGS = $(CPPFLAGS) $$($(1)_CPPFLAGS)
#.ts_$(1)_build: CFLAGS = $(CFLAGS) $$($(1)_CFLAGS)
#.ts_$(1)_build: CXXFLAGS = $(CXXFLAGS) $$($(1)_CXXFLAGS)
.ts_$(1)_build: LDFLAGS += $$($(1)_LDFLAGS)
.ts_$(1)_build: CPPFLAGS += $$($(1)_CPPFLAGS)
.ts_$(1)_build: CFLAGS += $$($(1)_CFLAGS)
.ts_$(1)_build: CXXFLAGS += $$($(1)_CXXFLAGS)
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

ifndef $(1)_BUILD_CMDS
define $(1)_BUILD_CMDS
	$$(CROSS_ENV) $$($(1)_MAKE_ENV) $$(AR) rcs $(1).a $$($(1)_OBJS)
endef
endif

ifndef $(1)_INSTALL_CMDS
define $(1)_INSTALL_CMDS
	@mkdir -p $(DESTDIR)/usr/lib/
	@cp -f $(1).a $(DESTDIR)/usr/lib/
	@mkdir -p $(BLDDIR)/usr/lib/pkgconfig
	@for i in $$($(1)_PKGCONFIG); do \
	    cp -f $$$${i} $(BLDDIR)/usr/lib/pkgconfig/; \
	done
	DST=`realpath $(DESTDIR)` && cd src && find ./ -name \*.h -exec cp --parents {} $${DST}/include/ \; && cd ..
endef
endif

$(call inner-project-binary,$(1))

endef

project-static-lib = $(call inner-project-static-lib,$(PKG))

###################################################################

include platform-$(ARCH).mk
