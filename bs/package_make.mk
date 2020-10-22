# $(1)_DEPENDENCIES
# $(1)_SRCDIR

# $1 package name
define inner-package-make

# $(1)_LDFLAGS = $$(shell $(PKG_CONFIG) --libs tinyhal)
# $(1)_CPPFLAGS = $$(shell $(PKG_CONFIG) --cflags tinyhal)

$(1)_DEPENDENCIES ?=
$(1)_SRCDIR ?=
$(1)_MAKE_ENV ?=
$(1)_MAKE_OPTS ?=
$(1)_CLEAN_OPTS ?= clean
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
	$(MAKE) -C $($(1)_SRCDIR) DESTDIR=$(BLDDIR) $$$((1)_INSTALL_OPTS)
	@touch $$@

ifeq ($($(1)_INSTALL_TARGET),YES)
$(1): $($(1)_SRCDIR)/.ts_install
else
$(1): $($(1)_SRCDIR)/.ts_build
endif

$(1)_clean:
	@rm -f $($(1)_SRCDIR)/.ts_*
	$(MAKE) -C $($(1)_SRCDIR) $$($(1)_CLEAN_OPTS)

clean: $(1)_clean

$(1):
	@echo "=== $(1): OK ==="

#    LDFLAGS += $(HAL_LDFLAGS)
#    CPPFLAGS += $(HAL_CFLAGS)
else

$(1):
	@echo "=== $(1): SKIPPED, no sources ==="
endif

endef

package-make = $(call inner-package-make,$(PKG))
