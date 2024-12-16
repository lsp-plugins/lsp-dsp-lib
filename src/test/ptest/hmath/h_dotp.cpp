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

#include <lsp-plug.in/common/alloc.h>
#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 5
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        float h_dotp(const float *a, const float *b, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }

        namespace avx
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }

        namespace avx512
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    typedef float (* h_dotp_t)(const float *a, const float *b, size_t count);
}

PTEST_BEGIN("dsp.hmath", h_dotp, 2, 10000)

    void call(const char *label, float *a, float *b, size_t count, h_dotp_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(a, b, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *a        = alloc_aligned<float>(data, buf_size * 2, 64);
        float *b        = &a[buf_size];

        randomize_sign(a, buf_size * 2);

        #define CALL(func) \
            call(#func, a, b, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::h_dotp);
            IF_ARCH_X86(CALL(sse::h_dotp));
            IF_ARCH_X86(CALL(avx::h_dotp));
            IF_ARCH_X86(CALL(avx512::h_dotp));
            IF_ARCH_ARM(CALL(neon_d32::h_dotp));
            IF_ARCH_AARCH64(CALL(asimd::h_dotp));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END



