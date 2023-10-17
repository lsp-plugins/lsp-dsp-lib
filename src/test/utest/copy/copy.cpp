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
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

namespace lsp
{
    namespace generic
    {
        void copy(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace x86
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace sse
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace sse3
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void copy(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void copy(float *dst, const float *src, size_t count);
        }
    )
}

typedef void (* copy_t)(float *dst, const float *src, size_t count);

UTEST_BEGIN("dsp.copy", copy)
    void call(const char *label, size_t align, copy_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x100, 0x1ff, 999, 0xfff, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                printf("Testing %s, of %d samples, mask=%x\n", label, int(count), int(mask));
                generic::copy(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }

                printf("Testing %s, of %d samples from %d offset, mask=%x\n", label, int(count >> 1), int(count >> 2), int(mask));
                float *dptr1 = dst1, *dptr2 = dst2;
                generic::copy(dptr1, &dptr1[count >> 2], count >> 1);
                func(dptr2, &dptr2[count >> 2], count >> 1);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                if (!dst1.equals_absolute(dst2))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        IF_ARCH_X86(call("x86::copy", 16, x86::copy));
        IF_ARCH_X86(call("sse::copy", 16, sse::copy));
        IF_ARCH_X86(call("sse3::copy", 16, sse3::copy));
        IF_ARCH_X86(call("avx::copy", 32, avx::copy));
        IF_ARCH_X86(call("avx512::copy", 64, avx512::copy));
        IF_ARCH_ARM(call("neon_d32::copy", 16, neon_d32::copy));
        IF_ARCH_AARCH64(call("asimd::copy", 16, asimd::copy));
    }

UTEST_END;





