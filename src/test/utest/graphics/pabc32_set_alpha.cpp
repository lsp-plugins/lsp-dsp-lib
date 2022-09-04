/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 июл. 2022 г.
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
#include <lsp-plug.in/test-fw/ByteBuffer.h>

namespace lsp
{
    namespace generic
    {
        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
    }

    IF_ARCH_X86(
        namespace x86
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }

        namespace sse2
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }

        namespace avx
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }

        namespace avx2
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count);
        }
    )

    typedef void (* pabc32_set_alpha_t)(void *dst, const void *src, uint8_t alpha, size_t count);
}

namespace
{
    void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
    {
        uint8_t *dp             = reinterpret_cast<uint8_t *>(dst);
        const uint8_t *sp       = reinterpret_cast<const uint8_t *>(src);

        while (count--)
        {
            dp[0]   = sp[0];
            dp[1]   = sp[1];
            dp[2]   = sp[2];
            dp[3]   = alpha;

            sp += 4;
            dp += 4;
        }
    }
}

UTEST_BEGIN("dsp.graphics", pabc32_set_alpha)
    void call(const char *label, size_t align, pabc32_set_alpha_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

//        size_t count = 4;
        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                24, 25, 30, 31, 32, 46, 47, 48, 49, 64, 65, 100, 768, 999, 1024, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                for (size_t alpha = 0x3; alpha <= 0xff; alpha <<= 2)
                {
                    ByteBuffer src(count*4, align, mask & 0x01);
                    ByteBuffer dst1(count*4, align, mask & 0x02);
                    ByteBuffer dst2(count*4, align, mask & 0x02);

                    uint8_t *dst = src.data();
                    for (size_t i=0; i<count*4; ++i)
                        dst[i]  = rand() * 0xff;

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Call functions
                    ::pabc32_set_alpha(dst1.data(), src, alpha, count);
                    func(dst2.data(), src, alpha, count);

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals(dst2))
                    {
                        src.dump ("src ");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        UTEST_FAIL_MSG("Result of functions differs");
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        CALL(generic::pabc32_set_alpha, 16);
        IF_ARCH_X86(CALL(x86::pabc32_set_alpha, 16));
        IF_ARCH_X86(CALL(sse2::pabc32_set_alpha, 16));
        IF_ARCH_X86(CALL(avx::pabc32_set_alpha, 16));
        IF_ARCH_X86(CALL(avx2::pabc32_set_alpha, 32));
        IF_ARCH_ARM(CALL(neon_d32::pabc32_set_alpha, 16));
        IF_ARCH_AARCH64(CALL(asimd::pabc32_set_alpha, 16));
    }

UTEST_END;



