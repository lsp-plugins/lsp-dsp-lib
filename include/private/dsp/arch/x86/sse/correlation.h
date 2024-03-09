/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 мар. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_CORRELATION_H_
#define PRIVATE_DSP_ARCH_X86_SSE_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {

        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")                /* xv = 0 */
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")                /* xa = 0 */
                __ASM_EMIT("xorps       %%xmm2, %%xmm2")                /* xb = 0 */
                /* 8x blocks */
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm4")    /* xmm4 = a1 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                /* xmm7 = a0 */
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm6")    /* xmm6 = b1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm7")                /* xmm7 = a0*b0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = a0*a0 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")                /* xmm5 = b0*b0 */
                __ASM_EMIT("addps       %%xmm7, %%xmm0")                /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")                /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("movaps      %%xmm4, %%xmm7")                /* xmm7 = a1 */
                __ASM_EMIT("addps       %%xmm5, %%xmm2")                /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("mulps       %%xmm6, %%xmm7")                /* xmm7 = a1*b1 */
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")                /* xmm4 = a1*a1 */
                __ASM_EMIT("mulps       %%xmm6, %%xmm6")                /* xmm6 = b1*b1 */
                __ASM_EMIT("addps       %%xmm7, %%xmm0")                /* xmm0 = xv + a1*b1 */
                __ASM_EMIT("addps       %%xmm4, %%xmm1")                /* xmm1 = xa + a1*a1 */
                __ASM_EMIT("addps       %%xmm6, %%xmm2")                /* xmm2 = xb + b1*b1 */
                __ASM_EMIT("add         $0x20, %[off]")                 /* ++off */
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* 4x block */
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                /* xmm7 = a0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = a0*a0 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm7")                /* xmm7 = a0*b0 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")                /* xmm5 = b0*b0 */
                __ASM_EMIT("addps       %%xmm7, %%xmm0")                /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")                /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("addps       %%xmm5, %%xmm2")                /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("add         $0x10, %[off]")                 /* ++off */
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("4:")
                /* Do horizontal sum */
                __ASM_EMIT("movhlps     %%xmm0, %%xmm4")                /* xmm4 = xv2 xv3 ? ? */
                __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                /* xmm5 = xa2 xa3 ? ? */
                __ASM_EMIT("movhlps     %%xmm2, %%xmm6")                /* xmm6 = xb2 xb3 ? ? */
                __ASM_EMIT("addps       %%xmm4, %%xmm0")                /* xmm0 = xv0+xv2 xv1+xv3 ? ? */
                __ASM_EMIT("addps       %%xmm5, %%xmm1")                /* xmm1 = xa0+xa2 xa1+xa3 ? ? */
                __ASM_EMIT("addps       %%xmm6, %%xmm2")                /* xmm2 = xb0+xb2 xb1+xb3 ? ? */
                __ASM_EMIT("unpcklps    %%xmm4, %%xmm0")                /* xmm0 = xv0+xv2 xv2 xv1+xv3 xv3 */
                __ASM_EMIT("unpcklps    %%xmm5, %%xmm1")                /* xmm1 = xa0+xa2 xa2 xa1+xa3 xa3 */
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm2")                /* xmm2 = xb0+xb2 xb2 xb1+xb3 xb3 */
                __ASM_EMIT("movhlps     %%xmm0, %%xmm4")                /* xmm4 = xv1+xv3 xv3 ? ? */
                __ASM_EMIT("movhlps     %%xmm1, %%xmm5")                /* xmm5 = xa1+xa3 xa3 ? ? */
                __ASM_EMIT("movhlps     %%xmm2, %%xmm6")                /* xmm6 = xb1+xb3 xb3 ? ? */
                __ASM_EMIT("addss       %%xmm4, %%xmm0")                /* xmm0 = xv0+xv1+xv2+xv3 xv2+xv3 ? ? */
                __ASM_EMIT("addss       %%xmm5, %%xmm1")                /* xmm1 = xa0+xa1+xa2+xa3 xa2+xa3 ? ? */
                __ASM_EMIT("addss       %%xmm6, %%xmm2")                /* xmm2 = xb0+xb1+xb2+xb3 xb2+xb3 ? ? */
                /* 1x blocks */
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                /* xmm7 = a0 */
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")                /* xmm3 = a0*a0 */
                __ASM_EMIT("mulss       %%xmm5, %%xmm7")                /* xmm7 = a0*b0 */
                __ASM_EMIT("mulss       %%xmm5, %%xmm5")                /* xmm5 = b0*b0 */
                __ASM_EMIT("addss       %%xmm7, %%xmm0")                /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("addss       %%xmm3, %%xmm1")                /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("addss       %%xmm5, %%xmm2")                /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("add         $0x04, %[off]")                 /* ++off */
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                __ASM_EMIT("6:")
                /* Store result */
                __ASM_EMIT("movss       0x00(%[corr]), %%xmm4")
                __ASM_EMIT("movss       0x04(%[corr]), %%xmm5")
                __ASM_EMIT("movss       0x08(%[corr]), %%xmm6")
                __ASM_EMIT("addss       %%xmm4, %%xmm0")
                __ASM_EMIT("addss       %%xmm5, %%xmm1")
                __ASM_EMIT("addss       %%xmm6, %%xmm2")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[corr])")
                __ASM_EMIT("movss       %%xmm1, 0x04(%[corr])")
                __ASM_EMIT("movss       %%xmm2, 0x08(%[corr])")

                : [corr] "+r" (corr), [off] "=&r" (off), [count] "+r" (count)
                : [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_CORRELATION_H_ */
