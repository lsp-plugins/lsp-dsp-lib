# lsp-dsp-lib

DSP library for digital signal processing (and more)

This library provides set of functions that perform SIMD-optimized
computing on several hardware architectures.

Currently supported set of SIMD extensions:
  * i586 architecture (32-bit): SSE, SSE2, SSE3, AVX, AVX2, FMA3 and partial support of AVX512;
  * x86_64 architecture (64-bit): SSE, SSE2, SSE3, AVX, AVX2, FMA3 and partial support of AVX512;
  * armv7 architecture (32-bit): NEON;
  * AArch64 architecture (64-bit): ASIMD.

All functions currently operate on IEEE-754 single-precision floating-point numbers.

Current set of functions provided:
  * Functions that gather system information and optimize CPU for better computing;
  * Cooley-Tukey 1-dimensional FFT algorithms with unpacked complex numbers;
  * Cooley-Tukey 1-dimensional FFT algorithms with packed complex numbers;
  * Direct convolution algorithm;
  * Fast convolution functions that enhance performance of FFT-based convolution algorithms;
  * Biquad static filter transform and processing algorithms;
  * Biquad dynamic filter transform and processing algorithms;
  * Floating-point operations: copying, moving, protection from NaNs and denormals;
  * Parallel arithmetics functions on long vectors including fused multiply operations;
  * Basic unpacked complex number arithmetics;
  * Basic packed complex number arithmetics;
  * Some functions that operate on RGB and HSL colors and their conversions;
  * Mid/Side matrix functions for converting Stereo channel to Mid/Side and back;
  * Functions for searching minimums and maximums;
  * Resampling functions based on Lanczos filter;
  * Interpolation functions;
  * Some set of function to work with 3D mathematics.

## Supported platforms

The build and correct unit test execution has been confirmed for following platforms:
* FreeBSD
* GNU/Linux
* MacOS
* OpenBSD
* Windows 32-bit
* Windows 64-bit

## Supported architectures

The support of following list of hardware architectures has been implemented:
* i386 (32-bit) - full support (SSE1-SSE3, AVX, AVX2, partial support for AVX-512).
* x86_64 (64-bit) - full support (SSE1-SSE3, AVX, AVX2, partial support for AVX-512).
* ARMv6A - full support.
* ARMv7A - full support.
* AArch64 - full support.

For all other architectures the generic implementation of algorithms is used, without any
architecture-specific optimizations. 

## Requirements

The following packages need to be installed for building:

* gcc >= 4.9
* make >= 4.0

## Building

To build the library, perform the following commands:

```bash
make config # Configure the build
make fetch # Fetch dependencies from Git repository
make
sudo make install
```

To get more build options, run:

```bash
make help
```

To uninstall library, simply issue:

```bash
make uninstall
```

To clean all binary files, run:

```bash
make clean
```

To clean the whole project tree including configuration files, run:

```bash
make prune
```

To fetch all possible dependencies and make the source code tree portable between
different architectures and platforms, run:

```bash
make tree
```

To build source code archive with all possible dependencies, run:

```bash
make distsrc
```

## Usage


Here's the code snippet of how the library can be initialized and used in C++:

```C++
#include <lsp-plug.in/dsp/dsp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv)
{
    // Initialize DSP
    lsp::dsp::init();

    // Optionally: output information about the system
    lsp::dsp::info_t *info = lsp::dsp::info();
    if (info != NULL)
    {
        printf("Architecture:   %s\n", info->arch);
        printf("Processor:      %s\n", info->cpu);
        printf("Model:          %s\n", info->model);
        printf("Features:       %s\n", info->features);

        ::free(info);
    }
    
    // For faster computing we can tune CPU by updating thread context.
    // This will enable Flush-to-Zero and Denormals-are-Zero flags on
    // CPUs that support them. This is thread-local change and should
    // be called in each individual processing thread
    lsp::dsp::context_t ctx;
    lsp::dsp::start(&ctx);
    
    // Here we call some dsp functions, for example dsp::fill_zero
    float v[0x1000];
    lsp::dsp::fill_zero(v, sizeof(v)/sizeof(float));
    
    // At the end, we need to restore the context and reset CPU settings to defaults
    lsp::dsp::finish(&ctx);
    
    return 0;
}


```

Also all functions can be accessed from pure C with ```lsp_dsp_``` prefix in the funcion and type names:

```C
#include <lsp-plug.in/dsp/dsp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, const char **argv)
{
    // Initialize DSP
    lsp_dsp_init();

    // Optionally: output information about the system
    lsp_dsp_info_t *info = lsp_dsp_info();
    if (info != NULL)
    {
        printf("Architecture:   %s\n", info->arch);
        printf("Processor:      %s\n", info->cpu);
        printf("Model:          %s\n", info->model);
        printf("Features:       %s\n", info->features);

        free(info);
    }
    
    // For faster computing we can tune CPU by updating thread context.
    // This will enable Flush-to-Zero and Denormals-are-Zero flags on
    // CPUs that support them. This is thread-local change and should
    // be called in each individual processing thread
    lsp_dsp_context_t ctx;
    lsp_dsp_start(&ctx);
    
    // Here we call some dsp functions, for example lsp_dsp_fill_zero
    float v[0x1000];
    lsp_dsp_fill_zero(v, sizeof(v)/sizeof(float));
    
    // At the end, we need to restore the context and reset CPU settings to defaults
    lsp_dsp_finish(&ctx);
    
    return 0;
}


```

## SAST Tools

* [PVS-Studio](https://pvs-studio.com/en/pvs-studio/?utm_source=website&utm_medium=github&utm_campaign=open_source) - static analyzer for C, C++, C#, and Java code.
