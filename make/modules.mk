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

include $(CONFIG)
include $(BASEDIR)/project.mk

DEPENDENCIES       += $(TEST_DEPENDENCIES)

# Form list of modules, exclude all modules that have 'system' version
SRC_MODULES         = $(foreach dep, $(DEPENDENCIES), $(if $(findstring src,$($(dep)_TYPE)),$(dep)))
HDR_MODULES         = $(foreach dep, $(DEPENDENCIES), $(if $(findstring hdr,$($(dep)_TYPE)),$(dep)))

# Branches
.PHONY: $(SRC_MODULES) $(HDR_MODULES)
.PHONY: fetch prune clean

$(SRC_MODULES) $(HDR_MODULES):
	@echo "Cloning $($(@)_URL) -> $($(@)_PATH) [$($(@)_BRANCH)]"
	@test -f "$($(@)_PATH)/.git/config" || $(GIT) clone "$($(@)_URL)" "$($(@)_PATH)"
	@$(GIT) -C "$($(@)_PATH)" reset --hard
	@$(GIT) -C "$($(@)_PATH)" fetch origin --force
	@$(GIT) -C "$($(@)_PATH)" fetch origin '+refs/heads/*:refs/tags/*' --force
	@$(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_BRANCH)

fetch: $(SRC_MODULES) $(HDR_MODULES)

clean:
	@echo rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"
	@-rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"

prune:
	@-find 'modules' -mindepth 1 -maxdepth 1 -type d -exec rm -rf '{}' \;

