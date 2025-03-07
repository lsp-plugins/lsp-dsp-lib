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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_FFT_H_
#define PRIVATE_DSP_ARCH_X86_SSE_FFT_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

#include <private/dsp/arch/x86/sse/fft/const.h>
#include <private/dsp/arch/x86/sse/fft/butterfly.h>
#include <private/dsp/arch/x86/sse/fft/p_butterfly.h>
#include <private/dsp/arch/x86/sse/fft/normalize.h>

// Use 8-bit-reverse algorithm
#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct8
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse8
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct8
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse8
#define FFT_TYPE                        uint8_t
#include <private/dsp/arch/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct8
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse8
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct8
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse8
#define FFT_TYPE                        uint8_t
#include <private/dsp/arch/x86/sse/fft/p_scramble.h>

// Use 16-bit-reverse algorithm
#define FFT_SCRAMBLE_SELF_DIRECT_NAME   scramble_self_direct16
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  scramble_self_reverse16
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   scramble_copy_direct16
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  scramble_copy_reverse16
#define FFT_TYPE                        uint16_t
#include <private/dsp/arch/x86/sse/fft/scramble.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME   packed_scramble_self_direct16
#define FFT_SCRAMBLE_SELF_REVERSE_NAME  packed_scramble_self_reverse16
#define FFT_SCRAMBLE_COPY_DIRECT_NAME   packed_scramble_copy_direct16
#define FFT_SCRAMBLE_COPY_REVERSE_NAME  packed_scramble_copy_reverse16
#define FFT_TYPE                        uint16_t
#include <private/dsp/arch/x86/sse/fft/p_scramble.h>

// Make set of scramble-switch implementations
#define FFT_SCRAMBLE_SELF_DIRECT_NAME       scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           scramble_reverse
#include <private/dsp/arch/x86/sse/fft/switch.h>

#define FFT_SCRAMBLE_SELF_DIRECT_NAME       packed_scramble_self_direct
#define FFT_SCRAMBLE_COPY_DIRECT_NAME       packed_scramble_copy_direct
#define FFT_SCRAMBLE_SELF_REVERSE_NAME      packed_scramble_self_reverse
#define FFT_SCRAMBLE_COPY_REVERSE_NAME      packed_scramble_copy_reverse
#define FFT_SCRAMBLE_DIRECT_NAME            packed_scramble_direct
#define FFT_SCRAMBLE_REVERSE_NAME           packed_scramble_reverse
#define FFT_REPACK                          packed_fft_repack
#define FFT_REPACK_NORMALIZE                packed_fft_repack_normalize
#include <private/dsp/arch/x86/sse/fft/p_switch.h>

namespace lsp
{
    namespace sse
    {
        // Make set of scramble implementations
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
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
                return;
            }

            scramble_direct(dst_re, dst_im, src_re, src_im, rank);

            for (size_t i=2; i < rank; ++i)
                butterfly_direct(dst_re, dst_im, i, 1 << (rank - i - 1));
        }

        void packed_direct_fft(float *dst, const float *src, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                if (rank == 2)
                {
                    float s0_re     = dst[0] + dst[2];
                    float s1_re     = dst[0] - dst[2];
                    float s0_im     = dst[1] + dst[3];
                    float s1_im     = dst[1] - dst[3];

                    float s2_re     = dst[4] + dst[6];
                    float s3_re     = dst[4] - dst[6];
                    float s2_im     = dst[5] + dst[7];
                    float s3_im     = dst[5] - dst[7];

                    dst[0]          = s0_re + s2_re;
                    dst[1]          = s0_im + s2_im;
                    dst[2]          = s1_re + s3_im;
                    dst[3]          = s1_im - s3_re;

                    dst[4]          = s0_re - s2_re;
                    dst[5]          = s0_im - s2_im;
                    dst[6]          = s1_re - s3_im;
                    dst[7]          = s1_im + s3_re;
                }
                else if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = src[2];
                    float s1_im     = src[3];
                    dst[2]          = src[0] - s1_re;
                    dst[3]          = src[1] - s1_im;
                    dst[0]          = src[0] + s1_re;
                    dst[1]          = src[1] + s1_im;
                }
                else
                {
                    dst[0]          = src[0];
                    dst[1]          = src[1];
                }
                return;
            }

            // Iterate butterflies
            packed_scramble_direct(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_direct(dst, i, 1 << (rank - i - 1));

            packed_fft_repack(dst, rank);
        }

        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
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

                    dst_re[0]       = (s0_re + s2_re)*0.25f;
                    dst_re[1]       = (s1_re - s3_im)*0.25f;
                    dst_re[2]       = (s0_re - s2_re)*0.25f;
                    dst_re[3]       = (s1_re + s3_im)*0.25f;

                    dst_im[0]       = (s0_im + s2_im)*0.25f;
                    dst_im[1]       = (s1_im + s3_re)*0.25f;
                    dst_im[2]       = (s0_im - s2_im)*0.25f;
                    dst_im[3]       = (s1_im - s3_re)*0.25f;
                }
                else if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = src_re[1];
                    float s1_im     = src_im[1];
                    dst_re[1]       = (src_re[0] - s1_re) * 0.5f;
                    dst_im[1]       = (src_im[0] - s1_im) * 0.5f;
                    dst_re[0]       = (src_re[0] + s1_re) * 0.5f;
                    dst_im[0]       = (src_im[0] + s1_im) * 0.5f;
                }
                else
                {
                    dst_re[0]       = src_re[0];
                    dst_im[0]       = src_im[0];
                }
                return;
            }

            // Iterate butterflies
            scramble_reverse(dst_re, dst_im, src_re, src_im, rank);

            for (size_t i=2; i < rank; ++i)
                butterfly_reverse(dst_re, dst_im, i, 1 << (rank - i - 1));

            dsp::normalize_fft2(dst_re, dst_im, rank);
        }

        void packed_reverse_fft(float *dst, const float *src, size_t rank)
        {
            // Check bounds
            if (rank <= 2)
            {
                if (rank == 2)
                {
                    float s0_re     = src[0] + src[2];
                    float s1_re     = src[0] - src[2];
                    float s2_re     = src[4] + src[6];
                    float s3_re     = src[4] - src[6];

                    float s0_im     = src[1] + src[3];
                    float s1_im     = src[1] - src[3];
                    float s2_im     = src[5] + src[7];
                    float s3_im     = src[5] - src[7];

                    dst[0]          = (s0_re + s2_re)*0.25f;
                    dst[1]          = (s0_im + s2_im)*0.25f;
                    dst[2]          = (s1_re - s3_im)*0.25f;
                    dst[3]          = (s1_im + s3_re)*0.25f;

                    dst[4]          = (s0_re - s2_re)*0.25f;
                    dst[5]          = (s0_im - s2_im)*0.25f;
                    dst[6]          = (s1_re + s3_im)*0.25f;
                    dst[7]          = (s1_im - s3_re)*0.25f;
                }
                else if (rank == 1)
                {
                    // s0' = s0 + s1
                    // s1' = s0 - s1
                    float s1_re     = src[2];
                    float s1_im     = src[3];
                    dst[2]          = src[0] - s1_re;
                    dst[3]          = src[1] - s1_im;
                    dst[0]          = src[0] + s1_re;
                    dst[1]          = src[1] + s1_im;
                }
                else
                {
                    dst[0]          = src[0];
                    dst[1]          = src[1];
                }
                return;
            }

            // Iterate butterflies
            packed_scramble_reverse(dst, src, rank);

            for (size_t i=2; i < rank; ++i)
                packed_butterfly_reverse(dst, i, 1 << (rank - i - 1));

            packed_fft_repack_normalize(dst, rank);
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_X86_SSE_FFT_H_ */
