/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 авг. 2023 г.
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
        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
        }

        namespace avx512
        {
            void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pcomplex_r2c_sub2(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* pcomplex_r2c_op2_t)(float *dst, const float *src, size_t count);
}

UTEST_BEGIN("dsp.pcomplex", r2c_sub)
    void call(const char *text, size_t align, pcomplex_r2c_op2_t func1, pcomplex_r2c_op2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, /*0, 1, 2, 3, 4, 5, 6, 7, */ 8, 9, 10, 11, 12, 13, 14, 15, 16,
                32, 33, 37, 48, 49, 64, 65, 0x3f, 100, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", text, int(count), int(mask));

                FloatBuffer dst1(count*2, align, mask & 0x01);
                dst1.randomize_sign();
                FloatBuffer src(count, align, mask & 0x02);
                src.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src, count);
                func2(dst2, src, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_absolute(dst2, 1e-4))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", text);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, generic::pcomplex_r2c_sub2, func)

        IF_ARCH_X86(CALL(sse::pcomplex_r2c_sub2, 16));
        IF_ARCH_X86(CALL(avx::pcomplex_r2c_sub2, 32));
        IF_ARCH_X86(CALL(avx512::pcomplex_r2c_sub2, 64));
        IF_ARCH_ARM(CALL(neon_d32::pcomplex_r2c_sub2, 16));
        IF_ARCH_AARCH64(CALL(asimd::pcomplex_r2c_sub2, 16));
    }

UTEST_END;



