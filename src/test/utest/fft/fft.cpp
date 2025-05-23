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
#include <lsp-plug.in/dsp/dsp.h>
#include <lsp-plug.in/test-fw/utest.h>
#include <lsp-plug.in/test-fw/FloatBuffer.h>

#define TOLERANCE       5e-2

namespace lsp
{
    namespace generic
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
    }

    IF_ARCH_X86(
        namespace sse
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        }

        namespace avx
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

            void direct_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        }

        namespace avx512
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        }
    )

    IF_ARCH_ARM(
        namespace neon_d32
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        }
    )

    IF_ARCH_AARCH64(
        namespace asimd
        {
            void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
            void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        }
    )
}

typedef void (* fft_t)(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);

UTEST_BEGIN("dsp.fft", fft)

    UTEST_TIMELIMIT(30)

    void call(const char *label, size_t align, fft_t func1, fft_t func2)
    {
        if (!UTEST_SUPPORTED(func1))
            return;
        if (!UTEST_SUPPORTED(func2))
            return;

        for (int same=0; same<2; ++same)
        {
            for (size_t rank=2; rank<=16; ++rank)
            {
                size_t count = 1 << rank;
                for (size_t mask=0; mask <= 0x0f; ++mask)
                {
                    FloatBuffer src_re(count, align, mask & 0x01);
                    FloatBuffer src_im(count, align, mask & 0x02);
                    FloatBuffer dst1_re(count, align, mask & 0x04);
                    FloatBuffer dst1_im(count, align, mask & 0x08);
                    FloatBuffer dst2_re(dst1_re);
                    FloatBuffer dst2_im(dst1_im);

                    printf("Testing '%s' for rank=%d, mask=0x%x, same=%s...\n", label, int(rank), int(mask), (same) ? "true" : "false");

                    if (same)
                    {
                        dsp::copy(dst1_re, src_re, count);
                        dsp::copy(dst1_im, src_im, count);
                        dsp::copy(dst2_re, src_re, count);
                        dsp::copy(dst2_im, src_im, count);

                        func1(dst1_re, dst1_im, dst1_re, dst1_im, rank);
                        func2(dst2_re, dst2_im, dst2_re, dst2_im, rank);
                    }
                    else
                    {
                        func1(dst1_re, dst1_im, src_re, src_im, rank);
                        func2(dst2_re, dst2_im, src_re, src_im, rank);
                    }

                    UTEST_ASSERT_MSG(src_re.valid(), "Source buffer RE corrupted");
                    UTEST_ASSERT_MSG(src_im.valid(), "Source buffer IM corrupted");
                    UTEST_ASSERT_MSG(dst1_re.valid(), "Destination buffer 1 RE corrupted");
                    UTEST_ASSERT_MSG(dst1_im.valid(), "Destination buffer 1 IM corrupted");
                    UTEST_ASSERT_MSG(dst2_re.valid(), "Destination buffer 2 RE corrupted");
                    UTEST_ASSERT_MSG(dst2_im.valid(), "Destination buffer 2 IM corrupted");

                    // Compare buffers
                    if ((!dst1_re.equals_adaptive(dst2_re, TOLERANCE)) || (!dst1_im.equals_adaptive(dst2_im, TOLERANCE)))
                    {
                        src_re.dump("src_re ");
                        src_im.dump("src_im ");
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
    }

    UTEST_MAIN
    {
        #define CALL(generic, func, align) \
            call(#func, align, generic, func)

        // Do tests
        IF_ARCH_X86(CALL(generic::direct_fft, sse::direct_fft, 16));
        IF_ARCH_X86(CALL(generic::reverse_fft, sse::reverse_fft, 16));
        IF_ARCH_X86(CALL(generic::direct_fft, avx::direct_fft, 32));
        IF_ARCH_X86(CALL(generic::reverse_fft, avx::reverse_fft, 32));
        IF_ARCH_X86(CALL(generic::direct_fft, avx::direct_fft_fma3, 32));
        IF_ARCH_X86(CALL(generic::reverse_fft, avx::reverse_fft_fma3, 32));
        IF_ARCH_X86(CALL(generic::direct_fft, avx512::direct_fft, 64));
		IF_ARCH_X86(CALL(generic::reverse_fft, avx512::reverse_fft, 64));

        IF_ARCH_ARM(CALL(generic::direct_fft, neon_d32::direct_fft, 16));
        IF_ARCH_ARM(CALL(generic::reverse_fft, neon_d32::reverse_fft, 16));

        IF_ARCH_AARCH64(CALL(generic::direct_fft, asimd::direct_fft, 16));
        IF_ARCH_AARCH64(CALL(generic::reverse_fft, asimd::reverse_fft, 16));
    }
UTEST_END;
