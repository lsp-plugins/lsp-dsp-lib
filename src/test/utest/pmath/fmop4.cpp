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

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace sse2
        {
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace avx
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);

            void    fmadd4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4_fma3(float *dst, const float *a, const float *b, const float *c, size_t count);
        }

        namespace avx512
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    fmadd4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrsub4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmul4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrdiv4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
            void    fmrmod4(float *dst, const float *a, const float *b, const float *c, size_t count);
        }
    )
}

typedef void (* fmop4_t)(float *dst, const float *a, const float *b, const float *c, size_t count);

UTEST_BEGIN("dsp.pmath", fmop4)

    void call(const char *label, size_t align, fmop4_t func1, fmop4_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);
                FloatBuffer c(count, align, mask & 0x04);
                FloatBuffer dst1(count, align, mask & 0x08);
                FloatBuffer dst2(count, align, mask & 0x08);

                // Call functions
                a.randomize_sign();
                b.randomize_sign();
                func1(dst1, a, b, c, count);
                func2(dst2, a, b, c, count);

                UTEST_ASSERT_MSG(a.valid(), "Buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Buffer B corrupted");
                UTEST_ASSERT_MSG(c.valid(), "Buffer C corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    a.dump("a   ");
                    b.dump("b   ");
                    c.dump("c   ");
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

        IF_ARCH_X86(CALL(generic::fmadd4, sse::fmadd4, 16));
        IF_ARCH_X86(CALL(generic::fmsub4, sse::fmsub4, 16));
        IF_ARCH_X86(CALL(generic::fmrsub4, sse::fmrsub4, 16));
        IF_ARCH_X86(CALL(generic::fmmul4, sse::fmmul4, 16));
        IF_ARCH_X86(CALL(generic::fmdiv4, sse::fmdiv4, 16));
        IF_ARCH_X86(CALL(generic::fmrdiv4, sse::fmrdiv4, 16));
        IF_ARCH_X86(CALL(generic::fmmod4, sse2::fmmod4, 16));
        IF_ARCH_X86(CALL(generic::fmrmod4, sse2::fmrmod4, 16));

        IF_ARCH_X86(CALL(generic::fmadd4, avx::fmadd4, 32));
        IF_ARCH_X86(CALL(generic::fmsub4, avx::fmsub4, 32));
        IF_ARCH_X86(CALL(generic::fmrsub4, avx::fmrsub4, 32));
        IF_ARCH_X86(CALL(generic::fmmul4, avx::fmmul4, 32));
        IF_ARCH_X86(CALL(generic::fmdiv4, avx::fmdiv4, 32));
        IF_ARCH_X86(CALL(generic::fmrdiv4, avx::fmrdiv4, 32));
        IF_ARCH_X86(CALL(generic::fmmod4, avx::fmmod4, 32));
        IF_ARCH_X86(CALL(generic::fmrmod4, avx::fmrmod4, 32));
        IF_ARCH_X86(CALL(generic::fmadd4, avx::fmadd4_fma3, 32));
        IF_ARCH_X86(CALL(generic::fmsub4, avx::fmsub4_fma3, 32));
        IF_ARCH_X86(CALL(generic::fmrsub4, avx::fmrsub4_fma3, 32));
        IF_ARCH_X86(CALL(generic::fmmod4, avx::fmmod4_fma3, 32));
        IF_ARCH_X86(CALL(generic::fmrmod4, avx::fmrmod4_fma3, 32));

        IF_ARCH_X86(CALL(generic::fmadd4, avx512::fmadd4, 64));
        IF_ARCH_X86(CALL(generic::fmsub4, avx512::fmsub4, 64));
        IF_ARCH_X86(CALL(generic::fmrsub4, avx512::fmrsub4, 64));
        IF_ARCH_X86(CALL(generic::fmmul4, avx512::fmmul4, 64));
        IF_ARCH_X86(CALL(generic::fmdiv4, avx512::fmdiv4, 64));
        IF_ARCH_X86(CALL(generic::fmrdiv4, avx512::fmrdiv4, 64));
        IF_ARCH_X86(CALL(generic::fmmod4, avx512::fmmod4, 64));
        IF_ARCH_X86(CALL(generic::fmrmod4, avx512::fmrmod4, 64));

        IF_ARCH_ARM(CALL(generic::fmadd4, neon_d32::fmadd4, 16));
        IF_ARCH_ARM(CALL(generic::fmsub4, neon_d32::fmsub4, 16));
        IF_ARCH_ARM(CALL(generic::fmrsub4, neon_d32::fmrsub4, 16));
        IF_ARCH_ARM(CALL(generic::fmmul4, neon_d32::fmmul4, 16));
        IF_ARCH_ARM(CALL(generic::fmdiv4, neon_d32::fmdiv4, 16));
        IF_ARCH_ARM(CALL(generic::fmrdiv4, neon_d32::fmrdiv4, 16));
        IF_ARCH_ARM(CALL(generic::fmmod4, neon_d32::fmmod4, 16));
        IF_ARCH_ARM(CALL(generic::fmrmod4, neon_d32::fmrmod4, 16));

        IF_ARCH_AARCH64(CALL(generic::fmadd4, asimd::fmadd4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmsub4, asimd::fmsub4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmrsub4, asimd::fmrsub4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmmul4, asimd::fmmul4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmdiv4, asimd::fmdiv4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmrdiv4, asimd::fmrdiv4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmmod4, asimd::fmmod4, 16));
        IF_ARCH_AARCH64(CALL(generic::fmrmod4, asimd::fmrmod4, 16));
    }
UTEST_END






