/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 8 мар. 2024 г.
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

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/utest.h>

namespace lsp
{
    namespace generic
    {
        void corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }

        namespace avx
        {
            void corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);

            void corr_incr_fma3(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }

        namespace avx512
        {
            void corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void corr_incr(dsp::correlation_t *corr, float *dst,
                const float *a_head, const float *b_head,
                const float *a_tail, const float *b_tail,
                size_t count);
        }
    )

    static void corr_incr(dsp::correlation_t *corr, float *dst,
        const float *a_head, const float *b_head,
        const float *a_tail, const float *b_tail,
        size_t count)
    {
        float vv    = corr->v;
        float va    = corr->a;
        float vb    = corr->b;

        for (size_t i=0; i<count; ++i)
        {
            float ah    = a_head[i];
            float bh    = b_head[i];
            float at    = a_tail[i];
            float bt    = b_tail[i];

            vv         += ah*bh - at*bt;
            va         += ah*ah - at*at;
            vb         += bh*bh - bt*bt;
            float d     = va * vb;

            dst[i]      = (d >= 1e-10f) ? vv / sqrtf(d) : 0.0f;
        }

        corr->v     = vv;
        corr->a     = va;
        corr->b     = vb;
    }

    typedef void (* corr_incr_t)(dsp::correlation_t *corr, float *dst,
        const float *a_head, const float *b_head,
        const float *a_tail, const float *b_tail,
        size_t count);
}

UTEST_BEGIN("dsp", corr_incr)
    void call(const char *label, size_t align, corr_incr_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
        {
            UTEST_FOREACH(tail, 0x80, 0x1ff)
            {
                UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
                {
                    FloatBuffer a(tail + count + 1, align, mask & 0x01);
                    FloatBuffer b(tail + count + 1, align, mask & 0x02);
                    FloatBuffer dst1(count, align, mask & 0x04);
                    FloatBuffer dst2(count, align, mask & 0x04);

                    dsp::correlation_t corr_a, corr_b;
                    corr_a.v = 0.0f;
                    corr_a.a = 0.0f;
                    corr_a.b = 0.0f;
                    dsp::corr_init(&corr_a, a, b, tail);
                    corr_b = corr_a;

                    printf("Tesing %s correlation tail=%d on buffer count=%d mask=0x%x\n", label, int(tail), int(count), int(mask));

                    const float *a_tail = a;
                    const float *b_tail = b;
                    const float *a_head = &a_tail[tail];
                    const float *b_head = &b_tail[tail];

                    corr_incr(&corr_a, dst1, a_head, b_head, a_tail, b_tail, count);
                    func(&corr_b, dst2, a_head, b_head, a_tail, b_tail, count);

                    UTEST_ASSERT_MSG(a.valid(), "Buffer A corrupted");
                    UTEST_ASSERT_MSG(b.valid(), "Buffer B corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals_adaptive(dst2, 1e-4))
                    {
                        a.dump("a   ");
                        b.dump("b   ");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs at index %d, value=%f vs %f\n"
                                "correlation state a={%f, %f, %f}, b={%f, %f, %f}",
                            label, int(dst1.last_diff()), dst1.get(dst1.last_diff()), dst2.get(dst1.last_diff()),
                            corr_a.v, corr_a.a, corr_a.b,
                            corr_b.v, corr_b.a, corr_b.b);
                    }

                    // Compare state
                    if ((!float_equals_adaptive(corr_a.v, corr_b.v, 1e-5)) ||
                        (!float_equals_adaptive(corr_a.a, corr_b.a, 1e-5)) ||
                        (!float_equals_adaptive(corr_a.b, corr_b.b, 1e-5)))
                    {
                        UTEST_FAIL_MSG("Correlation state differs a={%f, %f, %f}, b={%f, %f, %f}",
                            corr_a.v, corr_a.a, corr_a.b,
                            corr_b.v, corr_b.a, corr_b.b);
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        CALL(generic::corr_incr, 16);
        IF_ARCH_X86(CALL(sse::corr_incr, 16));
        IF_ARCH_X86_64(CALL(sse3::x64_corr_incr, 16));
        IF_ARCH_X86(CALL(avx::corr_incr, 32));
        IF_ARCH_X86(CALL(avx::corr_incr_fma3, 32));
        IF_ARCH_X86(CALL(avx512::corr_incr, 64));
        IF_ARCH_ARM(CALL(neon_d32::corr_incr, 16));
        IF_ARCH_AARCH64(CALL(asimd::corr_incr, 16));
    }

UTEST_END;



