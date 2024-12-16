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
ifneq ($(VERBOSE),1)
.SILENT:
endif

MODDIR                  = $(CURDIR)/..
CONFIG                 := $(MODDIR)/.config.mk

include $(CONFIG)
include $(MODDIR)/project.mk
include $(MODDIR)/dependencies.mk
include $(MODDIR)/make/functions.mk

ifeq ($(TEST),1)
  UNIQ_DEPENDENCIES      := $(call uniq, $(DEPENDENCIES) $(TEST_DEPENDENCIES))
  DEPENDENCIES            = $(UNIQ_DEPENDENCIES)
endif

# Artifact settings
ifeq ($(BUILD_HOST),1)
  override HOST                    = HOST_
else
  override HOST                    =
endif

ARTIFACT_NAME           = $($(ARTIFACT_ID)_NAME)
ARTIFACT_VERSION        = $($(ARTIFACT_ID)_VERSION)
ARTIFACT_DEPENDENCIES   = $(addprefix $(HOST),$(DEPENDENCIES))

ARTIFACT_BIN            = $($(HOST)$(ARTIFACT_ID)_BIN)
ARTIFACT_INC            = $($(HOST)$(ARTIFACT_ID)_INC)
ARTIFACT_OBJ            = $($(HOST)$(ARTIFACT_ID)_OBJ)
ARTIFACT_OBJ_TEST       = $($(HOST)$(ARTIFACT_ID)_OBJ_TEST)
ARTIFACT_MFLAGS         = $($(HOST)$(ARTIFACT_ID)_MFLAGS) $(foreach dep,$(DEPENDENCIES),-DUSE_$(dep))

ARTIFACT_TEST_BIN       = $(ARTIFACT_BIN)/$(ARTIFACT_NAME)-test$(EXECUTABLE_EXT)
ARTIFACT_LIB            = $(ARTIFACT_BIN)/$(LIBRARY_PREFIX)$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)$(LIBRARY_EXT)
ARTIFACT_SLIB           = $(ARTIFACT_BIN)/$(LIBRARY_PREFIX)$(ARTIFACT_NAME)-$(ARTIFACT_VERSION)$(STATICLIB_EXT)
ARTIFACT_LIBLINK        = $(LIBRARY_PREFIX)$(ARTIFACT_NAME)$(LIBRARY_EXT)
ARTIFACT_SLIBLINK       = $(LIBRARY_PREFIX)$(ARTIFACT_NAME)$(STATICLIB_EXT)
ARTIFACT_PC             = $(ARTIFACT_BIN)/$(ARTIFACT_NAME)$(PKGCONFIG_EXT)
ARTIFACT_DEPS           = $(call dquery, OBJ, $(ARTIFACT_DEPENDENCIES))
ARTIFACT_CFLAGS         = $(call query, CFLAGS, $(ARTIFACT_DEPENDENCIES) $(HOST)$(ARTIFACT_ID))
ARTIFACT_LDFLAGS        = $(call query, LDFLAGS, $(ARTIFACT_DEPENDENCIES) $(HOST)$(ARTIFACT_ID))
ARTIFACT_OBJFILES       = $(call query, OBJ, $(ARTIFACT_DEPENDENCIES) $(HOST)$(ARTIFACT_ID))

ARTIFACT_TARGETS        = $(ARTIFACT_LIB) $(ARTIFACT_SLIB) $(ARTIFACT_PC)

# Source code
CXX_SRC_MAIN            = main/dsp.cpp main/generic/generic.cpp
CXX_SRC_EXPORT          = $(call rwildcard, export, *.cpp)
CXX_SRC_TEST            = $(call rwildcard, test, *.cpp)
CXX_SRC_NOTEST          =
CXX_SRC_EXT             = $(patsubst $(ARTIFACT_BIN)/%.o, %.cpp, $(CXX_OBJ_EXT))
CXX_SRC                 = $(CXX_SRC_MAIN) $(CXX_SRC_EXT)

CXX_OBJ_MAIN            = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_MAIN))
CXX_OBJ_EXPORT          = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_EXPORT))
CXX_OBJ_TEST            = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_TEST))
CXX_OBJ_NOTEST          = $(patsubst %.cpp, $(ARTIFACT_BIN)/%.o, $(CXX_SRC_NOTEST))
CXX_OBJ_EXT             = 
CXX_OBJ_X86             = $(ARTIFACT_BIN)/main/x86/x86.o
CXX_OBJ_SSE             = $(ARTIFACT_BIN)/main/x86/sse.o
CXX_OBJ_SSE2            = $(ARTIFACT_BIN)/main/x86/sse2.o
CXX_OBJ_SSE3            = $(ARTIFACT_BIN)/main/x86/sse3.o
CXX_OBJ_SSE4            = $(ARTIFACT_BIN)/main/x86/sse4.o
CXX_OBJ_AVX             = $(ARTIFACT_BIN)/main/x86/avx.o
CXX_OBJ_AVX2            = $(ARTIFACT_BIN)/main/x86/avx2.o
CXX_OBJ_AVX512          = $(ARTIFACT_BIN)/main/x86/avx512.o
CXX_OBJ_ARM             = $(ARTIFACT_BIN)/main/arm/arm.o
CXX_OBJ_NEON_D32        = $(ARTIFACT_BIN)/main/arm/neon-d32.o
CXX_OBJ_AARCH64         = $(ARTIFACT_BIN)/main/aarch64/aarch64.o
CXX_OBJ_ASIMD           = $(ARTIFACT_BIN)/main/aarch64/asimd.o
CXX_OBJ                 = $(CXX_OBJ_MAIN) $(CXX_OBJ_EXT)
CXX_OBJ_ALL             = \
  $(CXX_OBJ_MAIN) \
  $(CXX_OBJ_EXPORT) \
  $(CXX_OBJ_EXT) \
  $(CXX_OBJ_TEST) \
  $(CXX_OBJ_NOTEST)

ifeq ($($(HOST)ARCHITECTURE_FAMILY),ia32)
  CXX_OBJ_EXT            += $(CXX_OBJ_X86) $(CXX_OBJ_SSE) $(CXX_OBJ_SSE2) $(CXX_OBJ_SSE3) $(CXX_OBJ_SSE4) $(CXX_OBJ_AVX) $(CXX_OBJ_AVX2) $(CXX_OBJ_AVX512)
else ifeq ($($(HOST)ARCHITECTURE_FAMILY),x86_64)
  CXX_OBJ_EXT            += $(CXX_OBJ_X86) $(CXX_OBJ_SSE) $(CXX_OBJ_SSE2) $(CXX_OBJ_SSE3) $(CXX_OBJ_SSE4) $(CXX_OBJ_AVX) $(CXX_OBJ_AVX2) $(CXX_OBJ_AVX512)
else ifeq ($($(HOST)ARCHITECTURE_FAMILY),arm32)
  CXX_OBJ_EXT            += $(CXX_OBJ_ARM) $(CXX_OBJ_NEON_D32)
else ifeq ($($(HOST)ARCHITECTURE_FAMILY),aarch64)
  CXX_OBJ_EXT            += $(CXX_OBJ_AARCH64) $(CXX_OBJ_ASIMD)
endif

ALL_HEADERS             = $(call rwildcard, $(ARTIFACT_INC), *.h)
CXX_FILE                = $(patsubst $(ARTIFACT_BIN)/%.o,%.cpp, $(@))
CXX_HDR_PATHS           = $(foreach hdr,$(ARTIFACT_HEADERS),$(ARTIFACT_INC)/$(hdr))
CXX_HEADERS             = $(foreach path,$(CXX_HDR_PATHS),$(call rwildcard, $(path), *.h))
CXX_INSTHEADERS         = $(patsubst $(ARTIFACT_INC)/%,$(DESTDIR)$(INCDIR)/%,$(CXX_HEADERS))
DEP_FILE                = $(patsubst %.o,%.d, $(@))

CXX_SSE_CFLAGS          = -mmmx -msse
CXX_SSE2_CFLAGS         = $(CXX_SSE_CFLAGS) -msse2
CXX_SSE3_CFLAGS         = $(CXX_SSE2_CFLAGS) -msse3 -mssse3
CXX_SSE4_CFLAGS         = $(CXX_SSE3_CFLAGS) -msse4 -msse4a -msse4.1 -msse4.2
CXX_AVX_CFLAGS          = -mavx -mvzeroupper
CXX_AVX2_CFLAGS         = $(CXX_AVX_CFLAGS) -mavx2
CXX_AVX512_CFLAGS       = -mavx512f -mavx512vl
CXX_NEON_D32_CFLAGS     = -mfpu=neon-vfpv4
CXX_ASIMD_CFLAGS      	= -march=armv8-a+simd

BUILD_ALL               = $(ARTIFACT_LIB) $(ARTIFACT_SLIB) $(ARTIFACT_PC)

ifeq ($($(ARTIFACT_ID)_TESTING),1)
  ARTIFACT_TARGETS       += $(ARTIFACT_TEST_BIN)
endif

DEP_CXX                 = $(foreach src,$(CXX_SRC_MAIN) $(CXX_SRC_EXPORT) $(CXX_SRC_EXT) $(CXX_SRC_TEST),$(patsubst %.cpp,$(ARTIFACT_BIN)/%.d,$(src)))
DEP_CXX_FILE            = $(patsubst $(ARTIFACT_BIN)/%.d,%.cpp,$(@))
DEP_DEP_FILE            = $(patsubst $(ARTIFACT_BIN)/%.d,%.o,$(@))

.DEFAULT_GOAL = all
.PHONY: compile all install uninstall
.PHONY: $(ARTIFACT_DEPS)

# Compile dependencies
$(ARTIFACT_DEPS):
	echo "make $(notdir $($(@)_OBJ))"
	$(MAKE) -C "$($(@)_PATH)" compile VERBOSE="$(VERBOSE)" CONFIG="$(CONFIG)" DEMO_TEST="0" BUILD_HOST="$(BUILD_HOST)"

# Compilation
compile: $(ARTIFACT_OBJ)

$(CXX_OBJ_ALL):
	echo "  $($(HOST)CXX)  [$(ARTIFACT_NAME)] $(CXX_FILE)"
	mkdir -p $(dir $@)
	$($(HOST)CXX) -o $(@) -c $(CXX_FILE) -fPIC $($(HOST)CXXFLAGS) $(ARTIFACT_MFLAGS) $(EXT_FLAGS) $(INCLUDE) $(ARTIFACT_CFLAGS) -MMD -MP -MF $(DEP_FILE) -MT $(@)
	
# Producing large object files
$(ARTIFACT_OBJ): $(CXX_OBJ)
	@echo "  $($(HOST)LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ))"
	$($(HOST)LD) -o $(ARTIFACT_OBJ) $($(HOST)LDFLAGS) $(CXX_OBJ)

$(ARTIFACT_OBJ_TEST): $(CXX_OBJ_TEST)
	echo "  $($(HOST)LD)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_OBJ_TEST))"
	$($(HOST)LD) -o $(ARTIFACT_OBJ_TEST) $($(HOST)LDFLAGS) $(CXX_OBJ_TEST)

# Linking
all: $(ARTIFACT_TARGETS)

$(ARTIFACT_LIB): $(ARTIFACT_DEPS) $(ARTIFACT_OBJ) $(CXX_OBJ_EXPORT) $(CXX_OBJ_NOTEST)
	echo "  $($(HOST)CXX)  [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_LIB))"
	$($(HOST)CXX) -o $(ARTIFACT_LIB) $(ARTIFACT_OBJFILES) $(CXX_OBJ_EXPORT) $(CXX_OBJ_NOTEST) $($(HOST)SO_FLAGS) $(ARTIFACT_LDFLAGS)
	
$(ARTIFACT_SLIB): $(ARTIFACT_DEPS) $(ARTIFACT_OBJ) $(CXX_OBJ_NOTEST)
	echo "  $($(HOST)AR)   [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_SLIB))"
	$($(HOST)AR) rcs $(ARTIFACT_SLIB) $(ARTIFACT_OBJFILES) $(CXX_OBJ_NOTEST)

$(ARTIFACT_TEST_BIN): $(ARTIFACT_DEPS) $(ARTIFACT_OBJ) $(ARTIFACT_OBJ_TEST)
	echo "  $($(HOST)CXX)  [$(ARTIFACT_NAME)] $(notdir $(ARTIFACT_TEST_BIN))"
	$($(HOST)CXX) -o $(ARTIFACT_TEST_BIN) $(ARTIFACT_OBJFILES) $(ARTIFACT_OBJ_TEST) $($(HOST)EXE_FLAGS) $(ARTIFACT_LDFLAGS)

$(ARTIFACT_PC):
	mkdir -p $(dir $(ARTIFACT_PC))
	echo "prefix=$(PREFIX)" > "$(ARTIFACT_PC)"
	echo "exec_prefix=$(BINDIR)" >> "$(ARTIFACT_PC)"
	echo "includedir=$(INCDIR)" >> "$(ARTIFACT_PC)"
	echo "libdir=$(LIBDIR)" >> "$(ARTIFACT_PC)"
	echo "" >> $(ARTIFACT_PC) >> "$(ARTIFACT_PC)"
	echo "Name: $($(ARTIFACT_ID)_NAME)" >> "$(ARTIFACT_PC)"
	echo "Description: $($(ARTIFACT_ID)_DESC)" >> "$(ARTIFACT_PC)"
	echo "Version: $($(ARTIFACT_ID)_VERSION)" >> "$(ARTIFACT_PC)"
	echo "Cflags: -I\$${includedir}" >> "$(ARTIFACT_PC)"
	echo "Libs: -L\$${libdir} -l$(notdir $($(ARTIFACT_ID)_NAME))" >> "$(ARTIFACT_PC)"

# Installation/deinstallation
install: all
	echo "Installing $($(ARTIFACT_ID)_NAME)"
	mkdir -p "$(DESTDIR)$(LIBDIR)/pkgconfig"
	$(if $(filter $(INSTALL_HEADERS),1), mkdir -p "$(DESTDIR)$(INCDIR)")
	$(if $(filter $(INSTALL_HEADERS),1), cp -r "$(CXX_HDR_PATHS)" "$(DESTDIR)$(INCDIR)/")
	cp $(ARTIFACT_PC) "$(DESTDIR)$(LIBDIR)/pkgconfig/"
	$(INSTALL) $(ARTIFACT_LIB) "$(DESTDIR)$(LIBDIR)/"
	cp $(ARTIFACT_SLIB) "$(DESTDIR)$(LIBDIR)/"
	ln -sf $(notdir $(ARTIFACT_LIB)) "$(DESTDIR)$(LIBDIR)/$(ARTIFACT_LIBLINK)"
	ln -sf $(notdir $(ARTIFACT_SLIB)) "$(DESTDIR)$(LIBDIR)/$(ARTIFACT_SLIBLINK)"
	echo "Install OK"

uninstall:
	echo "Uninstalling $($(ARTIFACT_ID)_NAME)"
	-rm -f "$(DESTDIR)$(LIBDIR)/$(notdir $(ARTIFACT_LIB))"
	-rm -f "$(DESTDIR)$(LIBDIR)/$(notdir $(ARTIFACT_SLIB))"
	-rm -f "$(DESTDIR)$(LIBDIR)/$(ARTIFACT_LIBLINK)"
	-rm -f "$(DESTDIR)$(LIBDIR)/$(ARTIFACT_SLIBLINK)"
	-rm -f "$(DESTDIR)$(LIBDIR)/pkgconfig/$(notdir $(ARTIFACT_PC))"
	-rm -f $(CXX_INSTHEADERS)
	echo "Uninstall OK"

# Additional extended rules:
$(CXX_OBJ_SSE):        EXT_FLAGS=$(CXX_SSE_CFLAGS)
$(CXX_OBJ_SSE2):       EXT_FLAGS=$(CXX_SSE2_CFLAGS)
$(CXX_OBJ_SSE3):       EXT_FLAGS=$(CXX_SSE3_CFLAGS)
$(CXX_OBJ_SSE4):       EXT_FLAGS=$(CXX_SSE4_CFLAGS)
$(CXX_OBJ_AVX):        EXT_FLAGS=$(CXX_AVX_CFLAGS)
$(CXX_OBJ_AVX2):       EXT_FLAGS=$(CXX_AVX2_CFLAGS)
$(CXX_OBJ_AVX512):     EXT_FLAGS=$(CXX_AVX512_CFLAGS)
$(CXX_OBJ_NEON_D32):   EXT_FLAGS=$(CXX_NEON_D32_CFLAGS)
$(CXX_OBJ_ASIMD):      EXT_FLAGS=$(CXX_ASIMD_CFLAGS)

# Dependencies
-include $(foreach objfile,$(CXX_OBJ_ALL),$(patsubst %.o,%.d,$(objfile)))
