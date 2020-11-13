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

BASEDIR            := $(CURDIR)
CONFIG             := $(CURDIR)/.config.mk

include $(BASEDIR)/make/functions.mk
include $(BASEDIR)/dependencies.mk
ifneq ($(TREE),1)
  -include $(CONFIG)
endif
include $(BASEDIR)/project.mk

SYS_DEPENDENCIES        = $(DEPENDENCIES) $(TEST_DEPENDENCIES)
ALL_DEPENDENCIES_UNIQ   = $(call uniq,$(ALL_DEPENDENCIES))

# Find the proper branch of the GIT repository
ifeq ($(TREE),1)
  MODULES            := $(BASEDIR)/modules
  GIT                := git
  
  ifeq ($(findstring -devel,$(ARTIFACT_VERSION)),-devel)
    $(foreach dep, $(ALL_DEPENDENCIES_UNIQ), \
      $(eval $(dep)_BRANCH=devel) \
      $(eval $(dep)_PATH=$(MODULES)/$($(dep)_NAME)) \
    )
  else
    $(foreach dep, $(ALL_DEPENDENCIES_UNIQ), \
      $(eval $(dep)_BRANCH="$($(dep)_VERSION)") \
      $(eval $(dep)_PATH=$(MODULES)/$($(dep)_NAME)) \
    )
  endif
endif

# Form list of modules, exclude all modules that have 'system' version
SRC_MODULES         = $(foreach dep, $(SYS_DEPENDENCIES), $(if $(findstring src,$($(dep)_TYPE)),$(dep)))
HDR_MODULES         = $(foreach dep, $(SYS_DEPENDENCIES), $(if $(findstring hdr,$($(dep)_TYPE)),$(dep)))
ALL_SRC_MODULES     = $(foreach dep, $(ALL_DEPENDENCIES_UNIQ), $(if $(findstring src,$($(dep)_TYPE)),$(dep)))
ALL_HDR_MODULES     = $(foreach dep, $(ALL_DEPENDENCIES_UNIQ), $(if $(findstring hdr,$($(dep)_TYPE)),$(dep)))
ALL_PATHS           = $(foreach dep, $(ALL_SRC_MODULES) $(ALL_HDR_MODULES), $($(dep)_PATH))

# Branches
.PHONY: $(ALL_SRC_MODULES) $(ALL_HDR_MODULES) $(ALL_PATHS)
.PHONY: fetch prune clean

$(ALL_SRC_MODULES) $(ALL_HDR_MODULES):
	@echo "Cloning $($(@)_URL) -> $($(@)_PATH) [$($(@)_BRANCH)]"
	@test -f "$($(@)_PATH)/.git/config" || $(GIT) clone "$($(@)_URL)" "$($(@)_PATH)"
	@$(GIT) -C "$($(@)_PATH)" reset --hard
	@$(GIT) -C "$($(@)_PATH)" fetch origin --force
	@$(GIT) -C "$($(@)_PATH)" fetch origin '+refs/heads/*:refs/tags/*' --force
	@$(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_NAME)-$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_NAME)-$($(@)_BRANCH)

$(ALL_PATHS):
	@echo "Removing $(notdir $(@))"
	@-rm -rf $(@)

fetch: $(SRC_MODULES) $(HDR_MODULES)

tree: $(ALL_SRC_MODULES) $(ALL_HDR_MODULES)

clean:
	@echo rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"
	@-rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"

prune: $(ALL_PATHS)

