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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

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
}

typedef void (* abs_op3_t)(float *dst, const float *src1, const float *src2, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.pmath", abs_op3)

    void call(const char *label, size_t align, abs_op3_t func1, abs_op3_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                src1.randomize_sign();
                src2.randomize_sign();
                func1(dst1, src1, src2, count);
                func2(dst2, src1, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func);

        IF_ARCH_X86(CALL(generic::abs_add3, sse::abs_add3, 16));
        IF_ARCH_X86(CALL(generic::abs_sub3, sse::abs_sub3, 16));
        IF_ARCH_X86(CALL(generic::abs_rsub3, sse::abs_rsub3, 16));
        IF_ARCH_X86(CALL(generic::abs_mul3, sse::abs_mul3, 16));
        IF_ARCH_X86(CALL(generic::abs_div3, sse::abs_div3, 16));
        IF_ARCH_X86(CALL(generic::abs_rdiv3, sse::abs_rdiv3, 16));
        IF_ARCH_X86(CALL(generic::abs_max3, sse::abs_max3, 16));
        IF_ARCH_X86(CALL(generic::abs_min3, sse::abs_min3, 16));

        IF_ARCH_X86_64(CALL(generic::abs_add3, avx::x64_abs_add3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_sub3, avx::x64_abs_sub3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_rsub3, avx::x64_abs_rsub3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_mul3, avx::x64_abs_mul3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_div3, avx::x64_abs_div3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_rdiv3, avx::x64_abs_rdiv3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_max3, avx::x64_abs_max3, 32));
        IF_ARCH_X86_64(CALL(generic::abs_min3, avx::x64_abs_min3, 32));

        IF_ARCH_X86(CALL(generic::abs_add3, avx512::abs_add3, 64));
        IF_ARCH_X86(CALL(generic::abs_sub3, avx512::abs_sub3, 64));
        IF_ARCH_X86(CALL(generic::abs_rsub3, avx512::abs_rsub3, 64));
        IF_ARCH_X86(CALL(generic::abs_mul3, avx512::abs_mul3, 64));
        IF_ARCH_X86(CALL(generic::abs_div3, avx512::abs_div3, 64));
        IF_ARCH_X86(CALL(generic::abs_rdiv3, avx512::abs_rdiv3, 64));
        IF_ARCH_X86(CALL(generic::abs_max3, avx512::abs_max3, 64));
        IF_ARCH_X86(CALL(generic::abs_min3, avx512::abs_min3, 64));

        IF_ARCH_ARM(CALL(generic::abs_add3, neon_d32::abs_add3, 16));
        IF_ARCH_ARM(CALL(generic::abs_sub3, neon_d32::abs_sub3, 16));
        IF_ARCH_ARM(CALL(generic::abs_rsub3, neon_d32::abs_rsub3, 16));
        IF_ARCH_ARM(CALL(generic::abs_mul3, neon_d32::abs_mul3, 16));
        IF_ARCH_ARM(CALL(generic::abs_div3, neon_d32::abs_div3, 16));
        IF_ARCH_ARM(CALL(generic::abs_rdiv3, neon_d32::abs_rdiv3, 16));
        IF_ARCH_ARM(CALL(generic::abs_max3, neon_d32::abs_max3, 16));
        IF_ARCH_ARM(CALL(generic::abs_min3, neon_d32::abs_min3, 16));

        IF_ARCH_AARCH64(CALL(generic::abs_add3, asimd::abs_add3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_sub3, asimd::abs_sub3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_rsub3, asimd::abs_rsub3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_mul3, asimd::abs_mul3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_div3, asimd::abs_div3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_rdiv3, asimd::abs_rdiv3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_max3, asimd::abs_max3, 16));
        IF_ARCH_AARCH64(CALL(generic::abs_min3, asimd::abs_min3, 16));
    }
UTEST_END



