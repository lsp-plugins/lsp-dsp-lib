/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 окт. 2023 г.
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

namespace lsp
{
    namespace generic
    {
        void loge2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void loge2(float *dst, const float *src, size_t count);
        }

        namespace avx2
        {
            void loge2(float *dst, const float *src, size_t count);
            void loge2_fma3(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void loge2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_loge2(float *dst, const float *src, size_t count);
            void x64_loge2_fma3(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void x64_loge2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void loge2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void loge2(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* log2_t)(float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", loge2)

    void call(const char *label, size_t align, log2_t func1, log2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize(1e-6, 1e+6);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        IF_ARCH_X86(CALL(generic::loge2, sse2::loge2, 16));
        IF_ARCH_X86(CALL(generic::loge2, avx2::loge2, 32));
        IF_ARCH_X86_64(CALL(generic::loge2, avx2::x64_loge2, 32));
        IF_ARCH_X86(CALL(generic::loge2, avx2::loge2_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::loge2, avx2::x64_loge2_fma3, 32));
        IF_ARCH_X86(CALL(generic::loge2, avx512::loge2, 64));
        IF_ARCH_X86_64(CALL(generic::loge2, avx512::x64_loge2, 64));

        IF_ARCH_ARM(CALL(generic::loge2, neon_d32::loge2, 16));
        IF_ARCH_AARCH64(CALL(generic::loge2, asimd::loge2, 16));
    }
UTEST_END



