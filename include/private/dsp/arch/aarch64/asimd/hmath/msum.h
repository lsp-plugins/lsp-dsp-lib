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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {

        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_AARCH64(
                register float result __asm__("s0") = value;
                register float m __asm__("s1") = avg;
            );
            ARCH_AARCH64_ASM
            (
                // Register allocation:
                // v0       = accumulator
                // v1       = averaging coefficient
                // v3       = 0
                // v16-q19  = triangular sums
                // v20-q23  = temporaries
                __ASM_EMIT("dup             v1.4s, v1.s[0]")                // v1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3       = 0 0 0 0
                __ASM_EMIT("dup             v0.4s, v0.s[0]")                // v0       = v v v v
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("ldp             q18, q19, [%[b]], #0x20")       // v18      = b8 b9 b10 b11
                __ASM_EMIT("ldp             q22, q23, [%[a]], #0x20")       // v22      = a8 a9 a10 a11
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fsub            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("fsub            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("fsub            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #8")  // v22      = 0 0 d8 d9
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #8")  // v23      = 0 0 d12 d13
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #12") // v22      = 0 d8 d9 d8+d10
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #12") // v23      = 0 d12 d13 d12+d14
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("dup             v22.4s, v18.s[3]")              // v22      = p11 p11 p11 p11
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v19.4s, v19.4s, v22.4s")        // v19      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("dup             v20.4s, v17.s[3]")              // v20      = s7 s7 s7 s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v20.4s")        // v18      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v20.4s")        // v19      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v0.4s")         // v18      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v0.4s")         // v19      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("dup             v0.4s, v19.s[3]")               // v0       = v' = v15 v15 v15 v15
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmul            v18.4s, v18.4s, v1.4s")         // v18      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("fmul            v19.4s, v19.4s, v1.4s")         // v19      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q18, q19, [%[dst]], #0x20")
                __ASM_EMIT("b.hs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fsub            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("dup             v0.4s, v17.s[3]")               // v0       = v' = v7 v7 v7 v7
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q16, [%[b]], #0x10")            // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldr             q20, [%[a]], #0x10")            // v20      = a0 a1 a2 a3
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("dup             v0.4s, v16.s[3]")               // v0       = v' = v3 v3 v3 v3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q16, [%[dst]], #0x10")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[b]], #0x04")       // v16      = b
                __ASM_EMIT("ld1r            {v20.4s}, [%[a]], #0x04")       // v20      = a
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d = b - a
                __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          // v16      = v' = v + d
                __ASM_EMIT("fmul            v16.4s, v0.4s, v1.4s")          // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v16.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [result] "+w" (result), [m] "+w" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v3",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );

            return result;
        }

        float moving_pavg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_AARCH64(
                register float result __asm__("s0") = value;
                register float m __asm__("s1") = avg;
            );
            ARCH_AARCH64_ASM
            (
                // Register allocation:
                // v0       = accumulator
                // v1       = averaging coefficient
                // v3       = 0
                // v16-q19  = triangular sums
                // v20-q23  = temporaries
                __ASM_EMIT("dup             v1.4s, v1.s[0]")                // v1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3       = 0 0 0 0
                __ASM_EMIT("dup             v0.4s, v0.s[0]")                // v0       = v v v v
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("ldp             q18, q19, [%[b]], #0x20")       // v18      = b8 b9 b10 b11
                __ASM_EMIT("ldp             q22, q23, [%[a]], #0x20")       // v22      = a8 a9 a10 a11
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fsub            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("fsub            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("fsub            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #8")  // v22      = 0 0 d8 d9
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #8")  // v23      = 0 0 d12 d13
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #12") // v22      = 0 d8 d9 d8+d10
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #12") // v23      = 0 d12 d13 d12+d14
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("dup             v22.4s, v18.s[3]")              // v22      = p11 p11 p11 p11
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v19.4s, v19.4s, v22.4s")        // v19      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("dup             v20.4s, v17.s[3]")              // v20      = s7 s7 s7 s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v20.4s")        // v18      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v20.4s")        // v19      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v0.4s")         // v18      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v0.4s")         // v19      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("dup             v0.4s, v19.s[3]")               // v0       = v' = v15 v15 v15 v15
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmul            v18.4s, v18.4s, v1.4s")         // v18      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("fmul            v19.4s, v19.4s, v1.4s")         // v19      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("fmax            v18.4s, v18.4s, v3.4s")
                __ASM_EMIT("fmax            v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q18, q19, [%[dst]], #0x20")
                __ASM_EMIT("b.hs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fsub            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("dup             v0.4s, v17.s[3]")               // v0       = v' = v7 v7 v7 v7
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q16, [%[b]], #0x10")            // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldr             q20, [%[a]], #0x10")            // v20      = a0 a1 a2 a3
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("dup             v0.4s, v16.s[3]")               // v0       = v' = v3 v3 v3 v3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q16, [%[dst]], #0x10")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[b]], #0x04")       // v16      = b
                __ASM_EMIT("ld1r            {v20.4s}, [%[a]], #0x04")       // v20      = a
                __ASM_EMIT("fsub            v16.4s, v16.4s, v20.4s")        // v16      = d = b - a
                __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          // v16      = v' = v + d
                __ASM_EMIT("fmul            v16.4s, v0.4s, v1.4s")          // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v16.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [result] "+w" (result), [m] "+w" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v3",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );

            return result;
        }

        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_AARCH64(
                register float result __asm__("s0") = value;
                register float m __asm__("s1") = avg;
            );
            ARCH_AARCH64_ASM
            (
                // Register allocation:
                // v0       = accumulator
                // v1       = averaging coefficient
                // v3       = 0
                // v16-q19  = triangular sums
                // v20-q23  = temporaries
                __ASM_EMIT("dup             v1.4s, v1.s[0]")                // v1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3       = 0 0 0 0
                __ASM_EMIT("dup             v0.4s, v0.s[0]")                // v0       = v v v v
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("ldp             q18, q19, [%[b]], #0x20")       // v18      = b8 b9 b10 b11
                __ASM_EMIT("ldp             q22, q23, [%[a]], #0x20")       // v22      = a8 a9 a10 a11
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmul            v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul            v19.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("fmls            v18.4s, v22.4s, v22.4s")        // v18      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("fmls            v19.4s, v23.4s, v23.4s")        // v19      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #8")  // v22      = 0 0 d8 d9
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #8")  // v23      = 0 0 d12 d13
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #12") // v22      = 0 d8 d9 d8+d10
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #12") // v23      = 0 d12 d13 d12+d14
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("dup             v22.4s, v18.s[3]")              // v22      = p11 p11 p11 p11
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v19.4s, v19.4s, v22.4s")        // v19      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("dup             v20.4s, v17.s[3]")              // v20      = s7 s7 s7 s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v20.4s")        // v18      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v20.4s")        // v19      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v0.4s")         // v18      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v0.4s")         // v19      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("dup             v0.4s, v19.s[3]")               // v0       = v' = v15 v15 v15 v15
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmul            v18.4s, v18.4s, v1.4s")         // v18      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("fmul            v19.4s, v19.4s, v1.4s")         // v19      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q18, q19, [%[dst]], #0x20")
                __ASM_EMIT("b.hs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("dup             v0.4s, v17.s[3]")               // v0       = v' = v7 v7 v7 v7
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q16, [%[b]], #0x10")            // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldr             q20, [%[a]], #0x10")            // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("dup             v0.4s, v16.s[3]")               // v0       = v' = v3 v3 v3 v3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q16, [%[dst]], #0x10")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[b]], #0x04")       // v16      = b
                __ASM_EMIT("ld1r            {v20.4s}, [%[a]], #0x04")       // v20      = a
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d = b - a
                __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          // v16      = v' = v + d
                __ASM_EMIT("fmul            v16.4s, v0.4s, v1.4s")          // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v16.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [result] "+w" (result), [m] "+w" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v3",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );

            return result;
        }

        float moving_mean_psqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_AARCH64(
                register float result __asm__("s0") = value;
                register float m __asm__("s1") = avg;
            );
            ARCH_AARCH64_ASM
            (
                // Register allocation:
                // v0       = accumulator
                // v1       = averaging coefficient
                // v3       = 0
                // v16-q19  = triangular sums
                // v20-q23  = temporaries
                __ASM_EMIT("dup             v1.4s, v1.s[0]")                // v1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3       = 0 0 0 0
                __ASM_EMIT("dup             v0.4s, v0.s[0]")                // v0       = v v v v
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("ldp             q18, q19, [%[b]], #0x20")       // v18      = b8 b9 b10 b11
                __ASM_EMIT("ldp             q22, q23, [%[a]], #0x20")       // v22      = a8 a9 a10 a11
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmul            v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul            v19.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("fmls            v18.4s, v22.4s, v22.4s")        // v18      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("fmls            v19.4s, v23.4s, v23.4s")        // v19      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #8")  // v22      = 0 0 d8 d9
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #8")  // v23      = 0 0 d12 d13
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #12") // v22      = 0 d8 d9 d8+d10
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #12") // v23      = 0 d12 d13 d12+d14
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("dup             v22.4s, v18.s[3]")              // v22      = p11 p11 p11 p11
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v19.4s, v19.4s, v22.4s")        // v19      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("dup             v20.4s, v17.s[3]")              // v20      = s7 s7 s7 s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v20.4s")        // v18      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v20.4s")        // v19      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v0.4s")         // v18      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v0.4s")         // v19      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("dup             v0.4s, v19.s[3]")               // v0       = v' = v15 v15 v15 v15
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmul            v18.4s, v18.4s, v1.4s")         // v18      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("fmul            v19.4s, v19.4s, v1.4s")         // v19      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("fmax            v18.4s, v18.4s, v3.4s")
                __ASM_EMIT("fmax            v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q18, q19, [%[dst]], #0x20")
                __ASM_EMIT("b.hs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("dup             v0.4s, v17.s[3]")               // v0       = v' = v7 v7 v7 v7
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q16, [%[b]], #0x10")            // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldr             q20, [%[a]], #0x10")            // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("dup             v0.4s, v16.s[3]")               // v0       = v' = v3 v3 v3 v3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q16, [%[dst]], #0x10")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[b]], #0x04")       // v16      = b
                __ASM_EMIT("ld1r            {v20.4s}, [%[a]], #0x04")       // v20      = a
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d = b - a
                __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          // v16      = v' = v + d
                __ASM_EMIT("fmul            v16.4s, v0.4s, v1.4s")          // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v16.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [result] "+w" (result), [m] "+w" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v3",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );

            return result;
        }

        float moving_rms(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_AARCH64(
                register float result __asm__("s0") = value;
                register float m __asm__("s1") = avg;
            );
            ARCH_AARCH64_ASM
            (
                // Register allocation:
                // v0       = accumulator
                // v1       = averaging coefficient
                // v3       = 0
                // v16-q19  = triangular sums
                // v20-q23  = temporaries
                __ASM_EMIT("dup             v1.4s, v1.s[0]")                // v1       = m m m m
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("eor             v3.16b, v3.16b, v3.16b")        // v3       = 0 0 0 0
                __ASM_EMIT("dup             v0.4s, v0.s[0]")                // v0       = v v v v
                __ASM_EMIT("b.lo            2f")
                // x16 blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("ldp             q18, q19, [%[b]], #0x20")       // v18      = b8 b9 b10 b11
                __ASM_EMIT("ldp             q22, q23, [%[a]], #0x20")       // v22      = a8 a9 a10 a11
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmul            v18.4s, v18.4s, v18.4s")
                __ASM_EMIT("fmul            v19.4s, v19.4s, v19.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("fmls            v18.4s, v22.4s, v22.4s")        // v18      = d8 d9 d10 d11 = b8-a8 ...
                __ASM_EMIT("fmls            v19.4s, v23.4s, v23.4s")        // v19      = d12 d13 d14 d15 = b12-a12 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #8")  // v22      = 0 0 d8 d9
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #8")  // v23      = 0 0 d12 d13
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = d8 d9 d8+d10 d9+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = d12 d13 d12+d14 d13+d15
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("ext             v22.16b, v3.16b, v18.16b, #12") // v22      = 0 d8 d9 d8+d10
                __ASM_EMIT("ext             v23.16b, v3.16b, v19.16b, #12") // v23      = 0 d12 d13 d12+d14
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v22.4s")        // v18      = p8 p9 p10 p11 = d8 d8+d9 d8+d9+d10 d9+d9+d10+d11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v23.4s")        // v19      = p12 p13 p14 p15 = d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("dup             v22.4s, v18.s[3]")              // v22      = p11 p11 p11 p11
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v19.4s, v19.4s, v22.4s")        // v19      = p11+p12 p11+p13 p11+p14 p11+p15
                __ASM_EMIT("dup             v20.4s, v17.s[3]")              // v20      = s7 s7 s7 s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v20.4s")        // v18      = s8 s9 s10 s11 = s7+p8 s7+p9 s7+p10 s7+p11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v20.4s")        // v19      = s12 s13 s14 s15 = s7+p11+p12 s7+p11+p13 s7+p11+p14 s7+p11+p15
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("fadd            v18.4s, v18.4s, v0.4s")         // v18      = v8 v9 v10 v11 = v+s8 v+s9 v+s10 v+s11
                __ASM_EMIT("fadd            v19.4s, v19.4s, v0.4s")         // v19      = v12 v13 v14 v15 = v+s12 v+s13 v+s14 v+s15
                __ASM_EMIT("dup             v0.4s, v19.s[3]")               // v0       = v' = v15 v15 v15 v15
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmul            v18.4s, v18.4s, v1.4s")         // v18      = v8*m v9*m v10*m v11*m
                __ASM_EMIT("fmul            v19.4s, v19.4s, v1.4s")         // v19      = v12*m v13*m v14*m v15*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("fmax            v18.4s, v18.4s, v3.4s")
                __ASM_EMIT("fmax            v19.4s, v19.4s, v3.4s")
                __ASM_EMIT("fsqrt           v16.4s, v16.4s")
                __ASM_EMIT("fsqrt           v17.4s, v17.4s")
                __ASM_EMIT("fsqrt           v18.4s, v18.4s")
                __ASM_EMIT("fsqrt           v19.4s, v19.4s")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                __ASM_EMIT("subs            %[count], %[count], #16")
                __ASM_EMIT("stp             q18, q19, [%[dst]], #0x20")
                __ASM_EMIT("b.hs             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("adds            %[count], %[count], #8")
                __ASM_EMIT("b.lt             4f")
                __ASM_EMIT("ldp             q16, q17, [%[b]], #0x20")       // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldp             q20, q21, [%[a]], #0x20")       // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmul            v17.4s, v17.4s, v17.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("fmls            v17.4s, v21.4s, v21.4s")        // v17      = d4 d5 d6 d7 = b4-a4 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #8")  // v21      = 0 0 d4 d5
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("ext             v21.16b, v3.16b, v17.16b, #12") // v21      = 0 d4 d5 d4+d6
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v21.4s")        // v17      = p4 p5 p6 p7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("dup             v20.4s, v16.s[3]")              // v20      = s3 s3 s3 s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v20.4s")        // v17      = s4 s5 s6 s7 = s3+p4 s3+p5 s3+p6 s3+p7
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("fadd            v17.4s, v17.4s, v0.4s")         // v17      = v4 v5 v6 v7 = v+s4 v+s5 v+s6 v+s7
                __ASM_EMIT("dup             v0.4s, v17.s[3]")               // v0       = v' = v7 v7 v7 v7
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmul            v17.4s, v17.4s, v1.4s")         // v17      = v4*m v5*m v6*m v7*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fmax            v17.4s, v17.4s, v3.4s")
                __ASM_EMIT("fsqrt           v16.4s, v16.4s")
                __ASM_EMIT("fsqrt           v17.4s, v17.4s")
                __ASM_EMIT("sub             %[count], %[count], #8")
                __ASM_EMIT("stp             q16, q17, [%[dst]], #0x20")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("adds            %[count], %[count], #4")
                __ASM_EMIT("b.lt            6f")
                __ASM_EMIT("ldr             q16, [%[b]], #0x10")            // v16      = b0 b1 b2 b3
                __ASM_EMIT("ldr             q20, [%[a]], #0x10")            // v20      = a0 a1 a2 a3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d0 d1 d2 d3 = b0-a0 ...
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #8")  // v20      = 0 0 d0 d1
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("ext             v20.16b, v3.16b, v16.16b, #12") // v20      = 0 d0 d1 d0+d2
                __ASM_EMIT("fadd            v16.4s, v16.4s, v20.4s")        // v16      = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("fadd            v16.4s, v16.4s, v0.4s")         // v16      = v0 v1 v2 v3 = v+s0 v+s1 v+s2 v+s3
                __ASM_EMIT("dup             v0.4s, v16.s[3]")               // v0       = v' = v3 v3 v3 v3
                __ASM_EMIT("fmul            v16.4s, v16.4s, v1.4s")         // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fsqrt           v16.4s, v16.4s")
                __ASM_EMIT("sub             %[count], %[count], #4")
                __ASM_EMIT("str             q16, [%[dst]], #0x10")
                // x1 block
                __ASM_EMIT("6:")
                __ASM_EMIT("adds            %[count], %[count], #3")
                __ASM_EMIT("blt             8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("ld1r            {v16.4s}, [%[b]], #0x04")       // v16      = b
                __ASM_EMIT("ld1r            {v20.4s}, [%[a]], #0x04")       // v20      = a
                __ASM_EMIT("fmul            v16.4s, v16.4s, v16.4s")
                __ASM_EMIT("fmls            v16.4s, v20.4s, v20.4s")        // v16      = d = b - a
                __ASM_EMIT("fadd            v0.4s, v0.4s, v16.4s")          // v16      = v' = v + d
                __ASM_EMIT("fmul            v16.4s, v0.4s, v1.4s")          // v16      = v0*m v1*m v2*m v3*m
                __ASM_EMIT("fmax            v16.4s, v16.4s, v3.4s")
                __ASM_EMIT("fsqrt           v16.4s, v16.4s")
                __ASM_EMIT("subs            %[count], %[count], #1")
                __ASM_EMIT("st1             {v16.s}[0], [%[dst]], #0x04")
                __ASM_EMIT("b.ge            7b")
                /* end */
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [a] "+r" (a), [b] "+r" (b),
                  [result] "+w" (result), [m] "+w" (m),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "v3",
                  "v16", "v17", "v18", "v19",
                  "v20", "v21", "v22", "v23"
            );

            return result;
        }

    } /* namespace asimd */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_HMATH_MSUM_H_ */
