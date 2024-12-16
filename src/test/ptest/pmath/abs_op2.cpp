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
#include <lsp-plug.in/test-fw/helpers.h>
#include <lsp-plug.in/test-fw/ptest.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void    abs_add2(float *dst, const float *src, size_t count);
        void    abs_sub2(float *dst, const float *src, size_t count);
        void    abs_rsub2(float *dst, const float *src, size_t count);
        void    abs_mul2(float *dst, const float *src, size_t count);
        void    abs_div2(float *dst, const float *src, size_t count);
        void    abs_rdiv2(float *dst, const float *src, size_t count);
        void    abs_max2(float *dst, const float *src, size_t count);
        void    abs_min2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    abs_add2(float *dst, const float *src, size_t count);
            void    abs_sub2(float *dst, const float *src, size_t count);
            void    abs_rsub2(float *dst, const float *src, size_t count);
            void    abs_mul2(float *dst, const float *src, size_t count);
            void    abs_div2(float *dst, const float *src, size_t count);
            void    abs_rdiv2(float *dst, const float *src, size_t count);
            void    abs_max2(float *dst, const float *src, size_t count);
            void    abs_min2(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void    abs_add2(float *dst, const float *src, size_t count);
            void    abs_sub2(float *dst, const float *src, size_t count);
            void    abs_rsub2(float *dst, const float *src, size_t count);
            void    abs_mul2(float *dst, const float *src, size_t count);
            void    abs_div2(float *dst, const float *src, size_t count);
            void    abs_rdiv2(float *dst, const float *src, size_t count);
            void    abs_max2(float *dst, const float *src, size_t count);
            void    abs_min2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx
        {
            void    x64_abs_add2(float *dst, const float *src, size_t count);
            void    x64_abs_sub2(float *dst, const float *src, size_t count);
            void    x64_abs_rsub2(float *dst, const float *src, size_t count);
            void    x64_abs_mul2(float *dst, const float *src, size_t count);
            void    x64_abs_div2(float *dst, const float *src, size_t count);
            void    x64_abs_rdiv2(float *dst, const float *src, size_t count);
            void    x64_abs_max2(float *dst, const float *src, size_t count);
            void    x64_abs_min2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    abs_add2(float *dst, const float *src, size_t count);
            void    abs_sub2(float *dst, const float *src, size_t count);
            void    abs_rsub2(float *dst, const float *src, size_t count);
            void    abs_mul2(float *dst, const float *src, size_t count);
            void    abs_div2(float *dst, const float *src, size_t count);
            void    abs_rdiv2(float *dst, const float *src, size_t count);
            void    abs_max2(float *dst, const float *src, size_t count);
            void    abs_min2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    abs_add2(float *dst, const float *src, size_t count);
            void    abs_sub2(float *dst, const float *src, size_t count);
            void    abs_rsub2(float *dst, const float *src, size_t count);
            void    abs_mul2(float *dst, const float *src, size_t count);
            void    abs_div2(float *dst, const float *src, size_t count);
            void    abs_rdiv2(float *dst, const float *src, size_t count);
            void    abs_max2(float *dst, const float *src, size_t count);
            void    abs_min2(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* abs_op2_t)(float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", abs_op2, 5, 1000)

    void call(const char *label, float *dst, const float *src, size_t count, abs_op2_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            dsp::copy(dst, src, count);
            func(dst, src, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 2, 64);
        float *src      = &dst[buf_size];

        for (size_t i=0; i < buf_size; ++i)
            src[i]          = randf(-1.0f, 1.0f);

        #define CALL(func) \
            call(#func, dst, src, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::abs_add2);
            IF_ARCH_X86(CALL(sse::abs_add2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_add2));
            IF_ARCH_X86(CALL(avx512::abs_add2));
            IF_ARCH_ARM(CALL(neon_d32::abs_add2));
            IF_ARCH_AARCH64(CALL(asimd::abs_add2));
            PTEST_SEPARATOR;

            CALL(generic::abs_sub2);
            IF_ARCH_X86(CALL(sse::abs_sub2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_sub2));
            IF_ARCH_X86(CALL(avx512::abs_sub2));
            IF_ARCH_ARM(CALL(neon_d32::abs_sub2));
            IF_ARCH_AARCH64(CALL(asimd::abs_sub2));
            PTEST_SEPARATOR;

            CALL(generic::abs_rsub2);
            IF_ARCH_X86(CALL(sse::abs_rsub2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_rsub2));
            IF_ARCH_X86(CALL(avx512::abs_rsub2));
            IF_ARCH_ARM(CALL(neon_d32::abs_rsub2));
            IF_ARCH_AARCH64(CALL(asimd::abs_rsub2));
            PTEST_SEPARATOR;

            CALL(generic::abs_mul2);
            IF_ARCH_X86(CALL(sse::abs_mul2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_mul2));
            IF_ARCH_X86(CALL(avx512::abs_mul2));
            IF_ARCH_ARM(CALL(neon_d32::abs_mul2));
            IF_ARCH_AARCH64(CALL(asimd::abs_mul2));
            PTEST_SEPARATOR;

            CALL(generic::abs_div2);
            IF_ARCH_X86(CALL(sse::abs_div2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_div2));
            IF_ARCH_X86(CALL(avx512::abs_div2));
            IF_ARCH_ARM(CALL(neon_d32::abs_div2));
            IF_ARCH_AARCH64(CALL(asimd::abs_div2));
            PTEST_SEPARATOR;

            CALL(generic::abs_rdiv2);
            IF_ARCH_X86(CALL(sse::abs_rdiv2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_rdiv2));
            IF_ARCH_X86(CALL(avx512::abs_rdiv2));
            IF_ARCH_ARM(CALL(neon_d32::abs_rdiv2));
            IF_ARCH_AARCH64(CALL(asimd::abs_rdiv2));
            PTEST_SEPARATOR;

            CALL(generic::abs_max2);
            IF_ARCH_X86(CALL(sse::abs_max2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_max2));
            IF_ARCH_X86(CALL(avx512::abs_max2));
            IF_ARCH_ARM(CALL(neon_d32::abs_max2));
            IF_ARCH_AARCH64(CALL(asimd::abs_max2));
            PTEST_SEPARATOR;

            CALL(generic::abs_min2);
            IF_ARCH_X86(CALL(sse::abs_min2));
            IF_ARCH_X86_64(CALL(avx::x64_abs_min2));
            IF_ARCH_X86(CALL(avx512::abs_min2));
            IF_ARCH_ARM(CALL(neon_d32::abs_min2));
            IF_ARCH_AARCH64(CALL(asimd::abs_min2));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END



