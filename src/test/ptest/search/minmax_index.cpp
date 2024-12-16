/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
        }

        namespace avx2
        {
            void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
        }

        namespace avx512
        {
            void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    minmax_index(const float *src, size_t count, size_t *min, size_t *max);
        }
    )

    typedef void    (* cond_minmax_t)(const float *src, size_t count, size_t *min, size_t *max);
}

//-----------------------------------------------------------------------------
// Performance test for minimum and maximum searching
PTEST_BEGIN("dsp.search", minmax_index, 5, 1000)

    void call(const char *label, const float *in, size_t count, cond_minmax_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);
        size_t min, max;

        PTEST_LOOP(buf,
            func(in, count, &min, &max);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *in       = alloc_aligned<float>(data, buf_size, 64);
        for (size_t i=0; i < (1 << MAX_RANK); ++i)
            in[i]          = randf(-1.0f, 1.0f);

        #define CALL(func) \
            call(#func, in, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            //--------------
            CALL(generic::minmax_index);
            IF_ARCH_X86(CALL(sse2::minmax_index));
            IF_ARCH_X86(CALL(avx2::minmax_index));
            IF_ARCH_X86(CALL(avx512::minmax_index));
            IF_ARCH_ARM(CALL(neon_d32::minmax_index));
            IF_ARCH_AARCH64(CALL(asimd::minmax_index));
            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }

PTEST_END


