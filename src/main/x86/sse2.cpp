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


    #define PRIVATE_DSP_ARCH_X86_SSE2_IMPL
        #include <private/dsp/arch/x86/sse2/dynamics.h>

        #include <private/dsp/arch/x86/sse2/float.h>

        #include <private/dsp/arch/x86/sse2/search/iminmax.h>

        #include <private/dsp/arch/x86/sse2/graphics/axis.h>
        #include <private/dsp/arch/x86/sse2/graphics/colors.h>
        #include <private/dsp/arch/x86/sse2/graphics/effects.h>
        #include <private/dsp/arch/x86/sse2/graphics/pixelfmt.h>

        #include <private/dsp/arch/x86/sse2/pmath.h>

        #include <private/dsp/arch/x86/sse2/3dmath.h>
    #undef PRIVATE_DSP_ARCH_X86_SSE2_IMPL

    namespace lsp
    {
        namespace sse2
        {
            using namespace x86;

            #define EXPORT2(function, export) \
            { \
                dsp::function                       = sse2::export; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = sse2::export; \
                TEST_EXPORT(sse2::export); \
            }
            #define EXPORT1(function)                   EXPORT2(function, function);

            void dsp_init(const cpu_features_t *f)
            {
                if (((f->features) & (CPU_OPTION_SSE | CPU_OPTION_SSE2)) != (CPU_OPTION_SSE | CPU_OPTION_SSE2))
                    return;

                EXPORT1(copy_saturated);
                EXPORT1(saturate);
                EXPORT1(limit_saturate1);
                EXPORT1(limit_saturate2);
                EXPORT1(sanitize1);
                EXPORT1(sanitize2);

                EXPORT1(mod_k2);
                EXPORT1(rmod_k2);
                EXPORT1(mod_k3);
                EXPORT1(rmod_k3);

                EXPORT1(fmmod_k3);
                EXPORT1(fmrmod_k3);
                EXPORT1(fmmod_k4);
                EXPORT1(fmrmod_k4);

                EXPORT1(mod2);
                EXPORT1(rmod2);
                EXPORT1(mod3);

                EXPORT1(fmmod3);
                EXPORT1(fmrmod3);

                EXPORT1(fmmod4);
                EXPORT1(fmrmod4);

                EXPORT1(exp1);
                EXPORT1(exp2);
                EXPORT1(logb1);
                EXPORT1(logb2);
                EXPORT1(loge1);
                EXPORT1(loge2);
                EXPORT1(logd1);
                EXPORT1(logd2);
                EXPORT1(powcv1);
                EXPORT1(powcv2);
                EXPORT1(powvc1);
                EXPORT1(powvc2);
                EXPORT1(powvx1);
                EXPORT1(powvx2);

                EXPORT1(cosf1);
                EXPORT1(cosf2);
                EXPORT1(cosf_kp1);

                EXPORT1(sinf1);
                EXPORT1(sinf2);
                EXPORT1(sinf_kp1);

                EXPORT1(lanczos1);

                EXPORT1(min_index);
                EXPORT1(max_index);
                EXPORT1(minmax_index);

                EXPORT1(abs_min_index);
                EXPORT1(abs_max_index);
                EXPORT1(abs_minmax_index);

                EXPORT1(hsla_to_rgba);
                EXPORT1(rgba_to_hsla);
                EXPORT1(rgba_to_bgra32);

                EXPORT1(eff_hsla_hue);
                EXPORT1(eff_hsla_sat);
                EXPORT1(eff_hsla_light);
                EXPORT1(eff_hsla_alpha);

                EXPORT1(axis_apply_log1);
                EXPORT1(axis_apply_log2);
                EXPORT1(rgba32_to_bgra32);
                EXPORT1(abgr32_to_bgrff32);
                EXPORT2(prgba32_set_alpha, pabc32_set_alpha);
                EXPORT2(pbgra32_set_alpha, pabc32_set_alpha);

                EXPORT1(compressor_x2_gain)
                EXPORT1(compressor_x2_curve)
                EXPORT1(gate_x1_gain)
                EXPORT1(gate_x1_curve)
                EXPORT1(uexpander_x1_gain)
                EXPORT1(uexpander_x1_curve)
                EXPORT1(dexpander_x1_gain)
                EXPORT1(dexpander_x1_curve)

                // 3D Math
                EXPORT1(transpose_matrix3d1);
                EXPORT1(transpose_matrix3d2);

                EXPORT1(colocation_x2_v1p2);
                EXPORT1(colocation_x2_v1pv);
                EXPORT1(colocation_x3_v1p3);
                EXPORT1(colocation_x3_v1pv);
                EXPORT1(colocation_x3_v3p1);
                EXPORT1(colocation_x3_vvp1);

                EXPORT1(split_triangle_raw);
                EXPORT1(cull_triangle_raw);
            }

            #undef EXPORT1
            #undef EXPORT2
        } /* namespace sse2 */
    } /* namespace lsp */

#endif /* ARCH_X86 */
