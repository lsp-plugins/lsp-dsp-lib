/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 окт. 2023 г.
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
        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
    }

    IF_ARCH_X86(
        namespace sse2
        {
            void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }

//        namespace avx2
//        {
//            void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
//        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count);
        }
    )
}

typedef void (* compressor_x2_func_t)(float *dst, const float *src, const lsp::dsp::compressor_x2_t *c, size_t count);

//-----------------------------------------------------------------------------
// Unit test for simple operations
UTEST_BEGIN("dsp.dynamics", compressor_x2_gain)

    void call(const char *label, size_t align, compressor_x2_func_t func1, compressor_x2_func_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        dsp::compressor_x2_t comp;
        comp.k[0] = {
            0.125891402,
            0.501197219,
            1.0f,
            { -0.271428347, -1.12498128, -1.16566944 },
            {-0.75, -1.03615928}};
        comp.k[1] = {
            0.0f,
            0.0f,
            1.0f,
            { 0.0f, 0.0f, 0.0f },
            { 0.0f, 0.0f }};

        UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
                32, 64, 65, 100, 999, 0xfff)
        {
            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                printf("Testing %s on input buffer of %d numbers, mask=0x%x...\n", label, int(count), int(mask));

                FloatBuffer src(count, align, mask & 0x01);
                FloatBuffer dst(count, align, mask & 0x02);

                src.randomize_0to1();
                dst.randomize_sign();
                FloatBuffer dst1(dst);
                FloatBuffer dst2(dst);

                // Call functions
                func1(dst1, src, &comp, count);
                func2(dst2, src, &comp, count);

                UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                UTEST_ASSERT_MSG(dst.valid(), "Destination buffer corrupted");
                UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                // Compare buffers
                if (!dst1.equals_relative(dst2, 1e-4))
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

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func);

        IF_ARCH_X86(CALL(generic::compressor_x2_gain, sse2::compressor_x2_gain, 16));

//        IF_ARCH_ARM(CALL(generic::compressor_x2_gain, neon_d32::compressor_x2_gain, 16));

//        IF_ARCH_AARCH64(CALL(generic::compressor_x2_gain, asimd::compressor_x2_gain, 16));
    }
UTEST_END



