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

#include <lsp-plug.in/common/types.h>
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace generic
    {
        void lanczos_resample_2x2(float *dst, const float *src, size_t count);
        void lanczos_resample_2x3(float *dst, const float *src, size_t count);
        void lanczos_resample_2x4(float *dst, const float *src, size_t count);
        void lanczos_resample_3x2(float *dst, const float *src, size_t count);
        void lanczos_resample_3x3(float *dst, const float *src, size_t count);
        void lanczos_resample_3x4(float *dst, const float *src, size_t count);
        void lanczos_resample_4x2(float *dst, const float *src, size_t count);
        void lanczos_resample_4x3(float *dst, const float *src, size_t count);
        void lanczos_resample_4x4(float *dst, const float *src, size_t count);
        void lanczos_resample_6x2(float *dst, const float *src, size_t count);
        void lanczos_resample_6x3(float *dst, const float *src, size_t count);
        void lanczos_resample_6x4(float *dst, const float *src, size_t count);
        void lanczos_resample_8x2(float *dst, const float *src, size_t count);
        void lanczos_resample_8x3(float *dst, const float *src, size_t count);
        void lanczos_resample_8x4(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lanczos_resample_2x2(float *dst, const float *src, size_t count);
            void lanczos_resample_2x3(float *dst, const float *src, size_t count);
            void lanczos_resample_2x4(float *dst, const float *src, size_t count);
            void lanczos_resample_3x2(float *dst, const float *src, size_t count);
            void lanczos_resample_3x3(float *dst, const float *src, size_t count);
            void lanczos_resample_3x4(float *dst, const float *src, size_t count);
            void lanczos_resample_4x2(float *dst, const float *src, size_t count);
            void lanczos_resample_4x3(float *dst, const float *src, size_t count);
            void lanczos_resample_4x4(float *dst, const float *src, size_t count);
            void lanczos_resample_6x2(float *dst, const float *src, size_t count);
            void lanczos_resample_6x3(float *dst, const float *src, size_t count);
            void lanczos_resample_6x4(float *dst, const float *src, size_t count);
            void lanczos_resample_8x2(float *dst, const float *src, size_t count);
            void lanczos_resample_8x3(float *dst, const float *src, size_t count);
            void lanczos_resample_8x4(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void lanczos_resample_2x2(float *dst, const float *src, size_t count);
            void lanczos_resample_2x3(float *dst, const float *src, size_t count);
            void lanczos_resample_2x4(float *dst, const float *src, size_t count);
            void lanczos_resample_3x2(float *dst, const float *src, size_t count);
            void lanczos_resample_3x3(float *dst, const float *src, size_t count);
            void lanczos_resample_3x4(float *dst, const float *src, size_t count);
            void lanczos_resample_4x2(float *dst, const float *src, size_t count);
            void lanczos_resample_4x3(float *dst, const float *src, size_t count);
            void lanczos_resample_4x4(float *dst, const float *src, size_t count);
            void lanczos_resample_6x2(float *dst, const float *src, size_t count);
            void lanczos_resample_6x3(float *dst, const float *src, size_t count);
            void lanczos_resample_6x4(float *dst, const float *src, size_t count);
            void lanczos_resample_8x2(float *dst, const float *src, size_t count);
            void lanczos_resample_8x3(float *dst, const float *src, size_t count);
            void lanczos_resample_8x4(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lanczos_resample_2x2(float *dst, const float *src, size_t count);
            void lanczos_resample_2x3(float *dst, const float *src, size_t count);
            void lanczos_resample_2x4(float *dst, const float *src, size_t count);
            void lanczos_resample_3x2(float *dst, const float *src, size_t count);
            void lanczos_resample_3x3(float *dst, const float *src, size_t count);
            void lanczos_resample_3x4(float *dst, const float *src, size_t count);
            void lanczos_resample_4x2(float *dst, const float *src, size_t count);
            void lanczos_resample_4x3(float *dst, const float *src, size_t count);
            void lanczos_resample_4x4(float *dst, const float *src, size_t count);
            void lanczos_resample_6x2(float *dst, const float *src, size_t count);
            void lanczos_resample_6x3(float *dst, const float *src, size_t count);
            void lanczos_resample_6x4(float *dst, const float *src, size_t count);
            void lanczos_resample_8x2(float *dst, const float *src, size_t count);
            void lanczos_resample_8x3(float *dst, const float *src, size_t count);
            void lanczos_resample_8x4(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lanczos_resample_2x2(float *dst, const float *src, size_t count);
            void lanczos_resample_2x3(float *dst, const float *src, size_t count);
            void lanczos_resample_2x4(float *dst, const float *src, size_t count);
            void lanczos_resample_3x2(float *dst, const float *src, size_t count);
            void lanczos_resample_3x3(float *dst, const float *src, size_t count);
            void lanczos_resample_3x4(float *dst, const float *src, size_t count);
            void lanczos_resample_4x2(float *dst, const float *src, size_t count);
            void lanczos_resample_4x3(float *dst, const float *src, size_t count);
            void lanczos_resample_4x4(float *dst, const float *src, size_t count);
            void lanczos_resample_6x2(float *dst, const float *src, size_t count);
            void lanczos_resample_6x3(float *dst, const float *src, size_t count);
            void lanczos_resample_6x4(float *dst, const float *src, size_t count);
            void lanczos_resample_8x2(float *dst, const float *src, size_t count);
            void lanczos_resample_8x3(float *dst, const float *src, size_t count);
            void lanczos_resample_8x4(float *dst, const float *src, size_t count);
        }
    )
}

UTEST_BEGIN("dsp.resampling", oversampling)

    void call(size_t times, const char *text, size_t align,
        dsp::resampling_function_t func1,
        dsp::resampling_function_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 31,
                32, 63, 64, 127, 100, 999)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s resampling for %d -> %d samples, mask=0x%x...\n", text, int(count), int(count * times), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count*times + LSP_DSP_RESAMPLING_RSV_SAMPLES, align, mask & 0x02);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                if (src.corrupted())
                    UTEST_FAIL_MSG("Source buffer corrupted");
                if (dst1.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 1 corrupted");
                if (dst2.corrupted())
                    UTEST_FAIL_MSG("Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2))
                {
                    src.dump("src1");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align, order) \
            call(order, #func, align, generic, func)

        // Do tests
        IF_ARCH_X86(CALL(generic::lanczos_resample_2x2, sse::lanczos_resample_2x2, 16, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_2x3, sse::lanczos_resample_2x3, 16, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_2x4, sse::lanczos_resample_2x4, 16, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x2, sse::lanczos_resample_3x2, 16, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x3, sse::lanczos_resample_3x3, 16, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x4, sse::lanczos_resample_3x4, 16, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x2, sse::lanczos_resample_4x2, 16, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x3, sse::lanczos_resample_4x3, 16, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x4, sse::lanczos_resample_4x4, 16, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x2, sse::lanczos_resample_6x2, 16, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x3, sse::lanczos_resample_6x3, 16, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x4, sse::lanczos_resample_6x4, 16, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x2, sse::lanczos_resample_8x2, 16, 8));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x3, sse::lanczos_resample_8x3, 16, 8));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x4, sse::lanczos_resample_8x4, 16, 8));

        IF_ARCH_X86(CALL(generic::lanczos_resample_2x2, avx::lanczos_resample_2x2, 32, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_2x3, avx::lanczos_resample_2x3, 32, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_2x4, avx::lanczos_resample_2x4, 32, 2));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x2, avx::lanczos_resample_3x2, 32, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x3, avx::lanczos_resample_3x3, 32, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_3x4, avx::lanczos_resample_3x4, 32, 3));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x2, avx::lanczos_resample_4x2, 32, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x3, avx::lanczos_resample_4x3, 32, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_4x4, avx::lanczos_resample_4x4, 32, 4));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x2, avx::lanczos_resample_6x2, 32, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x3, avx::lanczos_resample_6x3, 32, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_6x4, avx::lanczos_resample_6x4, 32, 6));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x2, avx::lanczos_resample_8x2, 32, 8));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x3, avx::lanczos_resample_8x3, 32, 8));
        IF_ARCH_X86(CALL(generic::lanczos_resample_8x4, avx::lanczos_resample_8x4, 32, 8));

        IF_ARCH_ARM(CALL(generic::lanczos_resample_2x2, neon_d32::lanczos_resample_2x2, 16, 2));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_2x3, neon_d32::lanczos_resample_2x3, 16, 2));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_2x4, neon_d32::lanczos_resample_2x4, 16, 2));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_3x2, neon_d32::lanczos_resample_3x2, 16, 3));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_3x3, neon_d32::lanczos_resample_3x3, 16, 3));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_3x4, neon_d32::lanczos_resample_3x4, 16, 3));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_4x2, neon_d32::lanczos_resample_4x2, 16, 4));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_4x3, neon_d32::lanczos_resample_4x3, 16, 4));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_4x4, neon_d32::lanczos_resample_4x4, 16, 4));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_6x2, neon_d32::lanczos_resample_6x2, 16, 6));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_6x3, neon_d32::lanczos_resample_6x3, 16, 6));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_6x4, neon_d32::lanczos_resample_6x4, 16, 6));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_8x2, neon_d32::lanczos_resample_8x2, 16, 8));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_8x3, neon_d32::lanczos_resample_8x3, 16, 8));
        IF_ARCH_ARM(CALL(generic::lanczos_resample_8x4, neon_d32::lanczos_resample_8x4, 16, 8));

        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_2x2, asimd::lanczos_resample_2x2, 16, 2));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_2x3, asimd::lanczos_resample_2x3, 16, 2));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_2x4, asimd::lanczos_resample_2x4, 16, 2));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_3x2, asimd::lanczos_resample_3x2, 16, 3));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_3x3, asimd::lanczos_resample_3x3, 16, 3));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_3x4, asimd::lanczos_resample_3x4, 16, 3));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_4x2, asimd::lanczos_resample_4x2, 16, 4));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_4x3, asimd::lanczos_resample_4x3, 16, 4));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_4x4, asimd::lanczos_resample_4x4, 16, 4));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_6x2, asimd::lanczos_resample_6x2, 16, 6));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_6x3, asimd::lanczos_resample_6x3, 16, 6));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_6x4, asimd::lanczos_resample_6x4, 16, 6));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_8x2, asimd::lanczos_resample_8x2, 16, 8));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_8x3, asimd::lanczos_resample_8x3, 16, 8));
        IF_ARCH_AARCH64(CALL(generic::lanczos_resample_8x4, asimd::lanczos_resample_8x4, 16, 8));
    }
UTEST_END;
