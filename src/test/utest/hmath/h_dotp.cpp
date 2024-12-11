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
#include <lsp-plug.in/test-fw/helpers.h>

#ifndef TOLERANCE
    #define TOLERANCE 1e-4
#endif

namespace lsp
{
    namespace generic
    {
        float h_dotp(const float *a, const float *b, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }

        namespace avx
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }

        namespace avx512
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            float h_dotp(const float *a, const float *b, size_t count);
        }
    )

    typedef float (* h_dotp_t)(const float *a, const float *b, size_t count);
}

UTEST_BEGIN("dsp.hmath", h_dotp)

    void call(const char *label, size_t align, h_dotp_t func1, h_dotp_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);

                a.randomize_sign();
                b.randomize_sign();

                // Call functions
                float xa = func1(a, b, count);
                float xb = func2(a, b, count);

                UTEST_ASSERT_MSG(a.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Source buffer B corrupted");

                // Compare buffers
                if (!float_equals_adaptive(xa, xb, TOLERANCE))
                {
                    a.dump("A");
                    b.dump("B");
                    UTEST_FAIL_MSG("%s: Result of function 1 (%f) differs result of function 2 (%f)", label, xa, xb);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func);

        IF_ARCH_X86(CALL(generic::h_dotp, sse::h_dotp, 16));
        IF_ARCH_X86(CALL(generic::h_dotp, avx::h_dotp, 32));
        IF_ARCH_X86(CALL(generic::h_dotp, avx512::h_dotp, 64));
        IF_ARCH_ARM(CALL(generic::h_dotp, neon_d32::h_dotp, 16));
        IF_ARCH_AARCH64(CALL(generic::h_dotp, asimd::h_dotp, 16));
    }
UTEST_END
