BASEDIR            := $(CURDIR)
CONFIG             := $(CURDIR)/.config.mk

include $(CONFIG)
include $(BASEDIR)/project.mk

DEPENDENCIES       += $(TEST_DEPENDENCIES)

# Form list of modules, exclude all modules that have 'system' version
MODULES             = $(foreach dep, $(DEPENDENCIES), $(if $(findstring system,$($(dep)_VERSION)),,$(dep)))

# Branches
.PHONY: $(MODULES)
.PHONY: fetch prune clean

$(MODULES):
	@echo "Cloning $($(@)_URL) -> $($(@)_PATH) [$($(@)_BRANCH)]"
	@test -f "$($(@)_PATH)/.git/config" || $(GIT) clone "$($(@)_URL)" "$($(@)_PATH)"
	@$(GIT) -C "$($(@)_PATH)" fetch origin --prune --force
	@$(GIT) -C "$($(@)_PATH)" fetch origin --prune '+refs/heads/*:refs/tags/*' --force
	@$(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout origin/$($(@)_BRANCH) || \
	 $(GIT) -c advice.detachedHead=false -C "$($(@)_PATH)" checkout refs/tags/$($(@)_BRANCH)

fetch: $(MODULES)

clean:
	@echo rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"
	@-rm -rf "$($(ARTIFACT_VARS)_BIN)/$(ARTIFACT_NAME)"

prune:
	@-find 'modules' -mindepth 1 -maxdepth 1 -type d -exec rm -rf '{}' \;

