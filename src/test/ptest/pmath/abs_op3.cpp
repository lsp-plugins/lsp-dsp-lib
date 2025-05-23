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
        void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_max3(float *dst, const float *src1, const float *src2, size_t count);
        void    abs_min3(float *dst, const float *src1, const float *src2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_max3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_min3(float *dst, const float *src1, const float *src2, size_t count);
        }

        namespace avx512
        {
            void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_max3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_min3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx
        {
            void    x64_abs_add3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_div3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_max3(float *dst, const float *src1, const float *src2, size_t count);
            void    x64_abs_min3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_max3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_min3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    abs_add3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_sub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rsub3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_mul3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_div3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_max3(float *dst, const float *src1, const float *src2, size_t count);
            void    abs_min3(float *dst, const float *src1, const float *src2, size_t count);
        }
    )

    typedef void (* abs_op3_t)(float *dst, const float *src1, const float *src2, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for complex multiplication
PTEST_BEGIN("dsp.pmath", abs_op3, 5, 1000)

    void call(const char *label, float *dst, const float *src1, const float *src2, size_t count, abs_op3_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        char buf[80];
        snprintf(buf, sizeof(buf), "%s x %d", label, int(count));
        printf("Testing %s numbers...\n", buf);

        PTEST_LOOP(buf,
            func(dst, src1, src2, count);
        );
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *dst      = alloc_aligned<float>(data, buf_size * 3, 64);
        float *src1     = &dst[buf_size];
        float *src2     = &src1[buf_size];

        randomize_sign(src1, buf_size);
        randomize_sign(src2, buf_size);

        #define CALL(func) \
            call(#func, dst, src1, src2, count, func)

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            CALL(generic::abs_add3);
            IF_ARCH_X86(CALL(sse::abs_add3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_add3));
            IF_ARCH_X86(CALL(avx512::abs_add3));
            IF_ARCH_ARM(CALL(neon_d32::abs_add3));
            IF_ARCH_AARCH64(CALL(asimd::abs_add3));
            PTEST_SEPARATOR;

            CALL(generic::abs_sub3);
            IF_ARCH_X86(CALL(sse::abs_sub3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_sub3));
            IF_ARCH_X86(CALL(avx512::abs_sub3));
            IF_ARCH_ARM(CALL(neon_d32::abs_sub3));
            IF_ARCH_AARCH64(CALL(asimd::abs_sub3));
            PTEST_SEPARATOR;

            CALL(generic::abs_rsub3);
            IF_ARCH_X86(CALL(sse::abs_rsub3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_rsub3));
            IF_ARCH_X86(CALL(avx512::abs_rsub3));
            IF_ARCH_ARM(CALL(neon_d32::abs_rsub3));
            IF_ARCH_AARCH64(CALL(asimd::abs_rsub3));
            PTEST_SEPARATOR;

            CALL(generic::abs_mul3);
            IF_ARCH_X86(CALL(sse::abs_mul3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_mul3));
            IF_ARCH_X86(CALL(avx512::abs_mul3));
            IF_ARCH_ARM(CALL(neon_d32::abs_mul3));
            IF_ARCH_AARCH64(CALL(asimd::abs_mul3));
            PTEST_SEPARATOR;

            CALL(generic::abs_div3);
            IF_ARCH_X86(CALL(sse::abs_div3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_div3));
            IF_ARCH_X86(CALL(avx512::abs_div3));
            IF_ARCH_ARM(CALL(neon_d32::abs_div3));
            IF_ARCH_AARCH64(CALL(asimd::abs_div3));
            PTEST_SEPARATOR;

            CALL(generic::abs_rdiv3);
            IF_ARCH_X86(CALL(sse::abs_rdiv3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_rdiv3));
            IF_ARCH_X86(CALL(avx512::abs_rdiv3));
            IF_ARCH_ARM(CALL(neon_d32::abs_rdiv3));
            IF_ARCH_AARCH64(CALL(asimd::abs_rdiv3));
            PTEST_SEPARATOR;

            CALL(generic::abs_max3);
            IF_ARCH_X86(CALL(sse::abs_max3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_max3));
            IF_ARCH_X86(CALL(avx512::abs_max3));
            IF_ARCH_ARM(CALL(neon_d32::abs_max3));
            IF_ARCH_AARCH64(CALL(asimd::abs_max3));
            PTEST_SEPARATOR;

            CALL(generic::abs_min3);
            IF_ARCH_X86(CALL(sse::abs_min3));
            IF_ARCH_X86_64(CALL(avx::x64_abs_min3));
            IF_ARCH_X86(CALL(avx512::abs_min3));
            IF_ARCH_ARM(CALL(neon_d32::abs_min3));
            IF_ARCH_AARCH64(CALL(asimd::abs_min3));
            PTEST_SEPARATOR2;
        }

        free_aligned(data);
    }
PTEST_END


