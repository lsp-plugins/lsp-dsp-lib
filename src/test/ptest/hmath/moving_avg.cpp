/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 нояб. 2026 г.
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

#define MIN_RANK 4
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count);
        }
    )

    typedef float (* moving_avg_t)(float *dst, const float *a, const float *b, float value, float avg, size_t count);
}

PTEST_BEGIN("dsp.hmath", moving_avg, 2, 10000)

    void call(const char *label, float *dst, const float *a, const float *b, size_t count, moving_avg_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, a, b, 0.0f, 0.1f, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *a        = &dst[buf_size];
        float *b        = &a[buf_size];

        randomize(a, buf_size, -1.0f, 1.0f);
        randomize(b, buf_size, -1.0f, 1.0f);

        #define CALL(func) \
            call(#func, dst, a, b, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::moving_avg);
            IF_ARCH_X86(CALL(sse::moving_avg));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


