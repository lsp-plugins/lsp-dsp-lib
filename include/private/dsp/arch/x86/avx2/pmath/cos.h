/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 14 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_PMATH_COS_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_PMATH_COS_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

#include <private/dsp/arch/x86/avx2/pmath/sin.h>

namespace lsp
{
    namespace avx2
    {
        void cosf1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("vsubps          0x20(%[dst]), %%ymm1, %%ymm4")  // ymm4     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%xmm1, %%xmm0")  // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vsubps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void cosf2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("vsubps          0x20(%[src]), %%ymm1, %%ymm4")  // ymm4     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%xmm1, %%xmm0")  // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vsubps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void cosf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm6")                  // ymm6     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm7")                  // ymm7     = p
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm0")        // ymm0     = PI
                __ASM_EMIT("vmovaps         0x00 + %[S2KP], %%ymm4")        // ymm4     = i = 0 1 2 3 4 5 6 7
                __ASM_EMIT("vmovaps         0x60 + %[S2KP], %%ymm5")        // ymm5     = step = 8 8 8 8 8 8 8 8
                __ASM_EMIT("vsubps          %%ymm7, %%ymm0, %%ymm7")        // ymm7     = PI/2 - p + PI/2
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm6, %%ymm0")        // ymm0     = k*i
                __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")        // ymm4     = i + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm7, %%ymm0")        // ymm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")        // xmm0     = k*i
                __ASM_EMIT("vaddps          0x80 + %[S2KP], %%xmm4, %%xmm4")// xmm4     = i + step
                __ASM_EMIT("vsubps          %%xmm0, %%xmm7, %%xmm0")        // xmm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")        // xmm0     = k*i
                __ASM_EMIT("vsubps          %%xmm0, %%xmm7, %%xmm0")        // xmm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_cosf_kp1(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm11")                     // ymm11    = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm12")                     // ymm12    = p
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm0")            // ymm0     = PI
                __ASM_EMIT("vmovaps         0x00 + %[S2KP], %%ymm8")            // ymm8     = i0 = 0 1 2 3 4 5 6 7
                __ASM_EMIT("vmovaps         0x20 + %[S2KP], %%ymm9")            // ymm9     = i1 = 8 9 10 11 12 13 14
                __ASM_EMIT("vmovaps         0x40 + %[S2KP], %%ymm10")           // ymm10    = step = 16 ...
                __ASM_EMIT("vsubps          %%ymm12, %%ymm0, %%ymm12")          // ymm12    = PI/2 - p + PI/2
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm11, %%ymm0")           // ymm0     = k*i0
                __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm4")           // ymm4     = k*i1
                __ASM_EMIT("vaddps          %%ymm10, %%ymm8, %%ymm8")           // ymm8     = i0 + step
                __ASM_EMIT("vaddps          %%ymm10, %%ymm9, %%ymm9")           // ymm9     = i1 + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm12, %%ymm0")           // ymm0     = PI/2 - (k*i0 + p) + PI/2
                __ASM_EMIT("vsubps          %%ymm4, %%ymm12, %%ymm4")           // ymm4     = PI/2 - (k*i1 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X16
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm11, %%ymm0")           // ymm0     = k*i0
                __ASM_EMIT("vaddps          0x60 + %[S2KP], %%ymm8, %%ymm8")    // ymm8     = i0 + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm12, %%ymm0")           // ymm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")           // xmm0     = k*i0
                __ASM_EMIT("vaddps          0x80 + %[S2KP], %%xmm8, %%xmm8")    // xmm8     = i0 + step
                __ASM_EMIT("vsubps          %%xmm0, %%xmm12, %%xmm0")           // xmm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             10f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")           // xmm0     = k*i0
                __ASM_EMIT("vsubps          %%xmm0, %%xmm12, %%xmm0")           // xmm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12"
            );
        }


        void cosf1_fma3(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("vsubps          0x20(%[dst]), %%ymm1, %%ymm4")  // ymm4     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("vsubps          0x00(%[dst]), %%xmm1, %%xmm0")  // xmm0     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vsubps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "=&r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void cosf2_fma3(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM(
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("vsubps          0x20(%[src]), %%ymm1, %%ymm4")  // ymm4     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[src]")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm1")        // ymm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%ymm1, %%ymm0")  // ymm0     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("vsubps          0x00(%[src]), %%xmm1, %%xmm0")  // xmm0     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             14f")
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%xmm1")        // xmm1     = PI
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          0x00(%[src]), %%xmm0")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovhps         0x00(%[src]), %%xmm0, %%xmm0")
                __ASM_EMIT("10:")
                __ASM_EMIT("vsubps          %%xmm0, %%xmm1, %%xmm0")        // xmm0     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("12:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              14f")
                __ASM_EMIT("vmovhps         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("14:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                : [S2C] "o" (sinf_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void cosf_kp1_fma3(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm6")                  // ymm6     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm7")                  // ymm7     = p
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm0")        // ymm0     = PI
                __ASM_EMIT("vmovaps         0x00 + %[S2KP], %%ymm4")        // ymm4     = i = 0 1 2 3 4 5 6 7
                __ASM_EMIT("vmovaps         0x60 + %[S2KP], %%ymm5")        // ymm5     = step = 8 8 8 8 8 8 8 8
                __ASM_EMIT("vsubps          %%ymm7, %%ymm0, %%ymm7")        // ymm7     = PI/2 - p + PI/2
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm4, %%ymm6, %%ymm0")        // ymm0     = k*i
                __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")        // ymm4     = i + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm7, %%ymm0")        // ymm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")        // xmm0     = k*i
                __ASM_EMIT("vaddps          0x80 + %[S2KP], %%xmm4, %%xmm4")// xmm4     = i + step
                __ASM_EMIT("vsubps          %%xmm0, %%xmm7, %%xmm0")        // xmm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             8f")
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm0")        // xmm0     = k*i
                __ASM_EMIT("vsubps          %%xmm0, %%xmm7, %%xmm0")        // xmm0     = PI/2 - (k*i + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_cosf_kp1_fma3(float *dst, float k, float p, size_t count)
        {
            ARCH_X86_64_ASM(
                // Prepare
                __ASM_EMIT("vbroadcastss    %[k], %%ymm11")                     // ymm11     = k
                __ASM_EMIT("vbroadcastss    %[p], %%ymm12")                     // ymm12     = p
                __ASM_EMIT("vmovaps         0x020 + %[S2C], %%ymm0")            // ymm0      = PI
                __ASM_EMIT("vmovaps         0x00 + %[S2KP], %%ymm8")            // ymm8      = i0 = 0 1 2 3 4 5 6 7
                __ASM_EMIT("vmovaps         0x20 + %[S2KP], %%ymm9")            // ymm9      = i1 = 8 9 10 11 12 13 14
                __ASM_EMIT("vmovaps         0x40 + %[S2KP], %%ymm10")           // ymm10     = step = 16 ...
                __ASM_EMIT("vsubps          %%ymm12, %%ymm0, %%ymm12")          // ymm12     = PI/2 + p + PI/2 ...
                // x16 blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm11, %%ymm0")           // ymm0     = k*i0
                __ASM_EMIT("vmulps          %%ymm9, %%ymm11, %%ymm4")           // ymm4     = k*i1
                __ASM_EMIT("vaddps          %%ymm10, %%ymm8, %%ymm8")           // ymm8     = i0 + step
                __ASM_EMIT("vaddps          %%ymm10, %%ymm9, %%ymm9")           // ymm9     = i1 + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm12, %%ymm0")           // ymm0     = PI/2 - (k*i0 + p) + PI/2
                __ASM_EMIT("vsubps          %%ymm4, %%ymm12, %%ymm4")           // ymm4     = PI/2 - (k*i1 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X16_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups         %%ymm4, 0x20(%[dst])")
                __ASM_EMIT("add             $0x40, %[dst]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x8 block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          %%ymm8, %%ymm11, %%ymm0")           // ymm0     = k*i0
                __ASM_EMIT("vaddps          0x60 + %[S2KP], %%ymm8, %%ymm8")    // ymm8     = i0 + step
                __ASM_EMIT("vsubps          %%ymm0, %%ymm12, %%ymm0")           // ymm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X8_FMA3
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("4:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")           // xmm0     = k*i0
                __ASM_EMIT("vaddps          0x80 + %[S2KP], %%xmm8, %%xmm8")    // xmm8     = i0 + step
                __ASM_EMIT("vsubps          %%xmm0, %%xmm12, %%xmm0")           // xmm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("6:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             10f")
                __ASM_EMIT("vmulps          %%xmm8, %%xmm11, %%xmm0")           // xmm0     = k*i0
                __ASM_EMIT("vsubps          %%xmm0, %%xmm12, %%xmm0")           // xmm0     = PI/2 - (k*i0 + p) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4_FMA3
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vshufps         $0x39, %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("8:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("10:")

                : [dst] "+r" (dst), [count] "+r" (count)
                : [S2C] "o" (sinf_const),
                  [S2KP] "o" (kp_gen_const),
                  [k] "m" (k),
                  [p] "m" (p)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12"
            );
        }

    } /* namespace avx2 */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX2_PMATH_COS_H_ */
