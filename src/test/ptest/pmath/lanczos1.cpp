/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 17 апр. 2025 г.
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
        void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
        }

        namespace avx2
        {
            void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
            void lanczos1_fma3(float *dst, float k, float p, float t, float a, size_t count);
        }

        namespace avx512
        {
            void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_lanczos1(float *dst, float k, float p, float t, float a, size_t count);
            void x64_lanczos1_fma3(float *dst, float k, float p, float t, float a, size_t count);
        }

        namespace avx512
        {
            void x64_lanczos1(float *dst, float k, float p, float t, float a, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lanczos1(float *dst, float k, float p, float t, float a, size_t count);
        }
    )

    typedef void (* lanczos1_t)(float *dst, float k, float p, float t, float a, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test
PTEST_BEGIN("dsp.pmath", lanczos1, 5, 1000)

    void call(const char *label, float *dst, size_t count, lanczos1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        const size_t length     = count;
        const size_t lobes      = 8.0f;
        const size_t center     = length >> 1;
        const float shift       = 0.0f;
        const float k           = M_PI * float(lobes) / float(center); // Number of samples per lobe
        const float p           = (float(center) - shift) * k;
        const float t           = float(center) * k;
        const float a           = 1.0f / float(lobes);

        PTEST_LOOP(buf,
            func(dst, k, p, t, a, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size, 64);

        #define CALL(func) \
            call(#func, dst, count, func);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::lanczos1);
            IF_ARCH_X86(CALL(sse2::lanczos1));
            IF_ARCH_X86(CALL(avx2::lanczos1));
            IF_ARCH_X86(CALL(avx2::lanczos1_fma3));
            IF_ARCH_X86_64(CALL(avx2::x64_lanczos1));
            IF_ARCH_X86_64(CALL(avx2::x64_lanczos1_fma3));
            IF_ARCH_X86(CALL(avx512::lanczos1));
            IF_ARCH_X86_64(CALL(avx512::x64_lanczos1));
            IF_ARCH_ARM(CALL(neon_d32::lanczos1));
            IF_ARCH_AARCH64(CALL(asimd::lanczos1));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END




