/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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
        void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
        void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
            void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
        }

        namespace avx
        {
            void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
            void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
        }

        namespace avx512
        {
            void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
            void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
            void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    lr_to_ms(float *m, float *s, const float *l, const float *r, size_t count);
            void    ms_to_lr(float *l, float *r, const float *m, const float *s, size_t count);
        }
    )

    typedef void (* conv2_t)(float *d1, float *d2, const float *s1, const float *s2, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test for complex multiplication
UTEST_BEGIN("dsp.msmatrix", conv2)

    void call(const char *label, size_t align, conv2_t func1, conv2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 33, 34, 35, 36, 37, 38, 39, 40, 63, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer srcA(count, align, mask & 0x01);
                FloatBuffer srcB(count, align, mask & 0x02);
                FloatBuffer dst1A(count, align, mask & 0x04);
                FloatBuffer dst1B(count, align, mask & 0x08);
                FloatBuffer dst2A(dst1A);
                FloatBuffer dst2B(dst1B);

                // Call functions
                func1(dst1A, dst1B, srcA, srcB, count);
                func1(dst2A, dst2B, srcA, srcB, count);

                UTEST_ASSERT_MSG(srcA.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(srcB.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(dst1A.valid(), "Destination buffer 1A corrupted");
                UTEST_ASSERT_MSG(dst1B.valid(), "Destination buffer 1B corrupted");
                UTEST_ASSERT_MSG(dst2A.valid(), "Destination buffer 2A corrupted");
                UTEST_ASSERT_MSG(dst2B.valid(), "Destination buffer 2B corrupted");

                // Compare buffers
                if ((!dst1A.equals_adaptive(dst2A, 1e-5)) || (!dst1B.equals_adaptive(dst2B, 1e-5)))
                {
                    srcA.dump("srcA ");
                    srcB.dump("srcA ");
                    dst1A.dump("dst1A");
                    dst1B.dump("dst1B");
                    dst2A.dump("dst2A");
                    dst2B.dump("dst2B");
                    if (dst1A.last_diff() >= 0)
                        printf("dst1A vs dst2A index=%d, %.6f vs %.6f\n", dst1A.last_diff(), dst1A.get_diff(), dst2A.get_diff());
                    if (dst1B.last_diff() >= 0)
                        printf("dst1B vs dst2B index=%d, %.6f vs %.6f\n", dst1B.last_diff(), dst1B.get_diff(), dst2B.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call( #func, align, generic, func)

        IF_ARCH_X86(CALL(generic::lr_to_ms, sse::lr_to_ms, 16));
        IF_ARCH_X86(CALL(generic::ms_to_lr, sse::ms_to_lr, 16));

        IF_ARCH_X86(CALL(generic::lr_to_ms, avx::lr_to_ms, 32));
        IF_ARCH_X86(CALL(generic::ms_to_lr, avx::ms_to_lr, 32));

        IF_ARCH_X86(CALL(generic::lr_to_ms, avx512::lr_to_ms, 64));
        IF_ARCH_X86(CALL(generic::ms_to_lr, avx512::ms_to_lr, 64));

        IF_ARCH_ARM(CALL(generic::lr_to_ms, neon_d32::lr_to_ms, 16));
        IF_ARCH_ARM(CALL(generic::ms_to_lr, neon_d32::ms_to_lr, 16));

        IF_ARCH_AARCH64(CALL(generic::lr_to_ms, asimd::lr_to_ms, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_to_lr, asimd::ms_to_lr, 16));
    }
UTEST_END


