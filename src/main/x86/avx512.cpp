/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 мая 2023 г.
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
        #include <private/dsp/arch/x86/features.h>
    #undef PRIVATE_DSP_ARCH_X86_IMPL

    #define PRIVATE_DSP_ARCH_X86_AVX512_IMPL
        #include <private/dsp/arch/x86/avx512/copy.h>
        #include <private/dsp/arch/x86/avx512/float.h>
        #include <private/dsp/arch/x86/avx512/complex.h>
        #include <private/dsp/arch/x86/avx512/msmatrix.h>
    #undef PRIVATE_DSP_ARCH_X86_AVX512_IMPL

    namespace lsp
    {
        namespace avx512
        {
            using namespace x86;

            #define CEXPORT2(cond, function, export)    \
            { \
                TEST_EXPORT(avx512::export); \
                if (cond) \
                    dsp::function = avx512::export; \
            }

            #define CEXPORT1(cond, export) CEXPORT2(cond, export, export)

            void dsp_init(const cpu_features_t *f)
            {
                const bool vl = (f->features & (CPU_OPTION_AVX512F | CPU_OPTION_AVX512VL)) != (CPU_OPTION_AVX512F | CPU_OPTION_AVX512VL);

                CEXPORT1(vl, copy);
                CEXPORT1(vl, move);

                CEXPORT1(vl, limit1);
                CEXPORT1(vl, limit2);
                CEXPORT1(vl, sanitize1);
                CEXPORT1(vl, sanitize2);
                CEXPORT1(vl, saturate);
                CEXPORT1(vl, copy_saturated);
                CEXPORT1(vl, limit_saturate1);
                CEXPORT1(vl, limit_saturate2);

                CEXPORT1(vl, complex_mul2);
                CEXPORT1(vl, complex_mul3);
                CEXPORT1(vl, complex_mod);
                CEXPORT1(vl, complex_div2);
                CEXPORT1(vl, complex_rdiv2);
                CEXPORT1(vl, complex_div3);
                CEXPORT1(vl, complex_rcp1);
                CEXPORT1(vl, complex_rcp2);

                CEXPORT1(vl, lr_to_ms);
                CEXPORT1(vl, lr_to_mid);
                CEXPORT1(vl, lr_to_side);
                CEXPORT1(vl, ms_to_lr);
                CEXPORT1(vl, ms_to_left);
                CEXPORT1(vl, ms_to_right);
            }
        } /* namespace avx2 */
    } /* namespace lsp */

#endif /* ARCH_X86 */





