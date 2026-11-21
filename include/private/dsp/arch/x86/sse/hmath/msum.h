/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 нояб. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_X86_SSE_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t offset;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[offset], %[offset]")                  // offset   = 0
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0     = v v v v
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1     = m m m m
                __ASM_EMIT("jb          2f")
                // x8 blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x10(%[b], %[offset]), %%xmm3")         // xmm3     = b4 b5 b6 b7
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("movups      0x10(%[a], %[offset]), %%xmm5")         // xmm5     = a4 a5 a6 a7
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("subps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d6 d7 = b4-a4 b5-a5 b6-a6 b7-a7
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")                        // xmm5     = 0 0 d4 d5
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm5")                 // xmm5     = 0 d4 d5 d4+d6
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s4 s5 s6 s7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                        // xmm0     = v4 v5 v6 v7
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                        // xmm3     = v4 v5 v6 v7
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")                        // xmm3     = v4*m v5*m v6*m v7*m
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[offset])")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("add         $0x20, %[offset]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[offset]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b
                __ASM_EMIT("subss       0x00(%[a], %[offset]), %%xmm2")         // xmm2     = d = b-a
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = s
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("add         $0x04, %[offset]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")

                : [offset] "=&r" (offset), [count] "+r" (count),
                  "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return value;
        }

        float moving_pavg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t offset;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[offset], %[offset]")                  // offset   = 0
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0     = v v v v
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1     = m m m m
                __ASM_EMIT("jb          2f")
                // x8 blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x10(%[b], %[offset]), %%xmm3")         // xmm3     = b4 b5 b6 b7
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("movups      0x10(%[a], %[offset]), %%xmm5")         // xmm5     = a4 a5 a6 a7
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("subps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d6 d7 = b4-a4 b5-a5 b6-a6 b7-a7
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")                        // xmm5     = 0 0 d4 d5
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm5")                 // xmm5     = 0 d4 d5 d4+d6
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s4 s5 s6 s7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                        // xmm0     = v4 v5 v6 v7
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                        // xmm3     = v4 v5 v6 v7
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")                        // xmm3     = v4*m v5*m v6*m v7*m
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[offset])")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("add         $0x20, %[offset]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[offset]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b
                __ASM_EMIT("subss       0x00(%[a], %[offset]), %%xmm2")         // xmm2     = d = b-a
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = s
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("maxss       %%xmm7, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("add         $0x04, %[offset]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")

                : [offset] "=&r" (offset), [count] "+r" (count),
                  "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return value;
        }

        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t offset;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[offset], %[offset]")                  // offset   = 0
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0     = v v v v
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1     = m m m m
                __ASM_EMIT("jb          2f")
                // x8 blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x10(%[b], %[offset]), %%xmm3")         // xmm3     = b4 b5 b6 b7
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("movups      0x10(%[a], %[offset]), %%xmm5")         // xmm5     = a4 a5 a6 a7
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("subps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d6 d7 = b4-a4 b5-a5 b6-a6 b7-a7
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")                        // xmm5     = 0 0 d4 d5
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm5")                 // xmm5     = 0 d4 d5 d4+d6
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s4 s5 s6 s7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                        // xmm0     = v4 v5 v6 v7
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                        // xmm3     = v4 v5 v6 v7
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")                        // xmm3     = v4*m v5*m v6*m v7*m
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[offset])")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("add         $0x20, %[offset]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[offset]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b
                __ASM_EMIT("movss       0x00(%[a], %[offset]), %%xmm3")         // xmm3     = a
                __ASM_EMIT("mulss       %%xmm2, %%xmm2")
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                        // xmm0     = value + b
                __ASM_EMIT("subss       %%xmm3, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = s
                __ASM_EMIT("mulss       %%xmm1, %%xmm2")                        // xmm2     = s * m
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("add         $0x04, %[offset]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")

                : [offset] "=&r" (offset), [count] "+r" (count),
                  "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return value;
        }

        float moving_mean_psqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t offset;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[offset], %[offset]")                  // offset   = 0
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0     = v v v v
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1     = m m m m
                __ASM_EMIT("jb          2f")
                // x8 blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x10(%[b], %[offset]), %%xmm3")         // xmm3     = b4 b5 b6 b7
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("movups      0x10(%[a], %[offset]), %%xmm5")         // xmm5     = a4 a5 a6 a7
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("subps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d6 d7 = b4-a4 b5-a5 b6-a6 b7-a7
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")                        // xmm5     = 0 0 d4 d5
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm5")                 // xmm5     = 0 d4 d5 d4+d6
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s4 s5 s6 s7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                        // xmm0     = v4 v5 v6 v7
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                        // xmm3     = v4 v5 v6 v7
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")                        // xmm3     = v4*m v5*m v6*m v7*m
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[offset])")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("add         $0x20, %[offset]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[offset]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b
                __ASM_EMIT("movss       0x00(%[a], %[offset]), %%xmm3")         // xmm3     = a
                __ASM_EMIT("mulss       %%xmm2, %%xmm2")
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                        // xmm0     = value + b
                __ASM_EMIT("subss       %%xmm3, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = s
                __ASM_EMIT("mulss       %%xmm1, %%xmm2")                        // xmm2     = s * m
                __ASM_EMIT("maxss       %%xmm7, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("add         $0x04, %[offset]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")

                : [offset] "=&r" (offset), [count] "+r" (count),
                  "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return value;
        }

        float moving_rms(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t offset;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[offset], %[offset]")                  // offset   = 0
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm0, %%xmm0")                 // xmm0     = v v v v
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1")                 // xmm1     = m m m m
                __ASM_EMIT("jb          2f")
                // x8 blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x10(%[b], %[offset]), %%xmm3")         // xmm3     = b4 b5 b6 b7
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("movups      0x10(%[a], %[offset]), %%xmm5")         // xmm5     = a4 a5 a6 a7
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("subps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d6 d7 = b4-a4 b5-a5 b6-a6 b7-a7
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("xorps       %%xmm5, %%xmm5")                        // xmm5     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("movlhps     %%xmm3, %%xmm5")                        // xmm5     = 0 0 d4 d5
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        // xmm7     = d4 d5 d4+d6 d5+d7
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("shufps      $0x99, %%xmm7, %%xmm5")                 // xmm5     = 0 d4 d5 d4+d6
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm5, %%xmm3")                        // xmm3     = s4 s5 s6 s7 = d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")                        // xmm0     = v4 v5 v6 v7
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")                        // xmm3     = v4 v5 v6 v7
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")                        // xmm3     = v4*m v5*m v6*m v7*m
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm3, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[offset])")
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("add         $0x20, %[offset]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // x4 block
                __ASM_EMIT("2:")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")                        // xmm7     = 0 0 0 0
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("movups      0x00(%[a], %[offset]), %%xmm4")         // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3 = b0-a0 b1-a1 b2-a2 b3-a3
                __ASM_EMIT("xorps       %%xmm4, %%xmm4")                        // xmm4     = 0 0 0 0
                __ASM_EMIT("movlhps     %%xmm2, %%xmm4")                        // xmm4     = 0 0 d0 d1
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        // xmm6     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("shufps      $0x99, %%xmm6, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("addps       %%xmm4, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = v0 v1 v2 v3
                __ASM_EMIT("shufps      $0xff, %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[offset]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[b], %[offset]), %%xmm2")         // xmm2     = b
                __ASM_EMIT("movss       0x00(%[a], %[offset]), %%xmm3")         // xmm3     = a
                __ASM_EMIT("mulss       %%xmm2, %%xmm2")
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                        // xmm0     = value + b
                __ASM_EMIT("subss       %%xmm3, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")                        // xmm2     = s
                __ASM_EMIT("mulss       %%xmm1, %%xmm2")                        // xmm2     = s * m
                __ASM_EMIT("sqrtss      %%xmm2, %%xmm2")
                __ASM_EMIT("maxss       %%xmm7, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[offset])")
                __ASM_EMIT("add         $0x04, %[offset]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")

                : [offset] "=&r" (offset), [count] "+r" (count),
                  "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            return value;
        }

    } /* namespace sse */
} /* namespace lsp */



#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_SSE_HMATH_MSUM_H_ */
