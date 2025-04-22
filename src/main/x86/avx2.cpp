/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

    #define PRIVATE_DSP_ARCH_X86_AVX2_IMPL
        #include <private/dsp/arch/x86/avx2/dynamics.h>

        #include <private/dsp/arch/x86/avx2/float.h>
        #include <private/dsp/arch/x86/avx2/pmath.h>

        #include <private/dsp/arch/x86/avx2/fft/normalize.h>

        #include <private/dsp/arch/x86/avx2/search/iminmax.h>

        #include <private/dsp/arch/x86/avx2/graphics/colors.h>
        #include <private/dsp/arch/x86/avx2/graphics/effects.h>
        #include <private/dsp/arch/x86/avx2/graphics/pixelfmt.h>
    #undef PRIVATE_DSP_ARCH_X86_AVX2_IMPL

namespace lsp
{
    namespace avx2
    {
        using namespace x86;

        #define EXPORT2(function, export) \
        { \
            dsp::function                       = avx2::export; \
            dsp::LSP_DSP_LIB_MANGLE(function)   = avx2::export; \
            TEST_EXPORT(avx2::export); \
        }
        #define EXPORT1(function)                       EXPORT2(function, function)

        #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
        #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx2::function))

        #define CEXPORT2(cond, function, export)    \
        IF_ARCH_X86( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::function = avx2::export; \
            );

        #define CEXPORT1(cond, export)    \
        IF_ARCH_X86( \
                TEST_EXPORT(avx2::export); \
                if (cond) \
                    dsp::export = avx2::export; \
            );

        #define CEXPORT2_X64(cond, function, export)    \
            IF_ARCH_X86_64( \
                    TEST_EXPORT(avx2::export); \
                    if (cond) \
                        dsp::function = avx2::export; \
                );

        #define CEXPORT1_X64(cond, export)    \
            IF_ARCH_X86_64( \
                    TEST_EXPORT(avx2::export); \
                    if (cond) \
                        dsp::export = avx2::export; \
                );

        void dsp_init(const cpu_features_t *f)
        {
            if ((f->features & (CPU_OPTION_AVX | CPU_OPTION_AVX2)) != (CPU_OPTION_AVX | CPU_OPTION_AVX2))
                return;

            bool favx   = feature_check(f, FEAT_FAST_AVX);

            CEXPORT1(favx, limit_saturate1);
            CEXPORT1(favx, limit_saturate2);
            CEXPORT1(favx, copy_saturated);
            CEXPORT1(favx, saturate);
            CEXPORT1(favx, sanitize1);
            CEXPORT1(favx, sanitize2);

            CEXPORT1(favx, add_k2);
            CEXPORT1(favx, sub_k2);
            CEXPORT1(favx, rsub_k2);
            CEXPORT1(favx, mul_k2);
            CEXPORT1(favx, div_k2);
            CEXPORT1(favx, rdiv_k2);
            CEXPORT1(favx, mod_k2);
            CEXPORT1(favx, rmod_k2);

            CEXPORT1(favx, add_k3);
            CEXPORT1(favx, sub_k3);
            CEXPORT1(favx, rsub_k3);
            CEXPORT1(favx, mul_k3);
            CEXPORT1(favx, div_k3);
            CEXPORT1(favx, rdiv_k3);
            CEXPORT1(favx, mod_k3);
            CEXPORT1(favx, rmod_k3);

            CEXPORT1(favx, fmadd_k3);
            CEXPORT1(favx, fmsub_k3);
            CEXPORT1(favx, fmrsub_k3);
            CEXPORT1(favx, fmmul_k3);
            CEXPORT1(favx, fmdiv_k3);
            CEXPORT1(favx, fmrdiv_k3);
            CEXPORT1(favx, fmmod_k3);
            CEXPORT1(favx, fmrmod_k3);

            CEXPORT1(favx, fmadd_k4);
            CEXPORT1(favx, fmsub_k4);
            CEXPORT1(favx, fmrsub_k4);
            CEXPORT1(favx, fmmul_k4);
            CEXPORT1(favx, fmdiv_k4);
            CEXPORT1(favx, fmrdiv_k4);
            CEXPORT1(favx, fmmod_k4);
            CEXPORT1(favx, fmrmod_k4);

            CEXPORT2(favx, exp1, exp1);
            CEXPORT2(favx, exp2, exp2);

            CEXPORT2_X64(favx, exp1, x64_exp1);
            CEXPORT2_X64(favx, exp2, x64_exp2);

            EXPORT1(min_index);
            EXPORT1(max_index);
            EXPORT1(minmax_index);

            EXPORT1(abs_min_index);
            EXPORT1(abs_max_index);
            EXPORT1(abs_minmax_index);

            CEXPORT1(favx, logb1);
            CEXPORT1(favx, logb2);
            CEXPORT1(favx, loge1);
            CEXPORT1(favx, loge2);
            CEXPORT1(favx, logd1);
            CEXPORT1(favx, logd2);

            CEXPORT2_X64(favx, logb1, x64_logb1);
            CEXPORT2_X64(favx, logb2, x64_logb2);
            CEXPORT2_X64(favx, loge1, x64_loge1);
            CEXPORT2_X64(favx, loge2, x64_loge2);
            CEXPORT2_X64(favx, logd1, x64_logd1);
            CEXPORT2_X64(favx, logd2, x64_logd2);

            CEXPORT2_X64(favx, powcv1, x64_powcv1);
            CEXPORT2_X64(favx, powcv2, x64_powcv2);
            CEXPORT2_X64(favx, powvc1, x64_powvc1);
            CEXPORT2_X64(favx, powvc2, x64_powvc2);
            CEXPORT2_X64(favx, powvx1, x64_powvx1);
            CEXPORT2_X64(favx, powvx2, x64_powvx2);

            CEXPORT1(favx, cosf1);
            CEXPORT1(favx, cosf2);
            CEXPORT1(favx, cosf_kp1);
            CEXPORT2_X64(favx, cosf_kp1, x64_cosf_kp1);

            CEXPORT1(favx, sinf1);
            CEXPORT1(favx, sinf2);
            CEXPORT1(favx, sinf_kp1);
            CEXPORT2_X64(favx, sinf_kp1, x64_sinf_kp1);

            CEXPORT1(favx, lanczos1);
            CEXPORT2_X64(favx, lanczos1, x64_lanczos1);

            CEXPORT2_X64(favx, eff_hsla_hue, x64_eff_hsla_hue);
            CEXPORT2_X64(favx, eff_hsla_sat, x64_eff_hsla_sat);
            CEXPORT2_X64(favx, eff_hsla_light, x64_eff_hsla_light);
            CEXPORT2_X64(favx, eff_hsla_alpha, x64_eff_hsla_alpha);

            CEXPORT2(favx, hsla_to_rgba, hsla_to_rgba);
            CEXPORT2(favx, rgba_to_hsla, rgba_to_hsla);

            CEXPORT1(favx, normalize_fft2);
            CEXPORT1(favx, normalize_fft3);
            CEXPORT1(favx, abgr32_to_bgrff32);
            CEXPORT2(favx, prgba32_set_alpha, pabc32_set_alpha);
            CEXPORT2(favx, pbgra32_set_alpha, pabc32_set_alpha);

            CEXPORT1(favx, fmrmod_k4);

            CEXPORT1(favx, compressor_x2_gain);
            CEXPORT1(favx, compressor_x2_curve);
            CEXPORT2_X64(favx, compressor_x2_gain, x64_compressor_x2_gain);
            CEXPORT2_X64(favx, compressor_x2_curve, x64_compressor_x2_curve);

            CEXPORT1(favx, gate_x1_gain);
            CEXPORT1(favx, gate_x1_curve);
            CEXPORT2_X64(favx, gate_x1_gain, x64_gate_x1_gain);
            CEXPORT2_X64(favx, gate_x1_curve, x64_gate_x1_curve);

            CEXPORT1(favx, uexpander_x1_gain);
            CEXPORT1(favx, uexpander_x1_curve);
            CEXPORT2_X64(favx, uexpander_x1_gain, x64_uexpander_x1_gain);
            CEXPORT2_X64(favx, uexpander_x1_curve, x64_uexpander_x1_curve);
            CEXPORT1(favx, dexpander_x1_gain);
            CEXPORT1(favx, dexpander_x1_curve);
            CEXPORT2_X64(favx, dexpander_x1_gain, x64_dexpander_x1_gain);
            CEXPORT2_X64(favx, dexpander_x1_curve, x64_dexpander_x1_curve);

            if (f->features & CPU_OPTION_FMA3)
            {
                CEXPORT2(favx, mod_k2, mod_k2_fma3);
                CEXPORT2(favx, rmod_k2, rmod_k2_fma3);

                CEXPORT2(favx, mod_k3, mod_k3_fma3);
                CEXPORT2(favx, rmod_k3, rmod_k3_fma3);

                CEXPORT2(favx, fmadd_k3, fmadd_k3_fma3);
                CEXPORT2(favx, fmsub_k3, fmsub_k3_fma3);
                CEXPORT2(favx, fmrsub_k3, fmrsub_k3_fma3);
                CEXPORT2(favx, fmmod_k3, fmmod_k3_fma3);
                CEXPORT2(favx, fmrmod_k3, fmrmod_k3_fma3);

                CEXPORT2(favx, fmadd_k4, fmadd_k4_fma3);
                CEXPORT2(favx, fmsub_k4, fmsub_k4_fma3);
                CEXPORT2(favx, fmrsub_k4, fmrsub_k4_fma3);
                CEXPORT2(favx, fmmod_k4, fmmod_k4_fma3);
                CEXPORT2(favx, fmrmod_k4, fmrmod_k4_fma3);

                CEXPORT2(favx, exp1, exp1_fma3);
                CEXPORT2(favx, exp2, exp2_fma3);

                CEXPORT2_X64(favx, exp1, x64_exp1_fma3);
                CEXPORT2_X64(favx, exp2, x64_exp2_fma3);

                CEXPORT2(favx, logb1, logb1_fma3);
                CEXPORT2(favx, logb2, logb2_fma3);
                CEXPORT2(favx, loge1, loge1_fma3);
                CEXPORT2(favx, loge2, loge2_fma3);
                CEXPORT2(favx, logd1, logd1_fma3);
                CEXPORT2(favx, logd2, logd2_fma3);

                CEXPORT2_X64(favx, logb1, x64_logb1_fma3);
                CEXPORT2_X64(favx, logb2, x64_logb2_fma3);
                CEXPORT2_X64(favx, loge1, x64_loge1_fma3);
                CEXPORT2_X64(favx, loge2, x64_loge2_fma3);
                CEXPORT2_X64(favx, logd1, x64_logd1_fma3);
                CEXPORT2_X64(favx, logd2, x64_logd2_fma3);

                CEXPORT2_X64(favx, powcv1, x64_powcv1_fma3);
                CEXPORT2_X64(favx, powcv2, x64_powcv2_fma3);
                CEXPORT2_X64(favx, powvc1, x64_powvc1_fma3);
                CEXPORT2_X64(favx, powvc2, x64_powvc2_fma3);
                CEXPORT2_X64(favx, powvx1, x64_powvx1_fma3);
                CEXPORT2_X64(favx, powvx2, x64_powvx2_fma3);

                CEXPORT2(favx, cosf1, cosf1_fma3);
                CEXPORT2(favx, cosf2, cosf2_fma3);
                CEXPORT2(favx, cosf_kp1, cosf_kp1_fma3);
                CEXPORT2_X64(favx, cosf_kp1, x64_cosf_kp1_fma3);

                CEXPORT2(favx, sinf1, sinf1_fma3);
                CEXPORT2(favx, sinf2, sinf2_fma3);
                CEXPORT2(favx, sinf_kp1, sinf_kp1_fma3);
                CEXPORT2_X64(favx, sinf_kp1, x64_sinf_kp1_fma3);

                CEXPORT2(favx, lanczos1, lanczos1_fma3);
                CEXPORT2_X64(favx, lanczos1, x64_lanczos1_fma3);

                CEXPORT2(favx, compressor_x2_gain, compressor_x2_gain_fma3);
                CEXPORT2(favx, compressor_x2_curve, compressor_x2_curve_fma3);
                CEXPORT2_X64(favx, compressor_x2_gain, x64_compressor_x2_gain_fma3);
                CEXPORT2_X64(favx, compressor_x2_curve, x64_compressor_x2_curve_fma3);

                CEXPORT2(favx, gate_x1_gain, gate_x1_gain_fma3);
                CEXPORT2(favx, gate_x1_curve, gate_x1_curve_fma3);
                CEXPORT2_X64(favx, gate_x1_gain, x64_gate_x1_gain_fma3);
                CEXPORT2_X64(favx, gate_x1_curve, x64_gate_x1_curve_fma3);

                CEXPORT2(favx, uexpander_x1_gain, uexpander_x1_gain_fma3);
                CEXPORT2(favx, uexpander_x1_curve, uexpander_x1_curve_fma3);
                CEXPORT2_X64(favx, uexpander_x1_gain, x64_uexpander_x1_gain_fma3);
                CEXPORT2_X64(favx, uexpander_x1_curve, x64_uexpander_x1_curve_fma3);

                CEXPORT2(favx, dexpander_x1_gain, dexpander_x1_gain_fma3);
                CEXPORT2(favx, dexpander_x1_curve, dexpander_x1_curve_fma3);
                CEXPORT2_X64(favx, dexpander_x1_gain, x64_dexpander_x1_gain_fma3);
                CEXPORT2_X64(favx, dexpander_x1_curve, x64_dexpander_x1_curve_fma3);
            }
        }
    } /* namespace avx2 */
} /* namespace lsp */

#endif /* ARCH_X86 */


