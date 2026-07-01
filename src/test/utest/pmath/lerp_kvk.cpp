/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 июл. 2026 г.
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
#include <lsp-plug.in/stdlib/math.h>

namespace lsp
{
    namespace generic
    {
        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
        }

        namespace avx
        {
            void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
//            void lerp_kvk_fma3(float *dst, float a, const float *b, float k, size_t count);
        }

//        namespace avx512
//        {
//            void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
//        }
    )

//    IF_ARCH_ARM(
//        namespace neon_d32
//        {
//            void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
//        }
//    )
//
//    IF_ARCH_AARCH64(
//        namespace asimd
//        {
//            void lerp_kvk(float *dst, float a, const float *b, float k, size_t count);
//        }
//    )

    typedef void (* lerp_kvk_t)(float *dst, float a, const float *b, float k, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", lerp_kvk)

    void call(const char *label, size_t align, lerp_kvk_t func1, lerp_kvk_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 17, 19, 24, 25, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer b(count, align, mask & 0x01);
                FloatBuffer dst1(count, align, mask & 0x02);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, 1.0f, b, 0.33f, count);
                func2(dst2, 1.0f, b, 0.33f, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-5f))
                {
                    b.dump("b   ");
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
        #define CALL(func, align) \
            call(#func, align, generic::lerp_kvk, func)

        IF_ARCH_X86(CALL(sse::lerp_kvk, 16));
        IF_ARCH_X86(CALL(avx::lerp_kvk, 32));
//        IF_ARCH_X86(CALL(avx::lerp_kvk_fma3, 32));
//        IF_ARCH_X86(CALL(avx512::lerp_kvk, 32));
//        IF_ARCH_ARM(CALL(neon_d32::lerp_kvk, 16));
//        IF_ARCH_ARM(CALL(asimd::lerp_kvk, 16));
    }
UTEST_END



