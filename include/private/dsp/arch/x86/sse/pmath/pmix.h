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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_PMIX_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_PMIX_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {

        void pmix_v1(float *dst, const float *src, const float *k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                // 8x blocks
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("movups      0x10(%[k],%[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("subps       %%xmm3, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("movups      0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movss       0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("movss       0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("subss       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")
                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src), [k] "r" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                // 8x blocks
                __ASM_EMIT32("subl      $8, %[count]")
                __ASM_EMIT64("sub       $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movups      0x10(%[src2],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("movups      0x10(%[src1],%[off]), %%xmm3")
                __ASM_EMIT("movups      0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("movups      0x10(%[k],%[off]), %%xmm5")
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("subps       %%xmm3, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT32("addl      $0x20, %[off]")
                __ASM_EMIT64("add       $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT32("addl      $4, %[count]")
                __ASM_EMIT64("add       $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movups      0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("movups      0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT32("subl      $4, %[count]")
                __ASM_EMIT64("sub       $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT32("addl      $3, %[count]")
                __ASM_EMIT64("add       $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movss       0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("movss       0x00(%[k],%[off]), %%xmm4")         /* xmm4 = k */
                __ASM_EMIT("subss       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT32("decl      %[count]")
                __ASM_EMIT64("dec       %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")
                : [off] "=&r" (off), [count] __ASM_ARG_RW(count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2), [k] "r" (k)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        void pmix_k1(float *dst, const float *src, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("shufps      $0x00, %[k], %[k]")
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movaps      %[k], %%xmm4")
                // 8x blocks
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movups      0x10(%[src],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("movups      0x10(%[dst],%[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("subps       %%xmm3, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("mulps       %%xmm4, %%xmm1")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movups      0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src],%[off]), %%xmm0")       /* xmm0 = s */
                __ASM_EMIT("movss       0x00(%[dst],%[off]), %%xmm2")       /* xmm2 = d */
                __ASM_EMIT("subss       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")
                : [off] "=&r" (off), [count] "+r" (count), [k] "+Yz" (k)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }

        void pmix_k2(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("shufps      $0x00, %[k], %[k]")
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("movaps      %[k], %%xmm4")
                // 8x blocks
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movups      0x10(%[src2],%[off]), %%xmm1")
                __ASM_EMIT("movups      0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("movups      0x10(%[src1],%[off]), %%xmm3")
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("subps       %%xmm3, %%xmm1")
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("mulps       %%xmm4, %%xmm1")
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("addps       %%xmm3, %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst],%[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                // 4x block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movups      0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("subps       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addps       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src2],%[off]), %%xmm0")      /* xmm0 = s */
                __ASM_EMIT("movss       0x00(%[src1],%[off]), %%xmm2")      /* xmm2 = d */
                __ASM_EMIT("subss       %%xmm2, %%xmm0")                    /* xmm0 = s - d */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")                    /* xmm0 = (s-d) * k */
                __ASM_EMIT("addss       %%xmm2, %%xmm0")                    /* xmm0 = mix = d + (s-d) * k */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst],%[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                // End
                __ASM_EMIT("6:")
                : [off] "=&r" (off), [count] "+r" (count), [k] "+Yz" (k)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4"
            );
        }

    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_PMIX_H_ */
