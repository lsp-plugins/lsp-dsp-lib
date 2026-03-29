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
        void lr_pmin2(float *dst, const float *src, size_t count);
        void lr_pmax2(float *dst, const float *src, size_t count);
        void lr_psmin2(float *dst, const float *src, size_t count);
        void lr_psmax2(float *dst, const float *src, size_t count);
        void lr_pamin2(float *dst, const float *src, size_t count);
        void lr_pamax2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void lr_pmin2(float *dst, const float *src, size_t count);
            void lr_pmax2(float *dst, const float *src, size_t count);
            void lr_psmin2(float *dst, const float *src, size_t count);
            void lr_psmax2(float *dst, const float *src, size_t count);
            void lr_pamin2(float *dst, const float *src, size_t count);
            void lr_pamax2(float *dst, const float *src, size_t count);
        }
    )
}

typedef void (* lr_poper2_t)(float *dst, const float *src, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", lr_minmax2)

    void call(const char *label, size_t align, lr_poper2_t func1, lr_poper2_t func2)
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
                src.randomize_sign();

                FloatBuffer dst(count, align, mask & 0x02);
                dst.randomize_sign();
                FloatBuffer dst1(dst);
                FloatBuffer dst2(dst);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst.dump("dst ");
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

        IF_ARCH_X86(CALL(generic::lr_pmin2, sse::lr_pmin2, 16));
        IF_ARCH_X86(CALL(generic::lr_pmax2, sse::lr_pmax2, 16));
        IF_ARCH_X86(CALL(generic::lr_psmin2, sse::lr_psmin2, 16));
        IF_ARCH_X86(CALL(generic::lr_psmax2, sse::lr_psmax2, 16));
        IF_ARCH_X86(CALL(generic::lr_pamin2, sse::lr_pamin2, 16));
        IF_ARCH_X86(CALL(generic::lr_pamax2, sse::lr_pamax2, 16));

        IF_ARCH_X86(CALL(generic::lr_pmin2, avx::lr_pmin2, 32));
        IF_ARCH_X86(CALL(generic::lr_pmax2, avx::lr_pmax2, 32));
        IF_ARCH_X86(CALL(generic::lr_psmin2, avx::lr_psmin2, 32));
        IF_ARCH_X86(CALL(generic::lr_psmax2, avx::lr_psmax2, 32));
        IF_ARCH_X86(CALL(generic::lr_pamin2, avx::lr_pamin2, 32));
        IF_ARCH_X86(CALL(generic::lr_pamax2, avx::lr_pamax2, 32));

        IF_ARCH_X86(CALL(generic::lr_pmin2, avx512::lr_pmin2, 64));
        IF_ARCH_X86(CALL(generic::lr_pmax2, avx512::lr_pmax2, 64));
        IF_ARCH_X86(CALL(generic::lr_psmin2, avx512::lr_psmin2, 64));
        IF_ARCH_X86(CALL(generic::lr_psmax2, avx512::lr_psmax2, 64));
        IF_ARCH_X86(CALL(generic::lr_pamin2, avx512::lr_pamin2, 64));
        IF_ARCH_X86(CALL(generic::lr_pamax2, avx512::lr_pamax2, 64));

        IF_ARCH_ARM(CALL(generic::lr_pmin2, neon_d32::lr_pmin2, 16));
        IF_ARCH_ARM(CALL(generic::lr_pmax2, neon_d32::lr_pmax2, 16));
        IF_ARCH_ARM(CALL(generic::lr_psmin2, neon_d32::lr_psmin2, 16));
        IF_ARCH_ARM(CALL(generic::lr_psmax2, neon_d32::lr_psmax2, 16));
        IF_ARCH_ARM(CALL(generic::lr_pamin2, neon_d32::lr_pamin2, 16));
        IF_ARCH_ARM(CALL(generic::lr_pamax2, neon_d32::lr_pamax2, 16));

        IF_ARCH_AARCH64(CALL(generic::lr_pmin2, asimd::lr_pmin2, 16));
        IF_ARCH_AARCH64(CALL(generic::lr_pmax2, asimd::lr_pmax2, 16));
        IF_ARCH_AARCH64(CALL(generic::lr_psmin2, asimd::lr_psmin2, 16));
        IF_ARCH_AARCH64(CALL(generic::lr_psmax2, asimd::lr_psmax2, 16));
        IF_ARCH_AARCH64(CALL(generic::lr_pamin2, asimd::lr_pamin2, 16));
        IF_ARCH_AARCH64(CALL(generic::lr_pamax2, asimd::lr_pamax2, 16));
    }
UTEST_END




