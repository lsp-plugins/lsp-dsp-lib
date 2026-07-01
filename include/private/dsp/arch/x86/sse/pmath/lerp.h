/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_LERP_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT32("subl      $16, %[count]")
                __ASM_EMIT64("sub       $16, %[count]")
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      0x20(%[a], %[off]), %%xmm6")        // xmm6     = a
                __ASM_EMIT("movups      0x30(%[a], %[off]), %%xmm7")
                __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x30(%[b], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x30(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm6, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm7, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm6")                    // xmm6     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm7")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT32("subl      $16, %[count]")
                __ASM_EMIT64("sub       $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl      $8, %[count]")
                __ASM_EMIT64("add       $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT32("subl      $8, %[count]")
                __ASM_EMIT64("sub       $8, %[count]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl      $4, %[count]")
                __ASM_EMIT64("add       $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT32("subl      $4, %[count]")
                __ASM_EMIT64("sub       $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT32("addl      $3, %[count]")
                __ASM_EMIT64("add       $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movss       0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subss       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulss       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT32("decl      %[count]")
                __ASM_EMIT64("dec       %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_vvk(float *dst, const float *a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[k], %%xmm4")                      // xmm4     = k
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm4, %%xmm4")             // xmm4     = k
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm4, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      0x20(%[a], %[off]), %%xmm6")        // xmm6     = a
                __ASM_EMIT("movups      0x30(%[a], %[off]), %%xmm7")
                __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x30(%[b], %[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm6, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm7, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm4, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm6")                    // xmm6     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm7")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm4, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("subss       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulss       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm6", "xmm7"
            );
        }

        void lerp_vkv(float *dst, const float *a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[b], %%xmm6")                      // xmm6     = b
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = b
                __ASM_EMIT("jb          2f")
                // 8x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movaps      %%xmm6, %%xmm3")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movss       0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subss       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulss       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                __ASM_EMIT("6:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_vkk(float *dst, const float *a, float b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[b], %%xmm6")                      // xmm6     = b
                __ASM_EMIT("movss       %[k], %%xmm7")                      // xmm7     = k
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = b
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movaps      %%xmm6, %%xmm3")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      0x20(%[a], %[off]), %%xmm4")        // xmm4     = a
                __ASM_EMIT("movups      0x30(%[a], %[off]), %%xmm5")
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movaps      %%xmm6, %%xmm3")
                __ASM_EMIT("subps       %%xmm4, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm5, %%xmm3")
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm4")                    // xmm4     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm5")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movups      0x10(%[a], %[off]), %%xmm1")
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("movaps      %%xmm6, %%xmm3")
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm1, %%xmm3")
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("subps       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[a], %[off]), %%xmm0")        // xmm0     = a
                __ASM_EMIT("movaps      %%xmm6, %%xmm2")                    // xmm2     = b
                __ASM_EMIT("subss       %%xmm0, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("mulss       %%xmm7, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "r" (a),
                  [b] "o" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6", "xmm7"
            );
        }

        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[a], %%xmm6")                      // xmm6     = a
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = a
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("subps       %%xmm6, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm2")        // xmm2     = b
                __ASM_EMIT("movups      0x30(%[b], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x30(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm6, %%xmm2")                    // xmm2     = b-a
                __ASM_EMIT("subps       %%xmm6, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm2")                    // xmm2     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm3")
                __ASM_EMIT("addps       %%xmm6, %%xmm2")                    // xmm2     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("subps       %%xmm6, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movss       0x00(%[k], %[off]), %%xmm4")        // xmm4     = k
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm4", "xmm5", "xmm6"
            );
        }

        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[a], %%xmm6")                      // xmm6     = a
                __ASM_EMIT("movss       %[k], %%xmm7")                      // xmm7     = k
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = a
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = k
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[b], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[b], %[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("subps       %%xmm6, %%xmm1")
                __ASM_EMIT("subps       %%xmm6, %%xmm2")
                __ASM_EMIT("subps       %%xmm6, %%xmm3")
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm1")
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm2")
                __ASM_EMIT("addps       %%xmm6, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("movups      0x10(%[b], %[off]), %%xmm1")
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("subps       %%xmm6, %%xmm1")
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("subps       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[b], %[off]), %%xmm0")        // xmm0     = b
                __ASM_EMIT("subss       %%xmm6, %%xmm0")                    // xmm0     = b-a
                __ASM_EMIT("mulss       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addss       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "r" (b),
                  [k] "o" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

        void lerp_kkv(float *dst, float a, float b, const float *k, size_t count)
        {
            IF_ARCH_X86( size_t off );
            ARCH_X86_ASM(
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movss       %[a], %%xmm6")                      // xmm6     = a
                __ASM_EMIT("movss       %[b], %%xmm7")                      // xmm7     = b
                __ASM_EMIT("shufps      $0x00, %%xmm6, %%xmm6")             // xmm6     = a
                __ASM_EMIT("shufps      $0x00, %%xmm7, %%xmm7")             // xmm7     = b
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("subps       %%xmm6, %%xmm7")                    // xmm7     = b-a
                __ASM_EMIT("jb          2f")
                // 16x blocks
                __ASM_EMIT(".p2align    4")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm0")        // xmm0     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[k], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[k], %[off]), %%xmm3")
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm1")
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm2")
                __ASM_EMIT("addps       %%xmm6, %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                // 8x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm0")        // xmm0     = k
                __ASM_EMIT("movups      0x10(%[k], %[off]), %%xmm1")
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("mulps       %%xmm7, %%xmm1")
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[k], %[off]), %%xmm0")        // xmm0     = k
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[k], %[off]), %%xmm0")        // xmm0     = k
                __ASM_EMIT("mulps       %%xmm7, %%xmm0")                    // xmm0     = (b-a)*k
                __ASM_EMIT("addps       %%xmm6, %%xmm0")                    // xmm0     = a + (b-a)*k
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst),
                  [a] "o" (a),
                  [b] "o" (b),
                  [k] "r" (k)
                : "cc", "memory",
                  "xmm0", "xmm1", "xmm2", "xmm3",
                  "xmm6", "xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_LERP_H_ */
