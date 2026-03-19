/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 18 мар. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PFFT_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PFFT_H_


#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/fft/const.h>
#include <private/dsp/arch/x86/avx512/fft/p_repack.h>
#include <private/dsp/arch/x86/avx512/fft/p_butterfly.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME  packed_scramble_self_direct8
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME packed_scramble_self_reverse8
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME  packed_scramble_copy_direct8
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME packed_scramble_copy_reverse8
#define FFT_TYPE                        uint8_t
#include <private/dsp/arch/x86/avx512/fft/p_scramble.h>

#define FFT_PSCRAMBLE_SELF_DIRECT_NAME  packed_scramble_self_direct16
#define FFT_PSCRAMBLE_SELF_REVERSE_NAME packed_scramble_self_reverse16
#define FFT_PSCRAMBLE_COPY_DIRECT_NAME  packed_scramble_copy_direct16
#define FFT_PSCRAMBLE_COPY_REVERSE_NAME packed_scramble_copy_reverse16
#define FFT_TYPE                        uint16_t
#include <private/dsp/arch/x86/avx512/fft/p_scramble.h>

namespace lsp
{
    namespace avx512
    {

        static void packed_small_direct_fft(float *dst, const float *src, size_t rank)
        {
            if (rank == 2)
            {
                float s0_re     = src[0] + src[4];
                float s1_re     = src[0] - src[4];
                float s0_im     = src[1] + src[5];
                float s1_im     = src[1] - src[5];

                float s2_re     = src[2] + src[6];
                float s3_re     = src[2] - src[6];
                float s2_im     = src[3] + src[7];
                float s3_im     = src[3] - src[7];

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
        }

        static void packed_small_reverse_fft(float *dst, const float *src, size_t rank)
        {
            if (rank == 2)
            {
                float s0_re     = src[0] + src[4];
                float s1_re     = src[0] - src[4];
                float s0_im     = src[1] + src[5];
                float s1_im     = src[1] - src[5];

                float s2_re     = src[2] + src[6];
                float s3_re     = src[2] - src[6];
                float s2_im     = src[3] + src[7];
                float s3_im     = src[3] - src[7];

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

        void packed_direct_fft(float *dst, const float *src, size_t rank)
        {
            if (rank <= 2)
            {
                packed_small_direct_fft(dst, src, rank);
                return;
            }

            if ((dst == src) || (rank < 5))
            {
                dsp::move(dst, src, 2 << rank); // 1 << rank + 1
                if (rank <= 8)
                    packed_scramble_self_direct8(dst, rank);
                else
                    packed_scramble_self_direct16(dst, rank);
            }
            else
            {
                if (rank <= 13)
                    packed_scramble_copy_direct8(dst, src, rank - 5);
                else
                    packed_scramble_copy_direct16(dst, src, rank - 5);
            }

            for (size_t i=4; i < rank; ++i)
                packed_butterfly_direct16p(dst, i, 1 << (rank - i - 1));

            packed_fft_repack(dst, rank);
        }

        void packed_reverse_fft(float *dst, const float *src, size_t rank)
        {
            if (rank <= 2)
            {
                packed_small_reverse_fft(dst, src, rank);
                return;
            }

            if ((dst == src) || (rank < 5))
            {
                dsp::move(dst, src, 2 << rank); // 1 << rank + 1
                if (rank <= 8)
                    packed_scramble_self_reverse8(dst, rank);
                else
                    packed_scramble_self_reverse16(dst, rank);
            }
            else
            {
                if (rank <= 13)
                    packed_scramble_copy_reverse8(dst, src, rank - 5);
                else
                    packed_scramble_copy_reverse16(dst, src, rank - 5);
            }

            for (size_t i=4; i < rank; ++i)
                packed_butterfly_reverse16p(dst, i, 1 << (rank - i - 1));

            packed_fft_repack_normalize(dst, rank);
        }

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PFFT_H_ */
