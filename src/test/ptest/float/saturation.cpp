/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 7
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void copy_saturated(float *dst, const float *src, size_t count);
        void saturate(float *dst, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void copy_saturated(float *dst, const float *src, size_t count);
            void saturate(float *dst, size_t count);
        }

        namespace avx2
        {
            void copy_saturated(float *dst, const float *src, size_t count);
            void saturate(float *dst, size_t count);
        }

        namespace avx512
        {
            void copy_saturated(float *dst, const float *src, size_t count);
            void saturate(float *dst, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void copy_saturated(float *dst, const float *src, size_t count);
            void saturate(float *dst, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void copy_saturated(float *dst, const float *src, size_t count);
            void saturate(float *dst, size_t count);
        }
    )

    typedef void (* copy_saturated_t)(float *dst, const float *src, size_t count);
    typedef void (* saturate_t)(float *dst, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for logarithmic axis calculation
PTEST_BEGIN("dsp.float", saturation, 5, 10000)

    void call(const char *label, float *dst, const float *src, size_t count, copy_saturated_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src, count);
        );
    }

    void call(const char *label, float *dst, const float *src, size_t count, saturate_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s samples...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size     = 1 << MAX_RANK;
        uint8_t *data       = NULL;

        float *dst          = alloc_aligned<float>(data, buf_size * 2, 64);
        float *src          = &dst[buf_size];

        for (size_t i=0; i<buf_size; ++i)
        {
            switch (i % 6)
            {
                case 0:  src[i] = +INFINITY; break;
                case 1:  src[i] = -INFINITY; break;
                case 2:  src[i] = NAN; break;
                case 3:  src[i] = -NAN; break;
                case 4:  src[i] = randf(0.0f, 1.0f); break;
                default: src[i] = randf(-1.0f, 1.0f); break;
            }
        }

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::saturate);
            IF_ARCH_X86(CALL(sse2::saturate));
            IF_ARCH_X86(CALL(avx2::saturate));
            IF_ARCH_X86(CALL(avx512::saturate));
            IF_ARCH_ARM(CALL(neon_d32::saturate));
            IF_ARCH_AARCH64(CALL(asimd::saturate));
            PTEST_SEPARATOR;

            CALL(generic::copy_saturated);
            IF_ARCH_X86(CALL(sse2::copy_saturated));
            IF_ARCH_X86(CALL(avx2::copy_saturated));
            IF_ARCH_X86(CALL(avx512::copy_saturated));
            IF_ARCH_ARM(CALL(neon_d32::copy_saturated));
            IF_ARCH_AARCH64(CALL(asimd::copy_saturated));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END




