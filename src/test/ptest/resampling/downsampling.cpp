/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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
#include <lsp-plug.in/test-fw/ptest.h>

#define RTEST_BUF_SIZE  0x1000

namespace lsp
{
    namespace generic
    {
        void downsample_2x(float *dst, const float *src, size_t count);
        void downsample_3x(float *dst, const float *src, size_t count);
        void downsample_4x(float *dst, const float *src, size_t count);
        void downsample_6x(float *dst, const float *src, size_t count);
        void downsample_8x(float *dst, const float *src, size_t count);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void downsample_2x(float *dst, const float *src, size_t count);
            void downsample_3x(float *dst, const float *src, size_t count);
            void downsample_4x(float *dst, const float *src, size_t count);
            void downsample_6x(float *dst, const float *src, size_t count);
            void downsample_8x(float *dst, const float *src, size_t count);
        }

        namespace avx
        {
            void downsample_2x(float *dst, const float *src, size_t count);
            void downsample_3x(float *dst, const float *src, size_t count);
            void downsample_4x(float *dst, const float *src, size_t count);
            void downsample_6x(float *dst, const float *src, size_t count);
            void downsample_8x(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void downsample_2x(float *dst, const float *src, size_t count);
            void downsample_3x(float *dst, const float *src, size_t count);
            void downsample_4x(float *dst, const float *src, size_t count);
            void downsample_6x(float *dst, const float *src, size_t count);
            void downsample_8x(float *dst, const float *src, size_t count);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void downsample_2x(float *dst, const float *src, size_t count);
            void downsample_3x(float *dst, const float *src, size_t count);
            void downsample_4x(float *dst, const float *src, size_t count);
            void downsample_6x(float *dst, const float *src, size_t count);
            void downsample_8x(float *dst, const float *src, size_t count);
        }
    )

    typedef void (* downsample_t)(float *dst, const float *src, size_t count);
}

//-----------------------------------------------------------------------------
// Performance test for lanczos resampling
PTEST_BEGIN("dsp.resampling", downsampling, 5, 1000)

    void call(float *out, const float *in, size_t count, size_t times, const char *text, downsample_t func)
    {
        if (!PTEST_SUPPORTED(func))
            return;

        printf("Testing %s downsampling for %d -> %d samples ...\n", text, int(count * times), int(count));

        PTEST_LOOP(text,
            func(out, in, count);
        );
    }

    PTEST_MAIN
    {
        float *out          = new float[RTEST_BUF_SIZE];
        float *in           = new float[RTEST_BUF_SIZE*8];

        // Prepare data
        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            in[i]               = (i % 1) ? 1.0f : -1.0f;

        #define CALL(func, order) \
            call(out, in, RTEST_BUF_SIZE, order, #func, func);

        // Do tests
        CALL(generic::downsample_2x, 2);
        IF_ARCH_X86(CALL(sse::downsample_2x, 2));
        IF_ARCH_X86(CALL(avx::downsample_2x, 2));
        IF_ARCH_ARM(CALL(neon_d32::downsample_2x, 2));
        IF_ARCH_AARCH64(CALL(asimd::downsample_2x, 2));
        PTEST_SEPARATOR;

        CALL(generic::downsample_3x, 3);
        IF_ARCH_X86(CALL(sse::downsample_3x, 3));
        IF_ARCH_X86(CALL(avx::downsample_3x, 3));
        IF_ARCH_ARM(CALL(neon_d32::downsample_3x, 3));
        IF_ARCH_AARCH64(CALL(asimd::downsample_3x, 3));
        PTEST_SEPARATOR;

        CALL(generic::downsample_4x, 4);
        IF_ARCH_X86(CALL(sse::downsample_4x, 4));
        IF_ARCH_X86(CALL(avx::downsample_4x, 4));
        IF_ARCH_ARM(CALL(neon_d32::downsample_4x, 4));
        IF_ARCH_AARCH64(CALL(asimd::downsample_4x, 4));
        PTEST_SEPARATOR;

        CALL(generic::downsample_6x, 6);
        IF_ARCH_X86(CALL(sse::downsample_6x, 6));
        IF_ARCH_X86(CALL(avx::downsample_6x, 6));
        IF_ARCH_ARM(CALL(neon_d32::downsample_6x, 6));
        IF_ARCH_AARCH64(CALL(asimd::downsample_6x, 6));
        PTEST_SEPARATOR;

        CALL(generic::downsample_8x, 8);
        IF_ARCH_X86(CALL(sse::downsample_8x, 8));
        IF_ARCH_X86(CALL(avx::downsample_8x, 8));
        IF_ARCH_ARM(CALL(neon_d32::downsample_8x, 8));
        IF_ARCH_AARCH64(CALL(asimd::downsample_8x, 8));
        PTEST_SEPARATOR;

        delete [] out;
        delete [] in;
    }

PTEST_END


