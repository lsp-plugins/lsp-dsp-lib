#!/usr/bin/make -f

# Location
BASEDIR                    := $(CURDIR)
MODULES                    := $(BASEDIR)/modules
BUILDDIR                   := $(BASEDIR)/.build
CONFIG                     := $(BASEDIR)/.config.mk

# Basic initialization
# Checks
ifeq ("$(wildcard $(CONFIG))", "")
  CONFIGURED          = 0
else
  CONFIGURED          = 1
endif

include $(BASEDIR)/project.mk

# Setup paths
CHK_CONFIG                  = test -f "$(CONFIG)" || (echo "System not properly configured. Please launch 'make config' first" && exit 1)

.DEFAULT_GOAL              := all
.PHONY: all compile install uninstall depend clean

compile all install uninstall depend:
	@$(CHK_CONFIG)
	@$(MAKE) -s -C "$(BASEDIR)/src" $(@) CONFIG="$(CONFIG)" DESTDIR="$(DESTDIR)"

clean:
	@echo "Cleaning build directory $(BUILDDIR)"
	@-rm -rf $(BUILDDIR)
	@echo "Clean OK"
	
# Module-related tasks
.PHONY: fetch prune
fetch:
	@echo "Fetching source code dependencies"
	@$(MAKE) -s -f "make/modules.mk" $(@) BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	@echo "Fetch OK"

prune: clean
	@echo "Pruning the whole project tree"
	@$(MAKE) -s -f "make/modules.mk" prune BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	@-rm -rf "$(CONFIG)"
	@echo "Prune OK"

# Configuration-related targets
.PHONY: config help

testconfig:
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@) CONFIG="$(CONFIG)" TEST="1" $(MAKEFLAGS)

config:
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@) CONFIG="$(CONFIG)" $(MAKEFLAGS)

help:
	@echo "Available targets:"
	@echo "  all                       Build all binaries"
	@echo "  clean                     Clean all build files and configuration file"
	@echo "  config                    Configure build"
	@echo "  depend                    Update build dependencies for current project"
	@echo "  fetch                     Fetch all source code dependencies from git"
	@echo "  help                      Print this help message"
	@echo "  info                      Output build configuration"
	@echo "  install                   Install all binaries into the system"
	@echo "  uninstall                 Uninstall binaries"
	@echo ""
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@)
	@echo ""
