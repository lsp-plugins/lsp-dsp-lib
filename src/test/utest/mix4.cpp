/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 дек. 2024 г.
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
        void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
        void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
            void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
            void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        }

        namespace avx
        {
            void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
            void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
            void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        }

        namespace avx512
        {
            void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
            void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
            void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
            void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
            void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void mix4(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
            void mix_copy4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
            void mix_add4(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
        }
    )

    typedef void (* mix4_t)(float *dst, const float *src1, const float *src2, const float *src3, float k1, float k2, float k3, float k4, size_t count);
    typedef void (* mix_dst4_t)(float *dst, const float *src1, const float *src2, const float *src3, const float *src4, float k1, float k2, float k3, float k4, size_t count);
}


UTEST_BEGIN("dsp", mix4)

    void call(const char *label, size_t align, mix4_t func1, mix4_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s for count=%d, mask=0x%x\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(dst1);
                FloatBuffer src1(count, align, mask & 0x02);
                FloatBuffer src2(count, align, mask & 0x04);
                FloatBuffer src3(count, align, mask & 0x08);

                func1(dst1, src1, src2, src3, 2.0f, 3.0f, 5.0f, 7.0f, count);
                func2(dst2, src1, src2, src3, 2.0f, 3.0f, 5.0f, 7.0f, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(src3.valid(), "Source buffer 3 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    src3.dump("src3");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, mix_dst4_t func1, mix_dst4_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0xfff)
        {
            for (size_t mask=0; mask <= 0x1f; ++mask)
            {
                printf("Testing %s for count=%d, mask=0x%x\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                FloatBuffer dst2(dst1);
                FloatBuffer src1(count, align, mask & 0x02);
                FloatBuffer src2(count, align, mask & 0x04);
                FloatBuffer src3(count, align, mask & 0x08);
                FloatBuffer src4(count, align, mask & 0x10);

                func1(dst1, src1, src2, src3, src4, 2.0f, 3.0f, 5.0f, 7.0f, count);
                func2(dst2, src1, src2, src3, src4, 2.0f, 3.0f, 5.0f, 7.0f, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(src3.valid(), "Source buffer 3 corrupted");
                UTEST_ASSERT_MSG(src4.valid(), "Source buffer 4 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    src3.dump("src3");
                    src4.dump("src4");
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

        // Do tests
        IF_ARCH_X86(CALL(generic::mix4, sse::mix4, 16));
        IF_ARCH_X86(CALL(generic::mix_copy4, sse::mix_copy4, 16));
        IF_ARCH_X86(CALL(generic::mix_add4, sse::mix_add4, 16));

        IF_ARCH_X86(CALL(generic::mix4, avx::mix4, 32));
        IF_ARCH_X86(CALL(generic::mix_copy4, avx::mix_copy4, 32));
        IF_ARCH_X86(CALL(generic::mix_add4, avx::mix_add4, 32));

        IF_ARCH_X86(CALL(generic::mix4, avx512::mix4, 64));
        IF_ARCH_X86(CALL(generic::mix_copy4, avx512::mix_copy4, 64));
        IF_ARCH_X86(CALL(generic::mix_add4, avx512::mix_add4, 64));

        IF_ARCH_ARM(CALL(generic::mix4, neon_d32::mix4, 16));
        IF_ARCH_ARM(CALL(generic::mix_copy4, neon_d32::mix_copy4, 16));
        IF_ARCH_ARM(CALL(generic::mix_add4, neon_d32::mix_add4, 16));

        IF_ARCH_AARCH64(CALL(generic::mix4, asimd::mix4, 16));
        IF_ARCH_AARCH64(CALL(generic::mix_copy4, asimd::mix_copy4, 16));
        IF_ARCH_AARCH64(CALL(generic::mix_add4, asimd::mix_add4, 16));
    }
UTEST_END;

