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
        #include <private/dsp/arch/x86/avx/3dmath_nz.h>
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

                CEXPORT1(favx, fcascade_fill_x2);
                CEXPORT1(favx, fcascade_fill_x4);
                CEXPORT1(favx, fcascade_fill_x8);
                CEXPORT1(favx, fcascade_fill_x16);

                // 3D math
                EXPORT1(init_point_xyz);
                EXPORT1(init_point);
                EXPORT1(normalize_point);

                EXPORT1(init_vector_dxyz);
                EXPORT1(init_vector);
                EXPORT1(normalize_vector);
                EXPORT1(normalize_vector2);

                EXPORT1(init_matrix3d_zero);
                EXPORT1(init_matrix3d_translate);
                EXPORT1(init_matrix3d_scale);
                EXPORT1(init_matrix3d_rotate_x);
                EXPORT1(init_matrix3d_rotate_y);
                EXPORT1(init_matrix3d_rotate_z);

                EXPORT1(transpose_matrix3d1);
                EXPORT1(transpose_matrix3d2);

                EXPORT1(apply_matrix3d_mm2);
                EXPORT1(apply_matrix3d_mm1);
                EXPORT1(apply_matrix3d_mv2);
                EXPORT1(apply_matrix3d_mv1);
                EXPORT1(apply_matrix3d_mp2);
                EXPORT1(apply_matrix3d_mp1);

                EXPORT1(calc_area_p3);
                EXPORT1(calc_area_pv);

                EXPORT1(colocation_x2_v1p2);
                EXPORT1(colocation_x2_v1pv);
                EXPORT1(colocation_x3_v1p3);
                EXPORT1(colocation_x3_v1pv);
                EXPORT1(colocation_x3_v3p1);
                EXPORT1(colocation_x3_vvp1);

                EXPORT1(split_triangle_raw);
                EXPORT1(cull_triangle_raw);

                EXPORT1(longest_edge3d_p3);
                EXPORT1(longest_edge3d_pv);

                EXPORT1(calc_normal3d_p3);
                EXPORT1(calc_normal3d_pv);
                EXPORT1(calc_normal3d_v2);
                EXPORT1(calc_normal3d_vv);

                EXPORT1(calc_plane_p3);
                EXPORT1(calc_plane_pv);
                EXPORT1(calc_plane_v1p2);

                EXPORT1(calc_split_point_p2v1);
                EXPORT1(calc_split_point_pvv1);

                EXPORT1(check_triplet3d_p3n);
                EXPORT1(check_triplet3d_pvn);
                EXPORT1(check_triplet3d_v2n);
                EXPORT1(check_triplet3d_vvn);
                EXPORT1(check_triplet3d_vv);

                EXPORT1(check_point3d_on_triangle_p3p);
                EXPORT1(check_point3d_on_triangle_pvp);

                // FMA3 support?
                if (f->hwcap[0] & CPU_HWCAP0_FMA3)
                {
                    CEXPORT2(ffma, biquad_process_x1, biquad_process_x1_fma3);
                    CEXPORT2(ffma, biquad_process_x2, biquad_process_x2_fma3);

                    CEXPORT2(favx, biquad_process_x4, biquad_process_x4_fma3);

                    CEXPORT2(ffma, dyn_biquad_process_x1, dyn_biquad_process_x1_fma3);
                    CEXPORT2(ffma, dyn_biquad_process_x2, dyn_biquad_process_x2_fma3);
                    CEXPORT2(ffma, dyn_biquad_process_x4, dyn_biquad_process_x4_fma3);

                    // 3D math
                    CEXPORT2(favx, apply_matrix3d_mm2, apply_matrix3d_mm2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mm1, apply_matrix3d_mm1_fma3);
                    CEXPORT2(favx, apply_matrix3d_mv2, apply_matrix3d_mv2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mv1, apply_matrix3d_mv1_fma3);
                    CEXPORT2(favx, apply_matrix3d_mp2, apply_matrix3d_mp2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mp1, apply_matrix3d_mp1_fma3);

                    CEXPORT2(favx, calc_area_p3, calc_area_p3_fma3);
                    CEXPORT2(favx, calc_area_pv, calc_area_pv_fma3);

                    CEXPORT2(favx, calc_normal3d_p3, calc_normal3d_p3_fma3);
                    CEXPORT2(favx, calc_normal3d_pv, calc_normal3d_pv_fma3);
                    CEXPORT2(favx, calc_normal3d_v2, calc_normal3d_v2_fma3);
                    CEXPORT2(favx, calc_normal3d_vv, calc_normal3d_vv_fma3);

                    CEXPORT2(favx, calc_plane_p3, calc_plane_p3_fma3);
                    CEXPORT2(favx, calc_plane_pv, calc_plane_pv_fma3);
                    CEXPORT2(favx, calc_plane_v1p2, calc_plane_v1p2_fma3);

                    CEXPORT2(favx, calc_split_point_p2v1, calc_split_point_p2v1_fma3);
                    CEXPORT2(favx, calc_split_point_pvv1, calc_split_point_pvv1_fma3);

                    CEXPORT2(favx, check_triplet3d_p3n, check_triplet3d_p3n_fma3);
                    CEXPORT2(favx, check_triplet3d_pvn, check_triplet3d_pvn_fma3);
                    CEXPORT2(favx, check_triplet3d_v2n, check_triplet3d_v2n_fma3);
                    CEXPORT2(favx, check_triplet3d_vvn, check_triplet3d_vvn_fma3);
                    CEXPORT2(favx, check_triplet3d_vv, check_triplet3d_vv_fma3);

                    CEXPORT2(favx, check_point3d_on_triangle_p3p, check_point3d_on_triangle_p3p_fma3);
                    CEXPORT2(favx, check_point3d_on_triangle_pvp, check_point3d_on_triangle_pvp_fma3);
                }
            }
        } /* namespace avx */
    } /* namespace lsp */

#endif /* ARCH_X86 */


