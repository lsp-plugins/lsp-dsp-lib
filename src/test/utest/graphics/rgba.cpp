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
#include <lsp-plug.in/test-fw/ByteBuffer.h>

namespace lsp
{
    namespace generic
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count);
    }

    IF_ARCH_X86(
        namespace x86
        {
            void rgba32_to_bgra32(void *dst, const void *src, size_t count);
        }

        namespace sse2
        {
            void rgba32_to_bgra32(void *dst, const void *src, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace sse3
        {
            void x64_rgba32_to_bgra32(void *dst, const void *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void rgba32_to_bgra32(void *dst, const void *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void rgba32_to_bgra32(void *dst, const void *src, size_t count);
        }
    )

    typedef void (* rgba32_to_bgra32_t)(void *dst, const void *src, size_t count);
}

UTEST_BEGIN("dsp.graphics", rgba)
    void call(const char *label, size_t align, rgba32_to_bgra32_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1024, 0x1fff)
//        size_t count = 4;
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                ByteBuffer src(count*4, align, mask & 0x01);
                ByteBuffer dst1(count*4, align, mask & 0x02);
                ByteBuffer dst2(dst1);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Call functions
                generic::rgba32_to_bgra32(dst1, src, count);
                func(dst2, src, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals(dst2))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Result of functions differs");
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(x86::rgba32_to_bgra32, 16));
        IF_ARCH_X86(CALL(sse2::rgba32_to_bgra32, 16));

        IF_ARCH_X86_64(CALL(sse3::x64_rgba32_to_bgra32, 16));

        IF_ARCH_ARM(CALL(neon_d32::rgba32_to_bgra32, 16));
        IF_ARCH_AARCH64(CALL(asimd::rgba32_to_bgra32, 16));
    }

UTEST_END;

