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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        static const uint16_t moving_op_masks[] =
        {
            0xcccc,
            0xeeee,
            0xf0f0,
            0xff00
        };

        static const uint32_t moving_op_perm[] __lsp_aligned64 =
        {
            0, 0, 0, 0, 3, 3, 3, 3, 0, 0, 0, 0, 11, 11, 11, 11,
            0, 0, 0, 0, 0, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 7,
            15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15
        };

        #define REG_SEL(r32, r64) __IF_32(r32) __IF_64(r64)

        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                                // offset   = 0
                __ASM_EMIT("vxorps          %%zmm7, %%zmm7, %%zmm7")                        // zmm7     = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm0")                                // zmm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %%xmm1, %%zmm1")                                // zmm1     = m m m m m m m m
                __ASM_EMIT("jb              4f")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k2")
                __ASM_EMIT("kmovw           0x04 + %[MASK], %%k3")
                __ASM_EMIT("kmovw           0x06 + %[MASK], %%k4")
                __ASM_EMIT64("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT64("vmovaps       0x40 + %[PERM], %%zmm8")
                __ASM_EMIT64("vmovaps       0x80 + %[PERM], %%zmm9")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                // x32 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm3")                    // zmm3     = b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%zmm2, %%zmm2")            // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vsubps          0x40(%[a], %[off]), %%zmm3, %%zmm3")            // zmm3     = d16 d17 d18 d19 d20 d21 d22 d23 d24 d25 d26 d27 d28 d29 d30 d31
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vpermilps       $0x40, %%zmm3, %%zmm5 %{%%k1%}%{z%}")           // zmm5     = 0 0 d16 d17 0 0 d20 d21 0 0 d24 d25 0 0 d28 d29
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = d16 d17 d16+d18 d17+d19 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm3, %%zmm5 %{%%k2%}%{z%}")           // zmm5     = 0 d16 d17 d16+d18 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = s16 s17 s18 s19 ... = d16 d16+d17 d16+d17+d18 d17+d18+d19+d20 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm5 %{%%k3%}%{z%}")          // zmm5     = 0 0 0 0 s19 s19 s19 s19 0 0 0 0 s27 s27 s27 s27
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = t16 t17 ... = s16 s17 s18 s19 s16+s20 s16+s21 s16+s22 s16+s23 s24 s25 s26 s27 s27+s28 s27+s29 s27+s30 s27+s31
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vpermps         %%zmm3, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm5 %{%%k4%}%{z%}")  // zmm5     = 0 0 0 0 0 0 0 0 t23 t23 t23 t23 t23 t23 t23 t23
                __ASM_EMIT32("vmovaps       0x80 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = u16 u17 ...
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm4")               // zmm4     = S15 S15 ...
                __ASM_EMIT("vaddps          %%zmm4, %%zmm3, %%zmm3")                        // zmm3     = S16 S17 ... = S15+u16 S15+u17 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                        // zmm2     = v0 v1 v2 v3 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm0")                        // zmm0     = v16 v17 v18 v19 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm3")                        // zmm3     = v16*m v17*m v18*m v19*m ...
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("vmovups         %%zmm3, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%zmm2, %%zmm2")            // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 S2 S3 ...
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")                        // zmm0     = v0 v1 v2 v3 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")                    // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%ymm2, %%ymm2")            // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vshufps         $0x40, %%ymm2, %%ymm7, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpermilps       $0x90, %%ymm2, %%ymm4")                         // ymm4     = d0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vblendps        $0x11, %%ymm7, %%ymm4, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")                 // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                         // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                        // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                        // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")                 // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                         // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%xmm2, %%xmm2")            // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vshufps         $0x40, %%xmm2, %%xmm7, %%xmm4")                 // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm4, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")                 // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b
                __ASM_EMIT("vsubss          0x00(%[a], %[off]), %%xmm2, %%xmm2")            // xmm2     = d = b-a
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                // End
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [avg] "m" (avg),
                  [MASK] "o" (moving_op_masks),
                  [PERM] "o" (moving_op_perm)
                : "cc", "memory",
                  "%k1", "%k2", "%k3", "%k4",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );

            return value;
        }

        float moving_pavg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                                // offset   = 0
                __ASM_EMIT("vxorps          %%zmm7, %%zmm7, %%zmm7")                        // zmm7     = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm0")                                // zmm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %%xmm1, %%zmm1")                                // zmm1     = m m m m m m m m
                __ASM_EMIT("jb              4f")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k2")
                __ASM_EMIT("kmovw           0x04 + %[MASK], %%k3")
                __ASM_EMIT("kmovw           0x06 + %[MASK], %%k4")
                __ASM_EMIT64("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT64("vmovaps       0x40 + %[PERM], %%zmm8")
                __ASM_EMIT64("vmovaps       0x80 + %[PERM], %%zmm9")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                // x32 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm3")                    // zmm3     = b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%zmm2, %%zmm2")            // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vsubps          0x40(%[a], %[off]), %%zmm3, %%zmm3")            // zmm3     = d16 d17 d18 d19 d20 d21 d22 d23 d24 d25 d26 d27 d28 d29 d30 d31
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vpermilps       $0x40, %%zmm3, %%zmm5 %{%%k1%}%{z%}")           // zmm5     = 0 0 d16 d17 0 0 d20 d21 0 0 d24 d25 0 0 d28 d29
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = d16 d17 d16+d18 d17+d19 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm3, %%zmm5 %{%%k2%}%{z%}")           // zmm5     = 0 d16 d17 d16+d18 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = s16 s17 s18 s19 ... = d16 d16+d17 d16+d17+d18 d17+d18+d19+d20 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm5 %{%%k3%}%{z%}")          // zmm5     = 0 0 0 0 s19 s19 s19 s19 0 0 0 0 s27 s27 s27 s27
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = t16 t17 ... = s16 s17 s18 s19 s16+s20 s16+s21 s16+s22 s16+s23 s24 s25 s26 s27 s27+s28 s27+s29 s27+s30 s27+s31
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vpermps         %%zmm3, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm5 %{%%k4%}%{z%}")  // zmm5     = 0 0 0 0 0 0 0 0 t23 t23 t23 t23 t23 t23 t23 t23
                __ASM_EMIT32("vmovaps       0x80 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = u16 u17 ...
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm4")               // zmm4     = S15 S15 ...
                __ASM_EMIT("vaddps          %%zmm4, %%zmm3, %%zmm3")                        // zmm3     = S16 S17 ... = S15+u16 S15+u17 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                        // zmm2     = v0 v1 v2 v3 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm0")                        // zmm0     = v16 v17 v18 v19 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm3")                        // zmm3     = v16*m v17*m v18*m v19*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("vmovups         %%zmm3, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%zmm2, %%zmm2")            // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 S2 S3 ...
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")                        // zmm0     = v0 v1 v2 v3 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")                    // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%ymm2, %%ymm2")            // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vshufps         $0x40, %%ymm2, %%ymm7, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpermilps       $0x90, %%ymm2, %%ymm4")                         // ymm4     = d0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vblendps        $0x11, %%ymm7, %%ymm4, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")                 // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                         // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                        // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                        // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmaxps          %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")                 // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                         // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vsubps          0x00(%[a], %[off]), %%xmm2, %%xmm2")            // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vshufps         $0x40, %%xmm2, %%xmm7, %%xmm4")                 // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm4, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmaxps          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")                 // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b
                __ASM_EMIT("vsubss          0x00(%[a], %[off]), %%xmm2, %%xmm2")            // xmm2     = d = b-a
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("vmaxss          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                // End
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [avg] "m" (avg),
                  [MASK] "o" (moving_op_masks),
                  [PERM] "o" (moving_op_perm)
                : "cc", "memory",
                  "%k1", "%k2", "%k3", "%k4",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
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
                __ASM_EMIT("xor             %[off], %[off]")                                // offset   = 0
                __ASM_EMIT("vxorps          %%zmm7, %%zmm7, %%zmm7")                        // zmm7     = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm0")                                // zmm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %%xmm1, %%zmm1")                                // zmm1     = m m m m m m m m
                __ASM_EMIT("jb              4f")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k2")
                __ASM_EMIT("kmovw           0x04 + %[MASK], %%k3")
                __ASM_EMIT("kmovw           0x06 + %[MASK], %%k4")
                __ASM_EMIT64("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT64("vmovaps       0x40 + %[PERM], %%zmm8")
                __ASM_EMIT64("vmovaps       0x80 + %[PERM], %%zmm9")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                // x32 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm3")                    // zmm3     = b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm5")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm3, %%zmm3, %%zmm3")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vfnmadd231ps    %%zmm5, %%zmm5, %%zmm3")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vpermilps       $0x40, %%zmm3, %%zmm5 %{%%k1%}%{z%}")           // zmm5     = 0 0 d16 d17 0 0 d20 d21 0 0 d24 d25 0 0 d28 d29
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = d16 d17 d16+d18 d17+d19 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm3, %%zmm5 %{%%k2%}%{z%}")           // zmm5     = 0 d16 d17 d16+d18 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = s16 s17 s18 s19 ... = d16 d16+d17 d16+d17+d18 d17+d18+d19+d20 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm5 %{%%k3%}%{z%}")          // zmm5     = 0 0 0 0 s19 s19 s19 s19 0 0 0 0 s27 s27 s27 s27
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = t16 t17 ... = s16 s17 s18 s19 s16+s20 s16+s21 s16+s22 s16+s23 s24 s25 s26 s27 s27+s28 s27+s29 s27+s30 s27+s31
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vpermps         %%zmm3, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm5 %{%%k4%}%{z%}")  // zmm5     = 0 0 0 0 0 0 0 0 t23 t23 t23 t23 t23 t23 t23 t23
                __ASM_EMIT32("vmovaps       0x80 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = u16 u17 ...
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm4")               // zmm4     = S15 S15 ...
                __ASM_EMIT("vaddps          %%zmm4, %%zmm3, %%zmm3")                        // zmm3     = S16 S17 ... = S15+u16 S15+u17 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                        // zmm2     = v0 v1 v2 v3 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm0")                        // zmm0     = v16 v17 v18 v19 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm3")                        // zmm3     = v16*m v17*m v18*m v19*m ...
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("vmovups         %%zmm3, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 S2 S3 ...
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")                        // zmm0     = v0 v1 v2 v3 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")                    // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                        // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vshufps         $0x40, %%ymm2, %%ymm7, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpermilps       $0x90, %%ymm2, %%ymm4")                         // ymm4     = d0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vblendps        $0x11, %%ymm7, %%ymm4, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")                 // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                         // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                        // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                        // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")                 // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                         // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vshufps         $0x40, %%xmm2, %%xmm7, %%xmm4")                 // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm4, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")                 // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                // End
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [avg] "m" (avg),
                  [MASK] "o" (moving_op_masks),
                  [PERM] "o" (moving_op_perm)
                : "cc", "memory",
                  "%k1", "%k2", "%k3", "%k4",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );

            return value;
        }

        float moving_mean_psqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                register float m __asm__("%xmm1") = avg;
            );
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")                                // offset   = 0
                __ASM_EMIT("vxorps          %%zmm7, %%zmm7, %%zmm7")                        // zmm7     = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm0")                                // zmm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %%xmm1, %%zmm1")                                // zmm1     = m m m m m m m m
                __ASM_EMIT("jb              4f")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k2")
                __ASM_EMIT("kmovw           0x04 + %[MASK], %%k3")
                __ASM_EMIT("kmovw           0x06 + %[MASK], %%k4")
                __ASM_EMIT64("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT64("vmovaps       0x40 + %[PERM], %%zmm8")
                __ASM_EMIT64("vmovaps       0x80 + %[PERM], %%zmm9")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                // x32 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm3")                    // zmm3     = b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm5")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm3, %%zmm3, %%zmm3")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vfnmadd231ps    %%zmm5, %%zmm5, %%zmm3")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vpermilps       $0x40, %%zmm3, %%zmm5 %{%%k1%}%{z%}")           // zmm5     = 0 0 d16 d17 0 0 d20 d21 0 0 d24 d25 0 0 d28 d29
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = d16 d17 d16+d18 d17+d19 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm3, %%zmm5 %{%%k2%}%{z%}")           // zmm5     = 0 d16 d17 d16+d18 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = s16 s17 s18 s19 ... = d16 d16+d17 d16+d17+d18 d17+d18+d19+d20 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm5 %{%%k3%}%{z%}")          // zmm5     = 0 0 0 0 s19 s19 s19 s19 0 0 0 0 s27 s27 s27 s27
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = t16 t17 ... = s16 s17 s18 s19 s16+s20 s16+s21 s16+s22 s16+s23 s24 s25 s26 s27 s27+s28 s27+s29 s27+s30 s27+s31
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vpermps         %%zmm3, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm5 %{%%k4%}%{z%}")  // zmm5     = 0 0 0 0 0 0 0 0 t23 t23 t23 t23 t23 t23 t23 t23
                __ASM_EMIT32("vmovaps       0x80 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = u16 u17 ...
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm4")               // zmm4     = S15 S15 ...
                __ASM_EMIT("vaddps          %%zmm4, %%zmm3, %%zmm3")                        // zmm3     = S16 S17 ... = S15+u16 S15+u17 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                        // zmm2     = v0 v1 v2 v3 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm0")                        // zmm0     = v16 v17 v18 v19 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm3")                        // zmm3     = v16*m v17*m v18*m v19*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("vmovups         %%zmm3, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 S2 S3 ...
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")                        // zmm0     = v0 v1 v2 v3 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")                    // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                        // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vshufps         $0x40, %%ymm2, %%ymm7, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpermilps       $0x90, %%ymm2, %%ymm4")                         // ymm4     = d0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vblendps        $0x11, %%ymm7, %%ymm4, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")                 // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                         // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                        // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                        // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmaxps          %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")                 // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                         // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vshufps         $0x40, %%xmm2, %%xmm7, %%xmm4")                 // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm4, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmaxps          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")                 // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("vmaxss          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                // End
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [avg] "m" (avg),
                  [MASK] "o" (moving_op_masks),
                  [PERM] "o" (moving_op_perm)
                : "cc", "memory",
                  "%k1", "%k2", "%k3", "%k4",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
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
                __ASM_EMIT("xor             %[off], %[off]")                                // offset   = 0
                __ASM_EMIT("vxorps          %%zmm7, %%zmm7, %%zmm7")                        // zmm7     = 0
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("vbroadcastss    %%xmm0, %%zmm0")                                // zmm0     = v v v v v v v v
                __ASM_EMIT("vbroadcastss    %%xmm1, %%zmm1")                                // zmm1     = m m m m m m m m
                __ASM_EMIT("jb              4f")
                __ASM_EMIT("kmovw           0x00 + %[MASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[MASK], %%k2")
                __ASM_EMIT("kmovw           0x04 + %[MASK], %%k3")
                __ASM_EMIT("kmovw           0x06 + %[MASK], %%k4")
                __ASM_EMIT64("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT64("vmovaps       0x40 + %[PERM], %%zmm8")
                __ASM_EMIT64("vmovaps       0x80 + %[PERM], %%zmm9")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                // x32 blocks
                __ASM_EMIT(".p2align        4")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm3")                    // zmm3     = b16 b17 b18 b19 b20 b21 b22 b23 b24 b25 b26 b27 b28 b29 b30 b31
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm5")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps          %%zmm3, %%zmm3, %%zmm3")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vfnmadd231ps    %%zmm5, %%zmm5, %%zmm3")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vpermilps       $0x40, %%zmm3, %%zmm5 %{%%k1%}%{z%}")           // zmm5     = 0 0 d16 d17 0 0 d20 d21 0 0 d24 d25 0 0 d28 d29
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = d16 d17 d16+d18 d17+d19 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm3, %%zmm5 %{%%k2%}%{z%}")           // zmm5     = 0 d16 d17 d16+d18 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = s16 s17 s18 s19 ... = d16 d16+d17 d16+d17+d18 d17+d18+d19+d20 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm5 %{%%k3%}%{z%}")          // zmm5     = 0 0 0 0 s19 s19 s19 s19 0 0 0 0 s27 s27 s27 s27
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = t16 t17 ... = s16 s17 s18 s19 s16+s20 s16+s21 s16+s22 s16+s23 s24 s25 s26 s27 s27+s28 s27+s29 s27+s30 s27+s31
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vpermps         %%zmm3, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm5 %{%%k4%}%{z%}")  // zmm5     = 0 0 0 0 0 0 0 0 t23 t23 t23 t23 t23 t23 t23 t23
                __ASM_EMIT32("vmovaps       0x80 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 ...
                __ASM_EMIT("vaddps          %%zmm5, %%zmm3, %%zmm3")                        // zmm3     = u16 u17 ...
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm4")               // zmm4     = S15 S15 ...
                __ASM_EMIT("vaddps          %%zmm4, %%zmm3, %%zmm3")                        // zmm3     = S16 S17 ... = S15+u16 S15+u17 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm2, %%zmm2")                        // zmm2     = v0 v1 v2 v3 ...
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm0")                        // zmm0     = v16 v17 v18 v19 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm3")                        // zmm3     = v16*m v17*m v18*m v19*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vsqrtps         %%zmm2, %%zmm2")
                __ASM_EMIT("vsqrtps         %%zmm3, %%zmm3")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("vmovups         %%zmm3, 0x40(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                // x16 blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm2")                    // zmm2     = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm4")
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm2")                        // zmm2     = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15
                __ASM_EMIT("vpermilps       $0x40, %%zmm2, %%zmm4 %{%%k1%}%{z%}")           // zmm4     = 0 0 d0 d1 0 0 d4 d5 0 0 d8 d9 0 0 d12 d13
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = d0 d1 d0+d2 d1+d3 ...
                __ASM_EMIT("vpermilps       $0x90, %%zmm2, %%zmm4 %{%%k2%}%{z%}")           // zmm4     = 0 d0 d1 d0+d2 ...
                __ASM_EMIT32("vmovaps       0x00 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = s0 s1 s2 s3 ... = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 ...
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm4 %{%%k3%}%{z%}")          // zmm4     = 0 0 0 0 s3 s3 s3 s3 0 0 0 0 s11 s11 s11 s11
                __ASM_EMIT32("vmovaps       0x40 + %[PERM], %%zmm6")
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = t0 t1 ... = s0 s1 s2 s3 s3+s4 s3+s5 s3+s6 s3+s7 s8 s9 s10 s11 s11+s12 s11+s13 s11+s14 s11+s15
                __ASM_EMIT("vpermps         %%zmm2, " REG_SEL("%%zmm6", "%%zmm8") ", %%zmm4 %{%%k4%}%{z%}")  // zmm4     = 0 0 0 0 0 0 0 0 t7 t7 t7 t7 t7 t7 t7 t7
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")                        // zmm2     = S0 S1 S2 S3 ...
                __ASM_EMIT("vaddps          %%zmm2, %%zmm0, %%zmm0")                        // zmm0     = v0 v1 v2 v3 ...
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")                        // zmm2     = v0*m v1*m v2*m v3*m ...
                __ASM_EMIT("vmaxps          %%zmm7, %%zmm2, %%zmm2")
                __ASM_EMIT("vsqrtps         %%zmm2, %%zmm2")
                __ASM_EMIT("vmovups         %%zmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpermps         %%zmm0, " REG_SEL("%%zmm6", "%%zmm9") ", %%zmm0")               // zmm0     = v31 v31 v31 v31 ...
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                // x8 block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm2")                    // ymm2     = b0 b1 b2 b3 b4 b5 b6 b7
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm4")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm2")                        // ymm2     = d0 d1 d2 d3 d4 d5 d6 d7
                __ASM_EMIT("vshufps         $0x40, %%ymm2, %%ymm7, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d1 d0+d2 d1+d3 d4 d5 d4+d6 d5+d7
                __ASM_EMIT("vpermilps       $0x90, %%ymm2, %%ymm4")                         // ymm4     = d0 d0 d1 d0+d2 0 d4 d5 d4+d6
                __ASM_EMIT("vblendps        $0x11, %%ymm7, %%ymm4, %%ymm4")                 // ymm4     = 0 0 d0 d1 0 0 d4 d5
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d4 d4+d5 d4+d5+d6 d4+d5+d6+d7
                __ASM_EMIT("vperm2f128      $0x08, %%ymm2, %%ymm2, %%ymm4")                 // ymm4     = 0 0 0 0 d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vpermilps       $0xff, %%ymm4, %%ymm4")                         // ymm4     = 0 0 0 0 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")                        // ymm2     = s0 s1 s2 s3 s4 s5 s6 s7 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3 d0+d1+d2+d3+d4 d0+d1+d2+d3+d4+d5 d0+d1+d2+d3+d4+d5+d6 d0+d1+d2+d3+d4+d5+d6+d7
                __ASM_EMIT("vaddps          %%ymm2, %%ymm0, %%ymm0")                        // ymm0     = v0 v1 v2 v3 v4 v5 v6 v7
                __ASM_EMIT("vmulps          %%ymm0, %%ymm1, %%ymm2")                        // ymm2     = v0*m v1*m v2*m v3*m v4*m v5*m v6*m v7*m
                __ASM_EMIT("vmaxps          %%ymm7, %%ymm2, %%ymm2")
                __ASM_EMIT("vsqrtps         %%ymm2, %%ymm2")
                __ASM_EMIT("vperm2f128      $0x11, %%ymm0, %%ymm0, %%ymm0")                 // ymm0     = v4 v5 v6 v7 v4 v5 v6 v7
                __ASM_EMIT("vpermilps       $0xff, %%ymm0, %%ymm0")                         // ymm0     = v7 v7 v7 v7 v7 v7 v7 v7
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                // x4 block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b0 b1 b2 b3
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0 d1 d2 d3
                __ASM_EMIT("vshufps         $0x40, %%xmm2, %%xmm7, %%xmm4")                 // xmm4     = 0 0 d0 d1
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = d0 d1 d0+d2 d1+d3
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm4, %%xmm4")                 // xmm4     = 0 d0 d1 d0+d2
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")                        // xmm2     = s0 s1 s2 s3 = d0 d0+d1 d0+d1+d2 d0+d1+d2+d3
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = v0 v1 v2 v3
                __ASM_EMIT("vmulps          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = v0*m v1*m v2*m v3*m
                __ASM_EMIT("vmaxps          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vsqrtps         %%xmm2, %%xmm2")
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm0")                 // xmm0     = v3 v3 v3 v3
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm2")                    // xmm2     = b
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm4")
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm2")                        // xmm2     = d0
                __ASM_EMIT("vaddss          %%xmm2, %%xmm0, %%xmm0")                        // xmm0     = s = value + b - a
                __ASM_EMIT("vmulss          %%xmm0, %%xmm1, %%xmm2")                        // xmm2     = s * avg
                __ASM_EMIT("vmaxss          %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vsqrtss         %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                // End
                __ASM_EMIT("10:")

                : [off] "=&r" (off), [count] "+r" (count),
                  [value] "+Yz" (value), [m] "+x" (m)
                : [dst] "r" (dst), [a] "r" (a), [b] "r" (b),
                  [avg] "m" (avg),
                  [MASK] "o" (moving_op_masks),
                  [PERM] "o" (moving_op_perm)
                : "cc", "memory",
                  "%k1", "%k2", "%k3", "%k4",
                  "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
                  __IF_64(, "%xmm8", "%xmm9")
            );

            return value;
        }

        #undef REG_SEL
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_HMATH_MSUM_H_ */
