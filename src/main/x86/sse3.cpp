/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

    #define PRIVATE_DSP_ARCH_X86_SSE3_IMPL
        #include <private/dsp/arch/x86/sse3/copy.h>
        #include <private/dsp/arch/x86/sse3/graphics.h>
        #include <private/dsp/arch/x86/sse3/filters/static.h>
        #include <private/dsp/arch/x86/sse3/filters/dynamic.h>
        #include <private/dsp/arch/x86/sse3/filters/transform.h>
        #include <private/dsp/arch/x86/sse3/pcomplex.h>
        #include <private/dsp/arch/x86/sse3/3dmath.h>
        #include <private/dsp/arch/x86/sse3/correlation.h>
    #undef PRIVATE_DSP_ARCH_X86_SSE3_IMPL

    namespace lsp
    {
        namespace sse3
        {
            using namespace x86;

            #define EXPORT2(function, export) \
            { \
                dsp::function                       = sse3::export; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = sse3::export; \
                TEST_EXPORT(sse3::export); \
            }
            #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(dsp::function = sse3::export; TEST_EXPORT(sse3::export));
            #define EXPORT1(export)                         EXPORT2(export, export)
            #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(sse3::function))

            void dsp_init(const cpu_features_t *f)
            {
                if (!(f->features & CPU_OPTION_SSE3))
                    return;

                // Additional xmm registers are available only in 64-bit mode
                EXPORT1(pcomplex_mul2);
                EXPORT1(pcomplex_mul3);
                EXPORT1(pcomplex_div2);
                EXPORT1(pcomplex_rdiv2);
                EXPORT1(pcomplex_div3);

                if (!feature_check(f, FEAT_FAST_MOVS))
                {
                    EXPORT1(copy);
                    EXPORT1(move);
                }
                else
                {
                    TEST_EXPORT(copy);
                }
                EXPORT2(pcomplex_mod, pcomplex_mod);
                EXPORT2_X64(pcomplex_mod, x64_pcomplex_mod);
                EXPORT2_X64(pcomplex_corr, x64_pcomplex_corr);

                EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);
                EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);
                EXPORT2_X64(bilinear_transform_x8, x64_bilinear_transform_x8);
                EXPORT2_X64(axis_apply_log1, x64_axis_apply_log1);
                EXPORT2_X64(axis_apply_log2, x64_axis_apply_log2);
                EXPORT2_X64(pcomplex_mul2, x64_pcomplex_mul2);
                EXPORT2_X64(pcomplex_mul3, x64_pcomplex_mul3);

                EXPORT2_X64(rgba32_to_bgra32, x64_rgba32_to_bgra32);

                EXPORT1(calc_split_point_p2v1);
                EXPORT1(calc_split_point_pvv1);

                EXPORT1(colocation_x2_v1p2);
                EXPORT1(colocation_x2_v1pv);
                EXPORT1(colocation_x3_v1p3);
                EXPORT1(colocation_x3_v1pv);
                EXPORT1(colocation_x3_v3p1);
                EXPORT1(colocation_x3_vvp1);

                EXPORT1(calc_min_distance_p3);
                EXPORT1(calc_min_distance_pv);

                EXPORT1(split_triangle_raw);
                EXPORT1(cull_triangle_raw);

                EXPORT2_X64(corr_incr, x64_corr_incr);

                if (f->features & CPU_OPTION_SSSE3)
                {
                    EXPORT2(split_triangle_raw, split_triangle_raw_ssse3);
                    EXPORT2(cull_triangle_raw, cull_triangle_raw_ssse3);
                }
            }

            #undef EXPORT2
        } /* namespace sse3 */
    } /* namespace lsp */

#endif /* ARCH_X86 */
