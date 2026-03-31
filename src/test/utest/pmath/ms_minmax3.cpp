/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 мар. 2026 г.
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
        void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
        void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
        void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
        void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
        void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
        void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
        }

        namespace avx
        {
            void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
        }

        namespace avx512
        {
            void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void ms_pmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmin3(float *dst, const float *a, const float *b, size_t count);
            void ms_psmax3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamin3(float *dst, const float *a, const float *b, size_t count);
            void ms_pamax3(float *dst, const float *a, const float *b, size_t count);
        }
    )
}

typedef void (* ms_poper3_t)(float *dst, const float *a, const float *b, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", ms_minmax3)

    void call(const char *label, size_t align, ms_poper3_t func1, ms_poper3_t func2)
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

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);
                a.randomize_sign();
                b.randomize_sign();

                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, a, b, count);
                func2(dst2, a, b, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    a.dump("srca");
                    b.dump("srcb");
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

        IF_ARCH_X86(CALL(generic::ms_pmin3, sse::ms_pmin3, 16));
        IF_ARCH_X86(CALL(generic::ms_pmax3, sse::ms_pmax3, 16));
        IF_ARCH_X86(CALL(generic::ms_psmin3, sse::ms_psmin3, 16));
        IF_ARCH_X86(CALL(generic::ms_psmax3, sse::ms_psmax3, 16));
        IF_ARCH_X86(CALL(generic::ms_pamin3, sse::ms_pamin3, 16));
        IF_ARCH_X86(CALL(generic::ms_pamax3, sse::ms_pamax3, 16));

        IF_ARCH_X86(CALL(generic::ms_pmin3, avx::ms_pmin3, 32));
        IF_ARCH_X86(CALL(generic::ms_pmax3, avx::ms_pmax3, 32));
        IF_ARCH_X86(CALL(generic::ms_psmin3, avx::ms_psmin3, 32));
        IF_ARCH_X86(CALL(generic::ms_psmax3, avx::ms_psmax3, 32));
        IF_ARCH_X86(CALL(generic::ms_pamin3, avx::ms_pamin3, 32));
        IF_ARCH_X86(CALL(generic::ms_pamax3, avx::ms_pamax3, 32));

        IF_ARCH_X86(CALL(generic::ms_pmin3, avx512::ms_pmin3, 64));
        IF_ARCH_X86(CALL(generic::ms_pmax3, avx512::ms_pmax3, 64));
        IF_ARCH_X86(CALL(generic::ms_psmin3, avx512::ms_psmin3, 64));
        IF_ARCH_X86(CALL(generic::ms_psmax3, avx512::ms_psmax3, 64));
        IF_ARCH_X86(CALL(generic::ms_pamin3, avx512::ms_pamin3, 64));
        IF_ARCH_X86(CALL(generic::ms_pamax3, avx512::ms_pamax3, 64));

        IF_ARCH_ARM(CALL(generic::ms_pmin3, neon_d32::ms_pmin3, 16));
        IF_ARCH_ARM(CALL(generic::ms_pmax3, neon_d32::ms_pmax3, 16));
        IF_ARCH_ARM(CALL(generic::ms_psmin3, neon_d32::ms_psmin3, 16));
        IF_ARCH_ARM(CALL(generic::ms_psmax3, neon_d32::ms_psmax3, 16));
        IF_ARCH_ARM(CALL(generic::ms_pamin3, neon_d32::ms_pamin3, 16));
        IF_ARCH_ARM(CALL(generic::ms_pamax3, neon_d32::ms_pamax3, 16));

        IF_ARCH_AARCH64(CALL(generic::ms_pmin3, asimd::ms_pmin3, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_pmax3, asimd::ms_pmax3, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_psmin3, asimd::ms_psmin3, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_psmax3, asimd::ms_psmax3, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_pamin3, asimd::ms_pamin3, 16));
        IF_ARCH_AARCH64(CALL(generic::ms_pamax3, asimd::ms_pamax3, 16));
    }
UTEST_END






