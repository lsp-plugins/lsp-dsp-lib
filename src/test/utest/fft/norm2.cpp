/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#define MIN_RANK    4
#define MAX_RANK    16

namespace lsp
{
    namespace generic
    {
        void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }

        namespace avx
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }

        namespace avx2
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }

        namespace avx512
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void normalize_fft2(float *dst_re, float *dst_im, size_t rank);
        }
    )
}

typedef void (* normalize_fft2_t)(float *dst_re, float *dst_im, size_t rank);

UTEST_BEGIN("dsp.fft", norm2)

    void call(const char *label, size_t align, normalize_fft2_t func1, normalize_fft2_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
        {
            size_t count = 1 << rank;

            for (size_t mask=0; mask <= 0x03; ++mask)
            {
                FloatBuffer dst1_re(count, align, mask & 0x01);
                FloatBuffer dst1_im(count, align, mask & 0x02);
                dst1_re.randomize_sign();
                dst1_im.randomize_sign();
                FloatBuffer dst2_re(dst1_re);
                FloatBuffer dst2_im(dst1_im);

                printf("Testing '%s' for rank=%d, mask=0x%x...\n", label, int(rank), int(mask));
                func1(dst1_re, dst1_im, rank);
                func2(dst2_re, dst2_im, rank);

                UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                // Compare buffers
                if ((!dst1_re.equals_adaptive(dst2_re)) || (!dst1_im.equals_adaptive(dst2_im)))
                {
                    dst1_re.dump("dst1_re");
                    dst2_re.dump("dst2_re");
                    dst1_im.dump("dst1_im");
                    dst2_im.dump("dst2_im");

                    ssize_t diff = dst1_re.last_diff();
                    if (diff >= 0)
                    {
                        UTEST_FAIL_MSG("Real output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_re.get(diff), dst2_re.get(diff));
                    }
                    else
                    {
                        diff = dst1_im.last_diff();
                        UTEST_FAIL_MSG("Imaginary output of functions for test '%s' differs at sample %d (%.5f vs %.5f)",
                                label, int(diff), dst1_im.get(diff), dst2_im.get(diff));
                    }
                }
            }
        }
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        // Do tests
        IF_ARCH_X86(CALL(generic::normalize_fft2, sse::normalize_fft2, 16));
        IF_ARCH_X86(CALL(generic::normalize_fft2, avx::normalize_fft2, 32));
        IF_ARCH_X86(CALL(generic::normalize_fft2, avx2::normalize_fft2, 32));
        IF_ARCH_X86(CALL(generic::normalize_fft2, avx512::normalize_fft2, 64));
        IF_ARCH_ARM(CALL(generic::normalize_fft2, neon_d32::normalize_fft2, 16));
        IF_ARCH_AARCH64(CALL(generic::normalize_fft2, asimd::normalize_fft2, 16));
    }
UTEST_END




