/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
 *
 * lsp-dsp-lib is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-dsp-lib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-dsp-lib. If not, see <https://www.gnu.org/licenses/>.
 */

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/cpuid.h>

#ifdef ARCH_X86
    #include <private/dsp/exports.h>
    #include <lsp-plug.in/dsp/dsp.h>
    #include <lsp-plug.in/stdlib/math.h>
    #include <lsp-plug.in/stdlib/string.h>
    #include <lsp-plug.in/stdlib/stdio.h>

    #ifdef LSP_TESTING
        #include <lsp-plug.in/test-fw/test.h>
    #else
        #define TEST_EXPORT(...)
    #endif /* LSP_TESTING */

    #include <stdio.h>
    #include <stdlib.h>

    #define PRIVATE_DSP_ARCH_X86_IMPL
        #include <private/dsp/arch/x86/defs.h>
        #include <private/dsp/arch/x86/init.h>
        #include <private/dsp/arch/x86/float.h>
        #include <private/dsp/arch/x86/copy.h>
        #include <private/dsp/arch/x86/graphics.h>
    #undef PRIVATE_DSP_ARCH_X86_IMPL

    //-------------------------------------------------------------------------
    // References to extension initialization routines
    namespace lsp
    {
        namespace sse
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace sse2
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace sse3
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace sse4
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace avx
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace avx2
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace avx512
        {
            extern void dsp_init(const cpuid_t *f);
        }

        namespace x86
        {
            static dsp::start_t     dsp_start       = NULL;
            static dsp::finish_t    dsp_finish      = NULL;

            static void start(dsp::context_t *ctx)
            {
                dsp_start(ctx);
                uint32_t cr                 = fpu_read_cr();
                ctx->data[ctx->top++]       = cr;

                fpu_write_cr(cr);
            }

            static void finish(dsp::context_t *ctx)
            {
                fpu_write_cr(ctx->data[--ctx->top]);
                dsp_finish(ctx);
            }

            bool feature_check(const cpuid_t *f, feature_t ops)
            {
                switch (ops)
                {
                    case FEAT_FAST_MOVS:
                        if (f->vendor == CPU_VENDOR_INTEL)
                        {
                            // Should be some Core i3 microarchitecture...
                            if (f->micro >= CPU_MICRO_INTEL_SKYLAKE)
                                return true;
                        }
                        break;

                    case FEAT_FAST_AVX:
                    case FEAT_FAST_FMA3:
                        if (f->vendor == CPU_VENDOR_INTEL) // Any Intel CPU is good enough with AVX
                            return true;
                        // Only starting with ZEN 1 architecture AMD's implementation of AVX is fast enough
                        if ((f->vendor == CPU_VENDOR_AMD) || (f->vendor == CPU_VENDOR_HYGON))
                        {
                            if (f->micro < CPU_MICRO_AMD_ZEN2)
                                return false;
                            if (f->micro == CPU_MICRO_AMD_DHYANA)
                                return false;
                            return true;
                        }
                        break;

                    case FEAT_FAST_AVX512:
                        if (f->vendor == CPU_VENDOR_INTEL) // Any Intel CPU seems to be good enough with AVX-512
                            return true;
                        // Only starting with ZEN 1 architecture AMD's implementation of AVX is fast enough
                        if ((f->vendor == CPU_VENDOR_AMD) || (f->vendor == CPU_VENDOR_HYGON))
                        {
                            if (f->micro == CPU_MICRO_AMD_DHYANA)
                                return false;
                            if (f->micro < CPU_MICRO_AMD_ZEN3)
                                return false;
                            return true;
                        }
                        break;

                    case FEAT_BELOW_ZEN3: // Test that this is AMD and below Zen 3 architecture
                        if ((f->vendor == CPU_VENDOR_AMD) || (f->vendor == CPU_VENDOR_HYGON))
                        {
                            if (f->micro < CPU_MICRO_AMD_ZEN3)
                                return true;
                            return false;
                        }
                        break;
                    default:
                        break;
                }

                return false;
            }

            #define EXPORT2(function, export) \
            { \
                dsp::function                       = x86::export; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = x86::export; \
                TEST_EXPORT(x86::export); \
            }
            #define EXPORT1(function)                   EXPORT2(function, function)

            void dsp_init(const cpuid_t *f)
            {
                // Save previous entry points
                dsp_start                   = dsp::start;
                dsp_finish                  = dsp::finish;

                // Export functions
                EXPORT1(start);
                EXPORT1(finish);

                EXPORT1(copy);
                EXPORT1(rgba32_to_bgra32);
                EXPORT1(abgr32_to_bgra32);
                EXPORT2(prgba32_set_alpha, pabc32_set_alpha);
                EXPORT2(pbgra32_set_alpha, pabc32_set_alpha);

                // Initialize extensions
                sse::dsp_init(f);
                sse2::dsp_init(f);
                sse3::dsp_init(f);
                sse4::dsp_init(f);
                avx::dsp_init(f);
                avx2::dsp_init(f);
                avx512::dsp_init(f);
            }

            #undef EXPORT1
            #undef EXPORT2
        } /* namespace x86 */
    } /* namespace lsp */

#endif /* ARCH_X86 */

