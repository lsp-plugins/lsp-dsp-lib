CONFIG                 := $(CURDIR)/.config.mk

include $(CONFIG)
include $(CURDIR)/../project.mk

rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))
CXX_SRCMAIN             = $(call rwildcard, main, *.cpp)
CXX_SRCTEST             = $(call rwildcard, test, *.cpp)
CXX_SRC                 = $(CXX_SRCMAIN)
CXX_OBJ                 = $(patsubst %.cpp, $($(ARTIFACT_VARS)_BIN)/%.o, $(CXX_SRC))
CXX_FILE                = $(patsubst $($(ARTIFACT_VARS)_BIN)/%.o,%.cpp, $(@))
CXX_HEADERS             = $(call rwildcard, $($(ARTIFACT_VARS)_INC), *.h)
CXX_INSTHEADERS         = $(patsubst $($(ARTIFACT_VARS)_INC)/%,$(DESTDIR)$(PREFIX)/include/%,$(CXX_HEADERS))

ARTIFACT_BIN            = $($(ARTIFACT_VARS)_BIN)
ARTIFACT_LIB            = $(ARTIFACT_BIN)/$($(ARTIFACT_VARS)_NAME)-$($(ARTIFACT_VARS)_VERSION)$(LIBRARY_EXT)
ARTIFACT_TEST           = $(ARTIFACT_BIN)/$($(ARTIFACT_VARS)_NAME)-test$(EXECUTABLE_EXT)
ARTIFACT_OBJ            = $($(ARTIFACT_VARS)_OBJ)
ARTIFACT_MFLAGS         = $($(ARTIFACT_VARS)_MFLAGS)

INCLUDE_DEPS            = $(foreach dep, $(DEPENDENCIES) $(ARTIFACT_VARS), $(if $($(dep)_INC), -I$($(dep)_INC)))
BUILD_DEPS              = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_OBJ), $(dep)))
LINKER_OBJS             = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_OBJ), $($(dep)_OBJ)))
LINKER_LIBS             = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_LIB), $($(dep)_LIB)))
BUILD_ALL               = $(ARTIFACT_LIB)

ifeq ($(TEST),1)
  CXX_SRC                += $(CXX_SRCTEST)
  BUILD_ALL              += $(ARTIFACT_TEST)
endif

CXX_DEPS                = $(foreach src,$(CXX_SRCMAIN) $(CXX_SRCTEST),$(patsubst %.cpp,$(ARTIFACT_BIN)/%.d,$(src)))
CXX_DEPFILE             = $(patsubst $(ARTIFACT_BIN)/%.d,%.cpp,$(@))
CXX_DEPTARGET           = $(patsubst $(ARTIFACT_BIN)/%.d,%.o,$(@))

.DEFAULT_GOAL = all
.PHONY: compile depend dep_clean all install uninstall
.PHONY: $(BUILD_DEPS)

compile: $(ARTIFACT_OBJ)

all: $(BUILD_ALL)

dep_clean:

$(CXX_DEPS): dep_clean
	@echo "  dep  $(CXX_DEPFILE)"
	@mkdir -p $(dir $(@))
	$(CXX) -MM -MT "\$$($(ARTIFACT_VARS)_BIN)/$(CXX_DEPTARGET)" -MF $(@) $(CXX_DEPFILE) $(CXXFLAGS) $(INCLUDE) $(INCLUDE_DEPS)

depend: $(CXX_DEPS)
	$(foreach dep,$(DEPENDENCIES) $(ARTIFACT_VARS),\
	  $(if $($(dep)_INC), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_INC)" | sed "s/\\//\\\\\//g")/\$$$$\\($(dep)_INC\\)/g;)\
	  )\
	  $(if $($(dep)_BIN), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_BIN)" | sed "s/\\//\\\\\//g")/\\$$$$\\($(dep)_BIN\\)/g;)\
	  )\
	)
	@cat $(CXX_DEPS) | sed -E "$(SED_RULES)" >Makefile.d

$(BUILD_DEPS):
	@echo "make $(notdir $($(@)_OBJ))"
	@$(MAKE) -s -C "$($(@)_PATH)" compile CONFIG="$(CONFIG)"

$(ARTIFACT_OBJ): $(BUILD_DEPS) $(CXX_OBJ)
	@echo "  $(LD)   $(notdir $(ARTIFACT_OBJ))"
	@$(LD) -o $(ARTIFACT_OBJ) -r $(CXX_OBJ)

$(ARTIFACT_LIB): $(ARTIFACT_OBJ)
	@echo "  $(CXX)  $(notdir $(ARTIFACT_LIB))"
	@$(CXX) -o $(ARTIFACT_LIB) $(LINKER_OBJS) $(ARTIFACT_OBJ) $(SO_FLAGS) $(LINKER_LIBS)

$(ARTIFACT_TEST): $(ARTIFACT_OBJ)
	@echo "  $(CXX)  $(notdir $(ARTIFACT_TEST))"
	@$(CXX) -o $(ARTIFACT_TEST) $(LINKER_OBJS) $(ARTIFACT_OBJ) $(EXE_FLAGS) $(LINKER_LIBS)

$(CXX_OBJ):
	@echo "  $(CXX)  $(CXX_FILE)"
	@mkdir -p $(dir $@)
	@$(CXX) -o $(@) -c $(CXX_FILE) -fPIC $(CXXFLAGS) $(ARTIFACT_MFLAGS) $(INCLUDE) $(INCLUDE_DEPS)

install: all
	@echo "Installing $($(ARTIFACT_VARS)_NAME)"
	@mkdir -p "$(DESTDIR)$(PREFIX)/include"
	@mkdir -p "$(DESTDIR)$(PREFIX)/lib"
	@cp -r $($(ARTIFACT_VARS)_INC)/* "$(DESTDIR)$(PREFIX)/include/"
	@echo $(INSTALL) $(ARTIFACT_LIB) -t "$(DESTDIR)$(PREFIX)/lib"
	@$(INSTALL) $(ARTIFACT_LIB) -t "$(DESTDIR)$(PREFIX)/lib"
	@ln -sf $(notdir $(ARTIFACT_LIB)) "$(DESTDIR)$(PREFIX)/lib/$($(ARTIFACT_VARS)_NAME)$(LIBRARY_EXT)"
	@echo "Install OK"

uninstall:
	@echo "Unnstalling $($(ARTIFACT_VARS)_NAME)"
	@-rm -f "$(DESTDIR)$(PREFIX)/lib/$(notdir $(ARTIFACT_LIB))"
	@-rm -f "$(DESTDIR)$(PREFIX)/lib/$($(ARTIFACT_VARS)_NAME)$(LIBRARY_EXT)"
	@echo rm -f $(CXX_INSTHEADERS)
	@-rm -f $(CXX_INSTHEADERS)
	@echo "Uninstall OK"

# Dependencies
-include $(CURDIR)/Makefile.d
