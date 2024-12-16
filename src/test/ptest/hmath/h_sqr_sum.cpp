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

#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/ptest.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/common/alloc.h>

#define MIN_RANK 5
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        float h_sqr_sum(const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float h_sqr_sum(const float *src, size_t count);
        }

        namespace avx
        {
            float h_sqr_sum(const float *src, size_t count);
            float h_sqr_sum_fma3(const float *src, size_t count);
        }

        namespace avx512
        {
            float h_sqr_sum(const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float h_sqr_sum(const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float h_sqr_sum(const float *src, size_t count);
        }
    )

    typedef float (* h_sum_t)(const float *src, size_t count);
}

PTEST_BEGIN("dsp.hmath", h_sqr_sum, 2, 10000)

    void call(const char *label, float *src, size_t count, h_sum_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *src      = alloc_aligned<float>(data, buf_size, 64);

        for (size_t i=0; i < buf_size; ++i)
            src[i]          = randf(0.0f, 1.0f);

        #define CALL(func) \
            call(#func, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::h_sqr_sum);
            IF_ARCH_X86(CALL(sse::h_sqr_sum));
            IF_ARCH_X86(CALL(avx::h_sqr_sum));
            IF_ARCH_X86(CALL(avx::h_sqr_sum_fma3));
            IF_ARCH_X86(CALL(avx512::h_sqr_sum));
            IF_ARCH_ARM(CALL(neon_d32::h_sqr_sum));
            IF_ARCH_AARCH64(CALL(asimd::h_sqr_sum));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END

