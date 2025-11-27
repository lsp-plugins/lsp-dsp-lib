/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 нояб. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                // 16x blocks
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[dst],%[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[dst],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm4")       /* xmm2 = min */
                __ASM_EMIT("movups      0x10(%[min],%[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[min],%[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[min],%[off]), %%xmm7")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("movups      0x10(%[max],%[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[max],%[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[max],%[off]), %%xmm7")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("minps       %%xmm6, %%xmm2")
                __ASM_EMIT("minps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst],%[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movups      0x10(%[min],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("movups      0x10(%[max],%[off]), %%xmm5")
                __ASM_EMIT("maxps       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm3, %%xmm1")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("maxps       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movss       0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movss       0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("maxss       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minss       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                // End
                __ASM_EMIT("8:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [min] "r" (min), [max] "r" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void clamp_vv2(float *dst, const float *src, const float *min, const float *max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                // 16x blocks
                __ASM_EMIT64("sub       $16, %[count]")
                __ASM_EMIT32("sub       $16, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[src],%[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[src],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm4")       /* xmm2 = min */
                __ASM_EMIT("movups      0x10(%[min],%[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[min],%[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[min],%[off]), %%xmm7")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm2")
                __ASM_EMIT("maxps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("movups      0x10(%[max],%[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[max],%[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[max],%[off]), %%xmm7")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("minps       %%xmm6, %%xmm2")
                __ASM_EMIT("minps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst],%[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT64("sub       $16, %[count]")
                __ASM_EMIT32("subl      $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT64("add       $8, %[count]")
                __ASM_EMIT32("addl      $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movups      0x10(%[min],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("movups      0x10(%[max],%[off]), %%xmm5")
                __ASM_EMIT("maxps       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm3, %%xmm1")
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm5, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT64("sub       $8, %[count]")
                __ASM_EMIT32("subl      $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT64("add       $4, %[count]")
                __ASM_EMIT32("addl      $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movups      0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("maxps       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minps       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT64("sub       $4, %[count]")
                __ASM_EMIT32("subl      $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movss       0x00(%[min],%[off]), %%xmm2")       /* xmm2 = min */
                __ASM_EMIT("movss       0x00(%[max],%[off]), %%xmm4")       /* xmm4 = max */
                __ASM_EMIT("maxss       %%xmm2, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minss       %%xmm4, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT64("dec       %[count]")
                __ASM_EMIT32("decl      %[count]")
                __ASM_EMIT("jge         7b")
                // End
                __ASM_EMIT("8:")
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst), [src] "r" (src), [min] "r" (min), [max] "r" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void clamp_kk1(float *dst, float min, float max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[min], %%xmm6")
                __ASM_EMIT("movss       %[max], %%xmm7")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")
                // 16x blocks
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[dst],%[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[dst],%[off]), %%xmm3")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm6, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm2")
                __ASM_EMIT("maxps       %%xmm6, %%xmm3")
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm7, %%xmm1")
                __ASM_EMIT("minps       %%xmm7, %%xmm2")
                __ASM_EMIT("minps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst],%[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm6, %%xmm1")
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm7, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[dst],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("maxss       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minss       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                // End
                __ASM_EMIT("8:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [min] "m" (min), [max] "m" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }

        void clamp_kk2(float *dst, const float *src, float min, float max, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[min], %%xmm6")
                __ASM_EMIT("movss       %[max], %%xmm7")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")
                // 16x blocks
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[src],%[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[src],%[off]), %%xmm3")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm6, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm2")
                __ASM_EMIT("maxps       %%xmm6, %%xmm3")
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm7, %%xmm1")
                __ASM_EMIT("minps       %%xmm7, %%xmm2")
                __ASM_EMIT("minps       %%xmm7, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst],%[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("maxps       %%xmm6, %%xmm1")
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("minps       %%xmm7, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minps       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src],%[off]), %%xmm0")       /* xmm0 = x */
                __ASM_EMIT("maxss       %%xmm6, %%xmm0")                    /* xmm0 = max(x, min) */
                __ASM_EMIT("minss       %%xmm7, %%xmm0")                    /* xmm0 = clamp(x, min, max) = min(max(x, min), max) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                // End
                __ASM_EMIT("8:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [min] "m" (min), [max] "m" (max)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm6", "%xmm7"
            );
        }
    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_CLAMP_H_ */
