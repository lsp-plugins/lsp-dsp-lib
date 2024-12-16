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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {

        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            ARCH_AARCH64_ASM(
                __ASM_EMIT("eor         v0.16b, v0.16b, v0.16b")    /* xv = 0 */
                __ASM_EMIT("eor         v1.16b, v1.16b, v1.16b")    /* xa = 0 */
                __ASM_EMIT("eor         v2.16b, v2.16b, v2.16b")    /* xb = 0 */
                /* 16x blocks */
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")    /* xv = 0 */
                __ASM_EMIT("eor         v4.16b, v4.16b, v4.16b")    /* xa = 0 */
                __ASM_EMIT("eor         v5.16b, v5.16b, v5.16b")    /* xb = 0 */
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q8, q9, [%[a], 0x00]")      /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldp         q10, q11, [%[a], 0x20]")    /* v10 = a2, v11 = a3 */
                __ASM_EMIT("ldp         q12, q13, [%[b], 0x00]")    /* v12 = b0, v13 = b1 */
                __ASM_EMIT("ldp         q14, q15, [%[b], 0x20]")    /* v14 = b2, v15 = b3 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v3.4s, v9.4s, v13.4s")      /* v3 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v4.4s, v9.4s, v9.4s")       /* v4 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v5.4s, v13.4s, v13.4s")     /* v5 = xb + b1*b1 */
                __ASM_EMIT("fmla        v0.4s, v10.4s, v14.4s")     /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v3.4s, v11.4s, v15.4s")     /* v3 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v10.4s, v10.4s")     /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v4.4s, v11.4s, v11.4s")     /* v4 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v14.4s, v14.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v5.4s, v15.4s, v15.4s")     /* v5 = xb + b1*b1 */
                __ASM_EMIT("subs        %[count], %[count], #16")
                __ASM_EMIT("add         %[a], %[a], #0x40")
                __ASM_EMIT("add         %[b], %[b], #0x40")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("adds        %[count], %[count], #8")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldp         q8, q9, [%[a], 0x00]")      /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldp         q12, q13, [%[b], 0x00]")    /* v12 = b0, v13 = b1 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("fmla        v0.4s, v9.4s, v13.4s")      /* v0 = xv + a1*b1 */
                __ASM_EMIT("fmla        v1.4s, v9.4s, v9.4s")       /* v1 = xa + a1*a1 */
                __ASM_EMIT("fmla        v2.4s, v13.4s, v13.4s")     /* v2 = xb + b1*b1 */
                __ASM_EMIT("sub         %[count], %[count], #8")
                __ASM_EMIT("add         %[a], %[a], #0x20")
                __ASM_EMIT("add         %[b], %[b], #0x20")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        6f")
                __ASM_EMIT("ldr         q8, [%[a], 0x00]")          /* v8 = a0, v9 = a1 */
                __ASM_EMIT("ldr         q12, [%[b], 0x00]")         /* v12 = b0, v13 = b1 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("add         %[a], %[a], #0x10")
                __ASM_EMIT("add         %[b], %[b], #0x10")
                __ASM_EMIT("6:")
                __ASM_EMIT("eor         v6.16b, v6.16b, v6.16b")    /* v6 = 0 */
                __ASM_EMIT("ext         v3.16b, v0.16b, v6.16b, #8")/* v3 = xv2 xv3 0 0 */
                __ASM_EMIT("ext         v4.16b, v1.16b, v6.16b, #8")/* v4 = xa2 xb3 0 0 */
                __ASM_EMIT("ext         v5.16b, v2.16b, v6.16b, #8")/* v5 = xa2 xb3 0 0 */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")       /* v0 = xv0+xv2 xv1+xv3 xv2 xv3 */
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")       /* v1 = xa0+xa2 xa1+xa3 xa2 xa3 */
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")       /* v2 = xb0+xb2 xb1+xb3 xb2 xb3 */
                __ASM_EMIT("ext         v3.16b, v0.16b, v6.16b, #4")/* v3 = xv1+xv3 xv2 xv3 0 */
                __ASM_EMIT("ext         v4.16b, v1.16b, v6.16b, #4")/* v4 = xa1+xa3 xa2 xa3 0 */
                __ASM_EMIT("ext         v5.16b, v2.16b, v6.16b, #4")/* v5 = xb1+xb3 xb2 xb3 0 */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")       /* v0 = xv0+xv1+xv2+xv3 xv1+xv2+xv3 xv2+xv3 xv3 */
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")       /* v1 = xa0+xa1+xa2+xa3 xa1+xa2+xa3 xa2+xa3 xa3 */
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")       /* v2 = xb0+xb1+xb2+xb3 xb1+xb2+xb3 xb2+xb3 xb3 */
                /* 1x blocks */
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("b.lt        8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r        {v8.4s}, [%[a]]")           /* v8 = a0 */
                __ASM_EMIT("ld1r        {v12.4s}, [%[b]]")          /* q12 = b0 */
                __ASM_EMIT("fmla        v0.4s, v8.4s, v12.4s")      /* v0 = xv + a0*b0 */
                __ASM_EMIT("fmla        v1.4s, v8.4s, v8.4s")       /* v1 = xa + a0*a0 */
                __ASM_EMIT("fmla        v2.4s, v12.4s, v12.4s")     /* v2 = xb + b0*b0 */
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("add         %[a], %[a], #0x04")
                __ASM_EMIT("add         %[b], %[b], #0x04")
                __ASM_EMIT("b.ge        7b")
                __ASM_EMIT("8:")
                /* Store result */
                __ASM_EMIT("ld3r        {v3.4s, v4.4s, v5.4s}, [%[corr]]")   /* v3 = v, v4 = a, v5 = b */
                __ASM_EMIT("fadd        v0.4s, v0.4s, v3.4s")
                __ASM_EMIT("fadd        v1.4s, v1.4s, v4.4s")
                __ASM_EMIT("fadd        v2.4s, v2.4s, v5.4s")
                __ASM_EMIT("st3         {v0.s, v1.s, v2.s}[0], [%[corr]]")

                : [a] "+r" (a), [b] "+r" (b), [count] "+r" (count)
                : [corr] "r" (corr)
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
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
            ARCH_AARCH64_ASM(
                __ASM_EMIT("ld3r        {v0.4s, v1.4s, v2.4s}, [%[corr]]")  /* v0 = xv, v1 = xa, v2 = xb */
                __ASM_EMIT("eor         v3.16b, v3.16b, v3.16b")            /* v3 = 0  */
                /* 8x blocks */
                __ASM_EMIT("subs        %[count], %[count], #8")
                __ASM_EMIT("b.lo        2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp         q4, q5, [%[a_head], 0x00]")         /* v4   = ah0, v5 = ah1 */
                __ASM_EMIT("ldp         q6, q7, [%[b_head], 0x00]")         /* v6   = ah0, v7 = ah1 */
                __ASM_EMIT("ldp         q8, q9, [%[a_tail], 0x00]")         /* v8   = ah0, v9 = ah1 */
                __ASM_EMIT("ldp         q10, q11, [%[b_tail], 0x00]")       /* v10  = ah0, v11 = ah1 */
                __ASM_EMIT("fmul        v12.4s, v4.4s, v6.4s")              /* v12  = ah0*bh0 */
                __ASM_EMIT("fmul        v13.4s, v5.4s, v7.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")               /* v4   = ah0*ah0 */
                __ASM_EMIT("fmul        v5.4s, v5.4s, v5.4s")
                __ASM_EMIT("fmul        v6.4s, v6.4s, v6.4s")               /* v6   = bh0*bh0 */
                __ASM_EMIT("fmul        v7.4s, v7.4s, v7.4s")
                __ASM_EMIT("fmls        v12.4s, v8.4s, v10.4s")             /* v12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("fmls        v13.4s, v9.4s, v11.4s")
                __ASM_EMIT("fmls        v4.4s, v8.4s, v8.4s")               /* v4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("fmls        v5.4s, v9.4s, v9.4s")
                __ASM_EMIT("fmls        v6.4s, v10.4s, v10.4s")             /* v6   = DB = bh0*bh0 - bt0*bt0 */
                __ASM_EMIT("fmls        v7.4s, v11.4s, v11.4s")

                __ASM_EMIT("ext         v14.16b, v3.16b, v12.16b, #8")      /* v14  = 0 0 DV[0] DV[1] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v13.16b, #8")      /* v15  = 0 0 DV[4] DV[5] */
                __ASM_EMIT("fadd        v12.4s, v12.4s, v14.4s")            /* v12  = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] */
                __ASM_EMIT("fadd        v13.4s, v13.4s, v15.4s")            /* v13  = DV[4] DV[5] DV[4]+DV[6] DV[5]+DV[7] */
                __ASM_EMIT("ext         v14.16b, v3.16b, v12.16b, #12")     /* v14  = 0 DV[0] DV[1] DV[0]+DV[2] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v13.16b, #12")     /* v15  = 0 DV[4] DV[5] DV[4]+DV[6] */
                __ASM_EMIT("fadd        v12.4s, v12.4s, v14.4s")            /* v12  = V[0..3] = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("fadd        v13.4s, v13.4s, v15.4s")            /* v13  = V[4..7] = DV[4] DV[4]+DV[5] DV[4]+DV[5]+DV[6] DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("ext         v14.16b, v3.16b, v4.16b, #8")       /* v14  = 0 0 DA[0] DA[1] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v5.16b, #8")       /* v15  = 0 0 DA[4] DA[5] */
                __ASM_EMIT("fadd        v4.4s, v4.4s, v14.4s")              /* v4   = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] */
                __ASM_EMIT("fadd        v5.4s, v5.4s, v15.4s")              /* v5   = DA[4] DA[5] DA[4]+DA[6] DA[5]+DA[7] */
                __ASM_EMIT("ext         v14.16b, v3.16b, v4.16b, #12")      /* v14  = 0 DA[0] DA[1] DA[0]+DA[2] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v5.16b, #12")      /* v15  = 0 DA[4] DA[5] DA[4]+DA[6] */
                __ASM_EMIT("fadd        v4.4s, v4.4s, v14.4s")              /* v4   = A[0..3] = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("fadd        v5.4s, v5.4s, v15.4s")              /* v5   = A[4..7] = DA[4] DA[4]+DA[5] DA[4]+DA[5]+DA[6] DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("ext         v14.16b, v3.16b, v6.16b, #8")       /* v14  = 0 0 DB[0] DB[1] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v7.16b, #8")       /* v15  = 0 0 DB[4] DB[5] */
                __ASM_EMIT("fadd        v6.4s, v6.4s, v14.4s")              /* v6   = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] */
                __ASM_EMIT("fadd        v7.4s, v7.4s, v15.4s")              /* v7   = DB[4] DB[5] DB[4]+DB[6] DB[5]+DB[7] */
                __ASM_EMIT("ext         v14.16b, v3.16b, v6.16b, #12")      /* v14  = 0 DB[0] DB[1] DB[0]+DB[2] */
                __ASM_EMIT("ext         v15.16b, v3.16b, v7.16b, #12")      /* v15  = 0 DB[4] DB[5] DB[4]+DB[6] */
                __ASM_EMIT("fadd        v6.4s, v6.4s, v14.4s")              /* v6   = B[0..3] = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] */
                __ASM_EMIT("fadd        v7.4s, v7.4s, v15.4s")              /* v7   = B[4..7] = DB[4] DB[4]+DB[5] DB[4]+DB[5]+DB[6] DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("dup         v8.4s, v12.s[3]")                   /* v8   = V[3] V[3] V[3] V[3] */
                __ASM_EMIT("dup         v9.4s, v4.s[3]")                    /* v9   = A[3] A[3] A[3] A[3] */
                __ASM_EMIT("dup         v10.4s, v6.s[3]")                   /* v10  = B[3] B[3] B[3] B[3] */
                __ASM_EMIT("fadd        v13.4s, v13.4s, v8.4s")             /* v13  = V[3]+V[4] V[3]+V[5] V[3]+V[6] V[3]+V[7] */
                __ASM_EMIT("fadd        v5.4s, v5.4s, v9.4s")               /* v5   = A[3]+A[4] A[3]+A[5] A[3]+A[6] A[3]+A[7] */
                __ASM_EMIT("fadd        v7.4s, v7.4s, v10.4s")              /* v7   = B[3]+B[4] B[3]+B[5] B[3]+B[6] B[3]+B[7] */

                __ASM_EMIT("fadd        v4.4s, v4.4s, v1.4s")               /* v4   = BA = xa + A */
                __ASM_EMIT("fadd        v5.4s, v5.4s, v1.4s")
                __ASM_EMIT("fadd        v6.4s, v6.4s, v2.4s")               /* v6   = BB = xb + B */
                __ASM_EMIT("fadd        v7.4s, v7.4s, v2.4s")
                __ASM_EMIT("fadd        v8.4s, v12.4s, v0.4s")              /* v8   = T = xv + V */
                __ASM_EMIT("fadd        v9.4s, v13.4s, v0.4s")
                __ASM_EMIT("fmul        v10.4s, v4.4s, v6.4s")              /* v10  = B = BA * BB */
                __ASM_EMIT("fmul        v11.4s, v5.4s, v7.4s")
                __ASM_EMIT("dup         v0.4s, v9.s[3]")                    /* v0   = xv' = T[7] */
                __ASM_EMIT("dup         v1.4s, v5.s[3]")                    /* v1   = xa' = BA[7] */
                __ASM_EMIT("dup         v2.4s, v7.s[3]")                    /* v2   = xb' = BB[7] */
                __ASM_EMIT("ldp         q14, q15, [%[CORR_CC]]")            /* v14  = threshold, v15 = threshold */

                __ASM_EMIT("fcmge       v14.4s, v8.4s, v14.4s")             /* v14  = T >= threshold */
                __ASM_EMIT("fcmge       v15.4s, v9.4s, v15.4s")
                __ASM_EMIT("frsqrte     v4.4s, v10.4s")                     /* v4   = x0 */
                __ASM_EMIT("frsqrte     v5.4s, v11.4s")
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x0 */
                __ASM_EMIT("fmul        v7.4s, v5.4s, v11.4s")
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("frsqrts     v13.4s, v7.4s, v5.4s")
                __ASM_EMIT("fmul        v4.4s, v4.4s, v12.4s")              /* v4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul        v5.4s, v5.4s, v13.4s")
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x1 */
                __ASM_EMIT("fmul        v7.4s, v5.4s, v11.4s")
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("frsqrts     v13.4s, v7.4s, v5.4s")
                __ASM_EMIT("fmul        v10.4s, v4.4s, v12.4s")             /* v10  = 1/svrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul        v11.4s, v5.4s, v13.4s")
                __ASM_EMIT("fmul        v10.4s, v8.4s, v10.4s")             /* v10  = T/svrtf(B) */
                __ASM_EMIT("fmul        v11.4s, v9.4s, v11.4s")
                __ASM_EMIT("and         v10.16b, v10.16b, v14.16b")         /* v10  = (T >= threshold) ? T/svrt(B) : 0 */
                __ASM_EMIT("and         v11.16b, v11.16b, v15.16b")
                __ASM_EMIT("add         %[a_head], %[a_head], #0x20")
                __ASM_EMIT("add         %[b_head], %[b_head], #0x20")
                __ASM_EMIT("subs        %[count], %[count], #8")
                __ASM_EMIT("stp         q10, q11, [%[dst], 0x00]")
                __ASM_EMIT("add         %[a_tail], %[a_tail], #0x20")
                __ASM_EMIT("add         %[b_tail], %[b_tail], #0x20")
                __ASM_EMIT("add         %[dst], %[dst], #0x20")
                __ASM_EMIT("b.hs        1b")
                __ASM_EMIT("2:")
                /* 4x block */
                __ASM_EMIT("adds        %[count], %[count], #4")
                __ASM_EMIT("b.lt        4f")
                __ASM_EMIT("ldr         q4, [%[a_head], 0x00]")             /* v4   = ah0 */
                __ASM_EMIT("ldr         q6, [%[b_head], 0x00]")             /* v6   = ah0 */
                __ASM_EMIT("ldr         q8, [%[a_tail], 0x00]")             /* v8   = ah0 */
                __ASM_EMIT("ldr         q10, [%[b_tail], 0x00]")            /* v10  = ah0 */
                __ASM_EMIT("fmul        v12.4s, v4.4s, v6.4s")              /* v12  = ah0*bh0 */
                __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")               /* v4   = ah0*ah0 */
                __ASM_EMIT("fmul        v6.4s, v6.4s, v6.4s")               /* v6   = bh0*bh0 */
                __ASM_EMIT("fmls        v12.4s, v8.4s, v10.4s")             /* v12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("fmls        v4.4s, v8.4s, v8.4s")               /* v4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("fmls        v6.4s, v10.4s, v10.4s")             /* v6   = DB = bh0*bh0 - bt0*bt0 */

                __ASM_EMIT("ext         v13.16b, v3.16b, v12.16b, #8")      /* v13  = 0 0 DV[0] DV[1] */
                __ASM_EMIT("ext         v5.16b, v3.16b, v4.16b, #8")        /* v5   = 0 0 DA[0] DA[1] */
                __ASM_EMIT("ext         v7.16b, v3.16b, v6.16b, #8")        /* v7   = 0 0 DB[0] DB[1] */
                __ASM_EMIT("fadd        v12.4s, v12.4s, v13.4s")            /* v12  = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] */
                __ASM_EMIT("fadd        v4.4s, v4.4s, v5.4s")               /* v4   = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] */
                __ASM_EMIT("fadd        v6.4s, v6.4s, v7.4s")               /* v6   = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] */
                __ASM_EMIT("ext         v13.16b, v3.16b, v12.16b, #12")     /* v13  = 0 DV[0] DV[1] DV[0]+DV[2] */
                __ASM_EMIT("ext         v5.16b, v3.16b, v4.16b, #12")       /* v5   = 0 DA[0] DA[1] DA[0]+DA[2] */
                __ASM_EMIT("ext         v7.16b, v3.16b, v6.16b, #12")       /* v7   = 0 DB[0] DB[1] DB[0]+DB[2] */
                __ASM_EMIT("fadd        v12.4s, v12.4s, v13.4s")            /* v12  = V[0..3] = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("fadd        v4.4s, v4.4s, v5.4s")               /* v4   = A[0..3] = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("fadd        v6.4s, v6.4s, v7.4s")               /* v6   = B[0..3] = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] */

                __ASM_EMIT("fadd        v4.4s, v4.4s, v1.4s")               /* v4   = BA = xa + A */
                __ASM_EMIT("fadd        v6.4s, v6.4s, v2.4s")               /* v6   = BB = xb + B */
                __ASM_EMIT("fadd        v8.4s, v12.4s, v0.4s")              /* v8   = T = xv + V */
                __ASM_EMIT("fmul        v10.4s, v4.4s, v6.4s")              /* v10  = B = BA * BB */
                __ASM_EMIT("dup         v1.4s, v4.s[3]")                    /* v1   = xa' = BA[7] */
                __ASM_EMIT("dup         v2.4s, v6.s[3]")                    /* v2   = xb' = BB[7] */
                __ASM_EMIT("dup         v0.4s, v8.s[3]")                    /* v0   = xv' = T[7] */
                __ASM_EMIT("ldr         q14, [%[CORR_CC]]")                 /* v14  = threshold */

                __ASM_EMIT("fcmge       v14.4s, v8.4s, v14.4s")             /* v14  = T >= threshold */
                __ASM_EMIT("frsqrte     v4.4s, v10.4s")                     /* v4   = x0 */
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x0 */
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul        v4.4s, v4.4s, v12.4s")              /* v4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x1 */
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul        v10.4s, v4.4s, v12.4s")             /* v10  = 1/svrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul        v10.4s, v8.4s, v10.4s")             /* v10  = T/svrtf(B) */
                __ASM_EMIT("and         v10.16b, v10.16b, v14.16b")         /* v10  = (T >= threshold) ? T/svrt(B) : 0 */
                __ASM_EMIT("add         %[a_head], %[a_head], #0x10")
                __ASM_EMIT("add         %[b_head], %[b_head], #0x10")
                __ASM_EMIT("sub         %[count], %[count], #4")
                __ASM_EMIT("str         q10, [%[dst], 0x00]")
                __ASM_EMIT("add         %[a_tail], %[a_tail], #0x10")
                __ASM_EMIT("add         %[b_tail], %[b_tail], #0x10")
                __ASM_EMIT("add         %[dst], %[dst], #0x10")
                __ASM_EMIT("4:")
                /* 1x blocks */
                __ASM_EMIT("adds        %[count], %[count], #3")
                __ASM_EMIT("blt         6f")
                __ASM_EMIT("ldr         q3, [%[CORR_CC]]")                  /* v3   = threshold */
                __ASM_EMIT("5:")
                __ASM_EMIT("ld1r        {v4.4s}, [%[a_head]]")              /* v4   = ah0 */
                __ASM_EMIT("ld1r        {v6.4s}, [%[b_head]]")              /* v6   = bh0 */
                __ASM_EMIT("ld1r        {v8.4s}, [%[a_tail]]")              /* v8   = at0 */
                __ASM_EMIT("ld1r        {v10.4s}, [%[b_tail]]")             /* v10  = bt0 */
                __ASM_EMIT("fmul        v12.4s, v4.4s, v6.4s")              /* v12  = ah0*bh0 */
                __ASM_EMIT("fmul        v4.4s, v4.4s, v4.4s")               /* v4   = ah0*ah0 */
                __ASM_EMIT("fmul        v6.4s, v6.4s, v6.4s")               /* v6   = bh0*bh0 */
                __ASM_EMIT("fmls        v12.4s, v8.4s, v10.4s")             /* v12  = DV = ah0*bh0 - at0*bt0 */
                __ASM_EMIT("fmls        v4.4s, v8.4s, v8.4s")               /* v4   = DA = ah0*ah0 - at0*at0 */
                __ASM_EMIT("fmls        v6.4s, v10.4s, v10.4s")             /* v6   = DB = bh0*bh0 - bt0*bt0 */

                __ASM_EMIT("fadd        v1.4s, v4.4s, v1.4s")               /* v1   = BA = xa + DA */
                __ASM_EMIT("fadd        v2.4s, v6.4s, v2.4s")               /* v2   = BB = xb + DB */
                __ASM_EMIT("fadd        v0.4s, v12.4s, v0.4s")              /* v0   = T = xv + DV */
                __ASM_EMIT("fmul        v10.4s, v1.4s, v2.4s")              /* v10  = B = BA * BB */

                __ASM_EMIT("fcmge       v14.4s, v0.4s, v3.4s")              /* v14  = T >= threshold */
                __ASM_EMIT("frsqrte     v4.4s, v10.4s")                     /* v4   = x0 */
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x0 */
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul        v4.4s, v4.4s, v12.4s")              /* v4   = x1 = x0 * (3 - R * x0 * x0) / 2 */
                __ASM_EMIT("fmul        v6.4s, v4.4s, v10.4s")              /* v6   = R * x1 */
                __ASM_EMIT("frsqrts     v12.4s, v6.4s, v4.4s")              /* v12  = (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul        v10.4s, v4.4s, v12.4s")             /* v10  = 1/svrtf(B) = x2 = x1 * (3 - R * x1 * x1) / 2 */
                __ASM_EMIT("fmul        v10.4s, v0.4s, v10.4s")             /* v10  = T/svrtf(B) */
                __ASM_EMIT("and         v10.16b, v10.16b, v14.16b")         /* v10  = (T >= threshold) ? T/svrt(B) : 0 */
                __ASM_EMIT("add         %[a_head], %[a_head], #0x04")
                __ASM_EMIT("add         %[b_head], %[b_head], #0x04")
                __ASM_EMIT("subs        %[count], %[count], #1")
                __ASM_EMIT("st1         {v10.s}[0], [%[dst]]")
                __ASM_EMIT("add         %[a_tail], %[a_tail], #0x04")
                __ASM_EMIT("add         %[b_tail], %[b_tail], #0x04")
                __ASM_EMIT("add         %[dst], %[dst], #0x04")
                __ASM_EMIT("b.ge        5b")
                __ASM_EMIT("6:")
                /* Store state */
                __ASM_EMIT("st3         {v0.s, v1.s, v2.s}[0], [%[corr]]")

                : [dst] "+r" (dst),
                  [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                  [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                  [count] "+r" (count)
                : [corr] "r" (corr),
                  [CORR_CC] "r" (&corr_const[0])
                : "cc", "memory",
                  "v0", "v1", "v2", "v3", "v4", "v5", "v6", "v7",
                  "v8", "v9", "v10", "v11", "v12", "v13", "v14", "v15"
            );
        }

    } /* namespace asimd */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_CORRELATION_H_ */
