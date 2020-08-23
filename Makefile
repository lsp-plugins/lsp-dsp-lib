#!/usr/bin/make -f
#
# Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
#
# This file is part of lsp-dsp-lib
#
# lsp-dsp-lib is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# any later version.
#
# lsp-dsp-lib is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with lsp-dsp-lib.  If not, see <https://www.gnu.org/licenses/>.
#

# Location
BASEDIR                    := $(CURDIR)
MODULES                    := $(BASEDIR)/modules
BUILDDIR                   := $(BASEDIR)/.build
CONFIG                     := $(BASEDIR)/.config.mk

# Prerequisites
MAKE_VERSION_REQUIRED      := 4.0
MAKE_VERSION_CHECK         := $(filter $(MAKE_VERSION_REQUIRED), $(firstword $(sort $(MAKE_VERSION) $(MAKE_VERSION_REQUIRED))))
ifeq ("$(MAKE_VERSION_CHECK)", "")
$(error make is too old, at least version $(MAKE_VERSION_REQUIRED) is required.)
endif

GCC_VERSION_REQUIRED       := 4.9
GCC_VERSION                := $(shell gcc -dumpversion)
GCC_VERSION_CHECK          := $(filter $(GCC_VERSION_REQUIRED), $(firstword $(sort $(GCC_VERSION) $(GCC_VERSION_REQUIRED))))
ifeq ("$(GCC_VERSION_CHECK)", "")
$(error gcc is too old, at least version $(GCC_VERSION_REQUIRED) is required.)
endif

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
