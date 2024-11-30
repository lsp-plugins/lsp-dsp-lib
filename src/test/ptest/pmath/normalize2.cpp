/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 нояб. 2022 г.
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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        static void normalize2_test(float *dst, const float *src, size_t count)
        {
            // Find maximum
            float max = 0.0f;
            for (size_t i=0; i<count; ++i)
            {
                float v = fabs(src[i]);
                if (max < v)
                    max = v;
            }

            if (max > 0.0f)
            {
                // Scale data
                max = 1.0f / max;
                for (size_t i=0; i<count; ++i)
                    dst[i] = src[i] * max;
            }
            else if (dst != src)
            {
                // Copy data
                for (size_t i=0; i<count; ++i)
                    dst[i] = src[i];
            }
        }

        void normalize2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void normalize2(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void normalize2(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void normalize2(float *dst, const float *src, size_t count);
        }
    )

}

typedef void (* normalize2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
PTEST_BEGIN("dsp.pmath", normalize2, 5, 2000)

    void call(const char *label, float *dst, const float *src, size_t count, normalize2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 4, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        randomize_sign(dst, buf_size*2);
        dsp::copy(backup, dst, buf_size*2);

        #define CALL(method) \
            dsp::copy(dst, backup, buf_size*2); \
            call(#method, dst, src, count, method);

        TEST_EXPORT(generic::normalize2_test);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::normalize2_test);
            CALL(generic::normalize2);
            IF_ARCH_X86(CALL(sse::normalize2));
            IF_ARCH_X86(CALL(avx::normalize2));
            IF_ARCH_X86(CALL(avx512::normalize2));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



