/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2025 г.
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
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
        }

        namespace avx
        {
            void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
        }

        namespace avx512
        {
            void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void clamp_vv1(float *dst, const float *min, const float *max, size_t count);
        }
    )

    typedef void (* clamp_vv1_t)(float *dst, const float *min, const float *max, size_t count);
}

//-----------------------------------------------------------------------------
// Unit test
UTEST_BEGIN("dsp.pmath", clamp_vv1)

    void call(const char *label, size_t align, clamp_vv1_t func1, clamp_vv1_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                16, 17, 19, 24, 25, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x07; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer min(count, align, mask & 0x02);
                FloatBuffer max(count, align, mask & 0x04);
                src.randomize(0.0f, 1.0f);
                min.randomize(0.2f, 0.25f);
                max.randomize(0.75f, 0.8f);

                FloatBuffer dst1(src);
                FloatBuffer dst2(src);

                // Call functions
                func1(dst1, min, max, count);
                func2(dst2, min, max, count);

                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_adaptive(dst2, 1e-5f))
                {
                    src.dump("src ");
                    dst1.dump("dst1");
                    dst2.dump("dst2");
                    const size_t index = dst1.last_diff();
                    UTEST_FAIL_MSG("Output of functions for test '%s' differs at index %d: src=%f, dst1=%f, dst2=%f",
                        label, int(index), src[index], dst1[index], dst2[index]);
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        IF_ARCH_X86(CALL(generic::clamp_vv1, sse::clamp_vv1, 16));
        IF_ARCH_X86(CALL(generic::clamp_vv1, avx::clamp_vv1, 32));
        IF_ARCH_X86(CALL(generic::clamp_vv1, avx512::clamp_vv1, 64));
        IF_ARCH_ARM(CALL(generic::clamp_vv1, neon_d32::clamp_vv1, 16));
        IF_ARCH_AARCH64(CALL(generic::clamp_vv1, asimd::clamp_vv1, 16));
    }
UTEST_END



