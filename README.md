# lsp-dsp-lib

DSP library for digital signal processing (and more)

This library provides set of functions that perform SIMD-optimized
computing on several hardware architectures.

Currently supported set of SIMD extensions:
  * i586 architecture (32-bit): SSE, SSE2, SSE3, AVX, AVX2 and FMA3;
  * x86_64 architecture (64-bit): SSE, SSE2, SSE3, AVX, AVX2 and FMA3;
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
  * Some set of function to work with 3D mathematics.

Requirements
======

The following packages need to be installed for building:

* gcc >= 4.9
* make >= 4.0

Building
======

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

Usage
======

Here's the code snippet of how the library can be initialized and used:

```C++

#include <lsp-plug.in/dsp/dsp.h>
#include <stdio.h>


int main(int argc, const char **argv)
{
    // Initialize DSP
    dsp::init();

    // Optionally: output information about the system
    dsp::info_t *info = dsp::info();
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
    // CPUs that support them
    dsp::context_t ctx;
    dsp::start(&ctx);
    
    // Here we call some dsp functions, for example dsp::fill_zero
    float v[0x1000];
    dsp::fill_zero(v, sizeof(v)/sizeof(float));
    
    // At the end, we need to restore the context
    dsp::finish(&ctx);
    
    return 0;
}


```



