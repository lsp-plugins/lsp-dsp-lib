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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PMATH_COS_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PMATH_COS_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE2_IMPL */

#include <private/dsp/arch/x86/sse2/pmath/sin.h>

namespace lsp
{
    namespace sse2
    {
        void cosf1(float *dst, size_t count)
        {
            IF_ARCH_X86(float *src);

            ARCH_X86_ASM(
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm1")          // xmm1     = x1
                __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4     = PI
                __ASM_EMIT("movups          0x10(%[dst]), %%xmm5")          // xmm5     = x2
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("subps           %%xmm5, %%xmm4")                // xmm4     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("movups          0x00(%[dst]), %%xmm1")          // xmm1     = x
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("mov             %[dst], %[src]")
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm1")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm1")
                __ASM_EMIT("8:")
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("12:")

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
                // x8 blocks
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("movups          0x00(%[src]), %%xmm1")          // xmm1     = x1
                __ASM_EMIT("movaps          %%xmm0, %%xmm4")                // xmm4     = PI
                __ASM_EMIT("movups          0x10(%[src]), %%xmm5")          // xmm5     = x2
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x1 + PI/2
                __ASM_EMIT("subps           %%xmm5, %%xmm4")                // xmm4     = PI/2 - x2 + PI/2
                SINF_X_PLUS_PI_2_CORE_X8
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups          %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("add             $0x20, %[src]")
                __ASM_EMIT("add             $0x20, %[dst]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // x4 block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("movups          0x00(%[src]), %%xmm1")          // xmm1     = x1
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[src]")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("4:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             12f")
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm0")         // xmm0     = PI
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movss           0x00(%[src]), %%xmm1")
                __ASM_EMIT("add             $4, %[src]")
                __ASM_EMIT("6:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              8f")
                __ASM_EMIT("movhps          0x00(%[src]), %%xmm1")
                __ASM_EMIT("8:")
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - x1 + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              10f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("10:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              12f")
                __ASM_EMIT("movhps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("12:")

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
                __ASM_EMIT("movss           %[k], %%xmm6")                  // xmm6     = k
                __ASM_EMIT("movss           %[p], %%xmm0")                  // xmm7     = p
                __ASM_EMIT("shufps          $0x00, %%xmm6, %%xmm6")         // xmm6     = k k k k
                __ASM_EMIT("shufps          $0x00, %%xmm0, %%xmm0")         // xmm0     = p p p p
                __ASM_EMIT("movaps          0x10 + %[S2C], %%xmm7")         // xmm7     = PI PI PI PI
                __ASM_EMIT("movaps          0x00 + %[S2KP], %%xmm4")        // xmm4     = i = 0 1 2 3
                __ASM_EMIT("movaps          0x10 + %[S2KP], %%xmm5")        // xmm5     = step = 4 4 4 4
                __ASM_EMIT("subps           %%xmm0, %%xmm7")                // xmm7     = PI/2 - p + PI/2
                // x4 blocks
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movaps          %%xmm4, %%xmm1")                // xmm1     = i
                __ASM_EMIT("movaps          %%xmm7, %%xmm0")                // xmm0     = PI/2 - p + PI/2
                __ASM_EMIT("mulps           %%xmm6, %%xmm1")                // xmm1     = k*i
                __ASM_EMIT("addps           %%xmm5, %%xmm4")                // xmm4     = i + step
                __ASM_EMIT("subps           %%xmm1, %%xmm0")                // xmm0     = PI/2 - (p + k*i) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[dst]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // Tail: 1x-3x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jle             6f")
                __ASM_EMIT("movaps          %%xmm7, %%xmm0")                // xmm0     = PI/2 - p + PI/2
                __ASM_EMIT("mulps           %%xmm6, %%xmm4")                // xmm4     = k*i
                __ASM_EMIT("subps           %%xmm4, %%xmm0")                // xmm0     = PI/2 - (p + k*i) + PI/2
                SINF_X_PLUS_PI_2_CORE_X4
                __ASM_EMIT("test            $1, %[count]")
                __ASM_EMIT("jz              4f")
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("shufps          $0x39, %%xmm0, %%xmm0")         // xmm0     = S1 S2 S3 S0
                __ASM_EMIT("add             $4, %[dst]")
                __ASM_EMIT("4:")
                __ASM_EMIT("test            $2, %[count]")
                __ASM_EMIT("jz              6f")
                __ASM_EMIT("movlps          %%xmm0, 0x00(%[dst])")
                // End
                __ASM_EMIT("6:")

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
    } /* namespace sse2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PMATH_COS_H_ */
