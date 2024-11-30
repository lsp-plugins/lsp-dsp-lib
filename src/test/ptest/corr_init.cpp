/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 мар. 2024 г.
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK        8
#define MAX_RANK        15

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

    typedef void (* corr_init_t)(dsp::correlation_t *corr, const float *a, const float *b, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp", corr_init, 5, 10000)

    void call(const char *label, const float *a, const float *b, size_t count, corr_init_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s correlation ...\n", buf);

        dsp::correlation_t corr;

        PTEST_LOOP(buf,
            corr.v = 0.0f;
            corr.a = 0.0f;
            corr.b = 0.0f;

            func(&corr, a, b, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *a        = alloc_aligned<float>(data, buf_size * 2, 64);
        float *b        = &a[buf_size];
        lsp_finally {
            free_aligned(data);
        };

        for (size_t i=0; i < buf_size*2; ++i)
            a[i]            = randf(-1.0f, 1.0f);

        #define CALL(func, count) \
            call(#func, a, b, count, func)

        for (size_t i=MIN_RANK; i<=MAX_RANK; ++i)
        {
            const size_t count = 1 << i;

            CALL(generic::corr_init, count);
            IF_ARCH_X86(CALL(sse::corr_init, count));
            IF_ARCH_X86(CALL(avx::corr_init, count));
            IF_ARCH_X86(CALL(avx::corr_init_fma3, count));
            IF_ARCH_X86(CALL(avx512::corr_init, count));
            IF_ARCH_ARM(CALL(neon_d32::corr_init, count));
            IF_ARCH_AARCH64(CALL(asimd::corr_init, count));

            PTEST_SEPARATOR;
        }
    }

PTEST_END


