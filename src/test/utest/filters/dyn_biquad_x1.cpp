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

#define BUF_SIZE        1024
#define TOLERANCE       1e-3f

namespace lsp
{
    namespace generic
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        }

        namespace avx
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
            void dyn_biquad_process_x1_fma3(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);
        }
    )

    typedef void (* dyn_biquad_process_x1_t)(float *dst, const float *src, float *d, size_t count, const dsp::biquad_x1_t *f);

    static dsp::biquad_x1_t filter =
    {
        .b0     = 0.0963056013f,
        .b1     = 0.0f,
        .b2     = -0.0963056013f,
        .a1     = 1.80482113f,
        .a2     = -0.807388783f,
        .p0     = 0.0f,
        .p1     = 0.0f,
        .p2     = 0.0f,
    };
}

UTEST_BEGIN("dsp.filters", dyn_biquad_x1)

    void call(const char *label, dyn_biquad_process_x1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        float d[2];

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
            0x1f, 0x20, 0x2f, 0x3f, 0x40, 0x41, 0x4f, 0x7f, 0x1ff, 0x200)
        {
            printf("Testing %s on input buffer size=%d...\n", label, int(count));

            FloatBuffer src(count);
            FloatBuffer dst1(count);
            FloatBuffer dst2(count);
            src.randomize_sign();

            // Initialize filters
            void *p1 = NULL, *p2 = NULL;
            dsp::biquad_x1_t *f1 = alloc_aligned<dsp::biquad_x1_t>(p1, count, 64);
            dsp::biquad_x1_t *f2 = alloc_aligned<dsp::biquad_x1_t>(p2, count, 64);
            UTEST_ASSERT_MSG(f1 != NULL, "Out of memory while allocating f1");
            UTEST_ASSERT_MSG(f2 != NULL, "Out of memory while allocating f2");

            // Filter with changing gain
            const float step    = 1.0f / (lsp_max(count, 2u) - 1);
            for (size_t i=0; i<count; ++i)
            {
                const float g       = 1.0f + float(i) * step;
                dsp::biquad_x1_t *f = &f1[i];
                f->b0               = filter.b0 * g;
                f->b1               = filter.b1 * g;
                f->b2               = filter.b2 * g;
                f->a1               = filter.a1;
                f->a2               = filter.a2;
                f->p0               = 0.0f;
                f->p1               = 0.0f;
                f->p2               = 0.0f;
            }

            for (size_t i=0; i<count; ++i)
            {
                const float g       = 1.0f + float(i) * step;
                dsp::biquad_x1_t *f = &f2[i];
                f->b0               = filter.b0 * g;
                f->b1               = filter.b1 * g;
                f->b2               = filter.b2 * g;
                f->a1               = filter.a1;
                f->a2               = filter.a2;
                f->p0               = 0.0f;
                f->p1               = 0.0f;
                f->p2               = 0.0f;
            }

            // Apply processing
            dsp::fill_zero(d, 2);
            generic::dyn_biquad_process_x1(dst1, src, d, count, f1);

            dsp::fill_zero(d, 2);
            func(dst2, src, d, count, f2);

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
        CALL(generic::dyn_biquad_process_x1);
        IF_ARCH_X86(CALL(sse::dyn_biquad_process_x1));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x1));
        IF_ARCH_X86(CALL(avx::dyn_biquad_process_x1_fma3));
        IF_ARCH_ARM(CALL(neon_d32::dyn_biquad_process_x1));
        IF_ARCH_AARCH64(CALL(asimd::dyn_biquad_process_x1));
    }

UTEST_END



