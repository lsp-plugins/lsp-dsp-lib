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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        float   min(const float *src, size_t count);
        float   max(const float *src, size_t count);
        void    minmax(const float *src, size_t count, float *min, float *max);

        float   abs_min(const float *src, size_t count);
        float   abs_max(const float *src, size_t count);
        void    abs_minmax(const float *src, size_t count, float *min, float *max);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float   min(const float *src, size_t count);
            float   max(const float *src, size_t count);
            void    minmax(const float *src, size_t count, float *min, float *max);

            float   abs_min(const float *src, size_t count);
            float   abs_max(const float *src, size_t count);
            void    abs_minmax(const float *src, size_t count, float *min, float *max);
        }

        namespace avx
        {
            float   min(const float *src, size_t count);
            float   max(const float *src, size_t count);
            void    minmax(const float *src, size_t count, float *min, float *max);

            float   abs_min(const float *src, size_t count);
            float   abs_max(const float *src, size_t count);
            void    abs_minmax(const float *src, size_t count, float *min, float *max);
        }

        namespace avx512
        {
            float   min(const float *src, size_t count);
            float   max(const float *src, size_t count);
            void    minmax(const float *src, size_t count, float *min, float *max);

            float   abs_min(const float *src, size_t count);
            float   abs_max(const float *src, size_t count);
            void    abs_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float   min(const float *src, size_t count);
            float   max(const float *src, size_t count);
            void    minmax(const float *src, size_t count, float *min, float *max);

            float   abs_min(const float *src, size_t count);
            float   abs_max(const float *src, size_t count);
            void    abs_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float   min(const float *src, size_t count);
            float   max(const float *src, size_t count);
            void    minmax(const float *src, size_t count, float *min, float *max);

            float   abs_min(const float *src, size_t count);
            float   abs_max(const float *src, size_t count);
            void    abs_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    typedef float (* search1_t) (const float *src, size_t count);
    typedef void  (* search2_t) (const float *src, size_t count, float *min, float *max);
}

//-----------------------------------------------------------------------------
// Performance test for minimum and maximum searching
PTEST_BEGIN("dsp.search", minmax, 5, 1000)

    void call(const char *label, const float *in, size_t count, search1_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(in, count);
        );
    }

    void call(const char *label, const float *in, size_t count, search2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        float min, max;
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
            CALL(generic::min);
            IF_ARCH_X86(CALL(sse::min));
            IF_ARCH_X86(CALL(avx::min));
            IF_ARCH_X86(CALL(avx512::min));
            IF_ARCH_ARM(CALL(neon_d32::min));
            IF_ARCH_AARCH64(CALL(asimd::min));

            CALL(generic::abs_min);
            IF_ARCH_X86(CALL(sse::abs_min));
            IF_ARCH_X86(CALL(avx::abs_min));
            IF_ARCH_X86(CALL(avx512::abs_min));
            IF_ARCH_ARM(CALL(neon_d32::abs_min));
            IF_ARCH_AARCH64(CALL(asimd::abs_min));
            PTEST_SEPARATOR;

            //--------------
            CALL(generic::max);
            IF_ARCH_X86(CALL(sse::max));
            IF_ARCH_X86(CALL(avx::max));
            IF_ARCH_X86(CALL(avx512::max));
            IF_ARCH_ARM(CALL(neon_d32::max));
            IF_ARCH_AARCH64(CALL(asimd::max));

            CALL(generic::abs_max);
            IF_ARCH_X86(CALL(sse::abs_max));
            IF_ARCH_X86(CALL(avx::abs_max));
            IF_ARCH_X86(CALL(avx512::abs_max));
            IF_ARCH_ARM(CALL(neon_d32::abs_max));
            IF_ARCH_AARCH64(CALL(asimd::abs_max));
            PTEST_SEPARATOR;

            //--------------
            CALL(generic::minmax);
            IF_ARCH_X86(CALL(sse::minmax));
            IF_ARCH_X86(CALL(avx::minmax));
            IF_ARCH_X86(CALL(avx512::minmax));
            IF_ARCH_ARM(CALL(neon_d32::minmax));
            IF_ARCH_AARCH64(CALL(asimd::minmax));

            CALL(generic::abs_minmax);
            IF_ARCH_X86(CALL(sse::abs_minmax));
            IF_ARCH_X86(CALL(avx::abs_minmax));
            IF_ARCH_X86(CALL(avx512::abs_minmax));
            IF_ARCH_ARM(CALL(neon_d32::abs_minmax));
            IF_ARCH_AARCH64(CALL(asimd::abs_minmax));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END


