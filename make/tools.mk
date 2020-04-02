# Define tool variables
CC                 := gcc
CXX                := g++
LD                 := ld
GIT                := git
INSTALL            := install

# Patch flags and tools
FLAG_RELRO          = -Wl,-z,relro,-z,now
FLAG_STDLIB         = -lc
ifeq ($(PLATFORM),Solaris)
  FLAG_RELRO              =
  LD                      = gld
else ifeq ($(PLATFORM),Windows)
  FLAG_RELRO              =
  FLAG_STDLIB             =
endif

ifeq ($(TEST),1)
  CFLAGS_EXT         += -DLSP_TESTING
  CXXFLAGS_EXT       += -DLSP_TESTING
else
  CFLAGS_EXT         += -O2 -fvisibility=hidden
  CXXFLAGS_EXT       += -O2 -fvisibility=hidden
endif

# ARMv7 architecture tuning
ifeq ($(ARCHITECTURE),armv6a)
  CXXFLAGS_EXT    += -march=armv6-a -marm
else ifeq ($(ARCHITECTURE),armv7a)
  CXXFLAGS_EXT    += -march=armv7-a -marm
else ifeq ($(ARCHITECTURE),armv7ve)
  CXXFLAGS_EXT    += -march=armv7ve -marm
else ifeq ($(ARCHITECTURE),arm32)
  CXXFLAGS_EXT    += -marm
else ifeq ($(ARCHITECTURE),armv8a)
  CXXFLAGS_EXT    += -march=armv7-a -marm
else ifeq ($(ARCHITECTURE),aarch64)
  CXXFLAGS_EXT    += -march=armv8-a
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
  -exceptions \
  -fno-rtti \
  -fdata-sections \
  -ffunction-sections \
  -fno-asynchronous-unwind-tables \
  -pipe \
  -Wall

INCLUDE            :=
LDFLAGS            := $(LDFLAGS_EXT) -r
EXE_FLAGS          := $(FLAG_RELRO) -Wl,--gc-sections
SO_FLAGS           := $(FLAG_RELRO) -Wl,--gc-sections -shared -Llibrary $(FLAG_STDLIB) -fPIC 

TOOL_VARS := \
  CC CXX LD GIT INSTALL \
  CFLAGS CXXFLAGS LDFLAGS EXE_FLAGS SO_FLAGS \
  INCLUDE

.PHONY: toolvars
toolvars:
	@echo "List of tool variables:"
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
