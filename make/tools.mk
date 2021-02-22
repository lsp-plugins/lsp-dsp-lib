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

# Determine tools
ifeq ($(PLATFORM),OpenBSD)
  X_CC_TOOL          := egcc
  X_CXX_TOOL         := eg++
  X_AS_TOOL          := gas
  X_AR_TOOL          := ar
else
  X_CC_TOOL          := gcc
  X_CXX_TOOL         := g++
  X_AS_TOOL          := as
  X_AR_TOOL          := ar
endif

# Define tool variables
CC                 := $(X_CC_TOOL)
CXX                := $(X_CXX_TOOL)
AS                 := $(X_AS_TOOL)
AR                 := $(X_AR_TOOL)
LD                 := ld
GIT                := git
INSTALL            := install

# Patch flags and tools
FLAG_RELRO          = -Wl,-z,relro,-z,now
FLAG_STDLIB         = -lc
CFLAGS_EXT          =
CXXFLAGS_EXT        =
LDFLAGS_EXT         =

ifeq ($(PLATFORM),Solaris)
  FLAG_RELRO          =
  LD                  = gld
else ifeq ($(PLATFORM),Windows)
  FLAG_RELRO          =
  FLAG_STDLIB         =
else ifeq ($(PLATFORM),BSD)
  EXE_FLAGS_EXT      += -L/usr/local/lib
  SO_FLAGS_EXT       += -L/usr/local/lib
endif

ifeq ($(DEBUG),1)
  CFLAGS_EXT         += -O0 -g3 -DLSP_DEBUG
  CXXFLAGS_EXT       += -O0 -g3 -DLSP_DEBUG
else
  CFLAGS_EXT         += -O2
  CXXFLAGS_EXT       += -O2
endif

ifeq ($(PROFILE),1)
  CFLAGS_EXT         += -pg -DLSP_PROFILE
  CXXFLAGS_EXT       += -pg -DLSP_PROFILE
endif

ifeq ($(TRACE),1)
  CFLAGS_EXT         += -DLSP_TRACE
  CXXFLAGS_EXT       += -DLSP_TRACE
endif

ifeq ($(TEST),1)
  CFLAGS_EXT         += -DLSP_TESTING
  CXXFLAGS_EXT       += -DLSP_TESTING
else
  ifneq ($(ARTIFACT_EXPORT_ALL),1)
    CFLAGS_EXT         += -fvisibility=hidden
    CXXFLAGS_EXT       += -fvisibility=hidden
  endif
endif

# ARMv7 architecture tuning
ifeq ($(ARCHITECTURE),i586)
  CXXFLAGS_EXT    += -m32
  CFLAGS_EXT      += -m32
else ifeq ($(ARCHITECTURE),x86_64)
  CXXFLAGS_EXT    += -m64
  CFLAGS_EXT      += -m64
else ifeq ($(ARCHITECTURE),armv6a)
  CXXFLAGS_EXT    += -march=armv6-a -marm
  CFLAGS_EXT      += -march=armv6-a -marm
else ifeq ($(ARCHITECTURE),armv7a)
  CXXFLAGS_EXT    += -march=armv7-a -marm
  CFLAGS_EXT      += -march=armv7-a -marm
else ifeq ($(ARCHITECTURE),armv7ve)
  CXXFLAGS_EXT    += -march=armv7ve -marm
  CFLAGS_EXT      += -march=armv7ve -marm
else ifeq ($(ARCHITECTURE),arm32)
  CXXFLAGS_EXT    += -marm
  CFLAGS_EXT      += -marm
else ifeq ($(ARCHITECTURE),armv8a)
  CXXFLAGS_EXT    += -march=armv7-a -marm
  CFLAGS_EXT      += -march=armv7-a -marm
else ifeq ($(ARCHITECTURE),aarch64)
  CXXFLAGS_EXT    += -march=armv8-a
  CFLAGS_EXT      += -march=armv8-a
endif

# Define flags
CFLAGS             := \
  $(CFLAGS_EXT) \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall
  
CXXFLAGS           := \
  $(CXXFLAGS_EXT) \
  -std=c++98 \
  -fno-exceptions \
  -fno-rtti \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall

INCLUDE            :=
LDFLAGS            := $(LDFLAGS_EXT) -r
EXE_FLAGS          := $(EXE_FLAGS_EXT) $(FLAG_RELRO) -Wl,--gc-sections
SO_FLAGS           := $(SO_FLAGS_EXT) $(FLAG_RELRO) -Wl,--gc-sections -shared -Llibrary $(FLAG_STDLIB) -fPIC 

TOOL_VARS := \
  AS CC CXX LD GIT INSTALL \
  CFLAGS CXXFLAGS LDFLAGS EXE_FLAGS SO_FLAGS \
  INCLUDE

.PHONY: toolvars
toolvars:
	@echo "List of tool variables:"
	@echo "  AR                        Archiver tool"
	@echo "  AS                        Assembler tool"
	@echo "  CC                        C compiler execution command line"
	@echo "  CFLAGS                    C compiler build flags"
	@echo "  CXX                       C++ compiler execution command line"
	@echo "  CXXFLAGS                  C++ compiler build flags"
	@echo "  EXE_FLAGS                 Flags to link executable files"
	@echo "  GIT                       The name of the Git version control tool"
	@echo "  INCLUDE                   Additional paths for include files"
	@echo "  LD                        Linker execution command line"
	@echo "  LDFLAGS                   Linker flags for merging object files"
	@echo "  SO_FLAGS                  Flags to link shared object/library files"
	@echo ""

