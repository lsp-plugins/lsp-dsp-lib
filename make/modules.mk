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

