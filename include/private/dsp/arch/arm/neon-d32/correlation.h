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

    } /* namespace neon_d32 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_CORRELATION_H_ */
