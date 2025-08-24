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

    #define PRIVATE_DSP_ARCH_X86_SSE_IMPL
        #include <private/dsp/arch/x86/sse/mxcsr.h>
        #include <private/dsp/arch/x86/sse/copy.h>
        #include <private/dsp/arch/x86/sse/float.h>

        #include <private/dsp/arch/x86/sse/pmath.h>

        #include <private/dsp/arch/x86/sse/hmath/hsum.h>
        #include <private/dsp/arch/x86/sse/hmath/hdotp.h>

        #include <private/dsp/arch/x86/sse/mix.h>
        #include <private/dsp/arch/x86/sse/pan.h>

        #include <private/dsp/arch/x86/sse/search/minmax.h>

        #include <private/dsp/arch/x86/sse/smath.h>

        #include <private/dsp/arch/x86/sse/fft.h>
        #include <private/dsp/arch/x86/sse/fastconv.h>
        #include <private/dsp/arch/x86/sse/graphics.h>
        #include <private/dsp/arch/x86/sse/msmatrix.h>
        #include <private/dsp/arch/x86/sse/resampling.h>

        #include <private/dsp/arch/x86/sse/complex.h>
        #include <private/dsp/arch/x86/sse/pcomplex.h>

        #include <private/dsp/arch/x86/sse/convolution.h>
        #include <private/dsp/arch/x86/sse/correlation.h>

        #include <private/dsp/arch/x86/sse/filters/static.h>
        #include <private/dsp/arch/x86/sse/filters/dynamic.h>
        #include <private/dsp/arch/x86/sse/filters/transform.h>
        #include <private/dsp/arch/x86/sse/filters/transfer.h>

        #include <private/dsp/arch/x86/sse/3dmath.h>

        #include <private/dsp/arch/x86/sse/interpolation/linear.h>
    #undef PRIVATE_DSP_ARCH_X86_SSE_IMPL

    namespace lsp
    {
        namespace sse
        {
            using namespace x86;

            static dsp::start_t     dsp_start       = NULL;
            static dsp::finish_t    dsp_finish      = NULL;

            void start(dsp::context_t *ctx)
            {
                dsp_start(ctx);
                uint32_t    mxcsr       = read_mxcsr();
                ctx->data[ctx->top++]   = mxcsr;
                write_mxcsr(mxcsr | MXCSR_ALL_MASK | MXCSR_FZ | MXCSR_DAZ);
            }

            void finish(dsp::context_t *ctx)
            {
                write_mxcsr(ctx->data[--ctx->top]);
                dsp_finish(ctx);
            }

            #define EXPORT2(function, export) \
            { \
                dsp::function                       = sse::export; \
                dsp::LSP_DSP_LIB_MANGLE(function)   = sse::export; \
                TEST_EXPORT(sse::export); \
            }
            #define EXPORT1(function)                   EXPORT2(function, function);

            void dsp_init(const cpu_features_t *f)
            {
                if (((f->features) & (CPU_OPTION_SSE | CPU_OPTION_SSE2)) != (CPU_OPTION_SSE | CPU_OPTION_SSE2))
                    return;

                // Initialize MXCSR mask
                if (f->features & CPU_OPTION_FXSAVE)
                    init_mxcsr_mask();
                else
                    mxcsr_mask  = MXCSR_DEFAULT;

                // Save previous entry points
                dsp_start                       = dsp::start;
                dsp_finish                      = dsp::finish;

                // Export routines
                EXPORT1(start);
                EXPORT1(finish);

                if (!feature_check(f, FEAT_FAST_MOVS))
                {
                    EXPORT1(copy);
                }
                else
                {
                    TEST_EXPORT(copy);
                }
                TEST_EXPORT(copy_movntps);

                EXPORT1(move);
                EXPORT1(fill);
                EXPORT1(fill_one);
                EXPORT1(fill_zero);
                EXPORT1(fill_minus_one);
                EXPORT1(limit1);
                EXPORT1(limit2);

                EXPORT1(ipowf);
                EXPORT1(irootf);

                EXPORT1(abs1);
                EXPORT1(abs2);
                EXPORT1(abs_add2);
                EXPORT1(abs_sub2);
                EXPORT1(abs_rsub2);
                EXPORT1(abs_mul2);
                EXPORT1(abs_div2);
                EXPORT1(abs_rdiv2);
                EXPORT1(abs_max2);
                EXPORT1(abs_min2);

                EXPORT1(abs_add3);
                EXPORT1(abs_sub3);
                EXPORT1(abs_rsub3);
                EXPORT1(abs_mul3);
                EXPORT1(abs_div3);
                EXPORT1(abs_rdiv3);
                EXPORT1(abs_max3);
                EXPORT1(abs_min3);

                EXPORT1(min);
                EXPORT1(max);
                EXPORT1(abs_min);
                EXPORT1(abs_max);
                EXPORT1(sign_min);
                EXPORT1(sign_max);
                EXPORT1(minmax);
                EXPORT1(abs_minmax);
                EXPORT1(sign_minmax);

                EXPORT1(add2);
                EXPORT1(sub2);
                EXPORT1(rsub2);
                EXPORT1(mul2);
                EXPORT1(div2);
                EXPORT1(rdiv2);

                EXPORT1(add_k2);
                EXPORT1(sub_k2);
                EXPORT1(rsub_k2);
                EXPORT1(mul_k2);
                EXPORT1(div_k2);
                EXPORT1(rdiv_k2);

                EXPORT1(add3);
                EXPORT1(sub3);
                EXPORT1(mul3);
                EXPORT1(div3);

                EXPORT1(add_k3);
                EXPORT1(sub_k3);
                EXPORT1(rsub_k3);
                EXPORT1(mul_k3);
                EXPORT1(div_k3);
                EXPORT1(rdiv_k3);

                EXPORT1(lramp_set1);
                EXPORT1(lramp1);
                EXPORT1(lramp2);
                EXPORT1(lramp_add2);
                EXPORT1(lramp_sub2);
                EXPORT1(lramp_rsub2);
                EXPORT1(lramp_mul2);
                EXPORT1(lramp_div2);
                EXPORT1(lramp_rdiv2);
                EXPORT1(lramp_add3);
                EXPORT1(lramp_sub3);
                EXPORT1(lramp_rsub3);
                EXPORT1(lramp_mul3);
                EXPORT1(lramp_div3);
                EXPORT1(lramp_rdiv3);

                EXPORT1(pmin2);
                EXPORT1(pmax2);
                EXPORT1(psmin2);
                EXPORT1(psmax2);
                EXPORT1(pamin2);
                EXPORT1(pamax2);
                EXPORT1(pmin3);
                EXPORT1(pmax3);
                EXPORT1(psmin3);
                EXPORT1(psmax3);
                EXPORT1(pamin3);
                EXPORT1(pamax3);

                EXPORT1(sqr1);
                EXPORT1(sqr2);
                EXPORT1(ssqrt1);
                EXPORT1(ssqrt2);

                EXPORT1(h_sum);
                EXPORT1(h_sqr_sum);
                EXPORT1(h_abs_sum);

                EXPORT1(h_dotp);
                EXPORT1(h_sqr_dotp);
                EXPORT1(h_abs_dotp);

                EXPORT1(fmadd_k3);
                EXPORT1(fmsub_k3);
                EXPORT1(fmrsub_k3);
                EXPORT1(fmmul_k3);
                EXPORT1(fmdiv_k3);
                EXPORT1(fmrdiv_k3);

                EXPORT1(fmadd_k4);
                EXPORT1(fmsub_k4);
                EXPORT1(fmrsub_k4);
                EXPORT1(fmmul_k4);
                EXPORT1(fmdiv_k4);
                EXPORT1(fmrdiv_k4);

                EXPORT1(fmadd3);
                EXPORT1(fmsub3);
                EXPORT1(fmrsub3);
                EXPORT1(fmmul3);
                EXPORT1(fmdiv3);
                EXPORT1(fmrdiv3);

                EXPORT1(fmadd4);
                EXPORT1(fmsub4);
                EXPORT1(fmrsub4);
                EXPORT1(fmmul4);
                EXPORT1(fmdiv4);
                EXPORT1(fmrdiv4);

                EXPORT1(mix2);
                EXPORT1(mix_copy2);
                EXPORT1(mix_add2);
                EXPORT1(mix3);
                EXPORT1(mix_copy3);
                EXPORT1(mix_add3);
                EXPORT1(mix4);
                EXPORT1(mix_copy4);
                EXPORT1(mix_add4);

                EXPORT1(depan_lin);
                EXPORT1(depan_eqpow);

                EXPORT1(reverse1);
                EXPORT1(reverse2);

                EXPORT1(direct_fft);
                EXPORT1(reverse_fft);
                EXPORT1(normalize_fft2);
                EXPORT1(normalize_fft3);
                EXPORT1(packed_direct_fft);
                EXPORT1(packed_reverse_fft);
        //            EXPORT1(center_fft);
        //            EXPORT1(combine_fft);

                EXPORT1(fastconv_parse);
                EXPORT1(fastconv_parse_apply);
                EXPORT1(fastconv_restore);
                EXPORT1(fastconv_apply);

                EXPORT1(complex_mul2);
                EXPORT1(complex_mul3);
                EXPORT1(complex_div2);
                EXPORT1(complex_rdiv2);
                EXPORT1(complex_div3);
                EXPORT1(complex_rcp1);
                EXPORT1(complex_rcp2);
                EXPORT1(complex_mod);

                EXPORT1(pcomplex_mul2);
                EXPORT1(pcomplex_mul3);
                EXPORT1(pcomplex_div2);
                EXPORT1(pcomplex_rdiv2);
                EXPORT1(pcomplex_div3);
                EXPORT1(pcomplex_rcp1);
                EXPORT1(pcomplex_rcp2);
                EXPORT1(pcomplex_r2c);
                EXPORT1(pcomplex_c2r);
                EXPORT1(pcomplex_mod);
                EXPORT1(pcomplex_corr);
        //            EXPORT1(complex_cvt2modarg);
        //            EXPORT1(complex_cvt2reim);

                EXPORT1(pcomplex_r2c_add2);
                EXPORT1(pcomplex_r2c_sub2);
                EXPORT1(pcomplex_r2c_rsub2);
                EXPORT1(pcomplex_r2c_mul2);
                EXPORT1(pcomplex_r2c_div2);
                EXPORT1(pcomplex_r2c_rdiv2);

                EXPORT1(lr_to_ms);
                EXPORT1(lr_to_mid);
                EXPORT1(lr_to_side);
                EXPORT1(ms_to_lr);
                EXPORT1(ms_to_left);
                EXPORT1(ms_to_right);

                EXPORT1(biquad_process_x1);
                EXPORT1(biquad_process_x2);
                EXPORT1(biquad_process_x4);
                EXPORT1(biquad_process_x8);

                EXPORT1(dyn_biquad_process_x1);
                EXPORT1(dyn_biquad_process_x2);
                EXPORT1(dyn_biquad_process_x4);
                EXPORT1(dyn_biquad_process_x8);

                EXPORT1(filter_transfer_calc_ri);
                EXPORT1(filter_transfer_apply_ri);
                EXPORT1(filter_transfer_calc_pc);
                EXPORT1(filter_transfer_apply_pc);

                EXPORT1(bilinear_transform_x1);
                EXPORT1(bilinear_transform_x2);
                EXPORT1(bilinear_transform_x4);
                EXPORT1(bilinear_transform_x8);

                EXPORT1(fill_rgba);
                EXPORT1(fill_hsla);
                EXPORT1(axis_apply_lin1);

                EXPORT1(lanczos_resample_2x2);
                EXPORT1(lanczos_resample_2x3);
                EXPORT1(lanczos_resample_2x4);
                EXPORT2(lanczos_resample_2x12bit, lanczos_resample_2x4);

                EXPORT1(lanczos_resample_3x2);
                EXPORT1(lanczos_resample_3x3);
                EXPORT1(lanczos_resample_3x4);
                EXPORT2(lanczos_resample_3x12bit, lanczos_resample_3x4);

                EXPORT1(lanczos_resample_4x2);
                EXPORT1(lanczos_resample_4x3);
                EXPORT1(lanczos_resample_4x4);
                EXPORT2(lanczos_resample_4x12bit, lanczos_resample_4x4);

                EXPORT1(lanczos_resample_6x2);
                EXPORT1(lanczos_resample_6x3);
                EXPORT1(lanczos_resample_6x4);
                EXPORT2(lanczos_resample_6x12bit, lanczos_resample_6x4);

                EXPORT1(lanczos_resample_8x2);
                EXPORT1(lanczos_resample_8x3);
                EXPORT1(lanczos_resample_8x4);
                EXPORT2(lanczos_resample_8x12bit, lanczos_resample_8x4);

                EXPORT1(downsample_2x);
                EXPORT1(downsample_3x);
                EXPORT1(downsample_4x);
                EXPORT1(downsample_6x);
                EXPORT1(downsample_8x);

                // 3D Math
                EXPORT1(init_point_xyz);
                EXPORT1(init_point);
                EXPORT1(normalize_point);
                EXPORT1(scale_point1);
                EXPORT1(scale_point2);

                EXPORT1(init_vector_dxyz);
                EXPORT1(init_vector);
                EXPORT1(normalize_vector);
                EXPORT1(normalize_vector2);
                EXPORT1(flip_vector_v1);
                EXPORT1(flip_vector_v2);
                EXPORT1(scale_vector1);
                EXPORT1(scale_vector2);

                EXPORT1(add_vector_pv1);
                EXPORT1(add_vector_pv2);
                EXPORT1(add_vector_pvk1);
                EXPORT1(add_vector_pvk2);

                EXPORT1(init_matrix3d);
                EXPORT1(init_matrix3d_zero);
                EXPORT1(init_matrix3d_one);
                EXPORT1(init_matrix3d_identity);
                EXPORT1(init_matrix3d_translate);
                EXPORT1(init_matrix3d_scale);
                EXPORT1(init_matrix3d_rotate_x);
                EXPORT1(init_matrix3d_rotate_y);
                EXPORT1(init_matrix3d_rotate_z);
                EXPORT1(init_matrix3d_rotate_xyz);
                EXPORT1(apply_matrix3d_mv2);
                EXPORT1(apply_matrix3d_mv1);
                EXPORT1(apply_matrix3d_mp2);
                EXPORT1(apply_matrix3d_mp1);
                EXPORT1(apply_matrix3d_mm2);
                EXPORT1(apply_matrix3d_mm1);
                EXPORT1(transpose_matrix3d1);
                EXPORT1(transpose_matrix3d2);

                EXPORT1(colocation_x2_v1p2);
                EXPORT1(colocation_x2_v1pv);
                EXPORT1(colocation_x3_v1p3);
                EXPORT1(colocation_x3_v1pv);
                EXPORT1(colocation_x3_v3p1);
                EXPORT1(colocation_x3_vvp1);

                EXPORT1(check_point3d_on_triangle_p3p);
                EXPORT1(check_point3d_on_triangle_pvp);

                EXPORT1(longest_edge3d_p3);
                EXPORT1(longest_edge3d_pv);

                EXPORT1(check_triplet3d_p3n);
                EXPORT1(check_triplet3d_pvn);
                EXPORT1(check_triplet3d_v2n);
                EXPORT1(check_triplet3d_vvn);
                EXPORT1(check_triplet3d_vv);

                EXPORT1(calc_normal3d_p3);
                EXPORT1(calc_normal3d_pv);
                EXPORT1(calc_normal3d_v2);
                EXPORT1(calc_normal3d_vv);

                EXPORT1(calc_split_point_p2v1);
                EXPORT1(calc_split_point_pvv1);

                EXPORT1(calc_plane_p3);
                EXPORT1(calc_plane_pv);
                EXPORT1(calc_plane_v1p2);

                EXPORT1(calc_area_p3);
                EXPORT1(calc_area_pv);
                EXPORT1(calc_min_distance_p3);
                EXPORT1(calc_min_distance_pv);

                EXPORT1(convolve);
                EXPORT1(corr_init);
                EXPORT1(corr_incr);

                EXPORT1(lin_inter_set);
                EXPORT1(lin_inter_mul2);
                EXPORT1(lin_inter_mul3);
                EXPORT1(lin_inter_fmadd2);
                EXPORT1(lin_inter_frmadd2);
                EXPORT1(lin_inter_fmadd3);

                EXPORT2(normalize, normalize2);
                EXPORT1(normalize1);
                EXPORT1(normalize2);
            }

            #undef EXPORT1
            #undef EXPORT2
        } /* namespace sse */
    } /* namespace lsp */

#endif /* ARCH_X86 */
