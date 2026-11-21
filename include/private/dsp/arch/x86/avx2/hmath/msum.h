/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 нояб. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

namespace lsp
{
    namespace avx2
    {
        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                        // offset   = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %[value], %%ymm0")                      // ymm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %[m], %%ymm1")                          // ymm1     = m m m m m m m m
                __ASM_EMIT("jb              2f")
                // x16 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm3")            // ymm3     = b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%ymm2, %%ymm2")    // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vsubps          0x20(%[a], %[off]), %%ymm3, %%ymm3")    // ymm3     = d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vpslldq         $8, %%ymm3, %%ymm5")                    // ymm5     = 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d8+d10 d9+d11 d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vpslldq         $4, %%ymm3, %%ymm5")                    // ymm5     = 0 d8 d9 d8+d10 0 d12 d13 d12+d14
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vperm2f128      $0x08, %%ymm3, %%ymm3, %%ymm5")         // ymm5     = 0 0 0 0 d8 d8+d9 d8+d9+d10 d8+d9+d10+d11
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm5, %%ymm5")                 // ymm5     = 0 0 0 0 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = s8 s9 s10 s11 s12 s13 s14 s15 = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d8+d9+d10+d11+d12 d8+d9+d10+d11+d12+d13 d8+d9+d10+d11+d12+d13+d14 d8+d9+d10+d11+d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x11, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = s4 s5 s6 s7 s4 s5 s6 s7
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = s7 s7 s7 s7 s7 s7 s7 s7
                __ASM_EMIT("vaddps          %%ymm4, %%ymm3, %%ymm3")                // ymm3     = s7+s8 s7+s9 s7+s10 s7+s11 s7+s12 s7+s13 s7+s14 s7+s15
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                // ymm2     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm0")                // ymm0     = v8 v9 v10 v11 v12 v13 v14 v15
                __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm3")                // ymm3     = v8*m v9*m v10*m v11*m v12*m v13*m v14*m v15*m
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v12 v13 v14 v15 v12 v13 v14 v15
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v15 v15 v15 v15 v15 v15 v15 v15
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%ymm2, %%ymm2")    // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")            // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%xmm2, %%xmm2")    // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vpslldq         $8, %%xmm2, %%xmm4")                    // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vpslldq         $4, %%xmm2, %%xmm4")                    // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")         // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")            // xmm2     = b
                __ASM_EMIT("vsubss          0x00(%[a], %[off]), %%xmm2, %%xmm2")    // xmm2     = d = b-a
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = s * avg
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                // End
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return value;
        }

        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                        // offset   = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %[value], %%ymm0")                      // ymm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %[m], %%ymm1")                          // ymm1     = m m m m m m m m
                __ASM_EMIT("jb              2f")
                // x16 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm3")            // ymm3     = b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm5")            // ymm5     = a8 a9 a10 a11 a12 a13 a14 a15
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vsubps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vpslldq         $8, %%ymm3, %%ymm5")                    // ymm5     = 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d8+d10 d9+d11 d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vpslldq         $4, %%ymm3, %%ymm5")                    // ymm5     = 0 d8 d9 d8+d10 0 d12 d13 d12+d14
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vperm2f128      $0x08, %%ymm3, %%ymm3, %%ymm5")         // ymm5     = 0 0 0 0 d8 d8+d9 d8+d9+d10 d8+d9+d10+d11
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm5, %%ymm5")                 // ymm5     = 0 0 0 0 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = s8 s9 s10 s11 s12 s13 s14 s15 = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d8+d9+d10+d11+d12 d8+d9+d10+d11+d12+d13 d8+d9+d10+d11+d12+d13+d14 d8+d9+d10+d11+d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x11, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = s4 s5 s6 s7 s4 s5 s6 s7
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = s7 s7 s7 s7 s7 s7 s7 s7
                __ASM_EMIT("vaddps          %%ymm4, %%ymm3, %%ymm3")                // ymm3     = s7+s8 s7+s9 s7+s10 s7+s11 s7+s12 s7+s13 s7+s14 s7+s15
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                // ymm2     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm0")                // ymm0     = v8 v9 v10 v11 v12 v13 v14 v15
                __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm3")                // ymm3     = v8*m v9*m v10*m v11*m v12*m v13*m v14*m v15*m
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v12 v13 v14 v15 v12 v13 v14 v15
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v15 v15 v15 v15 v15 v15 v15 v15
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")            // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")            // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vsubps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vpslldq         $8, %%xmm2, %%xmm4")                    // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vpslldq         $4, %%xmm2, %%xmm4")                    // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")         // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")            // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")            // xmm4     = a
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulss          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vsubss          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = s * avg
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                // End
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return value;
        }

        float moving_mean_sqr_fma3(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                        // offset   = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %[value], %%ymm0")                      // ymm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %[m], %%ymm1")                          // ymm1     = m m m m m m m m
                __ASM_EMIT("jb              2f")
                // x16 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm3")            // ymm3     = b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm5")            // ymm5     = a8 a9 a10 a11 a12 a13 a14 a15
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vfnmadd231ps    %%ymm5, %%ymm5, %%ymm3")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vpslldq         $8, %%ymm3, %%ymm5")                    // ymm5     = 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d8+d10 d9+d11 d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vpslldq         $4, %%ymm3, %%ymm5")                    // ymm5     = 0 d8 d9 d8+d10 0 d12 d13 d12+d14
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vperm2f128      $0x08, %%ymm3, %%ymm3, %%ymm5")         // ymm5     = 0 0 0 0 d8 d8+d9 d8+d9+d10 d8+d9+d10+d11
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm5, %%ymm5")                 // ymm5     = 0 0 0 0 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = s8 s9 s10 s11 s12 s13 s14 s15 = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d8+d9+d10+d11+d12 d8+d9+d10+d11+d12+d13 d8+d9+d10+d11+d12+d13+d14 d8+d9+d10+d11+d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x11, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = s4 s5 s6 s7 s4 s5 s6 s7
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = s7 s7 s7 s7 s7 s7 s7 s7
                __ASM_EMIT("vaddps          %%ymm4, %%ymm3, %%ymm3")                // ymm3     = s7+s8 s7+s9 s7+s10 s7+s11 s7+s12 s7+s13 s7+s14 s7+s15
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                // ymm2     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm0")                // ymm0     = v8 v9 v10 v11 v12 v13 v14 v15
                __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm3")                // ymm3     = v8*m v9*m v10*m v11*m v12*m v13*m v14*m v15*m
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v12 v13 v14 v15 v12 v13 v14 v15
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v15 v15 v15 v15 v15 v15 v15 v15
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")            // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")            // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm2")                // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vpslldq         $8, %%xmm2, %%xmm4")                    // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vpslldq         $4, %%xmm2, %%xmm4")                    // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")         // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")            // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")            // xmm4     = a
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm2")                // xmm2     = d
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = s * avg
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                // End
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return value;
        }

        float moving_rms(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                        // offset   = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %[value], %%ymm0")                      // ymm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %[m], %%ymm1")                          // ymm1     = m m m m m m m m
                __ASM_EMIT("jb              2f")
                // x16 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm3")            // ymm3     = b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm5")            // ymm5     = a8 a9 a10 a11 a12 a13 a14 a15
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
                __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vsubps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vpslldq         $8, %%ymm3, %%ymm5")                    // ymm5     = 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d8+d10 d9+d11 d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vpslldq         $4, %%ymm3, %%ymm5")                    // ymm5     = 0 d8 d9 d8+d10 0 d12 d13 d12+d14
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vperm2f128      $0x08, %%ymm3, %%ymm3, %%ymm5")         // ymm5     = 0 0 0 0 d8 d8+d9 d8+d9+d10 d8+d9+d10+d11
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm5, %%ymm5")                 // ymm5     = 0 0 0 0 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = s8 s9 s10 s11 s12 s13 s14 s15 = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d8+d9+d10+d11+d12 d8+d9+d10+d11+d12+d13 d8+d9+d10+d11+d12+d13+d14 d8+d9+d10+d11+d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x11, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = s4 s5 s6 s7 s4 s5 s6 s7
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = s7 s7 s7 s7 s7 s7 s7 s7
                __ASM_EMIT("vaddps          %%ymm4, %%ymm3, %%ymm3")                // ymm3     = s7+s8 s7+s9 s7+s10 s7+s11 s7+s12 s7+s13 s7+s14 s7+s15
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                // ymm2     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm0")                // ymm0     = v8 v9 v10 v11 v12 v13 v14 v15
                __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm3")                // ymm3     = v8*m v9*m v10*m v11*m v12*m v13*m v14*m v15*m
                __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
                __ASM_EMIT("vsqrtps         %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v12 v13 v14 v15 v12 v13 v14 v15
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v15 v15 v15 v15 v15 v15 v15 v15
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")
                __ASM_EMIT("vsubps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")            // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")            // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vsubps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vpslldq         $8, %%xmm2, %%xmm4")                    // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vpslldq         $4, %%xmm2, %%xmm4")                    // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vsqrtps         %%xmm2, %%xmm2")
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")         // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")            // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")            // xmm4     = a
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulss          %%xmm4, %%xmm4, %%xmm4")
                __ASM_EMIT("vsubss          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = s * avg
                __ASM_EMIT("vsqrtss         %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                // End
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return value;
        }

        float moving_rms_fma3(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                        // offset   = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %[value], %%ymm0")                      // ymm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %[m], %%ymm1")                          // ymm1     = m m m m m m m m
                __ASM_EMIT("jb              2f")
                // x16 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm3")            // ymm3     = b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm5")            // ymm5     = a8 a9 a10 a11 a12 a13 a14 a15
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vfnmadd231ps    %%ymm5, %%ymm5, %%ymm3")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vpslldq         $8, %%ymm3, %%ymm5")                    // ymm5     = 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d9 d8+d10 d9+d11 d12 d13 d12+d14 d13+d15
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vpslldq         $4, %%ymm3, %%ymm5")                    // ymm5     = 0 d8 d9 d8+d10 0 d12 d13 d12+d14
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d12 d12+d13 d12+d13+d14 d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vperm2f128      $0x08, %%ymm3, %%ymm3, %%ymm5")         // ymm5     = 0 0 0 0 d8 d8+d9 d8+d9+d10 d8+d9+d10+d11
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm5, %%ymm5")                 // ymm5     = 0 0 0 0 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11 d8+d9+d10+d11
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm5, %%ymm3, %%ymm3")                // ymm3     = s8 s9 s10 s11 s12 s13 s14 s15 = d8 d8+d9 d8+d9+d10 d8+d9+d10+d11 d8+d9+d10+d11+d12 d8+d9+d10+d11+d12+d13 d8+d9+d10+d11+d12+d13+d14 d8+d9+d10+d11+d12+d13+d14+d15
                __ASM_EMIT("vperm2f128      $0x11, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = s4 s5 s6 s7 s4 s5 s6 s7
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = s7 s7 s7 s7 s7 s7 s7 s7
                __ASM_EMIT("vaddps          %%ymm4, %%ymm3, %%ymm3")                // ymm3     = s7+s8 s7+s9 s7+s10 s7+s11 s7+s12 s7+s13 s7+s14 s7+s15
                __ASM_EMIT("vaddps          %%ymm0, %%ymm2, %%ymm2")                // ymm2     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm0")                // ymm0     = v8 v9 v10 v11 v12 v13 v14 v15
                __ASM_EMIT("vmulps          %%ymm1, %%ymm2, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm3")                // ymm3     = v8*m v9*m v10*m v11*m v12*m v13*m v14*m v15*m
                __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
                __ASM_EMIT("vsqrtps         %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v12 v13 v14 v15 v12 v13 v14 v15
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v15 v15 v15 v15 v15 v15 v15 v15
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                // x8 block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")            // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")            // ymm4     = a0 a1 a2 a3 a4 a5 a6 a7
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vpslldq         $8, %%ymm2, %%ymm4")                    // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpslldq         $4, %%ymm2, %%ymm4")                    // ymm4     = 0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")         // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                 // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")         // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                 // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")            // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")            // xmm4     = a0 a1 a2 a3
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm2")                // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vpslldq         $8, %%xmm2, %%xmm4")                    // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vpslldq         $4, %%xmm2, %%xmm4")                    // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vsqrtps         %%xmm2, %%xmm2")
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")         // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")            // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")            // xmm4     = a
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm2")                // xmm2     = d
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                // xmm2     = s * avg
                __ASM_EMIT("vsqrtss         %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                // End
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );

            return value;
        }

    } /* namespace avx2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX2_HMATH_MSUM_H_ */
