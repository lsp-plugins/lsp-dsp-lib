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
        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
        }

        namespace avx
        {
            void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
            void corr_init_fma3(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
        }

        namespace avx512
        {
            void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
        }
    )

    static void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
    {
        float vv    = 0.0f;
        float va    = 0.0f;
        float vb    = 0.0f;

        for (size_t i=0; i<count; ++i)
        {
            vv         += a[i] * b[i];
            va         += a[i] * a[i];
            vb         += b[i] * b[i];
        }

        corr->v    += vv;
        corr->a    += va;
        corr->b    += vb;
    }

    typedef void (* corr_init_t)(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
}

UTEST_BEGIN("dsp", corr_init)
    void call(const char *label, size_t align, corr_init_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
            {
                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);

                dsp::correlation_t corr_a, corr_b;
                corr_a.v = 0.1f;
                corr_a.a = 0.2f;
                corr_a.b = 0.3f;
                corr_b.v = 0.1f;
                corr_b.a = 0.2f;
                corr_b.b = 0.3f;

                printf("Tesing %s corr_init on buffer count=%d mask=0x%x\n", label, int(count), int(mask));

                corr_init(&corr_a, a, b, count);
                func(&corr_b, a, b, count);

                UTEST_ASSERT_MSG(a.valid(), "Buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Buffer B corrupted");

                // Compare state
                if ((!float_equals_adaptive(corr_a.v, corr_b.v, 1e-5f)) ||
                    (!float_equals_adaptive(corr_a.a, corr_b.a, 1e-5f)) ||
                    (!float_equals_adaptive(corr_a.b, corr_b.b, 1e-5f)))
                {
                    UTEST_FAIL_MSG("Correlation state differs a={%f, %f, %f}, b={%f, %f, %f}",
                        corr_a.v, corr_a.a, corr_a.b,
                        corr_b.v, corr_b.a, corr_b.b);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        CALL(generic::corr_init, 16);
        IF_ARCH_X86(CALL(sse::corr_init, 16));
        IF_ARCH_X86(CALL(avx::corr_init, 32));
        IF_ARCH_X86(CALL(avx::corr_init_fma3, 32));
        IF_ARCH_X86(CALL(avx512::corr_init, 64));
        IF_ARCH_ARM(CALL(neon_d32::corr_init, 16));
        IF_ARCH_AARCH64(CALL(asimd::corr_init, 16));
    }

UTEST_END;



