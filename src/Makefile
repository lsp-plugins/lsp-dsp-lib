CONFIG                 := $(CURDIR)/.config.mk

include $(CONFIG)
include $(CURDIR)/../project.mk

rwildcard               = $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2) $(filter $(subst *,%,$2),$d))

CXX_SRCMAIN             =
CXX_SRCEXT              = main/dsp.cpp main/dsp_c.cpp main/generic/generic.cpp
CXX_SRCALL              = $(call rwildcard, main, *.cpp)
CXX_SRCTEST             = $(call rwildcard, test, *.cpp)
CXX_SRC                 = $(CXX_SRCMAIN)

CXX_OBJ                 = $(patsubst %.cpp, $($(ARTIFACT_VARS)_BIN)/%.o, $(CXX_SRC))
CXX_OBJEXT              = $(patsubst %.cpp, $($(ARTIFACT_VARS)_BIN)/%.o, $(CXX_SRCEXT))

ALL_HEADERS             = $(call rwildcard, $($(ARTIFACT_VARS)_INC), *.h)
CXX_FILE                = $(patsubst $($(ARTIFACT_VARS)_BIN)/%.o,%.cpp, $(@))
CXX_HEADERS             = $(foreach hdr,$(ARTIFACT_HEADERS),$(call rwildcard, $($(ARTIFACT_VARS)_INC)/$(hdr), *.h)
CXX_INSTHEADERS         = $(patsubst $($(ARTIFACT_VARS)_INC)/%,$(DESTDIR)$(PREFIX)/include/%,$(CXX_HEADERS))

ARTIFACT_BIN            = $($(ARTIFACT_VARS)_BIN)
ARTIFACT_LIB            = $(ARTIFACT_BIN)/$($(ARTIFACT_VARS)_NAME)-$($(ARTIFACT_VARS)_VERSION)$(LIBRARY_EXT)
ARTIFACT_TESTBIN        = $(ARTIFACT_BIN)/$($(ARTIFACT_VARS)_NAME)-test$(EXECUTABLE_EXT)
ARTIFACT_OBJ            = $($(ARTIFACT_VARS)_OBJ)
ARTIFACT_MFLAGS         = $($(ARTIFACT_VARS)_MFLAGS) $(foreach dep,$(DEPENDENCIES),-DUSE_$(dep))

CFLAGS_DEPS             = $(foreach dep, $(DEPENDENCIES) $(ARTIFACT_VARS), $(if $($(dep)_CFLAGS), $($(dep)_CFLAGS)))
BUILD_DEPS              = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_OBJ), $(dep)))
LINKER_DEPS             = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_LDFLAGS), $($(dep)_LDFLAGS)))
LINKER_OBJS             = $(foreach dep, $(DEPENDENCIES), $(if $($(dep)_OBJ), $($(dep)_OBJ)))
BUILD_ALL               = $(ARTIFACT_LIB)

ifeq ($(TEST), 1)
  ifeq ($($(ARTIFACT_VARS)_TESTING),1)
    CXX_SRC                += $(CXX_SRCTEST)
    BUILD_ALL              += $(ARTIFACT_TESTBIN)
  endif
  DEPENDENCIES           += $(TEST_DEPENDENCIES)
endif

ifeq ($(NO_DEPENDENCIES),1)
  BUILD_DEPS              =
endif

CXX_DEPS                = $(foreach src,$(CXX_SRCALL) $(CXX_SRCTEST),$(patsubst %.cpp,$(ARTIFACT_BIN)/%.d,$(src)))
CXX_DEPFILE             = $(patsubst $(ARTIFACT_BIN)/%.d,%.cpp,$(@))
CXX_DEPTARGET           = $(patsubst $(ARTIFACT_BIN)/%.d,%.o,$(@))

# Configre list of extended build parameters
X86_IMPL                = $(ARTIFACT_BIN)/main/x86/x86.o
SSE_IMPL                = $(ARTIFACT_BIN)/main/x86/sse.o
SSE2_IMPL               = $(ARTIFACT_BIN)/main/x86/sse2.o
SSE3_IMPL               = $(ARTIFACT_BIN)/main/x86/sse3.o
SSE4_IMPL               = $(ARTIFACT_BIN)/main/x86/sse4.o
AVX_IMPL                = $(ARTIFACT_BIN)/main/x86/avx.o
AVX2_IMPL               = $(ARTIFACT_BIN)/main/x86/avx2.o
ARM_IMPL                = $(ARTIFACT_BIN)/main/arm/arm.o
NEON_D32_IMPL           = $(ARTIFACT_BIN)/main/arm/neon-d32.o
AARCH64_IMPL            = $(ARTIFACT_BIN)/main/aarch64/aarch64.o
ASIMD_IMPL              = $(ARTIFACT_BIN)/main/aarch64/asimd.o

SSE_INSTR_SET           = -mmmx -m3dnow -msse
SSE2_INSTR_SET          = $(SSE_INSTR_SET) -msse2
SSE3_INSTR_SET          = $(SSE2_INSTR_SET) -msse3 -mssse3
SSE4_INSTR_SET          = $(SSE3_INSTR_SET) -msse4 -msse4a -msse4.1 -msse4.2
AVX_INSTR_SET           = -mavx -mvzeroupper
AVX2_INSTR_SET          = $(AVX_INSTR_SET) -mavx2
NEON_D32_INSTR_SET      = -mfpu=neon-vfpv4
ASIMD_INSTR_SET      	= -march=armv8-a+simd

ifeq ($(PLATFORM),BSD)
  ifeq ($(ARCHITECTURE), arm)
    CXX_OBJEXT             += $(ARM_IMPL) $(NEON_D32_IMPL)
  endif
endif

ifeq ($(ARCHITECTURE),i586)
  CXX_OBJEXT             += $(X86_IMPL) $(SSE_IMPL) $(SSE2_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL) $(AVX2_IMPL)
else ifeq ($(ARCHITECTURE), x86_64)
  CXX_OBJEXT             += $(X86_IMPL) $(SSE_IMPL) $(SSE2_IMPL) $(SSE3_IMPL) $(SSE4_IMPL) $(AVX_IMPL) $(AVX2_IMPL)
else ifeq ($(ARCHITECTURE),armv7a)
  CXX_OBJEXT             += $(ARM_IMPL) $(NEON_D32_IMPL)
else ifeq ($(ARCHITECTURE),armv7ve)
  CXX_OBJEXT             += $(ARM_IMPL) $(NEON_D32_IMPL)
else ifeq ($(ARCHITECTURE),arm32)
  CXX_OBJEXT             += $(ARM_IMPL) $(NEON_D32_IMPL)
else ifeq ($(ARCHITECTURE),aarch64)
  CXX_OBJEXT             += $(AARCH64_IMPL) $(ASIMD_IMPL)
endif

# Define targets
.DEFAULT_GOAL = all
.PHONY: compile depend dep_clean all install uninstall
.PHONY: $(BUILD_DEPS)

compile: $(ARTIFACT_OBJ)

all: $(BUILD_ALL)

dep_clean:

$(CXX_DEPS): dep_clean
	@echo "  dep  [$(ARTIFACT_NAME)] $(CXX_DEPFILE)"
	@mkdir -p $(dir $(@))
	$(CXX) -MM -MT "\$$($(ARTIFACT_VARS)_BIN)/$(CXX_DEPTARGET)" -MF $(@) $(CXX_DEPFILE) $(CXXFLAGS) $(INCLUDE) $(CFLAGS_DEPS)

depend: $(CXX_DEPS)
	$(foreach dep,$(DEPENDENCIES) $(ARTIFACT_VARS),\
	  $(if $($(dep)_INC), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_INC)" | sed "s/\\//\\\\\//g")/\$$$$\\($(dep)_INC\\)/g;)\
	  )\
	  $(if $($(dep)_BIN), \
	    $(eval SED_RULES += s/$(shell echo "$($(dep)_BIN)" | sed "s/\\//\\\\\//g")/\\$$$$\\($(dep)_BIN\\)/g;)\
	  )\
	)
	@cat $(CXX_DEPS) | sed -E "$(SED_RULES)" >Makefile.$(ARCHITECTURE).d

$(BUILD_DEPS):
	@echo "make $(notdir $($(@)_OBJ))"
	@$(MAKE) -s -C "$($(@)_PATH)" compile CONFIG="$(CONFIG)" NO_DEPENDENCIES="1" DEMO_TEST="0"

$(ARTIFACT_OBJ): $(BUILD_DEPS) $(CXX_OBJ) $(CXX_OBJEXT)
	@echo "  $(LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ))"
	@$(LD) -o $(ARTIFACT_OBJ) -r $(CXX_OBJ) $(CXX_OBJEXT)

$(ARTIFACT_LIB): $(ARTIFACT_OBJ)
	@echo "  $(CXX)  [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_LIB))"
	@$(CXX) -o $(ARTIFACT_LIB) $(LINKER_OBJS) $(ARTIFACT_OBJ) $(SO_FLAGS) $(LINKER_DEPS)

$(ARTIFACT_TESTBIN): $(ARTIFACT_OBJ)
	@echo "  $(CXX)  [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_TESTBIN))"
	@$(CXX) -o $(ARTIFACT_TESTBIN) $(LINKER_OBJS) $(ARTIFACT_OBJ) $(EXE_FLAGS) $(LINKER_DEPS)

$(CXX_OBJ) $(CXX_OBJEXT):
	@echo "  $(CXX)  [$(ARTIFACT_NAME)] $(CXX_FILE)"
	@mkdir -p $(dir $@)
	@$(CXX) -o $(@) -c $(CXX_FILE) -fPIC $(CXXFLAGS) $(ARTIFACT_MFLAGS) $(EXT_FLAGS) $(INCLUDE) $(CFLAGS_DEPS)

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
	@echo "Uninstalling $($(ARTIFACT_VARS)_NAME)"
	@-rm -f "$(DESTDIR)$(PREFIX)/lib/$(notdir $(ARTIFACT_LIB))"
	@-rm -f "$(DESTDIR)$(PREFIX)/lib/$($(ARTIFACT_VARS)_NAME)$(LIBRARY_EXT)"
	@echo rm -f $(CXX_INSTHEADERS)
	@-rm -f $(CXX_INSTHEADERS)
	@echo "Uninstall OK"

# Additional extended rules:
$(SSE_IMPL):        EXT_FLAGS=$(SSE_INSTR_SET)
$(SSE2_IMPL):       EXT_FLAGS=$(SSE2_INSTR_SET)
$(SSE3_IMPL):       EXT_FLAGS=$(SSE3_INSTR_SET)
$(SSE4_IMPL):       EXT_FLAGS=$(SSE4_INSTR_SET)
$(AVX_IMPL):        EXT_FLAGS=$(AVX_INSTR_SET)
$(AVX2_IMPL):       EXT_FLAGS=$(AVX2_INSTR_SET)
$(NEON_D32_IMPL):   EXT_FLAGS=$(NEON_D32_INSTR_SET)
$(ASIMD_IMPL):      EXT_FLAGS=$(ASIMD_INSTR_SET)

# Dependencies
-include $(CURDIR)/Makefile.$(ARCHITECTURE).d
