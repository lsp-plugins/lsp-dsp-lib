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
DISTSRC_PATH                = $(BUILDDIR)/.distsrc
DISTSRC                     = $(DISTSRC_PATH)/$(ARTIFACT_NAME)

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
.PHONY: fetch tree prune
fetch:
	@$(CHK_CONFIG)
	@echo "Fetching desired source code dependencies"
	@$(MAKE) -s -f "$(BASEDIR)/make/modules.mk" $(@) BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	@echo "Fetch OK"
	
tree:
	@echo "Fetching all possible source code dependencies"
	@$(MAKE) -s -f "$(BASEDIR)/make/modules.mk" $(@) BASEDIR="$(BASEDIR)" TREE="1"
	@echo "Fetch OK"

prune: clean
	@echo "Pruning the whole project tree"
	@$(MAKE) -s -f "$(BASEDIR)/make/modules.mk" prune BASEDIR="$(BASEDIR)" CONFIG="$(CONFIG)"
	@$(MAKE) -s -f "$(BASEDIR)/make/modules.mk" prune BASEDIR="$(BASEDIR)" TREE="1"
	@-rm -rf "$(CONFIG)"
	@echo "Prune OK"

# Configuration-related targets
.PHONY: config help chkconfig

testconfig:
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@) CONFIG="$(CONFIG)" TEST="1" $(MAKEFLAGS)

config:
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@) CONFIG="$(CONFIG)" $(MAKEFLAGS)

# Release-related targets
.PHONY: distsrc
distsrc:
	@echo "Building source code archive"
	@mkdir -p "$(DISTSRC)/modules"
	@$(MAKE) -s -f "$(BASEDIR)/make/modules.mk" tree BASEDIR="$(BASEDIR)" MODULES="$(DISTSRC)/modules" TREE="1"
	@cp -R $(BASEDIR)/include $(BASEDIR)/make $(BASEDIR)/src "$(DISTSRC)/"
	@cp $(BASEDIR)/CHANGELOG $(BASEDIR)/COPYING* $(BASEDIR)/Makefile $(BASEDIR)/*.mk "$(DISTSRC)/"
	@find "$(DISTSRC)" -iname '.git' | xargs -exec rm -rf {}
	@find "$(DISTSRC)" -iname '.gitignore' | xargs -exec rm -rf {}
	@tar -C $(DISTSRC_PATH) -czf "$(BUILDDIR)/$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz" "$(ARTIFACT_NAME)"
	@echo "Created archive: $(BUILDDIR)/$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz"
	@ln -sf "$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)-src.tar.gz" "$(BUILDDIR)/$(ARTIFACT_NAME)-src.tar.gz"
	@echo "Created symlink: $(BUILDDIR)/$(ARTIFACT_NAME)-src.tar.gz"
	@rm -rf $(DISTSRC_PATH)
	@echo "Build OK"

# Help
help:
	@echo "Available targets:"
	@echo "  all                       Build all binaries"
	@echo "  clean                     Clean all build files and configuration file"
	@echo "  config                    Configure build"
	@echo "  depend                    Update build dependencies for current project"
	@echo "  distsrc                   Make tarball with source code for packagers"
	@echo "  fetch                     Fetch all desired source code dependencies from git"
	@echo "  help                      Print this help message"
	@echo "  info                      Output build configuration"
	@echo "  install                   Install all binaries into the system"
	@echo "  prune                     Cleanup build and all fetched dependencies from git"
	@echo "  tree                      Fetch all possible source code dependencies from git"
	@echo "                            to make source code portable between machines"
	@echo "  uninstall                 Uninstall binaries"
	@echo ""
	@$(MAKE) -s -f "$(BASEDIR)/make/configure.mk" $(@)
	@echo ""
