/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
    #include <lsp-plug.in/common/debug.h>
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
        #include <private/dsp/arch/x86/avx512/convolution.h>
        #include <private/dsp/arch/x86/avx512/copy.h>
        #include <private/dsp/arch/x86/avx512/dynamics.h>
        #include <private/dsp/arch/x86/avx512/float.h>
        #include <private/dsp/arch/x86/avx512/fft.h>
        #include <private/dsp/arch/x86/avx512/graphics/axis.h>
        #include <private/dsp/arch/x86/avx512/hmath.h>
        #include <private/dsp/arch/x86/avx512/msmatrix.h>
        #include <private/dsp/arch/x86/avx512/pcomplex.h>
        #include <private/dsp/arch/x86/avx512/pmath.h>
        #include <private/dsp/arch/x86/avx512/search.h>
        #include <private/dsp/arch/x86/avx512/mix.h>
        #include <private/dsp/arch/x86/avx512/pan.h>

        #include <private/dsp/arch/x86/avx512/correlation.h>
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

            #define CEXPORT2_X64(cond, function, export) \
                IF_ARCH_X86_64(CEXPORT2(cond, function, export))

            #define X64_CEXPORT1(cond, export) \
                IF_ARCH_X86_64(CEXPORT1(cond, export))


            void dsp_init(const cpu_features_t *f)
            {
                // Enable AVX-512 only for CPUs that really support it well
                const bool favx512  = feature_check(f, FEAT_FAST_AVX512);
                if (!favx512)
                    return;

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
                CEXPORT1(vl, abs_max2);
                CEXPORT1(vl, abs_min2);

                CEXPORT1(vl, abs_add3);
                CEXPORT1(vl, abs_sub3);
                CEXPORT1(vl, abs_rsub3);
                CEXPORT1(vl, abs_mul3);
                CEXPORT1(vl, abs_div3);
                CEXPORT1(vl, abs_rdiv3);
                CEXPORT1(vl, abs_max3);
                CEXPORT1(vl, abs_min3);

                CEXPORT1(vl, exp1);
                CEXPORT1(vl, exp2);
                CEXPORT2_X64(vl, exp1, x64_exp1);
                CEXPORT2_X64(vl, exp2, x64_exp2);

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

                CEXPORT1(vl, logb1);
                CEXPORT1(vl, logb2);
                CEXPORT1(vl, loge1);
                CEXPORT1(vl, loge2);
                CEXPORT1(vl, logd1);
                CEXPORT1(vl, logd2);

                CEXPORT2_X64(vl, logb1, x64_logb1);
                CEXPORT2_X64(vl, logb2, x64_logb2);
                CEXPORT2_X64(vl, loge1, x64_loge1);
                CEXPORT2_X64(vl, loge2, x64_loge2);
                CEXPORT2_X64(vl, logd1, x64_logd1);
                CEXPORT2_X64(vl, logd2, x64_logd2);

                CEXPORT1(vl, cosf1);
                CEXPORT1(vl, cosf2);
                CEXPORT1(vl, cosf_kp1);
                CEXPORT2_X64(vl, cosf_kp1, x64_cosf_kp1);

                CEXPORT1(vl, sinf1);
                CEXPORT1(vl, sinf2);
                CEXPORT1(vl, sinf_kp1);
                CEXPORT2_X64(vl, sinf_kp1, x64_sinf_kp1);

                CEXPORT1(vl, lanczos1);
                CEXPORT2_X64(vl, lanczos1, x64_lanczos1);

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

                CEXPORT1(vl, add2);
                CEXPORT1(vl, sub2);
                CEXPORT1(vl, rsub2);
                CEXPORT1(vl, mul2);
                CEXPORT1(vl, div2);
                CEXPORT1(vl, rdiv2);
                CEXPORT1(vl, mod2);
                CEXPORT1(vl, rmod2);

                CEXPORT1(vl, add3);
                CEXPORT1(vl, sub3);
                CEXPORT1(vl, mul3);
                CEXPORT1(vl, div3);
                CEXPORT1(vl, mod3);

                CEXPORT1(vl, pmin2);
                CEXPORT1(vl, pmax2);
                CEXPORT1(vl, psmin2);
                CEXPORT1(vl, psmax2);
                CEXPORT1(vl, pamin2);
                CEXPORT1(vl, pamax2);
                CEXPORT1(vl, pmin3);
                CEXPORT1(vl, pmax3);
                CEXPORT1(vl, psmin3);
                CEXPORT1(vl, psmax3);
                CEXPORT1(vl, pamin3);
                CEXPORT1(vl, pamax3);

                CEXPORT2(vl, normalize, normalize2);
                CEXPORT1(vl, normalize1);
                CEXPORT1(vl, normalize2);

                CEXPORT1(vl, add_k2);
                CEXPORT1(vl, sub_k2);
                CEXPORT1(vl, rsub_k2);
                CEXPORT1(vl, mul_k2);
                CEXPORT1(vl, div_k2);
                CEXPORT1(vl, rdiv_k2);
                CEXPORT1(vl, mod_k2);
                CEXPORT1(vl, rmod_k2);

                CEXPORT1(vl, add_k3);
                CEXPORT1(vl, sub_k3);
                CEXPORT1(vl, rsub_k3);
                CEXPORT1(vl, mul_k3);
                CEXPORT1(vl, div_k3);
                CEXPORT1(vl, rdiv_k3);
                CEXPORT1(vl, mod_k3);
                CEXPORT1(vl, rmod_k3);

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
                CEXPORT1(vl, pcomplex_corr);

                CEXPORT1(vl, min);
                CEXPORT1(vl, max);
                CEXPORT1(vl, minmax);
                CEXPORT1(vl, abs_min);
                CEXPORT1(vl, abs_max);
                CEXPORT1(vl, abs_minmax);
                CEXPORT1(vl, sign_min);
                CEXPORT1(vl, sign_max);
                CEXPORT1(vl, sign_minmax);

                CEXPORT1(vl, min_index);
                CEXPORT1(vl, max_index);
                CEXPORT1(vl, minmax_index);
                CEXPORT1(vl, abs_min_index);
                CEXPORT1(vl, abs_max_index);
                CEXPORT1(vl, abs_minmax_index);

                CEXPORT1(vl, lr_to_ms);
                CEXPORT1(vl, lr_to_mid);
                CEXPORT1(vl, lr_to_side);
                CEXPORT1(vl, ms_to_lr);
                CEXPORT1(vl, ms_to_left);
                CEXPORT1(vl, ms_to_right);

                CEXPORT1(vl, convolve);

                CEXPORT1(vl, axis_apply_lin1);

                CEXPORT1(vl, compressor_x2_gain);
                CEXPORT1(vl, compressor_x2_curve);
                CEXPORT1(vl, gate_x1_gain);
                CEXPORT1(vl, gate_x1_curve);

                CEXPORT1(vl, uexpander_x1_gain);
                CEXPORT1(vl, uexpander_x1_curve);
                CEXPORT1(vl, dexpander_x1_gain);
                CEXPORT1(vl, dexpander_x1_curve);

                CEXPORT1(vl, corr_init);
                CEXPORT1(vl, corr_incr);

                CEXPORT1(vl, depan_lin);
                CEXPORT1(vl, depan_eqpow);

                CEXPORT1(vl, mix2);
                CEXPORT1(vl, mix_copy2);
                CEXPORT1(vl, mix_add2);
                CEXPORT1(vl, mix3);
                CEXPORT1(vl, mix_copy3);
                CEXPORT1(vl, mix_add3);
                CEXPORT1(vl, mix4);
                CEXPORT1(vl, mix_copy4);
                CEXPORT1(vl, mix_add4);

                CEXPORT1(vl, h_sum);
                CEXPORT1(vl, h_sqr_sum);
                CEXPORT1(vl, h_abs_sum);

                CEXPORT1(vl, h_dotp);
                CEXPORT1(vl, h_sqr_dotp);
                CEXPORT1(vl, h_abs_dotp);

                CEXPORT1(vl, direct_fft);
                CEXPORT1(vl, reverse_fft);
                CEXPORT1(vl, normalize_fft2);
                CEXPORT1(vl, normalize_fft3);
            }
        } /* namespace avx2 */
    } /* namespace lsp */

#endif /* ARCH_X86 */





