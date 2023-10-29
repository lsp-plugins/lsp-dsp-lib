/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 нояб. 2022 г.
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
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/stdlib/math.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

namespace lsp
{
    namespace generic
    {
        static void normalize1_test(float *dst, size_t count)
        {
            // Find maximum
            float max = 0.0f;
            for (size_t i=0; i<count; ++i)
            {
                float v = fabs(dst[i]);
                if (max < v)
                    max = v;
            }

            if (max > 0.0f)
            {
                // Scale data
                max = 1.0f / max;
                for (size_t i=0; i<count; ++i)
                    dst[i] = dst[i] * max;
            }
        }

        void normalize1(float *dst, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void normalize1(float *dst, size_t count);
        }

        namespace avx
        {
            void normalize1(float *dst, size_t count);
        }

        namespace avx512
        {
            void normalize1(float *dst, size_t count);
        }
    )

}

typedef void (* normalize1_t)(float *dst, size_t count);

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", normalize1)

    void call(const char *label, size_t align, normalize1_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x01; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer dst1(count, align, mask & 0x01);
                dst1.randomize_sign();
                FloatBuffer dst2(dst1);

                // Call functions (case 1)
                generic::normalize1_test(dst1, count);
                func(dst2, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-4))
                {
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        IF_ARCH_X86(CALL(generic::normalize1, 16));
        IF_ARCH_X86(CALL(sse::normalize1, 16));
        IF_ARCH_X86(CALL(avx::normalize1, 32));
        IF_ARCH_X86(CALL(avx512::normalize1, 64));
    }
UTEST_END



