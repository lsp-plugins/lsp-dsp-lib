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

# Definitions
PREFIX                     := /usr/local
LIBDIR                     := $(PREFIX)/lib
BINDIR                     := $(PREFIX)/bin
INCDIR                     := $(PREFIX)/include
BASEDIR                    := $(CURDIR)
BUILDDIR                   := $(BASEDIR)/.build
MODULES                    := $(BASEDIR)/modules
CONFIG                     := $(BASEDIR)/.config.mk
TEST                       := 0
DEBUG                      := 0
PROFILE                    := 0
TRACE                      := 0

include $(BASEDIR)/make/functions.mk
include $(BASEDIR)/make/system.mk
include $(BASEDIR)/make/tools.mk
include $(BASEDIR)/dependencies.mk
include $(BASEDIR)/project.mk

# Compute the full list of dependencies
UNIQ_DEPENDENCIES          := $(call uniq, $(DEPENDENCIES) $(TEST_DEPENDENCIES))
DEPENDENCIES                = $(UNIQ_DEPENDENCIES)

# Determine versions
ifeq ($(findstring -devel,$(ARTIFACT_VERSION)),-devel)
  $(foreach dep, $(DEPENDENCIES), \
    $(eval $(dep)_BRANCH=devel) \
  )
  # Strip '-devel' from version
  tmp_version :=$(shell echo "$(ARTIFACT_VERSION)" | sed s/-devel//g)
  ARTIFACT_VERSION=$(tmp_version)
else
  $(foreach dep, $(DEPENDENCIES), \
    $(eval \
      $(dep)_BRANCH="$($(dep)_VERSION)" \
    ) \
  )
endif

define pkgconfig =
  $(eval name=$(1))
  $(if $($(name)_NAME), \
    $(if $($(name)_CFLAGS),,  $(eval $(name)_CFLAGS  := $(shell pkg-config --cflags "$($(name)_NAME)"))) \
  )
  $(if $($(name)_NAME), \
    $(if $($(name)_LDLAGS),,  $(eval $(name)_LDFLAGS := $(shell pkg-config --libs "$($(name)_NAME)"))) \
  )
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     :=))
endef

define libconfig =
  $(eval name=$(1))
  $(if $($(name)_NAME), \
    $(if $($(name)_LDLAGS),,  $(eval $(name)_LDFLAGS := -l$($(name)_NAME))) \
  )
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     :=))
endef

define optconfig =
  $(eval name=$(1))
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     :=))
endef

define srcconfig =
  $(eval name=$(1))
  $(eval builtin=$(patsubst $(ARTIFACT_NAME),,$($(name)_NAME)))
  $(if $($(name)_PATH),,    $(eval $(name)_PATH    := $(MODULES)/$($(name)_NAME)))
  $(if $($(name)_DESC),,    $(eval $(name)_DESC    := $($(name)_DESC)))
  $(if $($(name)_INC),,     $(eval $(name)_INC     := $($(name)_PATH)/include))
  $(if $($(name)_SRC),,     $(eval $(name)_SRC     := $($(name)_PATH)/src))
  $(if $($(name)_TEST),,    $(eval $(name)_TEST    := $($(name)_PATH)/test))
  $(if $($(name)_TESTING),, $(eval $(name)_TESTING := 0))
  $(if $($(name)_BIN),,     $(eval $(name)_BIN     := $(BUILDDIR)/$($(name)_NAME)))
  $(if $($(name)_CFLAGS),,  $(eval $(name)_CFLAGS  := "-I\"$($(name)_INC)\"" $(if $(builtin),"-D$(name)_BUILTIN")))
  $(if $($(name)_LDLAGS),,  $(eval $(name)_LDFLAGS :=))
  $(if $($(name)_OBJ),,     $(eval $(name)_OBJ     := "$($(name)_BIN)/$($(name)_NAME).o"))
  $(if $($(name)_MFLAGS),,  $(eval $(name)_MFLAGS  := $(if $(builtin),"-D$(name)_BUILTIN -fvisibility=hidden")))
endef

define hdrconfig =
  $(eval name=$(1))
  $(eval builtin=$(patsubst $(ARTIFACT_NAME),,$($(name)_NAME)))
  $(if $($(name)_PATH),,    $(eval $(name)_PATH    := $(MODULES)/$($(name)_NAME)))
  $(if $($(name)_DESC),,    $(eval $(name)_DESC    := $($(name)_DESC)))
  $(if $($(name)_INC),,     $(eval $(name)_INC     := $($(name)_PATH)/include))
  $(if $($(name)_TESTING),, $(eval $(name)_TESTING := 0))
  $(if $($(name)_CFLAGS),,  $(eval $(name)_CFLAGS  := "-I\"$($(name)_INC)\"" $(if $(builtin),"-D$(name)_BUILTIN")))
  $(if $($(name)_MFLAGS),,  $(eval $(name)_MFLAGS  := $(if $(builtin),"-D$(name)_BUILTIN -fvisibility=hidden")))
endef

define vardef =
  $(eval name = $(1))
  # Override variables if they are not defined
  $(if $(findstring pkg,$($(name)_TYPE)), $(eval $(call pkgconfig, $(name))))
  $(if $(findstring src,$($(name)_TYPE)), $(eval $(call srcconfig, $(name))))
  $(if $(findstring hdr,$($(name)_TYPE)), $(eval $(call hdrconfig, $(name))))
  $(if $(findstring lib,$($(name)_TYPE)), $(eval $(call libconfig, $(name))))
  $(if $(findstring opt,$($(name)_TYPE)), $(eval $(call optconfig, $(name))))
endef

# Define predefined variables
ifndef $(ARTIFACT_ID)_NAME
  $(ARTIFACT_ID)_NAME        := $(ARTIFACT_NAME)
endif
ifndef $(ARTIFACT_ID)_DESC
  $(ARTIFACT_ID)_DESC        := $(ARTIFACT_DESC)
endif
ifndef $(ARTIFACT_ID)_VERSION 
  $(ARTIFACT_ID)_VERSION     := $(ARTIFACT_VERSION)
endif
ifndef $(ARTIFACT_ID)_PATH
  $(ARTIFACT_ID)_PATH        := $(BASEDIR)
endif

$(ARTIFACT_ID)_TESTING      = $(TEST)
$(ARTIFACT_ID)_TYPE        := src

OVERALL_DEPS := $(DEPENDENCIES) $(ARTIFACT_ID)
__tmp := $(foreach dep,$(OVERALL_DEPS),$(call vardef, $(dep)))

CONFIG_VARS = \
  $(COMMON_VARS) \
  $(TOOL_VARS) \
  $(foreach name, $(OVERALL_DEPS), \
    $(name)_NAME \
    $(name)_DESC \
    $(name)_VERSION \
    $(name)_TYPE \
    $(name)_BRANCH \
    $(name)_PATH \
    $(name)_INC \
    $(name)_SRC \
    $(name)_TEST \
    $(name)_TESTING \
    $(name)_URL \
    $(name)_BIN \
    $(name)_CFLAGS \
    $(name)_MFLAGS \
    $(name)_LDFLAGS \
    $(name)_OBJ \
  )

.DEFAULT_GOAL      := config
.PHONY: config prepare help
.PHONY: $(CONFIG_VARS)

prepare:
	@echo "Configuring build..."
	@echo "# Project settings" > "$(CONFIG)"

$(CONFIG_VARS): prepare
	@echo "$(@)=$($(@))" >> "$(CONFIG)"

config: $(CONFIG_VARS)
	@echo "Configured OK"

help: | toolvars sysvars
	@echo ""
	@echo "List of variables for each dependency:"
	@echo "  <ARTIFACT>_BIN            location to put all binaries when building artifact"
	@echo "  <ARTIFACT>_BRANCH         git branch used to checkout source code"
	@echo "  <ARTIFACT>_CFLAGS         C/C++ flags to access headers of the artifact"
	@echo "  <ARTIFACT>_DESC           Full description of the artifact"
	@echo "  <ARTIFACT>_INC            path to include files of the artifact"
	@echo "  <ARTIFACT>_LDFLAGS        linker flags to link with artifact"
	@echo "  <ARTIFACT>_MFLAGS         artifact-specific compilation flags"
	@echo "  <ARTIFACT>_NAME           the artifact name used in pathnames"
	@echo "  <ARTIFACT>_OBJ            path to output object file for artifact"
	@echo "  <ARTIFACT>_PATH           location of the source code of the artifact"
	@echo "  <ARTIFACT>_SRC            path to source code files of the artifact"
	@echo "  <ARTIFACT>_TEST           location of test files of the artifact"
	@echo "  <ARTIFACT>_TYPE           artifact usage type"
	@echo "                            - src - use sources and headers from git"
	@echo "                            - hdr - use headers only from git"
	@echo "                            - pkg - use pkgconfig for configuration"
	@echo "                            - lib - use system headers and -l<libname> flags"
	@echo "                            - opt - use optional configuration"
	@echo "  <ARTIFACT>_URL            location of the artifact git repoisitory"
	@echo "  <ARTIFACT>_VERSION        version of the artifact used for building"
	@echo ""
	@echo "Artifacts used for build:"
	@echo "  $(DEPENDENCIES)"

