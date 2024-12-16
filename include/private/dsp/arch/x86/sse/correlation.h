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

        static const float corr_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(1e-18f)
        };

        void corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count)
        {
            IF_ARCH_I386(
                void *ptr;
            );

            ARCH_X86_ASM
            (
                /* 4x blocks */
                __ASM_EMIT32("subl      $4, %[count]")
                __ASM_EMIT64("sub       $4, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a_head]), %%xmm0")       /* xmm0 = ah */
                __ASM_EMIT("movups      0x00(%[b_head]), %%xmm1")       /* xmm1 = bh */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                /* xmm2 = ah */
                __ASM_EMIT("movups      0x00(%[a_tail]), %%xmm3")       /* xmm3 = at */
                __ASM_EMIT("movups      0x00(%[b_tail]), %%xmm4")       /* xmm4 = bt */
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")                /* xmm5 = at */
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                /* xmm2 = ah*bh */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                /* xmm0 = ah*ah */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                /* xmm1 = bh*bh */
                __ASM_EMIT("mulps       %%xmm4, %%xmm5")                /* xmm5 = at*bt */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                /* xmm3 = at*at */
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")                /* xmm4 = bt*bt */
                __ASM_EMIT("subps       %%xmm3, %%xmm0")                /* xmm0 = DA = ah*ah - at*at */
                __ASM_EMIT("subps       %%xmm4, %%xmm1")                /* xmm1 = DB = bh*bh - bt*bt */
                __ASM_EMIT("subps       %%xmm5, %%xmm2")                /* xmm2 = DV = ah*bh - at*bt */

                __ASM_EMIT("xorps       %%xmm3, %%xmm3")                /* xmm3 = 0 0 0 0 */
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                /* xmm4 = 0 0 0 0 */
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                /* xmm5 = 0 0 0 0 */
                __ASM_EMIT("movlhps     %%xmm0, %%xmm3")                /* xmm3 = 0 0 DA[0] DA[1] */
                __ASM_EMIT("movlhps     %%xmm1, %%xmm4")                /* xmm4 = 0 0 DB[0] DB[1] */
                __ASM_EMIT("movlhps     %%xmm2, %%xmm5")                /* xmm5 = 0 0 DV[0] DV[1] */
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                /* xmm0 = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] */
                __ASM_EMIT("addps       %%xmm4, %%xmm1")                /* xmm1 = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] */
                __ASM_EMIT("addps       %%xmm5, %%xmm2")                /* xmm2 = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] */
                __ASM_EMIT("shufps      $0x99, %%xmm0, %%xmm3")         /* xmm3 = 0 DA[0] DA[1] DA[0]+DA[2] */
                __ASM_EMIT("shufps      $0x99, %%xmm1, %%xmm4")         /* xmm4 = 0 DB[0] DB[1] DB[0]+DB[2] */
                __ASM_EMIT("shufps      $0x99, %%xmm2, %%xmm5")         /* xmm5 = 0 DV[0] DV[1] DV[0]+DV[2] */
                __ASM_EMIT("addps       %%xmm0, %%xmm3")                /* xmm3 = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("addps       %%xmm1, %%xmm4")                /* xmm4 = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] */
                __ASM_EMIT("addps       %%xmm2, %%xmm5")                /* xmm5 = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] */

                __ASM_EMIT32("mov       %[corr], %[ptr]")
                __ASM_EMIT32("movss     0x00(%[ptr]), %%xmm0")
                __ASM_EMIT32("movss     0x04(%[ptr]), %%xmm1")
                __ASM_EMIT32("movss     0x08(%[ptr]), %%xmm2")
                __ASM_EMIT64("movss     0x00(%[corr]), %%xmm0")
                __ASM_EMIT64("movss     0x04(%[corr]), %%xmm1")
                __ASM_EMIT64("movss     0x08(%[corr]), %%xmm2")

                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")         /* xmm0 = xv xv xv xv */
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")         /* xmm1 = xa xa xa xa */
                __ASM_EMIT("shufps      $0x00, %%xmm2, %%xmm2")         /* xmm2 = xb xb xb xb */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")                /* xmm1 = BA = xa+DA[0] xa+DA[0]+DA[1] xa+DA[0]+DA[1]+DA[2] xa+DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                /* xmm2 = BB = xb+DV[0] xb+DV[0]+DV[1] xb+DV[0]+DV[1]+DV[2] xb+DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("movaps      %%xmm1, %%xmm6")                /* xmm6 = BA */
                __ASM_EMIT("addps       %%xmm5, %%xmm0")                /* xmm0 = T = xv+DV[0] xv+DV[0]+DV[1] xv+DV[0]+DV[1]+DV[2] xv+DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("mulps       %%xmm2, %%xmm1")                /* xmm1 = B = BA*BB */
                __ASM_EMIT("movaps      %%xmm0, %%xmm5")                /* xmm5 = T */
                __ASM_EMIT("shufps      $0xff, %%xmm6, %%xmm6")         /* xmm6 = BA[3] BA[3] BA[3] BA[3] */
                __ASM_EMIT("shufps      $0xff, %%xmm5, %%xmm5")         /* xmm5 = T[3] T[3] T[3] T[3] */
                __ASM_EMIT("shufps      $0xff, %%xmm2, %%xmm2")         /* xmm7 = BB[3] BB[3] BB[3] BB[3] */

                __ASM_EMIT32("movss     %%xmm5, 0x00(%[ptr])")
                __ASM_EMIT32("movss     %%xmm6, 0x04(%[ptr])")
                __ASM_EMIT32("movss     %%xmm2, 0x08(%[ptr])")
                __ASM_EMIT64("movss     %%xmm5, 0x00(%[corr])")
                __ASM_EMIT64("movss     %%xmm6, 0x04(%[corr])")
                __ASM_EMIT64("movss     %%xmm2, 0x08(%[corr])")

                __ASM_EMIT("sqrtps      %%xmm1, %%xmm7")                /* xmm7 = sqrtf(B) */
                __ASM_EMIT("cmpps       $5, %[CORR_CC], %%xmm1")        /* xmm1 = B >= threshold */
                __ASM_EMIT("divps       %%xmm7, %%xmm0")                /* xmm0 = T/sqrtf(B) */
                __ASM_EMIT32("mov       %[dst], %[ptr]")
                __ASM_EMIT("andps       %%xmm1, %%xmm0")                /* xmm0 = (B >= threshold) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add         $0x10, %[a_head]")              /* ++a_head */
                __ASM_EMIT("add         $0x10, %[b_head]")              /* ++b_head */
                __ASM_EMIT32("movups    %%xmm0, 0x00(%[ptr])")
                __ASM_EMIT64("movups    %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x10, %[a_tail]")              /* ++a_tail */
                __ASM_EMIT("add         $0x10, %[b_tail]")              /* ++b_tail */
                __ASM_EMIT32("add       $0x10, %[ptr]")
                __ASM_EMIT64("add       $0x10, %[dst]")
                __ASM_EMIT32("mov       %[ptr], %[dst]")
                __ASM_EMIT32("subl      $4, %[count]")
                __ASM_EMIT64("sub       $4, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* 1x blocks */
                __ASM_EMIT32("addl      $3, %[count]")
                __ASM_EMIT64("add       $3, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss       0x00(%[a_head]), %%xmm0")       /* xmm0 = ah */
                __ASM_EMIT("movss       0x00(%[b_head]), %%xmm1")       /* xmm1 = bh */
                __ASM_EMIT("movss       %%xmm0, %%xmm2")                /* xmm2 = ah */
                __ASM_EMIT("movss       0x00(%[a_tail]), %%xmm3")       /* xmm3 = at */
                __ASM_EMIT("movss       0x00(%[b_tail]), %%xmm4")       /* xmm4 = bt */
                __ASM_EMIT("movss       %%xmm3, %%xmm5")                /* xmm5 = at */
                __ASM_EMIT("mulss       %%xmm1, %%xmm2")                /* xmm2 = ah*bh */
                __ASM_EMIT("mulss       %%xmm0, %%xmm0")                /* xmm0 = ah*ah */
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")                /* xmm1 = bh*bh */
                __ASM_EMIT("mulss       %%xmm4, %%xmm5")                /* xmm5 = at*bt */
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")                /* xmm3 = at*at */
                __ASM_EMIT("mulss       %%xmm4, %%xmm4")                /* xmm4 = bt*bt */
                __ASM_EMIT("subss       %%xmm3, %%xmm0")                /* xmm0 = DA = ah*ah - at*at */
                __ASM_EMIT("subss       %%xmm4, %%xmm1")                /* xmm1 = DB = bh*bh - bt*bt */
                __ASM_EMIT("subss       %%xmm5, %%xmm2")                /* xmm2 = DV = ah*bh - at*bt */

                __ASM_EMIT32("mov       %[corr], %[ptr]")
                __ASM_EMIT32("movss     0x00(%[ptr]), %%xmm3")
                __ASM_EMIT32("movss     0x04(%[ptr]), %%xmm4")
                __ASM_EMIT32("movss     0x08(%[ptr]), %%xmm5")
                __ASM_EMIT64("movss     0x00(%[corr]), %%xmm3")
                __ASM_EMIT64("movss     0x04(%[corr]), %%xmm4")
                __ASM_EMIT64("movss     0x08(%[corr]), %%xmm5")

                __ASM_EMIT("addss       %%xmm4, %%xmm0")                /* xmm0 = BA = xa+DA */
                __ASM_EMIT("addss       %%xmm3, %%xmm2")                /* xmm2 = T = xv+DV */
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                /* xmm3 = BA */
                __ASM_EMIT("addss       %%xmm5, %%xmm1")                /* xmm1 = BB = xb+DB */
                __ASM_EMIT("mulss       %%xmm1, %%xmm3")                /* xmm3 = B = BA*BB */

                __ASM_EMIT32("movss     %%xmm2, 0x00(%[ptr])")
                __ASM_EMIT32("movss     %%xmm0, 0x04(%[ptr])")
                __ASM_EMIT32("movss     %%xmm1, 0x08(%[ptr])")
                __ASM_EMIT64("movss     %%xmm2, 0x00(%[corr])")
                __ASM_EMIT64("movss     %%xmm0, 0x04(%[corr])")
                __ASM_EMIT64("movss     %%xmm1, 0x08(%[corr])")

                __ASM_EMIT("sqrtss      %%xmm3, %%xmm7")                /* xmm7 = sqrtf(B) */
                __ASM_EMIT("cmpss       $5, %[CORR_CC], %%xmm3")        /* xmm3 = B >= threshold */
                __ASM_EMIT("divss       %%xmm7, %%xmm2")                /* xmm2 = T/sqrtf(B) */
                __ASM_EMIT32("mov       %[dst], %[ptr]")
                __ASM_EMIT("andps       %%xmm3, %%xmm2")                /* xmm0 = (B >= threshold) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add         $0x04, %[a_head]")              /* ++a_head */
                __ASM_EMIT("add         $0x04, %[b_head]")              /* ++b_head */
                __ASM_EMIT32("movss     %%xmm2, 0x00(%[ptr])")
                __ASM_EMIT64("movss     %%xmm2, 0x00(%[dst])")
                __ASM_EMIT32("add       $0x04, %[ptr]")
                __ASM_EMIT64("add       $0x04, %[dst]")
                __ASM_EMIT("add         $0x04, %[a_tail]")              /* ++a_tail */
                __ASM_EMIT("add         $0x04, %[b_tail]")              /* ++b_tail */
                __ASM_EMIT32("mov       %[ptr], %[dst]")
                __ASM_EMIT32("decl      %[count]")
                __ASM_EMIT64("dec       %[count]")
                __ASM_EMIT("jge         3b")
                __ASM_EMIT("4:")

                : __IF_32(
                    [ptr] "=&r" (ptr),
                    [corr] "+m" (corr), [dst] "+m" (dst),
                    [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                    [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                    [count] "+m" (count)
                  )
                  __IF_64(
                    [dst] "+r" (dst),
                    [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                    [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                    [count] "+r" (count)
                  )
                : __IF_64( [corr] "r" (corr), )
                  [CORR_CC] "o" (corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_CORRELATION_H_ */
