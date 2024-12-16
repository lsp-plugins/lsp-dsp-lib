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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_CORRELATION_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {

        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("veor        q0, q0, q0")                /* xv = 0 */
                __ASM_EMIT("veor        q1, q1, q1")                /* xa = 0 */
                __ASM_EMIT("veor        q2, q2, q2")                /* xb = 0 */
                /* 16x blocks */
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("veor        q3, q3, q3")                /* xv = 0 */
                __ASM_EMIT("veor        q4, q4, q4")                /* xa = 0 */
                __ASM_EMIT("veor        q5, q5, q5")                /* xb = 0 */
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a]!, {q8-q11}")           /* q8 = a0, q9 = a1, q10 = a2, q11 = a3 */
                __ASM_EMIT("vldm        %[b]!, {q12-q15}")          /* q12 = b0, q13 = b1, q14 = b2, q15 = b3 */
                __ASM_EMIT("vmla.f32    q0, q8, q12")               /* q0 = xv + a0*b0 */
                __ASM_EMIT("vmla.f32    q3, q9, q13")               /* q3 = xv + a1*b1 */
                __ASM_EMIT("vmla.f32    q1, q8, q8")                /* q1 = xa + a0*a0 */
                __ASM_EMIT("vmla.f32    q4, q9, q9")                /* q4 = xa + a1*a1 */
                __ASM_EMIT("vmla.f32    q2, q12, q12")              /* q2 = xb + b0*b0 */
                __ASM_EMIT("vmla.f32    q5, q13, q13")              /* q5 = xb + b1*b1 */
                __ASM_EMIT("vmla.f32    q0, q10, q14")              /* q0 = xv + a2*b2 */
                __ASM_EMIT("vmla.f32    q3, q11, q15")              /* q3 = xv + a3*b3 */
                __ASM_EMIT("vmla.f32    q1, q10, q10")              /* q1 = xa + a2*a2 */
                __ASM_EMIT("vmla.f32    q4, q11, q11")              /* q4 = xa + a3*a3 */
                __ASM_EMIT("vmla.f32    q2, q14, q14")              /* q2 = xb + b2*b3 */
                __ASM_EMIT("vmla.f32    q5, q15, q15")              /* q5 = xb + b3*b3 */
                __ASM_EMIT("subs        %[count], #16")
                __ASM_EMIT("bhs         1b")
                __ASM_EMIT("vadd.f32    q0, q0, q3")
                __ASM_EMIT("vadd.f32    q1, q1, q4")
                __ASM_EMIT("vadd.f32    q2, q2, q5")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("adds        %[count], #8")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[a]!, {q8-q9}")            /* q8 = a0, q9 = a1 */
                __ASM_EMIT("vldm        %[b]!, {q12-q13}")          /* q12 = b0, q13 = b1 */
                __ASM_EMIT("vmla.f32    q0, q8, q12")               /* q0 = xv + a0*b0 */
                __ASM_EMIT("vmla.f32    q1, q8, q8")                /* q1 = xa + a0*a0 */
                __ASM_EMIT("vmla.f32    q2, q12, q12")              /* q2 = xb + b0*b0 */
                __ASM_EMIT("vmla.f32    q0, q9, q13")               /* q0 = xv + a1*b1 */
                __ASM_EMIT("vmla.f32    q1, q9, q9")                /* q1 = xa + a1*a1 */
                __ASM_EMIT("vmla.f32    q2, q13, q13")              /* q2 = xb + b1*b1 */
                __ASM_EMIT("sub         %[count], #8")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[a]!, {q8}")               /* q8 = a0 */
                __ASM_EMIT("vldm        %[b]!, {q12}")              /* q12 = b0 */
                __ASM_EMIT("vmla.f32    q0, q8, q12")               /* q0 = xv + a0*b0 */
                __ASM_EMIT("vmla.f32    q1, q8, q8")                /* q1 = xa + a0*a0 */
                __ASM_EMIT("vmla.f32    q2, q12, q12")              /* q2 = xb + b0*b0 */
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("6:")
                __ASM_EMIT("veor        q6, q6, q6")                /* q6 = 0 */
                __ASM_EMIT("vext.32     q3, q0, q6, #2")            /* q3 = xv2 xv3 0 0 */
                __ASM_EMIT("vext.32     q4, q1, q6, #2")            /* q4 = xa2 xa3 0 0 */
                __ASM_EMIT("vext.32     q5, q2, q6, #2")            /* q5 = xb2 xb3 0 0 */
                __ASM_EMIT("vadd.f32    q0, q0, q3")                /* q0 = xv0+xv2 xv1+xv3 xv2 xv3 */
                __ASM_EMIT("vadd.f32    q1, q1, q4")                /* q1 = xa0+xa2 xa1+xa3 xv2 xv3 */
                __ASM_EMIT("vadd.f32    q2, q2, q5")                /* q2 = xb0+xb2 xb1+xb3 xv2 xv3 */
                __ASM_EMIT("vext.32     q3, q0, q6, #1")            /* q3 = xv1+xv3 xv2 xv3 0 */
                __ASM_EMIT("vext.32     q4, q1, q6, #1")            /* q4 = xa1+xa3 xv2 xv3 0 */
                __ASM_EMIT("vext.32     q5, q2, q6, #1")            /* q5 = xb1+xb3 xv2 xv3 */
                __ASM_EMIT("vadd.f32    q0, q0, q3")                /* q0 = xv0+xv1+xv2+xv3 xv1+xv2+xv3 xv2+xv3 xv3 */
                __ASM_EMIT("vadd.f32    q1, q1, q4")                /* q1 = xa0+xa1+xa2+xa3 xa1+xa2+xa3 xa2+xa3 xa3 */
                __ASM_EMIT("vadd.f32    q2, q2, q5")                /* q2 = xb0+xb1+xb2+xb3 xb1+xb2+xb3 xb2+xb3 xb3 */
                /* 1x blocks */
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[a]]!")   /* q8 = a0 */
                __ASM_EMIT("vld1.32     {d24[], d25[]}, [%[b]]!")   /* q12 = b0 */
                __ASM_EMIT("vmla.f32    q0, q8, q12")               /* q0 = xv + a0*b0 */
                __ASM_EMIT("vmla.f32    q1, q8, q8")                /* q1 = xa + a0*a0 */
                __ASM_EMIT("vmla.f32    q2, q12, q12")              /* q2 = xb + b0*b0 */
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("bge         7b")
                __ASM_EMIT("8:")
                /* Store result */
                __ASM_EMIT("vld3.32     {d6[], d8[], d10[]}, [%[corr]]")   /* q3 = v, q4 = a, q5 = b */
                __ASM_EMIT("vadd.f32    q0, q0, q3")
                __ASM_EMIT("vadd.f32    q1, q1, q4")
                __ASM_EMIT("vadd.f32    q2, q2, q5")
                __ASM_EMIT("vst3.32     {d0[0], d2[0], d4[0]}, [%[corr]]")

                : [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
                : [corr] "r" (corr)
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

        static const float corr_const[] __lsp_aligned16 =
        {
            LSP_DSP_VEC8(1e-18f)
        };

        void corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count)
        {
            ARCH_ARM_ASM(
                __ASM_EMIT("vld3.32     {d0[], d2[], d4[]}, [%[corr]]")
                __ASM_EMIT("vld3.32     {d1[], d3[], d5[]}, [%[corr]]") /* q0 = xv, q1 = xa, q2 = xb */
                __ASM_EMIT("veor        q3, q3, q3")                    /* q3 = 0  */
                /* 8x blocks */
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("blo         2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm        %[a_head]!, {q4-q5}")           /* q4   = ah0, q5 = ah1 */
                __ASM_EMIT("vldm        %[b_head]!, {q6-q7}")           /* q6   = bh0, q7 = bh1 */
                __ASM_EMIT("vldm        %[a_tail]!, {q8-q9}")           /* q8   = at0, q9 = at1 */
                __ASM_EMIT("vldm        %[b_tail]!, {q10-q11}")         /* q10  = bt0, q11 = bt1 */
                __ASM_EMIT("vmul.f32    q12, q4, q6")                   /* q12  = ah0*bh0 */
                __ASM_EMIT("vmul.f32    q13, q5, q7")
                __ASM_EMIT("vmul.f32    q4, q4, q4")                    /* q4   = ah0*ah0 */
                __ASM_EMIT("vmul.f32    q5, q5, q5")
                __ASM_EMIT("vmul.f32    q6, q6, q6")                    /* q6   = bh0*bh0 */
                __ASM_EMIT("vmul.f32    q7, q7, q7")
                __ASM_EMIT("vmls.f32    q12, q8, q10")                  /* q12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("vmls.f32    q13, q9, q11")
                __ASM_EMIT("vmls.f32    q4, q8, q8")                    /* q4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("vmls.f32    q5, q9, q9")
                __ASM_EMIT("vmls.f32    q6, q10, q10")                  /* q6   = DB = bh0*bh0 - bt0*bt0 */
                __ASM_EMIT("vmls.f32    q7, q11, q11")

                __ASM_EMIT("vext.32     q14, q3, q12, #2")              /* q14  = 0 0 DV[0] DV[1] */
                __ASM_EMIT("vext.32     q15, q3, q13, #2")              /* q15  = 0 0 DV[4] DV[5] */
                __ASM_EMIT("vadd.f32    q12, q12, q14")                 /* q12  = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] */
                __ASM_EMIT("vadd.f32    q13, q13, q15")                 /* q13  = DV[4] DV[5] DV[4]+DV[6] DV[5]+DV[7] */
                __ASM_EMIT("vext.32     q14, q3, q12, #3")              /* q14  = 0 DV[0] DV[1] DV[0]+DV[2] */
                __ASM_EMIT("vext.32     q15, q3, q13, #3")              /* q15  = 0 DV[4] DV[5] DV[4]+DV[6] */
                __ASM_EMIT("vadd.f32    q12, q12, q14")                 /* q12  = V[0..3] = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("vadd.f32    q13, q13, q15")                 /* q13  = V[4..7] = DV[4] DV[4]+DV[5] DV[4]+DV[5]+DV[6] DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vext.32     q14, q3, q4, #2")               /* q14  = 0 0 DA[0] DA[1] */
                __ASM_EMIT("vext.32     q15, q3, q5, #2")               /* q15  = 0 0 DA[4] DA[5] */
                __ASM_EMIT("vadd.f32    q4, q4, q14")                   /* q4   = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] */
                __ASM_EMIT("vadd.f32    q5, q5, q15")                   /* q5   = DA[4] DA[5] DA[4]+DA[6] DA[5]+DA[7] */
                __ASM_EMIT("vext.32     q14, q3, q4, #3")               /* q14  = 0 DA[0] DA[1] DA[0]+DA[2] */
                __ASM_EMIT("vext.32     q15, q3, q5, #3")               /* q15  = 0 DA[4] DA[5] DA[4]+DA[6] */
                __ASM_EMIT("vadd.f32    q4, q4, q14")                   /* q4   = A[0..3] = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("vadd.f32    q5, q5, q15")                   /* q5   = A[4..7] = DA[4] DA[4]+DA[5] DA[4]+DA[5]+DA[6] DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vext.32     q14, q3, q6, #2")               /* q14  = 0 0 DB[0] DB[1] */
                __ASM_EMIT("vext.32     q15, q3, q7, #2")               /* q15  = 0 0 DB[4] DB[5] */
                __ASM_EMIT("vadd.f32    q6, q6, q14")                   /* q6   = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] */
                __ASM_EMIT("vadd.f32    q7, q7, q15")                   /* q7   = DB[4] DB[5] DB[4]+DB[6] DB[5]+DB[7] */
                __ASM_EMIT("vext.32     q14, q3, q6, #3")               /* q14  = 0 DB[0] DB[1] DB[0]+DB[2] */
                __ASM_EMIT("vext.32     q15, q3, q7, #3")               /* q15  = 0 DB[4] DB[5] DB[4]+DB[6] */
                __ASM_EMIT("vadd.f32    q6, q6, q14")                   /* q6   = B[0..3] = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] */
                __ASM_EMIT("vadd.f32    q7, q7, q15")                   /* q7   = B[4..7] = DB[4] DB[4]+DB[5] DB[4]+DB[5]+DB[6] DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vdup.32     q8, d25[1]")                    /* q8   = V[3] V[3] V[3] V[3] */
                __ASM_EMIT("vdup.32     q9, d9[1]")                     /* q9   = A[3] A[3] A[3] A[3] */
                __ASM_EMIT("vdup.32     q10, d13[1]")                   /* q10  = B[3] B[3] B[3] B[3] */
                __ASM_EMIT("vadd.f32    q13, q13, q8")                  /* q13  = V[3]+V[4] V[3]+V[5] V[3]+V[6] V[3]+V[7] */
                __ASM_EMIT("vadd.f32    q5, q5, q9")                    /* q5   = A[3]+A[4] A[3]+A[5] A[3]+A[6] A[3]+A[7] */
                __ASM_EMIT("vadd.f32    q7, q7, q10")                   /* q7   = B[3]+B[4] B[3]+B[5] B[3]+B[6] B[3]+B[7] */

                __ASM_EMIT("vadd.f32    q4, q4, q1")                    /* q4   = BA = xa + A */
                __ASM_EMIT("vadd.f32    q5, q5, q1")
                __ASM_EMIT("vadd.f32    q6, q6, q2")                    /* q6   = BB = xb + B */
                __ASM_EMIT("vadd.f32    q7, q7, q2")
                __ASM_EMIT("vadd.f32    q8, q12, q0")                   /* q8   = T = xv + V */
                __ASM_EMIT("vadd.f32    q9, q13, q0")
                __ASM_EMIT("vmul.f32    q10, q4, q6")                   /* q10  = B = BA * BB */
                __ASM_EMIT("vmul.f32    q11, q5, q7")
                __ASM_EMIT("vdup.32     q0, d19[1]")                    /* q4   = xv' = T[7] */
                __ASM_EMIT("vdup.32     q1, d11[1]")                    /* q5   = xa' = BA[7] */
                __ASM_EMIT("vdup.32     q2, d15[1]")                    /* q6   = xb' = BB[7] */
                __ASM_EMIT("vldm        %[CORR_CC], {q14-q15}")         /* q14  = threshold, q15 = threshold */

                __ASM_EMIT("vcge.f32    q14, q8, q14")                  /* q14  = T >= threshold */
                __ASM_EMIT("vcge.f32    q15, q9, q15")
                __ASM_EMIT("vrsqrte.f32 q4, q10")                       /* q4   = x0 */
                __ASM_EMIT("vrsqrte.f32 q5, q11")
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x0 */
                __ASM_EMIT("vmul.f32    q7, q5, q11")
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vrsqrts.f32 q13, q7, q5")
                __ASM_EMIT("vmul.f32    q4, q4, q12")                   /* q4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32    q5, q5, q13")
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x1 */
                __ASM_EMIT("vmul.f32    q7, q5, q11")
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vrsqrts.f32 q13, q7, q5")
                __ASM_EMIT("vmul.f32    q10, q4, q12")                  /* q10  = 1/sqrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32    q11, q5, q13")
                __ASM_EMIT("vmul.f32    q10, q8, q10")                  /* q10  = T/sqrtf(B) */
                __ASM_EMIT("vmul.f32    q11, q9, q11")
                __ASM_EMIT("vand        q10, q10, q14")                 /* q10  = (T >= threshold) ? T/sqrt(B) : 0 */
                __ASM_EMIT("vand        q11, q11, q15")
                __ASM_EMIT("subs        %[count], #8")
                __ASM_EMIT("vstm        %[dst]!, {q10-q11}")
                __ASM_EMIT("bhs         1b")
                __ASM_EMIT("2:")
                /* 4x block */
                __ASM_EMIT("adds        %[count], #4")
                __ASM_EMIT("blt         4f")
                __ASM_EMIT("vldm        %[a_head]!, {q4}")              /* q4   = ah0 */
                __ASM_EMIT("vldm        %[b_head]!, {q6}")              /* q6   = bh0 */
                __ASM_EMIT("vldm        %[a_tail]!, {q8}")              /* q8   = at0 */
                __ASM_EMIT("vldm        %[b_tail]!, {q10}")             /* q10  = bt0 */
                __ASM_EMIT("vmul.f32    q12, q4, q6")                   /* q12  = ah0*bh0 */
                __ASM_EMIT("vmul.f32    q4, q4, q4")                    /* q4   = ah0*ah0 */
                __ASM_EMIT("vmul.f32    q6, q6, q6")                    /* q6   = bh0*bh0 */
                __ASM_EMIT("vmls.f32    q12, q8, q10")                  /* q12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("vmls.f32    q4, q8, q8")                    /* q4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("vmls.f32    q6, q10, q10")                  /* q6   = DB = bh0*bh0 - bt0*bt0 */

                __ASM_EMIT("vext.32     q13, q3, q12, #2")              /* q13  = 0 0 DV[0] DV[1] */
                __ASM_EMIT("vext.32     q5, q3, q4, #2")                /* q5   = 0 0 DA[0] DA[1] */
                __ASM_EMIT("vext.32     q7, q3, q6, #2")                /* q7   = 0 0 DB[0] DB[1] */
                __ASM_EMIT("vadd.f32    q12, q12, q13")                 /* q12  = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] */
                __ASM_EMIT("vadd.f32    q4, q4, q5")                    /* q4   = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] */
                __ASM_EMIT("vadd.f32    q6, q6, q7")                    /* q6   = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] */
                __ASM_EMIT("vext.32     q13, q3, q12, #3")              /* q13  = 0 DV[0] DV[1] DV[0]+DV[2] */
                __ASM_EMIT("vext.32     q5, q3, q4, #3")                /* q5   = 0 DA[0] DA[1] DA[0]+DA[2] */
                __ASM_EMIT("vext.32     q7, q3, q6, #3")                /* q7  = 0 DB[0] DB[1] DB[0]+DB[2] */
                __ASM_EMIT("vadd.f32    q12, q12, q13")                 /* q12  = V = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("vadd.f32    q4, q4, q5")                    /* q4   = A = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("vadd.f32    q6, q6, q7")                    /* q6   = B = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] */

                __ASM_EMIT("vadd.f32    q4, q4, q1")                    /* q4   = BA = xa + A */
                __ASM_EMIT("vadd.f32    q6, q6, q2")                    /* q6   = BB = xb + B */
                __ASM_EMIT("vadd.f32    q8, q12, q0")                   /* q8   = T = xv + V */
                __ASM_EMIT("vmul.f32    q10, q4, q6")                   /* q10  = B = BA * BB */
                __ASM_EMIT("vdup.32     q1, d9[1]")                     /* q1   = xa' = BA[3] */
                __ASM_EMIT("vdup.32     q2, d13[1]")                    /* q2   = xb' = BB[3] */
                __ASM_EMIT("vdup.32     q0, d17[1]")                    /* q0   = xv' = T[3] */
                __ASM_EMIT("vldm        %[CORR_CC], {q14}")             /* q14  = threshold */

                __ASM_EMIT("vcge.f32    q14, q8, q14")                  /* q14  = T >= threshold */
                __ASM_EMIT("vrsqrte.f32 q4, q10")                       /* q4   = x0 */
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x0 */
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32    q4, q4, q12")                   /* q4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x1 */
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32    q10, q4, q12")                  /* q10  = 1/sqrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32    q10, q8, q10")                  /* q10  = T/sqrtf(B) */
                __ASM_EMIT("vand        q10, q10, q14")                 /* q10  = (T >= threshold) ? T/sqrt(B) : 0 */
                __ASM_EMIT("sub         %[count], #4")
                __ASM_EMIT("vstm        %[dst]!, {q10}")
                __ASM_EMIT("4:")
                /* 1x blocks */
                __ASM_EMIT("adds        %[count], #3")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("vldm        %[CORR_CC], {q3}")              /* q3   = threshold */
                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32     {d8[], d9[]}, [%[a_head]]!")    /* q4   = ah0 */
                __ASM_EMIT("vld1.32     {d12[], d13[]}, [%[b_head]]!")  /* q6   = bh0 */
                __ASM_EMIT("vld1.32     {d16[], d17[]}, [%[a_tail]]!")  /* q8   = at0 */
                __ASM_EMIT("vld1.32     {d20[], d21[]}, [%[b_tail]]!")  /* q10  = bt0 */
                __ASM_EMIT("vmul.f32    q12, q4, q6")                   /* q12  = ah0*bh0 */
                __ASM_EMIT("vmul.f32    q4, q4, q4")                    /* q4   = ah0*ah0 */
                __ASM_EMIT("vmul.f32    q6, q6, q6")                    /* q6   = bh0*bh0 */
                __ASM_EMIT("vmls.f32    q12, q8, q10")                  /* q12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("vmls.f32    q4, q8, q8")                    /* q4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("vmls.f32    q6, q10, q10")                  /* q6   = DB = bh0*bh0 - bt0*bt0 */

                __ASM_EMIT("vadd.f32    q1, q4, q1")                    /* q1   = BA = xa + DA */
                __ASM_EMIT("vadd.f32    q2, q6, q2")                    /* q2   = BB = xb + DB */
                __ASM_EMIT("vadd.f32    q0, q12, q0")                   /* q0   = T = xv + DV */
                __ASM_EMIT("vmul.f32    q10, q1, q2")                   /* q10  = B = BA * BB */

                __ASM_EMIT("vcge.f32    q14, q0, q3")                   /* q14  = T >= threshold */
                __ASM_EMIT("vrsqrte.f32 q4, q10")                       /* q4   = x0 */
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x0 */
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32    q4, q4, q12")                   /* q4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("vmul.f32    q6, q4, q10")                   /* q6   = R * x1 */
                __ASM_EMIT("vrsqrts.f32 q12, q6, q4")                   /* q12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32    q10, q4, q12")                  /* q10  = 1/sqrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("vmul.f32    q10, q0, q10")                  /* q10  = T/sqrtf(B) */
                __ASM_EMIT("vand        q10, q10, q14")                 /* q10  = (T >= threshold) ? T/sqrt(B) : 0 */
                __ASM_EMIT("subs        %[count], #1")
                __ASM_EMIT("vst1.32     {d20[0]}, [%[dst]]!")
                __ASM_EMIT("bge         5b")
                __ASM_EMIT("6:")
                /* Store state */
                __ASM_EMIT("vst3.32     {d0[0], d2[0], d4[0]}, [%[corr]]")

                : [dst] "+r" (dst),
                  [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                  [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                  [count] "+r" (count)
                : [corr] "r" (corr),
                  [CORR_CC] "r" (&corr_const[0])
                : "cc", "memory",
                  "q0", "q1", "q2", "q3", "q4", "q5", "q6", "q7",
                  "q8", "q9", "q10", "q11", "q12", "q13", "q14", "q15"
            );
        }

    } /* namespace neon_d32 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_CORRELATION_H_ */
