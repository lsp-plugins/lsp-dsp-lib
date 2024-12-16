/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 сент. 2023 г.
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
        void ssqrt1(float *dst, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void ssqrt1(float *dst, size_t count);
        }

        namespace avx
        {
            void ssqrt1(float *dst, size_t count);
        }

        namespace avx512
        {
            void ssqrt1(float *dst, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void ssqrt1(float *dst, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void ssqrt1(float *dst, size_t count);
        }
    )

    typedef void (* ssqrt1_t)(float *dst, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", ssqrt1, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, ssqrt1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *src      = &dst[buf_size];

        for (size_t i=0; i < buf_size*2; ++i)
            dst[i]          = randf(0.001f, 1000.0f);

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::ssqrt1);
            IF_ARCH_X86(CALL(sse::ssqrt1));
            IF_ARCH_X86(CALL(avx::ssqrt1));
            IF_ARCH_X86(CALL(avx512::ssqrt1));
            IF_ARCH_ARM(CALL(neon_d32::ssqrt1));
            IF_ARCH_AARCH64(CALL(asimd::ssqrt1));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


