/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 мар. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_CORRELATION_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */

namespace lsp
{
    namespace sse3
    {
        static const float corr_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC4(1e-10f)
        };

        void x64_corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count)
        {
            ARCH_X86_64_ASM
            (
                /* load data */
                __ASM_EMIT("movss       0x00(%[corr]), %%xmm8")         /* xmm8  = xv */
                __ASM_EMIT("movss       0x04(%[corr]), %%xmm9")         /* xmm9  = xa */
                __ASM_EMIT("movss       0x08(%[corr]), %%xmm10")        /* xmm10 = xb */
                __ASM_EMIT("movaps      %[CORR_CC], %%xmm11")           /* xmm11 = 1e-10f */
                /* 4x blocks */
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("shufps      $0x00, %%xmm8, %%xmm8")
                __ASM_EMIT("shufps      $0x00, %%xmm9, %%xmm9")
                __ASM_EMIT("shufps      $0x00, %%xmm10, %%xmm10")
                __ASM_EMIT(".align      0x10")
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

                __ASM_EMIT("addps       %%xmm3, %%xmm9")                /* xmm9 = BA = xa+DA[0] xa+DA[0]+DA[1] xa+DA[0]+DA[1]+DA[2] xa+DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("addps       %%xmm4, %%xmm10")               /* xmm10 = BB = xb+DV[0] xb+DV[0]+DV[1] xb+DV[0]+DV[1]+DV[2] xb+DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("movaps      %%xmm9, %%xmm6")                /* xmm6 = BA */
                __ASM_EMIT("addps       %%xmm5, %%xmm8")                /* xmm8 = T = xv+DV[0] xv+DV[0]+DV[1] xv+DV[0]+DV[1]+DV[2] xv+DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("mulps       %%xmm10, %%xmm6")               /* xmm6 = B = BA*BB */
                __ASM_EMIT("movaps      %%xmm8, %%xmm5")                /* xmm5 = T */
                __ASM_EMIT("shufps      $0xff, %%xmm8, %%xmm8")         /* xmm8 = T[3] T[3] T[3] T[3] */
                __ASM_EMIT("shufps      $0xff, %%xmm9, %%xmm9")         /* xmm9 = BA[3] BA[3] BA[3] BA[3] */
                __ASM_EMIT("shufps      $0xff, %%xmm10, %%xmm10")       /* xmm10 = BB[3] BB[3] BB[3] BB[3] */

                __ASM_EMIT("sqrtps      %%xmm6, %%xmm7")                /* xmm7 = sqrtf(B) */
                __ASM_EMIT("cmpps       $5, %%xmm11, %%xmm6")           /* xmm6 = B >= 1e-10f */
                __ASM_EMIT("divps       %%xmm7, %%xmm5")                /* xmm5 = T/sqrtf(B) */
                __ASM_EMIT("andps       %%xmm6, %%xmm5")                /* xmm5 = (B >= 1e-10f) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add         $0x10, %[a_head]")              /* ++a_head */
                __ASM_EMIT("add         $0x10, %[b_head]")              /* ++b_head */
                __ASM_EMIT("movups      %%xmm5, 0x00(%[dst])")
                __ASM_EMIT("add         $0x10, %[a_tail]")              /* ++a_tail */
                __ASM_EMIT("add         $0x10, %[b_tail]")              /* ++b_tail */
                __ASM_EMIT("add         $0x10, %[dst]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* 1x blocks */
                __ASM_EMIT("add         $3, %[count]")
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

                __ASM_EMIT("addss       %%xmm0, %%xmm9")                /* xmm9 = BA = xa+DA */
                __ASM_EMIT("addss       %%xmm1, %%xmm10")               /* xmm10 = BB = xb+DB */
                __ASM_EMIT("movaps      %%xmm9, %%xmm3")                /* xmm3 = BA */
                __ASM_EMIT("addss       %%xmm2, %%xmm8")                /* xmm8 = T = xv+DV */
                __ASM_EMIT("mulss       %%xmm10, %%xmm3")               /* xmm3 = B = BA*BB */
                __ASM_EMIT("movaps      %%xmm8, %%xmm0")                /* xmm0 = T */

                __ASM_EMIT("sqrtss      %%xmm3, %%xmm7")                /* xmm7 = sqrtf(B) */
                __ASM_EMIT("cmpss       $5, %[CORR_CC], %%xmm3")        /* xmm3 = B >= 1e-10f */
                __ASM_EMIT("divss       %%xmm7, %%xmm0")                /* xmm2 = T/sqrtf(B) */
                __ASM_EMIT32("mov       %[dst], %[ptr]")
                __ASM_EMIT("andps       %%xmm3, %%xmm0")                /* xmm0 = (B >= 1e-10f) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add         $0x04, %[a_head]")              /* ++a_head */
                __ASM_EMIT("add         $0x04, %[b_head]")              /* ++b_head */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x04, %[a_tail]")              /* ++a_tail */
                __ASM_EMIT("add         $0x04, %[b_tail]")              /* ++b_tail */
                __ASM_EMIT("add         $0x04, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         3b")
                __ASM_EMIT("4:")

                /* Store result */
                __ASM_EMIT("movss       %%xmm8, 0x00(%[corr])")
                __ASM_EMIT("movss       %%xmm9, 0x04(%[corr])")
                __ASM_EMIT("movss       %%xmm10, 0x08(%[corr])")

                : [dst] "+r" (dst),
                  [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                  [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                  [count] "+r" (count)
                : [corr] "r" (corr),
                  [CORR_CC] "o" (corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm9", "%xmm10"
            );
        }

    } /* namespace sse3 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_SSE3_CORRELATION_H_ */
