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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

#include <private/dsp/arch/x86/avx/fft/const.h>
#include <private/dsp/arch/x86/avx/fft/butterfly.h>
#include <private/dsp/arch/x86/avx/fft/normalize.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8
#define FFT_TYPE                        uint8_t
#define FFT_FMA(a, b)                   a
#include <private/dsp/arch/x86/avx/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16
#define FFT_TYPE                        uint16_t
#define FFT_FMA(a, b)                   a
#include <private/dsp/arch/x86/avx/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8_fma3
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8_fma3
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8_fma3
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8_fma3
#define FFT_TYPE                        uint8_t
#define FFT_FMA(a, b)                   b
#include <private/dsp/arch/x86/avx/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16_fma3
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16_fma3
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16_fma3
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16_fma3
#define FFT_TYPE                        uint16_t
#define FFT_FMA(a, b)                   b
#include <private/dsp/arch/x86/avx/fft/scramble.h>

namespace lsp
{
    namespace avx
    {
        static inline void small_direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[2];
                float s1_re     = src_re[0] - src_re[2];
                float s2_re     = src_re[1] + src_re[3];
                float s3_re     = src_re[1] - src_re[3];

                float s0_im     = src_im[0] + src_im[2];
                float s1_im     = src_im[0] - src_im[2];
                float s2_im     = src_im[1] + src_im[3];
                float s3_im     = src_im[1] - src_im[3];

                dst_re[0]       = s0_re + s2_re;
                dst_re[1]       = s1_re + s3_im;
                dst_re[2]       = s0_re - s2_re;
                dst_re[3]       = s1_re - s3_im;

                dst_im[0]       = s0_im + s2_im;
                dst_im[1]       = s1_im - s3_re;
                dst_im[2]       = s0_im - s2_im;
                dst_im[3]       = s1_im + s3_re;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = src_re[0] - s1_re;
                dst_im[1]       = src_im[0] - s1_im;
                dst_re[0]       = src_re[0] + s1_re;
                dst_im[0]       = src_im[0] + s1_im;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
        }

        static inline void small_reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            if (rank == 2)
            {
                float s0_re     = src_re[0] + src_re[2];
                float s1_re     = src_re[0] - src_re[2];
                float s2_re     = src_re[1] + src_re[3];
                float s3_re     = src_re[1] - src_re[3];

                float s0_im     = src_im[0] + src_im[2];
                float s1_im     = src_im[0] - src_im[2];
                float s2_im     = src_im[1] + src_im[3];
                float s3_im     = src_im[1] - src_im[3];

                dst_re[0]       = (s0_re + s2_re) * 0.25f;
                dst_re[1]       = (s1_re - s3_im) * 0.25f;
                dst_re[2]       = (s0_re - s2_re) * 0.25f;
                dst_re[3]       = (s1_re + s3_im) * 0.25f;

                dst_im[0]       = (s0_im + s2_im) * 0.25f;
                dst_im[1]       = (s1_im + s3_re) * 0.25f;
                dst_im[2]       = (s0_im - s2_im) * 0.25f;
                dst_im[3]       = (s1_im - s3_re) * 0.25f;
            }
            else if (rank == 1)
            {
                // s0' = s0 + s1
                // s1' = s0 - s1
                float s1_re     = src_re[1];
                float s1_im     = src_im[1];
                dst_re[1]       = src_re[0] - s1_re;
                dst_im[1]       = src_im[0] - s1_im;
                dst_re[0]       = src_re[0] + s1_re;
                dst_im[0]       = src_im[0] + s1_im;
            }
            else
            {
                dst_re[0]       = src_re[0];
                dst_im[0]       = src_im[0];
            }
        }

        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                small_direct_fft(dst_re, dst_im, src_re, src_im, rank);
                return;
            }

            if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
            {
                dsp::move(dst_re, src_re, 1 << rank);
                dsp::move(dst_im, src_im, 1 << rank);
                if (rank <= 8)
                    scramble_self_direct8(dst_re, dst_im, rank);
                else
                    scramble_self_direct16(dst_re, dst_im, rank);
            }
            else
            {
                if (rank <= 12)
                    scramble_copy_direct8(dst_re, dst_im, src_re, src_im, rank-4);
                else
                    scramble_copy_direct16(dst_re, dst_im, src_re, src_im, rank-4);
            }

            for (size_t i=3; i < rank; ++i)
                butterfly_direct8p(dst_re, dst_im, i, 1 << (rank - i - 1));
        }

        void direct_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                small_direct_fft(dst_re, dst_im, src_re, src_im, rank);
                return;
            }

            if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
            {
                dsp::move(dst_re, src_re, 1 << rank);
                dsp::move(dst_im, src_im, 1 << rank);
                if (rank <= 8)
                    scramble_self_direct8_fma3(dst_re, dst_im, rank);
                else
                    scramble_self_direct16_fma3(dst_re, dst_im, rank);
            }
            else
            {
                if (rank <= 12)
                    scramble_copy_direct8_fma3(dst_re, dst_im, src_re, src_im, rank-4);
                else
                    scramble_copy_direct16_fma3(dst_re, dst_im, src_re, src_im, rank-4);
            }

            for (size_t i=3; i < rank; ++i)
                butterfly_direct8p_fma3(dst_re, dst_im, i, 1 << (rank - i - 1));
        }

        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                small_reverse_fft(dst_re, dst_im, src_re, src_im, rank);
                return;
            }

            if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
            {
                dsp::move(dst_re, src_re, 1 << rank);
                dsp::move(dst_im, src_im, 1 << rank);
                if (rank <= 8)
                    scramble_self_reverse8(dst_re, dst_im, rank);
                else
                    scramble_self_reverse16(dst_re, dst_im, rank);
            }
            else
            {
                if (rank <= 12)
                    scramble_copy_reverse8(dst_re, dst_im, src_re, src_im, rank-4);
                else
                    scramble_copy_reverse16(dst_re, dst_im, src_re, src_im, rank-4);
            }

            for (size_t i=3; i < rank; ++i)
                butterfly_reverse8p(dst_re, dst_im, i, 1 << (rank - i - 1));

            dsp::normalize_fft2(dst_re, dst_im, rank);
        }

        void reverse_fft_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                small_reverse_fft(dst_re, dst_im, src_re, src_im, rank);
                return;
            }

            if ((dst_re == src_re) || (dst_im == src_im) || (rank < 4))
            {
                dsp::move(dst_re, src_re, 1 << rank);
                dsp::move(dst_im, src_im, 1 << rank);
                if (rank <= 8)
                    scramble_self_reverse8_fma3(dst_re, dst_im, rank);
                else
                    scramble_self_reverse16_fma3(dst_re, dst_im, rank);
            }
            else
            {
                if (rank <= 12)
                    scramble_copy_reverse8_fma3(dst_re, dst_im, src_re, src_im, rank-4);
                else
                    scramble_copy_reverse16_fma3(dst_re, dst_im, src_re, src_im, rank-4);
            }

            for (size_t i=3; i < rank; ++i)
                butterfly_reverse8p_fma3(dst_re, dst_im, i, 1 << (rank - i - 1));

            dsp::normalize_fft2(dst_re, dst_im, rank);
        }

    } /* namespace avx */
} /* namespace lsp */
