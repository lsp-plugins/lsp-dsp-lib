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
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

namespace lsp
{
    namespace generic
    {
        void powcv1(float *v, float c, size_t count);
        void powcv2(float *dst, const float *v, float c, size_t count);
        void powvc1(float *c, float v, size_t count);
        void powvc2(float *dst, const float *c, float v, size_t count);
        void powvx1(float *v, const float *x, size_t count);
        void powvx2(float *dst, const float *v, const float *x, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void powcv1(float *v, float c, size_t count);
            void powcv2(float *dst, const float *v, float c, size_t count);
            void powvc1(float *c, float v, size_t count);
            void powvc2(float *dst, const float *c, float v, size_t count);
            void powvx1(float *v, const float *x, size_t count);
            void powvx2(float *dst, const float *v, const float *x, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_powcv1(float *v, float c, size_t count);
            void x64_powcv2(float *dst, const float *v, float c, size_t count);
            void x64_powvc1(float *c, float v, size_t count);
            void x64_powvc2(float *dst, const float *c, float v, size_t count);
            void x64_powvx1(float *v, const float *x, size_t count);
            void x64_powvx2(float *dst, const float *v, const float *x, size_t count);

            void x64_powcv1_fma3(float *v, float c, size_t count);
            void x64_powcv2_fma3(float *dst, const float *v, float c, size_t count);
            void x64_powvc1_fma3(float *c, float v, size_t count);
            void x64_powvc2_fma3(float *dst, const float *c, float v, size_t count);
            void x64_powvx1_fma3(float *v, const float *x, size_t count);
            void x64_powvx2_fma3(float *dst, const float *v, const float *x, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void powcv1(float *v, float c, size_t count);
            void powcv2(float *dst, const float *v, float c, size_t count);
            void powvc1(float *c, float v, size_t count);
            void powvc2(float *dst, const float *c, float v, size_t count);
            void powvx1(float *v, const float *x, size_t count);
            void powvx2(float *dst, const float *v, const float *x, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void powcv1(float *v, float c, size_t count);
            void powcv2(float *dst, const float *v, float c, size_t count);
            void powvc1(float *c, float v, size_t count);
            void powvc2(float *dst, const float *c, float v, size_t count);
            void powvx1(float *v, const float *x, size_t count);
            void powvx2(float *dst, const float *v, const float *x, size_t count);
        }
    )

    typedef void (* powav1_t)(float *v, float c, size_t count);
    typedef void (* powav2_t)(float *dst, const float *v, float c, size_t count);
    typedef void (* powvx1_t)(float *v, const float *x, size_t count);
    typedef void (* powvx2_t)(float *dst, const float *v, const float *x, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", pow)

    void call(const char *label, size_t align, powav1_t func1, powav1_t func2)
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

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize(1.0f/6.0f, 6.0f);

                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                // Call functions
                func1(dst1, 3.0f, count);
                func2(dst2, 3.0f, count);

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

                // Call functions
                dst1.copy(src);
                dst2.copy(src);
                func1(dst1, 0.3f, count);
                func2(dst2, 0.3f, count);

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

    void call(const char *label, size_t align, powav2_t func1, powav2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize(1.0f/6.0f, 6.0f);
                FloatBuffer dst1(count, align, mask & 0x02);
                dst1.randomize(1.0f/6.0f, 6.0f);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, 3.0f, count);
                func2(dst2, src, 3.0f, count);

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

                // Call functions
                dst1.copy(src);
                dst2.copy(src);
                func1(dst1, src, 0.3f, count);
                func2(dst2, src, 0.3f, count);

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

    void call(const char *label, size_t align, powvx1_t func1, powvx1_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                src1.randomize(1.0f/6.0f, 6.0f);
                src2.randomize(1.0f/6.0f, 6.0f);
                FloatBuffer dst1(src1);
                FloatBuffer dst2(src1);

                // Call functions
                func1(dst1, src2, count);
                func2(dst2, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    void call(const char *label, size_t align, powvx2_t func1, powvx2_t func2)
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
                src1.randomize(1.0f/6.0f, 6.0f);
                src2.copy(src1);
                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(count, align, mask & 0x04);

                // Call functions
                func1(dst1, src1, src2, count);
                func2(dst2, src1, src2, count);

                UTEST_ASSERT_MSG(src1.valid(), "Source buffer 1 corrupted");
                UTEST_ASSERT_MSG(src2.valid(), "Source buffer 2 corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src1.dump("src1");
                    src2.dump("src2");
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

        IF_ARCH_X86(CALL(generic::powcv1, sse2::powcv1, 16));
        IF_ARCH_X86(CALL(generic::powcv2, sse2::powcv2, 16));
        IF_ARCH_X86(CALL(generic::powvc1, sse2::powvc1, 16));
        IF_ARCH_X86(CALL(generic::powvc2, sse2::powvc2, 16));
        IF_ARCH_X86(CALL(generic::powvx1, sse2::powvx1, 16));
        IF_ARCH_X86(CALL(generic::powvx2, sse2::powvx2, 16));

        IF_ARCH_X86_64(CALL(generic::powcv1, avx2::x64_powcv1, 32));
        IF_ARCH_X86_64(CALL(generic::powcv2, avx2::x64_powcv2, 32));
        IF_ARCH_X86_64(CALL(generic::powvc1, avx2::x64_powvc1, 32));
        IF_ARCH_X86_64(CALL(generic::powvc2, avx2::x64_powvc2, 32));
        IF_ARCH_X86_64(CALL(generic::powvx1, avx2::x64_powvx1, 32));
        IF_ARCH_X86_64(CALL(generic::powvx2, avx2::x64_powvx2, 32));

        IF_ARCH_X86_64(CALL(generic::powcv1, avx2::x64_powcv1_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::powcv2, avx2::x64_powcv2_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::powvc1, avx2::x64_powvc1_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::powvc2, avx2::x64_powvc2_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::powvx1, avx2::x64_powvx1_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::powvx2, avx2::x64_powvx2_fma3, 32));

        IF_ARCH_ARM(CALL(generic::powcv1, neon_d32::powcv1, 16));
        IF_ARCH_ARM(CALL(generic::powcv2, neon_d32::powcv2, 16));
        IF_ARCH_ARM(CALL(generic::powvc1, neon_d32::powvc1, 16));
        IF_ARCH_ARM(CALL(generic::powvc2, neon_d32::powvc2, 16));
        IF_ARCH_ARM(CALL(generic::powvx1, neon_d32::powvx1, 16));
        IF_ARCH_ARM(CALL(generic::powvx2, neon_d32::powvx2, 16));

        IF_ARCH_AARCH64(CALL(generic::powcv1, asimd::powcv1, 16));
        IF_ARCH_AARCH64(CALL(generic::powcv2, asimd::powcv2, 16));
        IF_ARCH_AARCH64(CALL(generic::powvc1, asimd::powvc1, 16));
        IF_ARCH_AARCH64(CALL(generic::powvc2, asimd::powvc2, 16));
        IF_ARCH_AARCH64(CALL(generic::powvx1, asimd::powvx1, 16));
        IF_ARCH_AARCH64(CALL(generic::powvx2, asimd::powvx2, 16));
    }
UTEST_END


