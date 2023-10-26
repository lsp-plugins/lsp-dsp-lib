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
        #include <private/dsp/arch/x86/avx512/complex.h>
        #include <private/dsp/arch/x86/avx512/copy.h>
        #include <private/dsp/arch/x86/avx512/float.h>
        #include <private/dsp/arch/x86/avx512/graphics/axis.h>
        #include <private/dsp/arch/x86/avx512/msmatrix.h>
        #include <private/dsp/arch/x86/avx512/pcomplex.h>
        #include <private/dsp/arch/x86/avx512/pmath.h>
    #undef PRIVATE_DSP_ARCH_X86_AVX512_IMPL

    namespace lsp
    {
        namespace avx512
        {
            using namespace x86;

            #define CEXPORT2(cond, function, export)    \
            { \
                if (cond) \
                { \
                    TEST_EXPORT(avx512::export); \
                    dsp::function = avx512::export; \
                } \
            }

            #define CEXPORT1(cond, export) CEXPORT2(cond, export, export)

            #define X64_CEXPORT2(cond, function, export) \
                IF_ARCH_X86_64(CEXPORT2(cond, function, export))

            #define X64_CEXPORT1(cond, export) \
                IF_ARCH_X86_64(CEXPORT1(cond, export))


            void dsp_init(const cpu_features_t *f)
            {
                const bool vl = (f->features & (CPU_OPTION_AVX512F | CPU_OPTION_AVX512VL)) ==
                                (CPU_OPTION_AVX512F | CPU_OPTION_AVX512VL);

                CEXPORT1(vl, copy);
                CEXPORT1(vl, move);

                CEXPORT1(vl, abs1);
                CEXPORT1(vl, abs2);
                CEXPORT1(vl, abs_add2);
                CEXPORT1(vl, abs_sub2);
                CEXPORT1(vl, abs_rsub2);
                CEXPORT1(vl, abs_mul2);
                CEXPORT1(vl, abs_div2);
                CEXPORT1(vl, abs_rdiv2);

                CEXPORT1(vl, abs_add3);
                CEXPORT1(vl, abs_sub3);
                CEXPORT1(vl, abs_rsub3);
                CEXPORT1(vl, abs_mul3);
                CEXPORT1(vl, abs_div3);
                CEXPORT1(vl, abs_rdiv3);

                CEXPORT1(vl, exp1);
                CEXPORT1(vl, exp2);
                X64_CEXPORT2(vl, exp1, x64_exp1);
                X64_CEXPORT2(vl, exp2, x64_exp2);

                CEXPORT1(vl, fmadd_k3);
                CEXPORT1(vl, fmsub_k3);
                CEXPORT1(vl, fmrsub_k3);
                CEXPORT1(vl, fmmul_k3);
                CEXPORT1(vl, fmdiv_k3);
                CEXPORT1(vl, fmrdiv_k3);
                CEXPORT1(vl, fmmod_k3);
                CEXPORT1(vl, fmrmod_k3);

                CEXPORT1(vl, fmadd_k4);
                CEXPORT1(vl, fmsub_k4);
                CEXPORT1(vl, fmrsub_k4);
                CEXPORT1(vl, fmmul_k4);
                CEXPORT1(vl, fmdiv_k4);
                CEXPORT1(vl, fmrdiv_k4);
                CEXPORT1(vl, fmmod_k4);
                CEXPORT1(vl, fmrmod_k4);

                CEXPORT1(vl, fmadd3);
                CEXPORT1(vl, fmsub3);
                CEXPORT1(vl, fmrsub3);
                CEXPORT1(vl, fmmul3);
                CEXPORT1(vl, fmdiv3);
                CEXPORT1(vl, fmrdiv3);
                CEXPORT1(vl, fmmod3);
                CEXPORT1(vl, fmrmod3);

                CEXPORT1(vl, fmadd4);
                CEXPORT1(vl, fmsub4);
                CEXPORT1(vl, fmrsub4);
                CEXPORT1(vl, fmmul4);
                CEXPORT1(vl, fmdiv4);
                CEXPORT1(vl, fmrdiv4);
                CEXPORT1(vl, fmmod4);
                CEXPORT1(vl, fmrmod4);

                CEXPORT1(vl, lramp_set1);
                CEXPORT1(vl, lramp1);
                CEXPORT1(vl, lramp2);
                CEXPORT1(vl, lramp_add2);
                CEXPORT1(vl, lramp_sub2);
                CEXPORT1(vl, lramp_rsub2);
                CEXPORT1(vl, lramp_mul2);
                CEXPORT1(vl, lramp_div2);
                CEXPORT1(vl, lramp_rdiv2);
                CEXPORT1(vl, lramp_add3);
                CEXPORT1(vl, lramp_sub3);
                CEXPORT1(vl, lramp_rsub3);
                CEXPORT1(vl, lramp_mul3);
                CEXPORT1(vl, lramp_div3);
                CEXPORT1(vl, lramp_rdiv3);

                CEXPORT1(vl, sqr1);
                CEXPORT1(vl, sqr2);
                CEXPORT1(vl, ssqrt1);
                CEXPORT1(vl, ssqrt2);

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

                CEXPORT1(vl, pcomplex_mul2);
                CEXPORT1(vl, pcomplex_mul3);
                CEXPORT1(vl, pcomplex_mod);
                CEXPORT1(vl, pcomplex_div2);
                CEXPORT1(vl, pcomplex_rdiv2);
                CEXPORT1(vl, pcomplex_div3);
                CEXPORT1(vl, pcomplex_r2c);
                CEXPORT1(vl, pcomplex_r2c_add2);
                CEXPORT1(vl, pcomplex_r2c_rsub2);
                CEXPORT1(vl, pcomplex_r2c_sub2);
                CEXPORT1(vl, pcomplex_r2c_mul2);
                CEXPORT1(vl, pcomplex_r2c_div2);
                CEXPORT1(vl, pcomplex_c2r);

                CEXPORT1(vl, lr_to_ms);
                CEXPORT1(vl, lr_to_mid);
                CEXPORT1(vl, lr_to_side);
                CEXPORT1(vl, ms_to_lr);
                CEXPORT1(vl, ms_to_left);
                CEXPORT1(vl, ms_to_right);

                CEXPORT1(vl, axis_apply_lin1);
            }
        } /* namespace avx2 */
    } /* namespace lsp */

#endif /* ARCH_X86 */





