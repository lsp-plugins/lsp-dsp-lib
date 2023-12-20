/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 19 окт. 2023 г.
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
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define MIN_RANK 8
#define MAX_RANK 16

namespace lsp
{
    namespace generic
    {
        void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }

        namespace avx2
        {
            void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
            void uexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }

        namespace avx512
        {
            void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }
    )

    IF_ARCH_X86_64(
        namespace avx2
        {
            void x64_uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
            void x64_uexpander_x1_gain_fma3(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void uexpander_x1_gain(float *dst, const float *src, const dsp::expander_knee_t *c, size_t count);
        }
    )
}

typedef void (* expander_x1_func_t)(float *dst, const float *src, const lsp::dsp::expander_knee_t *c, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.dynamics", uexpander_x1_gain)

    void call(const char *label, size_t align, expander_x1_func_t func1, expander_x1_func_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        dsp::expander_knee_t exp;
        exp = {
            0.0316223241f,
            0.125894368f,
            63.0957451f,
            { 0.361904532f, 2.49995828f, 4.31729317f },
            { 1.0f, 2.76310205f }
        };

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on expander, input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst(count, align, mask & 0x02);

                src.randomize_0to1();
                dst.randomize_sign();
                {
                    FloatBuffer dst1(dst);
                    FloatBuffer dst2(dst);

                    // Call functions
                    func1(dst1, src, &exp, count);
                    func2(dst2, src, &exp, count);

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(dst.valid(), "Destination buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals_absolute(dst2, 2e-4))
                    {
                        src.dump("src ");
                        dst.dump("dst ");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                    }
                }

                // Check for special values
                for (size_t i=0; i<count; ++i)
                {
                    if ((i % 3) == 0)
                        dst[i] = 1e-20f;
                    else if ((i % 7) == 0)
                        dst[i] = 1e+20f;
                }

                {
                    FloatBuffer dst1(dst);
                    FloatBuffer dst2(dst);

                    // Call functions
                    func1(dst1, src, &exp, count);
                    func2(dst2, src, &exp, count);

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(dst.valid(), "Destination buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals_absolute(dst2, 2e-4))
                    {
                        src.dump("src ");
                        dst.dump("dst ");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        printf("index=%d, %.6f vs %.6f\n", dst1.last_diff(), dst1.get_diff(), dst2.get_diff());
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func);

        IF_ARCH_X86(CALL(generic::uexpander_x1_gain, sse2::uexpander_x1_gain, 16));
        IF_ARCH_X86(CALL(generic::uexpander_x1_gain, avx2::uexpander_x1_gain, 32));
        IF_ARCH_X86(CALL(generic::uexpander_x1_gain, avx2::uexpander_x1_gain_fma3, 32));
        IF_ARCH_X86_64(CALL(generic::uexpander_x1_gain, avx2::x64_uexpander_x1_gain, 32));
        IF_ARCH_X86_64(CALL(generic::uexpander_x1_gain, avx2::x64_uexpander_x1_gain_fma3, 32));
        IF_ARCH_X86(CALL(generic::uexpander_x1_gain, avx512::uexpander_x1_gain, 64));

        IF_ARCH_ARM(CALL(generic::uexpander_x1_gain, neon_d32::uexpander_x1_gain, 16));

        IF_ARCH_AARCH64(CALL(generic::uexpander_x1_gain, asimd::uexpander_x1_gain, 16));
    }
UTEST_END



