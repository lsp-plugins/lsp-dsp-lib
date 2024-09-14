#!/usr/bin/make -f
#
# Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
#           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

# Command-line flag to silence nested $(MAKE).
ifneq ($(VERBOSE),1)
.SILENT:
endif

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
DISTSRC_PATH                = $(BUILDDIR)/distsrc
DISTSRC                     = $(DISTSRC_PATH)/$(ARTIFACT_NAME)

.DEFAULT_GOAL              := all
.PHONY: all compile install uninstall clean

compile all install uninstall:
	$(CHK_CONFIG)
	$(MAKE) -C "$(BASEDIR)/src" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" DESTDIR="$(DESTDIR)"

clean:
	echo "Cleaning build directory $(BUILDDIR)"
	-rm -rf $(BUILDDIR)
	echo "Clean OK"
	
# Module-related tasks
.PHONY: fetch tree prune
fetch:
	$(CHK_CONFIG)
	echo "Fetching desired source code dependencies"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	echo "Fetch OK"
	
tree:
	echo "Fetching all possible source code dependencies"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" $(@) VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	echo "Fetch OK"

prune: clean
	echo "Pruning the whole project tree"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" prune VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" TREE="1"
	-rm -rf "$(CONFIG)"
	echo "Prune OK"

# Configuration-related targets
.PHONY: config help chkconfig

testconfig:
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" TEST="1" -$(MAKEFLAGS)

config:
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" -$(MAKEFLAGS)

# Release-related targets
.PHONY: distsrc
distsrc:
	echo "Building source code archive"
	mkdir -p "$(DISTSRC)/modules"
	$(MAKE) -f "$(BASEDIR)/make/modules.mk" tree VERBOSE="$(VERBOSE)" BASEDIR="$(BASEDIR)" MODULES="$(DISTSRC)/modules" TREE="1"
	cp -R $(BASEDIR)/include $(BASEDIR)/make $(BASEDIR)/src "$(DISTSRC)/"
	cp $(BASEDIR)/CHANGELOG $(BASEDIR)/COPYING* $(BASEDIR)/Makefile $(BASEDIR)/*.mk "$(DISTSRC)/"
	find "$(DISTSRC)" -iname '.git' | xargs rm -rf {}
	find "$(DISTSRC)" -iname '.gitignore' | xargs rm -rf {}
	tar -C $(DISTSRC_PATH) -czf "$(BUILDDIR)/$(ARTIFACT_NAME)-src-$(ARTIFACT_VERSION).tar.gz" "$(ARTIFACT_NAME)"
	echo "Created archive: $(BUILDDIR)/$(ARTIFACT_NAME)-src-$(ARTIFACT_VERSION).tar.gz"
	rm -rf $(DISTSRC_PATH)
	echo "Build OK"

# Help
help:
	echo "Available targets:"
	echo "  all                       Build all binaries"
	echo "  clean                     Clean all build files and configuration file"
	echo "  config                    Configure build"
	echo "  distsrc                   Make tarball with source code for packagers"
	echo "  fetch                     Fetch all desired source code dependencies from git"
	echo "  help                      Print this help message"
	echo "  info                      Output build configuration"
	echo "  install                   Install all binaries into the system"
	echo "  prune                     Cleanup build and all fetched dependencies from git"
	echo "  tree                      Fetch all possible source code dependencies from git"
	echo "                            to make source code portable between machines"
	echo "  uninstall                 Uninstall binaries"
	echo ""
	$(MAKE) -f "$(BASEDIR)/make/configure.mk" $(@) VERBOSE="$(VERBOSE)"
	echo ""
