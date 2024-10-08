/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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
        void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
        }

        namespace avx
        {
            void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
            void convolve_fma3(float *dst, const float *src, const float *conv, size_t length, size_t count);
        }

        namespace avx512
        {
            void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count);
        }
    )

    static void convolve(float *dst, const float *src, const float *conv, size_t length, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            for (size_t j=0; j<length; ++j)
                dst[i+j] += src[i] * conv[j];
        }
    }

    typedef void (* convolve_t)(float *dst, const float *src, const float *conv, size_t length, size_t count);
}

UTEST_BEGIN("dsp", convolve)
    void call(const char *label, size_t align, convolve_t func)
    {
        if (!UTEST_SUPPORTED(func))
            return;

        for (size_t mask=0; mask <= 0x07; ++mask)
//        size_t mask = 0;
        {
            UTEST_FOREACH(count, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
//            size_t count = 4;
            {
                FloatBuffer src(count, align, mask & 0x01);
//                src.fill_zero();
//                src[31] = 1.0f;
//                src[2] = -1.0f;

                UTEST_FOREACH(length, 0, 1, 2, 3, 4, 5, 8, 16, 24, 32, 33, 64, 47, 0x80, 0x1ff)
//                size_t length = 128;
                {
                    printf("Tesing %s convolution length=%d on buffer count=%d mask=0x%x\n", label, int(length), int(count), int(mask));

                    ssize_t clen = count + length - 1;
                    FloatBuffer conv(length, align, mask & 0x02);
                    for (size_t i=0; i<conv.size(); ++i)
                        conv[i] = i + 1;

                    FloatBuffer dst1((clen > 0) ? clen : 0, align, mask & 0x04);
                    dst1.fill_zero();
                    FloatBuffer dst2(dst1);

//                    src.dump("src ");
//                    conv.dump("conv");
                    convolve(dst1, src, conv, length, count);
//                    dst1.dump("dst1");
                    func(dst2, src, conv, length, count);
//                    dst2.dump("dst2");

                    UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
                    UTEST_ASSERT_MSG(conv.valid(), "Convolution buffer corrupted");
                    UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
                    UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

                    // Compare buffers
                    if (!dst1.equals_relative(dst2, 1e-5))
                    {
                        src.dump("src ");
                        conv.dump("conv");
                        dst1.dump("dst1");
                        dst2.dump("dst2");
                        UTEST_FAIL_MSG("Output of functions for test '%s' differs", label);
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(func, align) \
            call(#func, align, func)

        CALL(generic::convolve, 16);
        IF_ARCH_X86(CALL(sse::convolve, 16));
        IF_ARCH_X86(CALL(avx::convolve, 32));
        IF_ARCH_X86(CALL(avx::convolve_fma3, 32));
        IF_ARCH_X86(CALL(avx512::convolve, 64));
        IF_ARCH_ARM(CALL(neon_d32::convolve, 16));
        IF_ARCH_AARCH64(CALL(asimd::convolve, 16));
    }

UTEST_END;


