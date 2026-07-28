/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 июл. 2026 г.
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
#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define FILTER_TIMES    2
#define BUF_SIZE        1024
#define TOLERANCE       1e-3f

namespace lsp
{
    namespace generic
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        }

        namespace avx
        {
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
            void dyn_biquad_process_x2_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);
        }
    )

    typedef void (* dyn_biquad_process_x2_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x2_t *f);

    static dsp::biquad_x1_t bq_normal =
    {
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

UTEST_BEGIN("dsp.filters", dyn_biquad_x2)

    void call(const char *label, dyn_biquad_process_x2_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d1[2] __lsp_aligned64;
        float d2[2 * FILTER_TIMES] __lsp_aligned64;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            0x1f, 0x20, 0x2f, 0x3f, 0x40, 0x41, 0x4f, 0x7f, 0x1ff)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            const size_t total = count + FILTER_TIMES - 1;
            dsp::biquad_x1_t *f1 = alloc_aligned<dsp::biquad_x1_t>(p1, count, 64);
            dsp::biquad_x2_t *f2 = alloc_aligned<dsp::biquad_x2_t>(p2, total, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            for (size_t i=0; i<count; ++i)
                f1[i]       = bq_normal;
            bzero(f2, total * sizeof(dsp::biquad_x2_t));
            for (size_t j=0; j<FILTER_TIMES; ++j)
            {
                dsp::biquad_x2_t *f = &f2[j];
                for (size_t i=0; i<count; ++i, ++f)
                {
                    f->b0[j] = bq_normal.b0;
                    f->b1[j] = bq_normal.b1;
                    f->b2[j] = bq_normal.b2;
                    f->a1[j] = bq_normal.a1;
                    f->a2[j] = bq_normal.a2;
                }
            }

            // Apply processing
            for (size_t i=0; i<FILTER_TIMES; ++i)
            {
                dsp::fill_zero(d1, 2);
                generic::dyn_biquad_process_x1(dst1, (i == 0) ? src.data() : dst1.data(), d1, count, f1);
            }

            dsp::fill_zero(d2, 2 * FILTER_TIMES);
            func(dst2, src, d2, count, f2);

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

            free_aligned(p1);
            free_aligned(p2);
        }
    }

    UTEST_MAIN
    {
        #define CALL(func) \
            call(#func, func)

        // Do overall check
        CALL( generic::dyn_biquad_process_x2);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x2));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x2));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x2_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x2));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x2));
    }

UTEST_END



