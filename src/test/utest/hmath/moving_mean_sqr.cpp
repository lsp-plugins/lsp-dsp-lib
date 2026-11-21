/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 нояб. 2026 г.
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
    #define TOLERANCE 1e-3
#endif

namespace lsp
{
    namespace generic
    {
        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count);
        }

        namespace avx
        {
            float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count);
            float moving_mean_sqr_fma3(float *dst, const float *a, const float *b, float value, float avg, size_t count);
        }

        namespace avx2
        {
            float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count);
            float moving_mean_sqr_fma3(float *dst, const float *a, const float *b, float value, float avg, size_t count);
        }
    )

    typedef float (* moving_mean_sqr_t)(float *dst, const float *a, const float *b, float value, float avg, size_t count);
}


UTEST_BEGIN("dsp.hmath", moving_mean_sqr)

    void call(const char *label, size_t align, moving_mean_sqr_t func1, moving_mean_sqr_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 32, 63, 64, 65, 100, 768, 999, 0x1fff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer a(count, align, mask & 0x01);
                FloatBuffer b(count, align, mask & 0x02);

                FloatBuffer dst1(count, align, mask & 0x04);
                FloatBuffer dst2(dst1);

                a.randomize_sign();
                b.randomize_sign();

                // Call functions
//                for (size_t i=0; i<count; ++i)
//                {
//                    a[i]        = -0.1f * i;
//                    b[i]        = 0.2f * i;
//                }

                const float xa = func1(dst1, a, b, 0.5f, 0.1f, count);
                const float xb = func2(dst2, a, b, 0.5f, 0.1f, count);

                UTEST_ASSERT_MSG(a.valid(), "Source buffer A corrupted");
                UTEST_ASSERT_MSG(b.valid(), "Source buffer B corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!float_equals_relative(xa, xb, TOLERANCE))
                {
                    a.dump("A   ");
                    b.dump("B   ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("%s: Result differs, xa=%f, xb=%f", label, xa, xb);
                }
                else if (!dst1.equals_adaptive(dst2, TOLERANCE))
                {
                    a.dump("A   ");
                    b.dump("B   ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    const size_t last_diff = dst1.last_diff();
                    UTEST_FAIL_MSG("%s: Result differs, xa=%f, xb=%f, index=%d, dst1[index]=%f, dst2[index]=%f",
                        label, xa, xb, int(last_diff), dst1[last_diff], dst2[last_diff]);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, generic::moving_mean_sqr, func);

        IF_ARCH_X86(CALL(sse::moving_mean_sqr, 16));
        IF_ARCH_X86(CALL(avx::moving_mean_sqr, 32));
        IF_ARCH_X86(CALL(avx::moving_mean_sqr_fma3, 32));
        IF_ARCH_X86(CALL(avx2::moving_mean_sqr, 32));
        IF_ARCH_X86(CALL(avx2::moving_mean_sqr_fma3, 32));
    }
UTEST_END


