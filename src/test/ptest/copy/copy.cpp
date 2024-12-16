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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 20

namespace lsp
{
    namespace generic
    {
        void copy(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace x86
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace sse
        {
            void copy(float *dst, const float *src, size_t count);
            void copy_movntps(float *dst, const float *src, size_t count);
        }

        namespace sse3
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* copy_t)(float *dst, const float *src, size_t count);
}

// Standard implementation provided by C library
static void copy(float *dst, const float *src, size_t count)
{
    memcpy(dst, src, count * sizeof(float));
}

PTEST_BEGIN("dsp.copy", copy, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, copy_t copy)
    {
        if (!PTEST_SUPPORTED(copy))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            copy(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *out      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *in       = &out[buf_size];

        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = randf(-1.0f, 1.0f);

        TEST_EXPORT(::copy);

        for (size_t i=MIN_RANK; i <= MAX_RANK; i += 2)
        {
            size_t count = 1 << i;

            call("std::copy", out, in, count, ::copy);
            call("generic::copy", out, in, count, generic::copy);
            IF_ARCH_X86(call("x86::movs_copy", out, in, count, x86::copy));
            IF_ARCH_X86(call("sse::copy", out, in, count, sse::copy));
            IF_ARCH_X86(call("sse::copy_movntps", out, in, count, sse::copy_movntps));
            IF_ARCH_X86(call("sse3::copy", out, in, count, sse3::copy));
            IF_ARCH_X86(call("avx::copy", out, in, count, avx::copy));
            IF_ARCH_X86(call("avx512::copy", out, in, count, avx512::copy));
            IF_ARCH_ARM(call("neon_d32::copy", out, in, count, neon_d32::copy));
            IF_ARCH_AARCH64(call("asimd::copy", out, in, count, asimd::copy));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


