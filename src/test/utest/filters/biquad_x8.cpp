/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 июл. 2026 г.
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
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define FILTER_TIMES    8
#define BUF_SIZE        1024
#define BUF_STEP        32
#define TOLERANCE       1e-3f

namespace lsp
{
    namespace generic
    {
        void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

        void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);
        }

//        namespace avx
//        {
//            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_t *f);
//            void biquad_process_x8_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_t *f);
//        }
    )
//
//    IF_ARCH_ARM(
//        namespace neon_d32
//        {
//            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_t *f);
//        }
//    )
//
//    IF_ARCH_AARCH64(
//        namespace asimd
//        {
//            void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const dsp::biquad_t *f);
//        }
//    )

    typedef void (* biquad_process_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x8_t *f);

    const dsp::biquad_x1_t filter = {
        .b0     = 0.992303491f,
        .b1     = -1.98460698f,
        .b2     = 0.992303491f,
        .a1     = 1.98398674f,
        .a2     = -0.985227287f,
        .p0     = 0.0f,
        .p1     = 0.0f,
        .p2     = 0.0f,
    };
}

UTEST_BEGIN("dsp.filters", biquad_x8)

    void call(const char *label, biquad_process_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        dsp::biquad_x1_t f1 __lsp_aligned64 = filter;
        dsp::biquad_x8_t f2 __lsp_aligned64;

        for (size_t i=0; i<FILTER_TIMES; ++i)
        {
            f2.b0[i]    = filter.b0;
            f2.b1[i]    = filter.b1;
            f2.b2[i]    = filter.b2;
            f2.a1[i]    = filter.a1;
            f2.a2[i]    = filter.a2;
        }

        float d1[2] __lsp_aligned64;
        float d2[2 * FILTER_TIMES] __lsp_aligned64;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 0x1f, 0x80, 0x8f, 0x5f, 0x1ff)
        {
            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            // Apply processing
            for (size_t i=0; i<FILTER_TIMES; ++i)
            {
                dsp::fill_zero(d1, 2);
                generic::biquad_process_x1(dst1, (i > 0) ? dst1.data() : src.data(), d1, count, &f1);
            }

            dsp::fill_zero(d2, 2 * FILTER_TIMES);
            func(dst2, src, d2, count, &f2);

            // Perform validation
            UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
            UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
            UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

            if (!dst1.equals_adaptive(dst2, TOLERANCE))
            {
                src.dump("src");
                dst1.dump("dst1");
                dst2.dump("dst2");
                UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                        label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
            }
        }
    }

    void call(const char *label, const dsp::biquad_x8_t *bq, biquad_process_t func1, biquad_process_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        printf("Testing %s on buffer size %d...\n", label, BUF_SIZE);

        float d1[2 * FILTER_TIMES] __lsp_aligned64;
        float d2[2 * FILTER_TIMES] __lsp_aligned64;

        FloatBuffer src(BUF_SIZE);
        FloatBuffer dst1(BUF_SIZE);
        FloatBuffer dst2(BUF_SIZE);

        dsp::fill_zero(d1, 2 * FILTER_TIMES);
        dsp::fill_zero(d2, 2 * FILTER_TIMES);

    //        for (size_t i=0; i<BUF_SIZE; ++i)
    //            src[i] = (i & 1) ? -0.001 * float(i+1) : 0.001 * float(i+1);

        for (size_t i=0; i<BUF_SIZE; i += BUF_STEP)
        {
            size_t count = BUF_SIZE - i;
            if (count > BUF_STEP)
                count = BUF_STEP;
            func1(dst1.data(i), src.data(i), d1, count, bq);
            func2(dst2.data(i), src.data(i), d2, count, bq);
        }

        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");
        if (!dst1.equals_adaptive(dst2, TOLERANCE))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
            UTEST_FAIL_MSG("Output of functions for test '%s' differs at sample %d: %.6f vs %.6f",
                    label, int(dst1.last_diff()), dst1.get_diff(), dst2.get_diff());
        }

        for (size_t j=0; j<2; ++j)
        {
            if (float_equals_absolute(d1[j], d2[j], TOLERANCE))
                continue;
            UTEST_FAIL_MSG("Filter memory items #%d for test '%s' differ: %.6f vs %.6f",
                    int(j), label, d1[j], d2[j]);
        }
    }


    UTEST_MAIN
    {
        #define CALL(func) \
            call(#func, func)

        // PART 1, overall check correctness
        CALL(generic::biquad_process_x8);
        IF_ARCH_X86(CALL(sse::biquad_process_x8));
//        IF_ARCH_X86(CALL(avx::biquad_process_x8));
//        IF_ARCH_X86(CALL(avx::biquad_process_x8_fma3));
//        IF_ARCH_ARM(CALL(neon_d32::biquad_process_x8));
//        IF_ARCH_AARCH64(CALL(asimd::biquad_process_x8));

        #undef CALL

        // PART 2
        // Prepare 16 zero, 16 pole filter
        dsp::biquad_x8_t x8 __lsp_aligned64;

        x8.b0[0]    = 1.79906213f;
        x8.b0[1]    = 1.16191483f;
        x8.b0[2]    = 1.13150513f;
        x8.b0[3]    = 1.11161804f;
        x8.b0[4]    = 1.79906213f;
        x8.b0[5]    = 1.16191483f;
        x8.b0[6]    = 1.13150513f;
        x8.b0[7]    = 1.11161804f;

        x8.b1[0]    = -3.38381839f;
        x8.b1[1]    = -2.20469999f;
        x8.b1[2]    = -2.18261695f;
        x8.b1[3]    = -2.19184852f;
        x8.b1[4]    = -3.38381839f;
        x8.b1[5]    = -2.20469999f;
        x8.b1[6]    = -2.18261695f;
        x8.b1[7]    = -2.19184852f;

        x8.b2[0]    = 1.59139514f;
        x8.b2[1]    = 1.04720736f;
        x8.b2[2]    = 1.05562544f;
        x8.b2[3]    = 1.08485937f;
        x8.b2[4]    = 1.59139514f;
        x8.b2[5]    = 1.04720736f;
        x8.b2[6]    = 1.05562544f;
        x8.b2[7]    = 1.08485937f;

        x8.a1[0]    = 1.8580488f;
        x8.a1[1]    = 1.88010871f;
        x8.a1[2]    = 1.91898823f;
        x8.a1[3]    = 1.96808743f;
        x8.a1[4]    = 1.8580488f;
        x8.a1[5]    = 1.88010871f;
        x8.a1[6]    = 1.91898823f;
        x8.a1[7]    = 1.96808743f;

        x8.a2[0]    = -0.863286555f;
        x8.a2[1]    = -0.88529253f;
        x8.a2[2]    = -0.924120247f;
        x8.a2[3]    = -0.97324127f;
        x8.a2[4]    = -0.863286555f;
        x8.a2[5]    = -0.88529253f;
        x8.a2[6]    = -0.924120247f;
        x8.a2[7]    = -0.97324127f;

        #define CALL(func) \
            call(#func, &x8, generic::biquad_process_x8, func)

        IF_ARCH_X86(CALL(sse::biquad_process_x8));
//        IF_ARCH_X86(CALL(avx::biquad_process_x8));
//        IF_ARCH_X86(CALL(avx::biquad_process_x8_fma3));
//        IF_ARCH_ARM(CALL(neon_d32::biquad_process_x8));
//        IF_ARCH_AARCH64(CALL(asimd::biquad_process_x8));

    }

UTEST_END



