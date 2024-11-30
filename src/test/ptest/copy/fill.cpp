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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/stdlib/string.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void fill(float *dst, float value, size_t count);
        void fill_one(float *dst, size_t count);
        void fill_zero(float *dst, size_t count);
        void fill_minus_one(float *dst, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void fill(float *dst, float value, size_t count);
            void fill_one(float *dst, size_t count);
            void fill_zero(float *dst, size_t count);
            void fill_minus_one(float *dst, size_t count);
        }

        namespace avx
        {
            void fill(float *dst, float value, size_t count);
            void fill_one(float *dst, size_t count);
            void fill_zero(float *dst, size_t count);
            void fill_minus_one(float *dst, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void fill(float *dst, float value, size_t count);
            void fill_one(float *dst, size_t count);
            void fill_zero(float *dst, size_t count);
            void fill_minus_one(float *dst, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void fill(float *dst, float value, size_t count);
            void fill_one(float *dst, size_t count);
            void fill_zero(float *dst, size_t count);
            void fill_minus_one(float *dst, size_t count);
        }
    )

    typedef void (* fill_t)(float *dst, size_t count);
    typedef void (* fill_value_t)(float *dst, float value, size_t count);
}

static void fill_zero_memset(float *dst, size_t count)
{
    memset(dst, 0, count*sizeof(float));
}

static void fill_zero_bzero(float *dst, size_t count)
{
    bzero(dst, count*sizeof(float));
}

//-----------------------------------------------------------------------------
// Performance test for destination buffer filling
PTEST_BEGIN("dsp.copy", fill, 5, 5000)

    void call(const char *label, float *out, size_t count, fill_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(out, count);
        );
    }

    void call(const char *label, float *out, size_t count, fill_value_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(out, M_PI, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;

        float *out       = alloc_aligned<float>(data, buf_size, 64);

        #define CALL(func) \
            call(#func, out, count, func)

        TEST_EXPORT(::fill_zero_memset);
        TEST_EXPORT(::fill_zero_bzero);

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::fill);
            IF_ARCH_X86(CALL(sse::fill));
            IF_ARCH_X86(CALL(avx::fill));
            IF_ARCH_ARM(CALL(neon_d32::fill));
            IF_ARCH_AARCH64(CALL(asimd::fill));
            PTEST_SEPARATOR;

            CALL(generic::fill_zero);
            CALL(fill_zero_memset);
            CALL(fill_zero_bzero);
            IF_ARCH_X86(CALL(sse::fill_zero));
            IF_ARCH_X86(CALL(avx::fill_zero));
            IF_ARCH_ARM(CALL(neon_d32::fill_zero));
            IF_ARCH_AARCH64(CALL(asimd::fill_zero));
            PTEST_SEPARATOR;

            CALL(generic::fill_one);
            IF_ARCH_X86(CALL(sse::fill_one));
            IF_ARCH_X86(CALL(avx::fill_one));
            IF_ARCH_ARM(CALL(neon_d32::fill_one));
            IF_ARCH_AARCH64(CALL(asimd::fill_one));
            PTEST_SEPARATOR;

            CALL(generic::fill_minus_one);
            IF_ARCH_X86(CALL(sse::fill_minus_one));
            IF_ARCH_X86(CALL(avx::fill_minus_one));
            IF_ARCH_ARM(CALL(neon_d32::fill_minus_one));
            IF_ARCH_AARCH64(CALL(asimd::fill_minus_one));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }

PTEST_END
