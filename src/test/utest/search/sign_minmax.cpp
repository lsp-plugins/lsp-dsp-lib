/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 нояб. 2024 г.
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

namespace lsp
{
    namespace generic
    {
        void    sign_minmax(const float *src, size_t count, float *min, float *max);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void    sign_minmax(const float *src, size_t count, float *min, float *max);
        }

        namespace avx
        {
            void    sign_minmax(const float *src, size_t count, float *min, float *max);
        }

        namespace avx512
        {
            void    sign_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void    sign_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void    sign_minmax(const float *src, size_t count, float *min, float *max);
        }
    )

    typedef void  (* minmax_t)(const float *src, size_t count, float *min, float *max);
}

UTEST_BEGIN("dsp.search", sign_minmax)

    void call(const char *label, size_t align, minmax_t func1, minmax_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 768, 999, 1023, 1024, 1025)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                src.randomize_sign();

                // Call functions
                float min1, min2, max1, max2;
                func1(src, count, &min1, &max1);
                func2(src, count, &min2, &max2);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");

                // Compare buffers
                if ((!float_equals_relative(min1, min2, 1e-5)) && (!float_equals_relative(max1, max2, 1e-5)))
                {
                    src.dump("src1");
                    UTEST_FAIL_MSG("Result of function 1 (%f, %f) differs result of function 2 (%f, %f)", min1, max1, min2, max2);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, generic::sign_minmax, func)

        IF_ARCH_X86(CALL(sse::sign_minmax, 16));
        IF_ARCH_X86(CALL(avx::sign_minmax, 32));
        IF_ARCH_X86(CALL(avx512::sign_minmax, 64));
        IF_ARCH_ARM(CALL(neon_d32::sign_minmax, 16));
        IF_ARCH_AARCH64(CALL(asimd::sign_minmax, 16));
    }
UTEST_END


