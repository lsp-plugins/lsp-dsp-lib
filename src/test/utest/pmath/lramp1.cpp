/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 февр. 2023 г.
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

namespace
{
    void lramp_set1(float *dst, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = v1 + delta * i;
    }

    void lramp1(float *dst, float v1, float v2, size_t count)
    {
        if (count == 0)
            return;
        float delta = (v2 - v1)/count;
        for (size_t i=0; i<count; ++i)
            dst[i] = dst[i] * (v1 + delta * i);
    }
}

namespace lsp
{
    namespace generic
    {
        void lramp_set1(float *dst, float v1, float v2, size_t count);
        void lramp1(float *dst, float v1, float v2, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }

        namespace avx
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }

        namespace avx512
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lramp_set1(float *dst, float v1, float v2, size_t count);
            void lramp1(float *dst, float v1, float v2, size_t count);
        }
    )
}

typedef void (* lramp1_t)(float *dst, float v1, float v2, size_t count);

UTEST_BEGIN("dsp.pmath", lramp1)

    void call(const char *label, size_t align, lramp1_t func1, lramp1_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(count, align, mask & 0x01);

                // Call functions
                dst1.randomize_sign();
                dst2.copy(dst1);
                func1(dst1, 0.25f, 0.75f, count);
                func2(dst2, 0.25f, 0.75f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }

                // Call functions
                dst1.randomize_sign();
                dst2.copy(dst1);
                func1(dst1, 0.5f, 0.5f, count);
                func2(dst2, 0.5f, 0.5f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
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
        #define CALL(generic, func, align) call(#func, align, generic, func)

        TEST_EXPORT(lramp_set1);
        TEST_EXPORT(lramp1);

        CALL(lramp_set1, generic::lramp_set1, 16);
        CALL(lramp1, generic::lramp1, 16);

        IF_ARCH_X86(CALL(lramp_set1, sse::lramp_set1, 16));
        IF_ARCH_X86(CALL(lramp1, sse::lramp1, 16));
        IF_ARCH_X86(CALL(lramp_set1, avx::lramp_set1, 32));
        IF_ARCH_X86(CALL(lramp1, avx::lramp1, 32));
        IF_ARCH_X86(CALL(lramp_set1, avx512::lramp_set1, 64));
        IF_ARCH_X86(CALL(lramp1, avx512::lramp1, 64));

        IF_ARCH_ARM(CALL(lramp_set1, neon_d32::lramp_set1, 16));
        IF_ARCH_ARM(CALL(lramp1, neon_d32::lramp1, 16));

        IF_ARCH_AARCH64(CALL(lramp_set1, asimd::lramp_set1, 16));
        IF_ARCH_AARCH64(CALL(lramp1, asimd::lramp1, 16));
    }
UTEST_END



