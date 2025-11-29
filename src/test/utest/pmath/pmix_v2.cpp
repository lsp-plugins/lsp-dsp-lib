/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 нояб. 2025 г.
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
        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
        }

        namespace avx
        {
            void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
            void pmix_v2_fma3(float *dst, const float *src1, const float *src2, const float *k, size_t count);
        }

        namespace avx512
        {
            void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count);
        }
    )

    typedef void (* pmix_v2_t)(float *dst, const float *src1, const float *src2, const float *k, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", pmix_v2)

    void call(const char *label, size_t align, pmix_v2_t func1, pmix_v2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 17, 19, 24, 25, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x0f; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src1(count, align, mask & 0x01);
                FloatBuffer src2(count, align, mask & 0x02);
                FloatBuffer k(count, align, mask & 0x04);
                src1.randomize(-1.0f, 1.0f);
                src2.randomize(-1.0f, 1.0f);
                k.randomize(0.25f, 0.75f);

                FloatBuffer dst1(count, align, mask & 0x08);
                FloatBuffer dst2(dst1);

                // Call functions
                func1(dst1, src1, src2, k, count);
                func2(dst2, src1, src2, k, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-5f))
                {
                    src1.dump("src1");
                    src2.dump("src2");
                    k.dump("k   ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    const size_t index = dst1.last_diff();
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs at index %d: src1=%f, src2=%f, dst1=%f, dst2=%f, k=%f",
                        label, int(index), src1[index], src2[index], dst1[index], dst2[index], k[index]);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        IF_ARCH_X86(CALL(generic::pmix_v2, sse::pmix_v2, 16));
        IF_ARCH_X86(CALL(generic::pmix_v2, avx::pmix_v2, 32));
        IF_ARCH_X86(CALL(generic::pmix_v2, avx::pmix_v2_fma3, 32));
        IF_ARCH_X86(CALL(generic::pmix_v2, avx512::pmix_v2, 64));
        IF_ARCH_ARM(CALL(generic::pmix_v2, neon_d32::pmix_v2, 16));
        IF_ARCH_AARCH64(CALL(generic::pmix_v2, asimd::pmix_v2, 16));
    }
UTEST_END



