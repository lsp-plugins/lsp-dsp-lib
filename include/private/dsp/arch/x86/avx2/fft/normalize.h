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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

namespace lsp
{
    namespace avx2
    {
        void normalize_fft3(float *dre, float *dim, const float *re, const float *im, size_t rank)
        {
            IF_ARCH_X86(
                float k = 1.0f/(1 << rank);
                size_t count = 1 << rank, off = 0;
            );
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT  ("vbroadcastss  %%xmm0, %%ymm0")                 // ymm0   = k
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT  ("vmovaps       %%ymm0, %%ymm1")
                __ASM_EMIT  ("jb            2f")
                __ASM_EMIT  ("1:")
                __ASM_EMIT  ("vmulps        0x00(%[s_re], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT  ("vmulps        0x20(%[s_re], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT  ("vmulps        0x00(%[s_im], %[off]), %%ymm0, %%ymm6")
                __ASM_EMIT  ("vmulps        0x20(%[s_im], %[off]), %%ymm1, %%ymm7")
                __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm5, 0x20(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm7, 0x20(%[d_im], %[off])")
                __ASM_EMIT  ("add           $0x40, %[off]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT  ("jae           1b")
                __ASM_EMIT  ("2:")
                // x8 block
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT  ("jl            4f")
                __ASM_EMIT  ("vmulps        0x00(%[s_re], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT  ("vmulps        0x00(%[s_im], %[off]), %%ymm1, %%ymm6")
                __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
                __ASM_EMIT  ("4:")
                : [off] "+r" (off), [count] __ASM_ARG_RW(count),
                  [k] "+Yz" (k)
                : [s_re] "r" (re), [s_im] "r" (im),
                  [d_re] "r" (dre), [d_im] "r" (dim)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void normalize_fft2(float *re, float *im, size_t rank)
        {
            IF_ARCH_X86(
                float k = 1.0f/(1 << rank);
                size_t count = 1 << rank, off = 0;
            );
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT  ("vbroadcastss  %%xmm0, %%ymm0")                 // ymm0   = k
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT  ("vmovaps       %%ymm0, %%ymm1")
                __ASM_EMIT  ("jb            2f")
                __ASM_EMIT  ("1:")
                __ASM_EMIT  ("vmulps        0x00(%[d_re], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT  ("vmulps        0x20(%[d_re], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT  ("vmulps        0x00(%[d_im], %[off]), %%ymm0, %%ymm6")
                __ASM_EMIT  ("vmulps        0x20(%[d_im], %[off]), %%ymm1, %%ymm7")
                __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm5, 0x20(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm7, 0x20(%[d_im], %[off])")
                __ASM_EMIT  ("add           $0x40, %[off]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT  ("jae           1b")
                __ASM_EMIT  ("2:")
                // x8 block
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT  ("jl            4f")
                __ASM_EMIT  ("vmulps        0x00(%[d_re], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT  ("vmulps        0x00(%[d_im], %[off]), %%ymm1, %%ymm6")
                __ASM_EMIT  ("vmovups       %%ymm4, 0x00(%[d_re], %[off])")
                __ASM_EMIT  ("vmovups       %%ymm6, 0x00(%[d_im], %[off])")
                __ASM_EMIT  ("4:")
                : [off] "+r" (off), [count] "+r" (count),
                  [k] "+Yz" (k)
                : [d_re] "r" (re), [d_im] "r" (im)
                : "cc", "memory",
                  "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    } /* namespace avx2 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX2_FFT_NORMALIZE_H_ */
