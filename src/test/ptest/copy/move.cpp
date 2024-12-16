/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 мая 2023 г.
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
        void move(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void move(float *dst, const float *src, size_t count);
        }

        namespace sse3
        {
            void move(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void move(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void move(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void move(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void move(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* move_t)(float *dst, const float *src, size_t count);
}

// Standard implementation provided by C library
static void move(float *dst, const float *src, size_t count)
{
    memmove(dst, src, count * sizeof(float));
}

PTEST_BEGIN("dsp.copy", move, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, move_t move)
    {
        if (!PTEST_SUPPORTED(move))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            move(dst, src, count);
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

        TEST_EXPORT(::move);

        for (size_t i=MIN_RANK; i <= MAX_RANK; i += 2)
        {
            size_t count = 1 << i;

            call("std::move", out, in, count, ::move);
            call("generic::move", out, in, count, generic::move);
            IF_ARCH_X86(call("sse::move", out, in, count, sse::move));
            IF_ARCH_X86(call("sse3::move", out, in, count, sse3::move));
            IF_ARCH_X86(call("avx::move", out, in, count, avx::move));
            IF_ARCH_X86(call("avx512::move", out, in, count, avx512::move));
            IF_ARCH_ARM(call("neon_d32::move", out, in, count, neon_d32::move));
            IF_ARCH_AARCH64(call("asimd::move", out, in, count, asimd::move));

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END


