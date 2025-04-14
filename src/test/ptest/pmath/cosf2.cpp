/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 14 апр. 2025 г.
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
#include <lsp-plug.in/stdlib/math.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void cosf2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void cosf2(float *dst, const float *src, size_t count);
        }

        namespace avx2
        {
            void cosf2(float *dst, const float *src, size_t count);
            void cosf2_fma3(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void cosf2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void cosf2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void cosf2(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* cosf2_t)(float *dst, const float *src, size_t count);
}


//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", cosf2, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, cosf2_t func)
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
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src      = &dst[buf_size];
        float *backup   = &src[buf_size];

        for (size_t i=0; i < buf_size*3; ++i)
            dst[i]          = randf(-100.0f * M_PI, 100.0f * M_PI);
        dsp::copy(backup, dst, buf_size);

        #define CALL(func) \
            dsp::copy(dst, backup, buf_size); \
            call(#func, dst, src, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::cosf2);
            IF_ARCH_X86(CALL(sse2::cosf2));
            IF_ARCH_X86(CALL(avx2::cosf2));
            IF_ARCH_X86(CALL(avx2::cosf2_fma3));
            IF_ARCH_X86(CALL(avx512::cosf2));
            IF_ARCH_ARM(CALL(neon_d32::cosf2));
            IF_ARCH_AARCH64(CALL(asimd::cosf2));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END




