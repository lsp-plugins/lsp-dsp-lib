/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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
#include <lsp-plug.in/common/cpuid.h>

#ifdef ARCH_X86
    #include <private/dsp/exports.h>
    #include <lsp-plug.in/dsp/dsp.h>
    #include <lsp-plug.in/common/bits.h>
    #include <lsp-plug.in/stdlib/math.h>

    // Test framework
    #ifdef LSP_TESTING
        #include <lsp-plug.in/test-fw/test.h>
    #else
        #define TEST_EXPORT(...)
    #endif /* LSP_TESTING */

    // Feature detection
    #define PRIVATE_DSP_ARCH_X86_IMPL
        #include <private/dsp/arch/x86/defs.h>
        #include <private/dsp/arch/x86/init.h>
    #undef PRIVATE_DSP_ARCH_X86_IMPL

    // AVX-specific function implementations
    #define PRIVATE_DSP_ARCH_X86_AVX_IMPL
        #include <private/dsp/arch/x86/avx/export.h>

        #include <private/dsp/arch/x86/avx/filters_nz.h>
    #undef PRIVATE_DSP_ARCH_X86_AVX_IMPL

    namespace lsp
    {
        namespace avx
        {
            using namespace x86;

            void dsp_init_nz(const cpuid_t *f)
            {
                if (!(f->hwcap[0] & CPU_HWCAP0_AVX))
                    return;

                // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
                // Not tested on AMD Processors above Bulldozer family
                const bool favx = feature_check(f, FEAT_FAST_AVX);
                const bool ffma = favx && feature_check(f, FEAT_FAST_FMA3);

                CEXPORT1(favx, biquad_process_x1);
                CEXPORT1(favx, biquad_process_x2);
                CEXPORT1(favx, biquad_process_x4);

                CEXPORT1(favx, dyn_biquad_process_x1);
                CEXPORT1(favx, dyn_biquad_process_x2);
                CEXPORT1(favx, dyn_biquad_process_x4);

                CEXPORT1(favx, biquad_pack_x2);
                CEXPORT1(favx, biquad_pack_x4);

                // FMA3 support?
                if (f->hwcap[0] & CPU_HWCAP0_FMA3)
                {
                    CEXPORT2(ffma, biquad_process_x1, biquad_process_x1_fma3);
                    CEXPORT2(ffma, biquad_process_x2, biquad_process_x2_fma3);

                    CEXPORT2(favx, biquad_process_x4, biquad_process_x4_fma3);

                    CEXPORT2(ffma, dyn_biquad_process_x1, dyn_biquad_process_x1_fma3);
                    CEXPORT2(ffma, dyn_biquad_process_x2, dyn_biquad_process_x2_fma3);
                    CEXPORT2(ffma, dyn_biquad_process_x4, dyn_biquad_process_x4_fma3);
                }
            }
        } /* namespace avx */
    } /* namespace lsp */

#endif /* ARCH_X86 */


