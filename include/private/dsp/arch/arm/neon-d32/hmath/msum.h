/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 23 нояб. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {
        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_ARM(
                register float result __asm__("s0");
                register float m __asm__("s1") = avg;
            );
            ARCH_ARM_ASM
            (
                // Register allocation:
                // q0       = accumulator
                // q1       = averaging coefficient
                // q3       = 0
                // q8-q11   = triangular sums
                // q12-q15  = temporaries
                __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("veor            q3, q3, q3")                // q3       = 0 0 0 0
                __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0       = v v v v
                __ASM_EMIT("blo             2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[b]!, {q8-q11}")           // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q15}")          // q15      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vsub.f32        q9, q9, q13")               // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vsub.f32        q10, q10, q14")             // q10      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("vsub.f32        q11, q11, q15")             // q11      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vext.32         q14, q3, q10, #2")          // q14      = 0 0 d8 d9
                __ASM_EMIT("vext.32         q15, q3, q11, #2")          // q15      = 0 0 d12 d13
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vext.32         q14, q3, q10, #3")          // q14      = 0 d8 d9 d8+d10
                __ASM_EMIT("vext.32         q15, q3, q11, #3")          // q15      = 0 d12 d13 d12+d14
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vdup.32         q14, d21[1]")               // q14      = p11 p11 p11 p11
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q11, q11, q14")             // q11      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("vdup.32         q12, d19[1]")               // q12      = s7 s7 s7 s7
                __ASM_EMIT("vadd.f32        q10, q10, q12")             // q10      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("vadd.f32        q11, q11, q12")             // q11      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vadd.f32        q10, q0, q10")              // q10      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("vadd.f32        q11, q0, q11")              // q11      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("vdup.32         q0, d23[1]")                // q0       = v' = v15 v15 v15 v15
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmul.f32        q10, q10, q1")              // q10      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("vmul.f32        q11, q11, q1")              // q11      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("vstm            %[dst]!, {q8-q11}")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[b]!, {q8-q9}")            // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q13}")          // q12      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vsub.f32        q9, q9, q13")               // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vdup.32         q0, d19[1]")                // q0       = v' = v7 v7 v7 v7
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q8-q9}")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vldm            %[b]!, {q8}")               // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12}")              // q12      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vdup.32         q0, d17[1]")                // q0       = v' = v3 v3 v3 v3
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q8}")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[b]]!")   // q8       = b
                __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[a]]!")   // q12      = a
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d = b - a
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0       = v' = v + d
                __ASM_EMIT("vmul.f32        q8, q0, q1")                // q8       = v' * m
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vst1.32         d16[0], [%[dst]]!")
                __ASM_EMIT("bge             7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [value] "+t" (value), [m] "+t" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q1", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );

            return result;
        }

        float moving_pavg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_ARM(
                register float result __asm__("s0");
                register float m __asm__("s1") = avg;
            );
            ARCH_ARM_ASM
            (
                // Register allocation:
                // q0       = accumulator
                // q1       = averaging coefficient
                // q3       = 0
                // q8-q11   = triangular sums
                // q12-q15  = temporaries
                __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("veor            q3, q3, q3")                // q3       = 0 0 0 0
                __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0       = v v v v
                __ASM_EMIT("blo             2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[b]!, {q8-q11}")           // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q15}")          // q15      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vsub.f32        q9, q9, q13")               // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vsub.f32        q10, q10, q14")             // q10      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("vsub.f32        q11, q11, q15")             // q11      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vext.32         q14, q3, q10, #2")          // q14      = 0 0 d8 d9
                __ASM_EMIT("vext.32         q15, q3, q11, #2")          // q15      = 0 0 d12 d13
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vext.32         q14, q3, q10, #3")          // q14      = 0 d8 d9 d8+d10
                __ASM_EMIT("vext.32         q15, q3, q11, #3")          // q15      = 0 d12 d13 d12+d14
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vdup.32         q14, d21[1]")               // q14      = p11 p11 p11 p11
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q11, q11, q14")             // q11      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("vdup.32         q12, d19[1]")               // q12      = s7 s7 s7 s7
                __ASM_EMIT("vadd.f32        q10, q10, q12")             // q10      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("vadd.f32        q11, q11, q12")             // q11      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vadd.f32        q10, q0, q10")              // q10      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("vadd.f32        q11, q0, q11")              // q11      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("vdup.32         q0, d23[1]")                // q0       = v' = v15 v15 v15 v15
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmul.f32        q10, q10, q1")              // q10      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("vmul.f32        q11, q11, q1")              // q11      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vmax.f32        q9, q9, q3")
                __ASM_EMIT("vmax.f32        q10, q10, q3")
                __ASM_EMIT("vmax.f32        q11, q11, q3")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("vstm            %[dst]!, {q8-q11}")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[b]!, {q8-q9}")            // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q13}")          // q12      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vsub.f32        q9, q9, q13")               // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vdup.32         q0, d19[1]")                // q0       = v' = v7 v7 v7 v7
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vmax.f32        q9, q9, q3")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q8-q9}")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vldm            %[b]!, {q8}")               // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12}")              // q12      = a0 a1 a2 a3
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vdup.32         q0, d17[1]")                // q0       = v' = v3 v3 v3 v3
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q8}")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[b]]!")   // q8       = b
                __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[a]]!")   // q12      = a
                __ASM_EMIT("vsub.f32        q8, q8, q12")               // q8       = d = b - a
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0       = v' = v + d
                __ASM_EMIT("vmul.f32        q8, q0, q1")                // q8       = v' * m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vst1.32         d16[0], [%[dst]]!")
                __ASM_EMIT("bge             7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [value] "+t" (value), [m] "+t" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q1", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );

            return result;
        }

        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_ARM(
                register float result __asm__("s0");
                register float m __asm__("s1") = avg;
            );
            ARCH_ARM_ASM
            (
                // Register allocation:
                // q0       = accumulator
                // q1       = averaging coefficient
                // q3       = 0
                // q8-q11   = triangular sums
                // q12-q15  = temporaries
                __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("veor            q3, q3, q3")                // q3       = 0 0 0 0
                __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0       = v v v v
                __ASM_EMIT("blo             2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[b]!, {q8-q11}")           // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q15}")          // q15      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmul.f32        q9, q9, q9")
                __ASM_EMIT("vmul.f32        q10, q10, q10")
                __ASM_EMIT("vmul.f32        q11, q11, q11")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vmls.f32        q9, q13, q13")              // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vmls.f32        q10, q14, q14")             // q10      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("vmls.f32        q11, q15, q15")             // q11      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vext.32         q14, q3, q10, #2")          // q14      = 0 0 d8 d9
                __ASM_EMIT("vext.32         q15, q3, q11, #2")          // q15      = 0 0 d12 d13
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vext.32         q14, q3, q10, #3")          // q14      = 0 d8 d9 d8+d10
                __ASM_EMIT("vext.32         q15, q3, q11, #3")          // q15      = 0 d12 d13 d12+d14
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vdup.32         q14, d21[1]")               // q14      = p11 p11 p11 p11
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q11, q11, q14")             // q11      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("vdup.32         q12, d19[1]")               // q12      = s7 s7 s7 s7
                __ASM_EMIT("vadd.f32        q10, q10, q12")             // q10      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("vadd.f32        q11, q11, q12")             // q11      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vadd.f32        q10, q0, q10")              // q10      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("vadd.f32        q11, q0, q11")              // q11      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("vdup.32         q0, d23[1]")                // q0       = v' = v15 v15 v15 v15
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmul.f32        q10, q10, q1")              // q10      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("vmul.f32        q11, q11, q1")              // q11      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("vstm            %[dst]!, {q8-q11}")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[b]!, {q8-q9}")            // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q13}")          // q12      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmul.f32        q9, q9, q9")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vmls.f32        q9, q13, q13")              // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vdup.32         q0, d19[1]")                // q0       = v' = v7 v7 v7 v7
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q8-q9}")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vldm            %[b]!, {q8}")               // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12}")              // q12      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vdup.32         q0, d17[1]")                // q0       = v' = v3 v3 v3 v3
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q8}")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[b]]!")   // q8       = b
                __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[a]]!")   // q12      = a
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d = b - a
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0       = v' = v + d
                __ASM_EMIT("vmul.f32        q8, q0, q1")                // q8       = v' * m
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vst1.32         d16[0], [%[dst]]!")
                __ASM_EMIT("bge             7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [value] "+t" (value), [m] "+t" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q1", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );

            return result;
        }

        float moving_mean_psqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_ARM(
                register float result __asm__("s0");
                register float m __asm__("s1") = avg;
            );
            ARCH_ARM_ASM
            (
                // Register allocation:
                // q0       = accumulator
                // q1       = averaging coefficient
                // q3       = 0
                // q8-q11   = triangular sums
                // q12-q15  = temporaries
                __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("veor            q3, q3, q3")                // q3       = 0 0 0 0
                __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0       = v v v v
                __ASM_EMIT("blo             2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[b]!, {q8-q11}")           // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q15}")          // q15      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmul.f32        q9, q9, q9")
                __ASM_EMIT("vmul.f32        q10, q10, q10")
                __ASM_EMIT("vmul.f32        q11, q11, q11")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vmls.f32        q9, q13, q13")              // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vmls.f32        q10, q14, q14")             // q10      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("vmls.f32        q11, q15, q15")             // q11      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vext.32         q14, q3, q10, #2")          // q14      = 0 0 d8 d9
                __ASM_EMIT("vext.32         q15, q3, q11, #2")          // q15      = 0 0 d12 d13
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vext.32         q14, q3, q10, #3")          // q14      = 0 d8 d9 d8+d10
                __ASM_EMIT("vext.32         q15, q3, q11, #3")          // q15      = 0 d12 d13 d12+d14
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vadd.f32        q10, q10, q14")             // q10      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("vadd.f32        q11, q11, q15")             // q11      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vdup.32         q14, d21[1]")               // q14      = p11 p11 p11 p11
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q11, q11, q14")             // q11      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("vdup.32         q12, d19[1]")               // q12      = s7 s7 s7 s7
                __ASM_EMIT("vadd.f32        q10, q10, q12")             // q10      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("vadd.f32        q11, q11, q12")             // q11      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vadd.f32        q10, q0, q10")              // q10      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("vadd.f32        q11, q0, q11")              // q11      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("vdup.32         q0, d23[1]")                // q0       = v' = v15 v15 v15 v15
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmul.f32        q10, q10, q1")              // q10      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("vmul.f32        q11, q11, q1")              // q11      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vmax.f32        q9, q9, q3")
                __ASM_EMIT("vmax.f32        q10, q10, q3")
                __ASM_EMIT("vmax.f32        q11, q11, q3")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("vstm            %[dst]!, {q8-q11}")
                __ASM_EMIT("bhs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[b]!, {q8-q9}")            // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12-q13}")          // q12      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmul.f32        q9, q9, q9")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vmls.f32        q9, q13, q13")              // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q13, q3, q9, #2")           // q13      = 0 0 d4 d5
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q13, q3, q9, #3")           // q13      = 0 d4 d5 d4+d6
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q13")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vdup.32         q12, d17[1]")               // q12      = s3 s3 s3 s3
                __ASM_EMIT("vadd.f32        q9, q9, q12")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vdup.32         q0, d19[1]")                // q0       = v' = v7 v7 v7 v7
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vmax.f32        q9, q9, q3")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q8-q9}")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("vldm            %[b]!, {q8}")               // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q12}")              // q12      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vext.32         q12, q3, q8, #2")           // q12      = 0 0 d0 d1
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vext.32         q12, q3, q8, #3")           // q12      = 0 d0 d1 d0+d2
                __ASM_EMIT("vadd.f32        q8, q8, q12")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vdup.32         q0, d17[1]")                // q0       = v' = v3 v3 v3 v3
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q8}")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[b]]!")   // q8       = b
                __ASM_EMIT("vld1.32         {d24[], d25[]}, [%[a]]!")   // q12      = a
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q12, q12")              // q8       = d = b - a
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0       = v' = v + d
                __ASM_EMIT("vmul.f32        q8, q0, q1")                // q8       = v' * m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vst1.32         d16[0], [%[dst]]!")
                __ASM_EMIT("bge             7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [value] "+t" (value), [m] "+t" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q1", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );

            return result;
        }

        float moving_rms(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_ARM(
                register float result __asm__("s0");
                register float m __asm__("s1") = avg;
            );
            ARCH_ARM_ASM
            (
                // Register allocation:
                // q0       = accumulator
                // q1       = averaging coefficient
                // q3       = 0
                // q8-q9    = triangular sums
                // q10-q15  = temporaries
                __ASM_EMIT("vdup.32         q1, d0[1]")                 // q1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("veor            q3, q3, q3")                // q3       = 0 0 0 0
                __ASM_EMIT("vdup.32         q0, d0[0]")                 // q0       = v v v v
                __ASM_EMIT("blo             2f")
                // x8 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("vldm            %[b]!, {q8-q9}")            // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q10-q11}")          // q10      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmul.f32        q9, q9, q9")
                __ASM_EMIT("vmls.f32        q8, q10, q10")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vmls.f32        q9, q11, q11")              // q9       = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("vext.32         q10, q3, q8, #2")           // q10      = 0 0 d0 d1
                __ASM_EMIT("vext.32         q11, q3, q9, #2")           // q11      = 0 0 d4 d5
                __ASM_EMIT("vadd.f32        q8, q8, q10")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vadd.f32        q9, q9, q11")               // q9       = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vext.32         q10, q3, q8, #3")           // q10      = 0 d0 d1 d0+d2
                __ASM_EMIT("vext.32         q11, q3, q9, #3")           // q11      = 0 d4 d5 d4+d6
                __ASM_EMIT("vadd.f32        q8, q8, q10")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q9, q9, q11")               // q9       = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vdup.32         q10, d17[1]")               // q10      = s3 s3 s3 s3
                __ASM_EMIT("vadd.f32        q9, q9, q10")               // q9       = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vadd.f32        q9, q0, q9")                // q9       = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("vdup.32         q0, d19[1]")                // q0       = v' = v7 v7 v7 v7
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmul.f32        q9, q9, q1")                // q9       = v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vmax.f32        q9, q9, q3")
                __ASM_EMIT("vrsqrte.f32     q10, q8")                   // q10      = x0
                __ASM_EMIT("vrsqrte.f32     q11, q9")
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x0
                __ASM_EMIT("vmul.f32        q13, q11, q9")
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vrsqrts.f32     q15, q13, q11")
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q11, q11, q15")
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x1
                __ASM_EMIT("vmul.f32        q13, q11, q9")
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vrsqrts.f32     q15, q13, q11")
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q11, q11, q15")
                __ASM_EMIT("vmul.f32        q8, q8, q10")               // q8       = R/sqrt(R) = sqrt(R)
                __ASM_EMIT("vmul.f32        q9, q9, q11")

                __ASM_EMIT("subs            %[count], %[count], #8")
                __ASM_EMIT("vstm            %[dst]!, {q8-q9}")
                __ASM_EMIT("bhs             1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("blt             4f")
                __ASM_EMIT("vldm            %[b]!, {q8}")               // q8       = b0 b1 b2 b3
                __ASM_EMIT("vldm            %[a]!, {q10}")              // q10      = a0 a1 a2 a3
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q10, q10")              // q8       = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("vext.32         q10, q3, q8, #2")           // q10      = 0 0 d0 d1
                __ASM_EMIT("vadd.f32        q8, q8, q10")               // q8       = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vext.32         q10, q3, q8, #3")           // q10      = 0 d0 d1 d0+d2
                __ASM_EMIT("vadd.f32        q8, q8, q10")               // q8       = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vadd.f32        q8, q0, q8")                // q8       = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("vdup.32         q0, d17[1]")                // q0       = v' = v3 v3 v3 v3
                __ASM_EMIT("vmul.f32        q8, q8, q1")                // q8       = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vrsqrte.f32     q10, q8")                   // q10      = x0
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x0
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x1
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q8, q8, q10")               // q8       = R/sqrt(R) = sqrt(R)
                __ASM_EMIT("vmul.f32        q9, q9, q11")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("vstm            %[dst]!, {q8}")
                // x1 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vld1.32         {d16[], d17[]}, [%[b]]!")   // q8       = b
                __ASM_EMIT("vld1.32         {d20[], d21[]}, [%[a]]!")   // q10      = a
                __ASM_EMIT("vmul.f32        q8, q8, q8")
                __ASM_EMIT("vmls.f32        q8, q10, q10")              // q8       = d = b - a
                __ASM_EMIT("vadd.f32        q0, q0, q8")                // q0       = v' = v + d
                __ASM_EMIT("vmul.f32        q8, q0, q1")                // q8       = v' * m
                __ASM_EMIT("vmax.f32        q8, q8, q3")
                __ASM_EMIT("vrsqrte.f32     q10, q8")                   // q10      = x0
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x0
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = x1 = x0 * (3 - R * x0 * x0) / 2
                __ASM_EMIT("vmul.f32        q12, q10, q8")              // q12      = R * x1
                __ASM_EMIT("vrsqrts.f32     q14, q12, q10")             // q14      = (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q10, q10, q14")             // q10      = 1/sqrt(R) = x2 = x1 * (3 - R * x1 * x1) / 2
                __ASM_EMIT("vmul.f32        q8, q8, q10")               // q8       = R/sqrt(R) = sqrt(R)
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("vst1.32         d16[0], [%[dst]]!")
                __ASM_EMIT("bge             5b")
                /* end */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [value] "+t" (value), [m] "+t" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "q1", "q3",
                  "q8", "q9", "q10", "q11",
                  "q12", "q13", "q14", "q15"
            );

            return result;
        }

    } /* namespace neon_d32 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_HMATH_MSUM_H_ */
