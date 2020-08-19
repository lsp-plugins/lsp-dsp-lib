# Detect operating system
ifndef PLATFORM
  ifeq ($(findstring Windows,$(OS)),Windows)
    BUILD_SYSTEM   := Windows
  else
    BUILD_SYSTEM   := $(shell uname -s 2>/dev/null || echo "Unknown")
  endif
  
  PLATFORM       := Unknown

  ifeq ($(BUILD_SYSTEM),Windows)
    PLATFORM       := Windows
  else ifeq ($(findstring OpenBSD,$(BUILD_SYSTEM)),OpenBSD)
    PLATFORM       := OpenBSD
  else ifeq ($(findstring BSD,$(BUILD_SYSTEM)),BSD)
    PLATFORM       := BSD
  else ifeq ($(findstring Linux,$(BUILD_SYSTEM)),Linux)
    PLATFORM       := Linux
  else ifeq ($(findstring SunOS,$(BUILD_SYSTEM)),SunOS)
    PLATFORM       := Solaris
  endif
endif

# Detect processor architecture
ifndef ARCHITECTURE
  ifeq ($(PLATFORM),Windows)
    ifeq ($(PROCESSOR_ARCHITECTURE),x86)
      ARCHITECTURE             := i586
    else ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
      ARCHITECTURE             := x86_64
    else
      ARCHITECTURE             := i586
    endif
  else # BUILD_PLATFORM != Windows
    BUILD_ARCH             := $(shell uname -m)
    ifeq ($(patsubst armv6%,armv6,$(BUILD_ARCH)), armv6)
      ARCHITECTURE           := armv6a
    else ifeq ($(patsubst armv7%,armv7,$(BUILD_ARCH)), armv7)
      ARCHITECTURE           := armv7a
    else ifeq ($(patsubst armv8%,armv8,$(BUILD_ARCH)), armv8)
      ARCHITECTURE           := armv8a
    else ifeq ($(patsubst aarch64%,aarch64,$(BUILD_ARCH)), aarch64)
      ARCHITECTURE           := aarch64
    else ifeq ($(BUILD_ARCH),x86_64)
      ARCHITECTURE           := x86_64
    else ifeq ($(BUILD_ARCH),amd64)
      ARCHITECTURE           := x86_64
    else ifeq ($(BUILD_ARCH),i86pc)
      ARCHITECTURE           := x86_64
    else ifeq ($(patsubst i%86,i586,$(BUILD_ARCH)), i586)
      ARCHITECTURE           := i586
    else
      ARCHITECTURE           := $(BUILD_ARCH)
    endif
  endif # PLATFORM != Windows
endif

# Extension of libraries
ifndef LIBRARY_EXT
  ifeq ($(PLATFORM),Windows)
    LIBRARY_EXT              := .dll
  else
    LIBRARY_EXT              := .so
  endif
endif

# Extension of executables
ifndef EXECUTABLE_EXT
  ifeq ($(PLATFORM),Windows)
    EXECUTABLE_EXT           := .exe
  else
    EXECUTABLE_EXT           :=
  endif
endif

# Extension of pkgconfig files
ifndef PKGCONFIG_EXT
  PKGCONFIG_EXT            := .pc
endif

# Installation prefix
ifndef PREFIX
  ifeq ($(PLATFORM),Windows)
    PREFIX                   := $(ProgramFiles)
  else
    PREFIX                   := /usr/local
  endif
endif

# Temporary directory
ifndef TEMPDIR
  ifeq ($(PLATFORM),Windows)
    TEMPDIR                  := $(TEMP)
  else
    TEMPDIR                  := /tmp
  endif
endif

TEST                       := 0

# Set-up list of common variables
COMMON_VARS = \
	PLATFORM \
	ARCHITECTURE \
	LIBRARY_EXT \
	EXECUTABLE_EXT \
	PKGCONFIG_EXT \
	PREFIX \
	TEMPDIR \
	TEST \
	DEBUG \
	PROFILE \
	TRACE

.PHONY: sysvars

sysvars:
	@echo "List of available system variables:"
	@echo "  ARCHITECTURE              target architecture to perform build"
	@echo "  DEBUG                     build with debug options"
	@echo "  EXECUTABLE_EXT            file extension for executable files"
	@echo "  LIBRARY_EXT               file extension for library files"
	@echo "  PKGCONFIG_EXT             file extension for pkgconfig files"
	@echo "  PLATFORM                  target software platform to perform build"
	@echo "  PREFIX                    installation prefix for binary files"
	@echo "  PROFILE                   build with profile options"
	@echo "  TEMPDIR                   location of temporary directory"
	@echo "  TEST                      use test build"
	@echo "  TRACE                     compile with additional trace information output"

