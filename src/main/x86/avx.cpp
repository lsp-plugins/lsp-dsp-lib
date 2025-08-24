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

    // AVX-specific function implementations
    #define PRIVATE_DSP_ARCH_X86_AVX_IMPL
        #include <private/dsp/arch/x86/avx/xcr.h>

        #include <private/dsp/arch/x86/avx/copy.h>
        #include <private/dsp/arch/x86/avx/float.h>
        #include <private/dsp/arch/x86/avx/complex.h>
        #include <private/dsp/arch/x86/avx/pcomplex.h>

        #include <private/dsp/arch/x86/avx/pmath.h>

        #include <private/dsp/arch/x86/avx/hmath/hsum.h>
        #include <private/dsp/arch/x86/avx/hmath/hdotp.h>

        #include <private/dsp/arch/x86/avx/mix.h>
        #include <private/dsp/arch/x86/avx/pan.h>
        #include <private/dsp/arch/x86/avx/search/minmax.h>

        #include <private/dsp/arch/x86/avx/fft.h>
        #include <private/dsp/arch/x86/avx/pfft.h>
        #include <private/dsp/arch/x86/avx/fastconv.h>

        #include <private/dsp/arch/x86/avx/filters/static.h>
        #include <private/dsp/arch/x86/avx/filters/dynamic.h>
        #include <private/dsp/arch/x86/avx/filters/transform.h>
        #include <private/dsp/arch/x86/avx/filters/transfer.h>

        #include <private/dsp/arch/x86/avx/msmatrix.h>
        #include <private/dsp/arch/x86/avx/resampling.h>
        #include <private/dsp/arch/x86/avx/convolution.h>
        #include <private/dsp/arch/x86/avx/correlation.h>

        #include <private/dsp/arch/x86/avx/interpolation/linear.h>

        #include <private/dsp/arch/x86/avx/graphics/axis.h>
        #include <private/dsp/arch/x86/avx/graphics/pixelfmt.h>

        #include <private/dsp/arch/x86/avx/3dmath.h>
    #undef PRIVATE_DSP_ARCH_X86_AVX_IMPL

    namespace lsp
    {
        namespace avx
        {
            using namespace x86;

            #define EXPORT2(function, export) \
            { \
                dsp::function                       = avx::export; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = avx::export; \
                TEST_EXPORT(avx::export); \
            }
            #define EXPORT1(function)                       EXPORT2(function, function)

            #define EXPORT2_X64(function, export)           IF_ARCH_X86_64(EXPORT2(function, export));
            #define SUPPORT_X64(function)                   IF_ARCH_X86_64(TEST_EXPORT(avx::function))

            #define CEXPORT2(cond, function, export)    \
            IF_ARCH_X86( \
                    TEST_EXPORT(avx::export); \
                    if (cond) \
                        dsp::function = avx::export; \
                );

            #define CEXPORT1(cond, export)    \
            IF_ARCH_X86( \
                    TEST_EXPORT(avx::export); \
                    if (cond) \
                        dsp::export = avx::export; \
                );

            #define CEXPORT2_X64(cond, function, export)    \
                IF_ARCH_X86_64( \
                        TEST_EXPORT(avx::export); \
                        if (cond) \
                            dsp::function = avx::export; \
                    );

            #define CEXPORT1_X64(cond, export)    \
                IF_ARCH_X86_64( \
                        TEST_EXPORT(avx::export); \
                        if (cond) \
                            dsp::export = avx::export; \
                    );

            void dsp_init(const cpu_features_t *f)
            {
                if (!(f->features & CPU_OPTION_AVX))
                    return;

                // This routine sucks on AMD Bulldozer processor family but is pretty great on Intel
                // Not tested on AMD Processors above Bulldozer family
                bool favx       = feature_check(f, FEAT_FAST_AVX);
                bool ffma       = favx && feature_check(f, FEAT_FAST_FMA3);
                bool below_zen3 = feature_check(f, FEAT_BELOW_ZEN3);

                CEXPORT2_X64(favx, reverse1, reverse1);
                CEXPORT2_X64(favx, reverse2, reverse2);

                CEXPORT1(favx, copy);
                CEXPORT1(favx, move);
                CEXPORT1(favx, fill);
                CEXPORT1(favx, fill_zero);
                CEXPORT1(favx, fill_one);
                CEXPORT1(favx, fill_minus_one);

                CEXPORT1(favx, limit1);
                CEXPORT1(favx, limit2);
                CEXPORT1(favx, sanitize1);
                CEXPORT1(favx, sanitize2);

                // Conditional export, depending on fast AVX implementation
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

                CEXPORT1(favx, add2);
                CEXPORT1(favx, sub2);
                CEXPORT1(favx, rsub2);
                CEXPORT1(favx, mul2);
                CEXPORT1(favx, div2);
                CEXPORT1(favx, rdiv2);
                CEXPORT1(favx, mod2);
                CEXPORT1(favx, rmod2);

                CEXPORT1(favx, add3);
                CEXPORT1(favx, sub3);
                CEXPORT1(favx, mul3);
                CEXPORT1(favx, div3);
                CEXPORT1(favx, mod3);

                CEXPORT1(favx, pmin2);
                CEXPORT1(favx, pmax2);
                CEXPORT1(favx, psmin2);
                CEXPORT1(favx, psmax2);
                CEXPORT1(favx, pamin2);
                CEXPORT1(favx, pamax2);
                CEXPORT1(favx, pmin3);
                CEXPORT1(favx, pmax3);
                CEXPORT1(favx, psmin3);
                CEXPORT1(favx, psmax3);
                CEXPORT1(favx, pamin3);
                CEXPORT1(favx, pamax3);

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

                CEXPORT1(favx, fmadd3);
                CEXPORT1(favx, fmsub3);
                CEXPORT1(favx, fmrsub3);
                CEXPORT1(favx, fmmul3);
                CEXPORT1(favx, fmdiv3);
                CEXPORT1(favx, fmrdiv3);
                CEXPORT1(favx, fmmod3);
                CEXPORT1(favx, fmrmod3);

                CEXPORT1(favx, fmadd4);
                CEXPORT1(favx, fmsub4);
                CEXPORT1(favx, fmrsub4);
                CEXPORT1(favx, fmmul4);
                CEXPORT1(favx, fmdiv4);
                CEXPORT1(favx, fmrdiv4);
                CEXPORT1(favx, fmmod4);
                CEXPORT1(favx, fmrmod4);

                CEXPORT1(favx, sqr1);
                CEXPORT1(favx, sqr2);
                CEXPORT1(favx, ssqrt1);
                CEXPORT1(favx, ssqrt2);

                CEXPORT1(favx, lramp_set1);
                CEXPORT1(favx, lramp1);
                CEXPORT1(favx, lramp2);
                CEXPORT1(favx, lramp_add2);
                CEXPORT1(favx, lramp_sub2);
                CEXPORT1(favx, lramp_rsub2);
                CEXPORT1(favx, lramp_mul2);
                CEXPORT1(favx, lramp_div2);
                CEXPORT1(favx, lramp_rdiv2);
                CEXPORT1(favx, lramp_add3);
                CEXPORT1(favx, lramp_sub3);
                CEXPORT1(favx, lramp_rsub3);
                CEXPORT1(favx, lramp_mul3);
                CEXPORT1(favx, lramp_div3);
                CEXPORT1(favx, lramp_rdiv3);

                CEXPORT2_X64(favx, abs_add2, x64_abs_add2);
                CEXPORT2_X64(favx, abs_sub2, x64_abs_sub2);
                CEXPORT2_X64(favx, abs_rsub2, x64_abs_rsub2);
                CEXPORT2_X64(favx, abs_mul2, x64_abs_mul2);
                CEXPORT2_X64(favx, abs_div2, x64_abs_div2);
                CEXPORT2_X64(favx, abs_rdiv2, x64_abs_rdiv2);
                CEXPORT2_X64(favx, abs_max2, x64_abs_max2);
                CEXPORT2_X64(favx, abs_min2, x64_abs_min2);

                CEXPORT2_X64(favx, abs_add3, x64_abs_add3);
                CEXPORT2_X64(favx, abs_sub3, x64_abs_sub3);
                CEXPORT2_X64(favx, abs_rsub3, x64_abs_rsub3);
                CEXPORT2_X64(favx, abs_mul3, x64_abs_mul3);
                CEXPORT2_X64(favx, abs_div3, x64_abs_div3);
                CEXPORT2_X64(favx, abs_rdiv3, x64_abs_rdiv3);
                CEXPORT2_X64(favx, abs_max3, x64_abs_max3);
                CEXPORT2_X64(favx, abs_min3, x64_abs_min3);

                CEXPORT2_X64(favx, abs1, x64_abs1);
                CEXPORT2_X64(favx, abs2, x64_abs2);

                CEXPORT1(favx, complex_mul2);
                CEXPORT1(favx, complex_mul3);
                CEXPORT1(favx, complex_div2);
                CEXPORT1(favx, complex_rdiv2);
                CEXPORT1(favx, complex_div3);
                CEXPORT1(favx, complex_mod);
                CEXPORT1(favx, complex_rcp1);
                CEXPORT1(favx, complex_rcp2);

                CEXPORT1(favx, pcomplex_mul2);
                CEXPORT1(favx, pcomplex_mul3);
                CEXPORT1(favx, pcomplex_div2);
                CEXPORT1(favx, pcomplex_rdiv2);
                CEXPORT1(favx, pcomplex_div3);
                CEXPORT1(favx, pcomplex_mod);
                CEXPORT1(favx, pcomplex_rcp1);
                CEXPORT1(favx, pcomplex_rcp2);

                CEXPORT1(favx, pcomplex_r2c);
                CEXPORT1(favx, pcomplex_r2c_add2);
                CEXPORT1(favx, pcomplex_r2c_sub2);
                CEXPORT1(favx, pcomplex_r2c_rsub2);
                CEXPORT1(favx, pcomplex_r2c_mul2);
                CEXPORT1(favx, pcomplex_r2c_div2);
                CEXPORT1(favx, pcomplex_r2c_rdiv2);
                CEXPORT1(favx, pcomplex_corr);

                CEXPORT1(favx, biquad_process_x1);
                CEXPORT1(favx, biquad_process_x2);
                CEXPORT1(favx, biquad_process_x4);
                EXPORT2_X64(biquad_process_x8, x64_biquad_process_x8);

                CEXPORT1(favx, dyn_biquad_process_x1);
                CEXPORT1(favx, dyn_biquad_process_x2);
                CEXPORT1(favx, dyn_biquad_process_x4);
                EXPORT2_X64(dyn_biquad_process_x8, x64_dyn_biquad_process_x8);

                CEXPORT1(favx, bilinear_transform_x1);
                CEXPORT1(favx, bilinear_transform_x2);
                CEXPORT1(favx, bilinear_transform_x4);
                CEXPORT2_X64(favx, bilinear_transform_x8, x64_bilinear_transform_x8);

                CEXPORT1(favx, h_sum);
                CEXPORT1(favx, h_sqr_sum);
                CEXPORT1(favx, h_abs_sum);

                CEXPORT1(favx, h_dotp);
                CEXPORT1(favx, h_sqr_dotp);
                CEXPORT1(favx, h_abs_dotp);

                CEXPORT1(favx, mix2);
                CEXPORT1(favx, mix_copy2);
                CEXPORT1(favx, mix_add2);
                CEXPORT1(favx, mix3);
                CEXPORT1(favx, mix_copy3);
                CEXPORT1(favx, mix_add3);
                CEXPORT1(favx, mix4);
                CEXPORT1(favx, mix_copy4);
                CEXPORT1(favx, mix_add4);

                CEXPORT1(favx, depan_lin);
                CEXPORT1(favx, depan_eqpow);

                CEXPORT1(favx, min);
                CEXPORT1(favx, max);
                CEXPORT1(favx, minmax);
                CEXPORT1(favx, abs_min);
                CEXPORT1(favx, abs_max);
                CEXPORT1(favx, abs_minmax);
                CEXPORT1(favx, sign_min);
                CEXPORT1(favx, sign_max);
                CEXPORT1(favx, sign_minmax);

                CEXPORT1(favx, lr_to_ms);
                CEXPORT1(favx, lr_to_mid);
                CEXPORT1(favx, lr_to_side);
                CEXPORT1(favx, ms_to_lr);
                CEXPORT1(favx, ms_to_left);
                CEXPORT1(favx, ms_to_right);

                CEXPORT1(favx, direct_fft);
                CEXPORT1(favx, reverse_fft);
                CEXPORT1(favx, normalize_fft2);
                CEXPORT1(favx, normalize_fft3);

                CEXPORT1(favx, packed_direct_fft);
                CEXPORT1(favx, packed_reverse_fft);

                CEXPORT1(favx, fastconv_parse);
                CEXPORT1(favx, fastconv_restore);
                CEXPORT1(favx, fastconv_apply);
                CEXPORT1(favx, fastconv_parse_apply);

                CEXPORT1(favx, filter_transfer_calc_ri);
                CEXPORT1(favx, filter_transfer_apply_ri);
                CEXPORT1(favx, filter_transfer_calc_pc);
                CEXPORT1(favx, filter_transfer_apply_pc);

                CEXPORT1(favx, lanczos_resample_2x2);
                CEXPORT1(favx, lanczos_resample_2x3);
                CEXPORT1(favx, lanczos_resample_2x4);
                CEXPORT2(favx, lanczos_resample_2x12bit, lanczos_resample_2x4);

                CEXPORT1(favx, lanczos_resample_3x2);
                CEXPORT1(favx, lanczos_resample_3x3);
                CEXPORT1(favx, lanczos_resample_3x4);
                CEXPORT2(favx, lanczos_resample_3x12bit, lanczos_resample_3x4);

                CEXPORT1(favx, lanczos_resample_4x2);
                CEXPORT1(favx, lanczos_resample_4x3);
                CEXPORT1(favx, lanczos_resample_4x4);
                CEXPORT2(favx, lanczos_resample_4x12bit, lanczos_resample_4x4);

                CEXPORT1(favx, lanczos_resample_6x2);
                CEXPORT1(favx, lanczos_resample_6x3);
                CEXPORT1(favx, lanczos_resample_6x4);
                CEXPORT2(favx, lanczos_resample_6x12bit, lanczos_resample_6x4);

                CEXPORT1(favx, lanczos_resample_8x2);
                CEXPORT1(favx, lanczos_resample_8x3);
                CEXPORT1(favx, lanczos_resample_8x4);
                CEXPORT2(favx, lanczos_resample_8x12bit, lanczos_resample_8x4);

                CEXPORT1(favx, downsample_2x);
                CEXPORT1(favx, downsample_3x);
                CEXPORT1(favx, downsample_4x);
                CEXPORT1(favx, downsample_6x);
                CEXPORT1(favx, downsample_8x);

                CEXPORT1(favx, convolve);
                CEXPORT1(favx, corr_init);
                CEXPORT1(favx, corr_incr);

                CEXPORT1(favx, lin_inter_set);
                CEXPORT1(favx, lin_inter_mul2);
                CEXPORT1(favx, lin_inter_mul3);
                CEXPORT1(favx, lin_inter_fmadd2);
                CEXPORT1(favx, lin_inter_frmadd2);
                CEXPORT1(favx, lin_inter_fmadd3);

                CEXPORT2(favx, prgba32_set_alpha, pabc32_set_alpha);
                CEXPORT2(favx, pbgra32_set_alpha, pabc32_set_alpha);

                CEXPORT1(favx, axis_apply_lin1);

                CEXPORT2(favx, normalize, normalize2);
                CEXPORT1(favx, normalize1);
                CEXPORT1(favx, normalize2);

                // 3D math
                EXPORT1(init_point_xyz);
                EXPORT1(init_point);
                EXPORT1(normalize_point);

                EXPORT1(init_vector_dxyz);
                EXPORT1(init_vector);
                EXPORT1(normalize_vector);
                EXPORT1(normalize_vector2);

                EXPORT1(init_matrix3d);
                EXPORT1(init_matrix3d_zero);
                EXPORT1(init_matrix3d_one);
                EXPORT1(init_matrix3d_identity);
                EXPORT1(init_matrix3d_translate);
                EXPORT1(init_matrix3d_scale);
                EXPORT1(init_matrix3d_rotate_x);
                EXPORT1(init_matrix3d_rotate_y);
                EXPORT1(init_matrix3d_rotate_z);
//                EXPORT1(init_matrix3d_rotate_xyz);
                EXPORT1(apply_matrix3d_mv2);
                EXPORT1(apply_matrix3d_mv1);
                EXPORT1(apply_matrix3d_mp2);
                EXPORT1(apply_matrix3d_mp1);
                EXPORT1(apply_matrix3d_mm2);
                EXPORT1(apply_matrix3d_mm1);
                EXPORT1(transpose_matrix3d1);
                EXPORT1(transpose_matrix3d2);

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
                if (f->features & CPU_OPTION_FMA3)
                {
                    // Conditional export, depending on fast AVX implementation
                    CEXPORT2(favx, mod2, mod2_fma3);
                    CEXPORT2(favx, rmod2, rmod2_fma3);

                    CEXPORT2(favx, mod3, mod3_fma3);

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

                    CEXPORT2(favx, fmadd3, fmadd3_fma3);
                    CEXPORT2(favx, fmsub3, fmsub3_fma3);
                    CEXPORT2(favx, fmrsub3, fmrsub3_fma3);
                    CEXPORT2(favx, fmmod3, fmmod3_fma3);
                    CEXPORT2(favx, fmrmod3, fmrmod3_fma3);

                    CEXPORT2(favx, fmadd4, fmadd4_fma3);
                    CEXPORT2(favx, fmsub4, fmsub4_fma3);
                    CEXPORT2(favx, fmrsub4, fmrsub4_fma3);
                    CEXPORT2(favx, fmmod4, fmmod4_fma3);
                    CEXPORT2(favx, fmrmod4, fmrmod4_fma3);

                    CEXPORT2(favx, complex_mul2, complex_mul2_fma3);
                    CEXPORT2(favx, complex_mul3, complex_mul3_fma3);
                    CEXPORT2(favx, complex_div2, complex_div2_fma3);
                    CEXPORT2(favx, complex_rdiv2, complex_rdiv2_fma3);
                    CEXPORT2(favx, complex_div3, complex_div3_fma3);
                    CEXPORT2(favx, complex_mod, complex_mod_fma3);
                    CEXPORT2(favx, complex_rcp1, complex_rcp1_fma3);
                    CEXPORT2(favx, complex_rcp2, complex_rcp2_fma3);

                    CEXPORT2(favx, pcomplex_mul2, pcomplex_mul2_fma3);
                    CEXPORT2(favx, pcomplex_mul3, pcomplex_mul3_fma3);
                    CEXPORT2(favx, pcomplex_div2, pcomplex_div2_fma3);
                    CEXPORT2(favx, pcomplex_rdiv2, pcomplex_rdiv2_fma3);
                    CEXPORT2(favx, pcomplex_div3, pcomplex_div3_fma3);
                    CEXPORT2(favx, pcomplex_corr, pcomplex_corr_fma3);

                    if (!below_zen3)
                    {
                        CEXPORT2(favx, h_sqr_sum, h_sqr_sum_fma3);
                    }

                    CEXPORT2(favx, direct_fft, direct_fft_fma3);
                    CEXPORT2(favx, reverse_fft, reverse_fft_fma3);
                    CEXPORT2(favx, packed_direct_fft, packed_direct_fft_fma3);
                    CEXPORT2(favx, packed_reverse_fft, packed_reverse_fft_fma3);

                    CEXPORT2(favx, fastconv_parse, fastconv_parse_fma3);
                    CEXPORT2(favx, fastconv_restore, fastconv_restore_fma3);
                    CEXPORT2(favx, fastconv_apply, fastconv_apply_fma3);
                    CEXPORT2(favx, fastconv_parse_apply, fastconv_parse_apply_fma3);

                    CEXPORT2(favx, filter_transfer_calc_ri, filter_transfer_calc_ri_fma3);
                    CEXPORT2(favx, filter_transfer_apply_ri, filter_transfer_apply_ri_fma3);
                    CEXPORT2(favx, filter_transfer_calc_pc, filter_transfer_calc_pc_fma3);
                    CEXPORT2(favx, filter_transfer_apply_pc, filter_transfer_apply_pc_fma3);

                    CEXPORT2(favx, convolve, convolve_fma3);
                    CEXPORT2(favx, corr_init, corr_init_fma3);
                    CEXPORT2(favx, corr_incr, corr_incr_fma3);

                    CEXPORT2(favx, axis_apply_lin1, axis_apply_lin1_fma3);

                    CEXPORT2(favx, biquad_process_x1, biquad_process_x1_fma3);
                    CEXPORT2(favx, biquad_process_x2, biquad_process_x2_fma3);
                    CEXPORT2(favx, biquad_process_x4, biquad_process_x4_fma3);
                    CEXPORT2(ffma, biquad_process_x8, biquad_process_x8_fma3);

                    CEXPORT2(ffma, dyn_biquad_process_x1, dyn_biquad_process_x1_fma3);
                    CEXPORT2(favx, dyn_biquad_process_x2, dyn_biquad_process_x2_fma3);
                    CEXPORT2(favx, dyn_biquad_process_x4, dyn_biquad_process_x4_fma3);
                    CEXPORT2(ffma, dyn_biquad_process_x8, dyn_biquad_process_x8_fma3);

                    CEXPORT2(favx, depan_eqpow, depan_eqpow_fma3);

                    // 3D math
                    CEXPORT2(favx, apply_matrix3d_mm2, apply_matrix3d_mm2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mm1, apply_matrix3d_mm1_fma3);
                    CEXPORT2(favx, apply_matrix3d_mp2, apply_matrix3d_mp2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mp1, apply_matrix3d_mp1_fma3);
                    CEXPORT2(favx, apply_matrix3d_mv2, apply_matrix3d_mv2_fma3);
                    CEXPORT2(favx, apply_matrix3d_mv1, apply_matrix3d_mv1_fma3);

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

            #undef EXPORT1
            #undef EXPORT2
        } /* namespace avx */
    } /* namespace lsp */

#endif /* ARCH_X86 */



