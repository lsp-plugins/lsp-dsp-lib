/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 нояб. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PMATH_NORMALIZE_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PMATH_NORMALIZE_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const uint32_t normalize_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x7fffffff),
                LSP_DSP_VEC4(0x3f800000)    /* 1.0f */
            };
        );

        void normalize1(float *dst, size_t count)
        {
            IF_ARCH_X86( size_t off, count2 );

            ARCH_X86_ASM
            (
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                /* Search minimum */
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")        /* xmm1 = 0         */
                __ASM_EMIT("movaps      0x00(%[MASK]), %%xmm2") /* xmm2 = abs mask  */
                __ASM_EMIT("xor         %[off], %[off]")        /* off  = 0         */
                __ASM_EMIT("mov         %[count], %[count2]")

                /* Perform search of absolute max value */
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")        /* xmm0 = 0         */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")        /* xmm3 = abs mask  */
                __ASM_EMIT("jb          2f")
                /* x16 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[dst], %[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[dst], %[off]), %%xmm7")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("andps       %%xmm3, %%xmm5")
                __ASM_EMIT("andps       %%xmm2, %%xmm6")
                __ASM_EMIT("andps       %%xmm3, %%xmm7")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")
                __ASM_EMIT("maxps       %%xmm7, %%xmm1")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm5")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("andps       %%xmm3, %%xmm5")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("maxps       %%xmm4, %%xmm1")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("maxps       %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm0")
                __ASM_EMIT("maxps       %%xmm0, %%xmm1")
                __ASM_EMIT("unpcklps    %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm0")
                __ASM_EMIT("maxss       %%xmm0, %%xmm1")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("maxss       %%xmm4, %%xmm1")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")
                /* xmm1 = max { abs{ src } } */
                __ASM_EMIT("mov         %[count2], %[count]")   /* restore count */
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")        /* xmm0 = 0.0f  */
                __ASM_EMIT("xor         %[off], %[off]")        /* off  = 0         */
                __ASM_EMIT("ucomiss     %%xmm1, %%xmm0")        /* abs mask <=> 0.0f */
                __ASM_EMIT("jle         2000f")
                __ASM_EMIT("movaps      0x10(%[MASK]), %%xmm0") /* xmm0 = 1.0f  */
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1") /* xmm0 = 1.0f  */
                __ASM_EMIT("divps       %%xmm1, %%xmm0")        /* xmm0 = 1.0f / max { abs { src } } */

                /* Scale data by the coefficient stored in xmm0 and store to destination */
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")
                __ASM_EMIT("jb          2f")
                /* x24 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      0x30(%[dst], %[off]), %%xmm5")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      0x40(%[dst], %[off]), %%xmm6")
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")
                __ASM_EMIT("movups      0x50(%[dst], %[off]), %%xmm7")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm0, %%xmm6")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm7")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm6, 0x40(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm7, 0x50(%[dst], %[off])")
                __ASM_EMIT("add         $0x60, %[off]")
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jae         1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      0x30(%[dst], %[off]), %%xmm5")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm3")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("mulss       %%xmm0, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                __ASM_EMIT("2000:")

                : [off] "=&r" (off), [count] "+r" (count), [count2] "=&r" (count2)
                : [dst] "r" (dst),
                  [MASK] "r" (normalize_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void normalize2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86( size_t off, count2 );

            ARCH_X86_ASM
            (
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          2000f")

                /* Search minimum */
                __ASM_EMIT("xorps       %%xmm1, %%xmm1")        /* xmm1 = 0         */
                __ASM_EMIT("movaps      0x00(%[MASK]), %%xmm2") /* xmm2 = abs mask  */
                __ASM_EMIT("xor         %[off], %[off]")        /* off  = 0         */
                __ASM_EMIT("mov         %[count], %[count2]")

                /* Perform search of absolute max value */
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")        /* xmm0 = 0         */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")        /* xmm3 = abs mask  */
                __ASM_EMIT("jb          2f")
                /* x16 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm6")
                __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm7")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("andps       %%xmm3, %%xmm5")
                __ASM_EMIT("andps       %%xmm2, %%xmm6")
                __ASM_EMIT("andps       %%xmm3, %%xmm7")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("maxps       %%xmm6, %%xmm0")
                __ASM_EMIT("maxps       %%xmm7, %%xmm1")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm5")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("andps       %%xmm3, %%xmm5")
                __ASM_EMIT("maxps       %%xmm4, %%xmm0")
                __ASM_EMIT("maxps       %%xmm5, %%xmm1")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("maxps       %%xmm4, %%xmm1")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("maxps       %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm0")
                __ASM_EMIT("maxps       %%xmm0, %%xmm1")
                __ASM_EMIT("unpcklps    %%xmm0, %%xmm1")
                __ASM_EMIT("movhlps     %%xmm1, %%xmm0")
                __ASM_EMIT("maxss       %%xmm0, %%xmm1")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm4")
                __ASM_EMIT("andps       %%xmm2, %%xmm4")
                __ASM_EMIT("maxss       %%xmm4, %%xmm1")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")
                __ASM_EMIT("8:")
                /* xmm1 = max { abs{ src } } */
                __ASM_EMIT("mov         %[count2], %[count]")   /* restore count */
                __ASM_EMIT("xorps       %%xmm0, %%xmm0")        /* xmm0 = 0.0f  */
                __ASM_EMIT("xor         %[off], %[off]")        /* off  = 0         */
                __ASM_EMIT("ucomiss     %%xmm1, %%xmm0")        /* abs mask <=> 0.0f */
                __ASM_EMIT("jle         1000f")
                __ASM_EMIT("movaps      0x10(%[MASK]), %%xmm0") /* xmm0 = 1.0f  */
                __ASM_EMIT("shufps      $0x00, %%xmm1, %%xmm1") /* xmm0 = 1.0f  */
                __ASM_EMIT("divps       %%xmm1, %%xmm0")        /* xmm0 = 1.0f / max { abs { src } } */

                /* Scale data by the coefficient stored in xmm0 and store to destination */
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")
                __ASM_EMIT("jb          2f")
                /* x24 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm4")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm5")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      0x40(%[src], %[off]), %%xmm6")
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")
                __ASM_EMIT("movups      0x50(%[src], %[off]), %%xmm7")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm0, %%xmm6")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm7")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm6, 0x40(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm7, 0x50(%[dst], %[off])")
                __ASM_EMIT("add         $0x60, %[off]")
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jae         1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm4")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm5")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")
                __ASM_EMIT("movups      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("mulps       %%xmm1, %%xmm3")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("mulps       %%xmm0, %%xmm2")
                __ASM_EMIT("movups      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("mulss       %%xmm0, %%xmm2")
                __ASM_EMIT("movss       %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")
                __ASM_EMIT("jmp         2000f")

                /* Copy data to destination */
                __ASM_EMIT("1000:")
                __ASM_EMIT("cmp         %[src], %[dst]")
                __ASM_EMIT("je          2000f")
                __ASM_EMIT("sub         $32, %[count]")
                __ASM_EMIT("jb          2f")
                /* x32 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movups      0x40(%[src], %[off]), %%xmm4")
                __ASM_EMIT("movups      0x50(%[src], %[off]), %%xmm5")
                __ASM_EMIT("movups      0x60(%[src], %[off]), %%xmm6")
                __ASM_EMIT("movups      0x70(%[src], %[off]), %%xmm7")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm5, 0x50(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm6, 0x60(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm7, 0x70(%[dst], %[off])")
                __ASM_EMIT("add         $0x80, %[off]")
                __ASM_EMIT("sub         $32, %[count]")
                __ASM_EMIT("jae         1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $16, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movups      0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movups      0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("add         $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                __ASM_EMIT("2000:")

                : [off] "=&r" (off), [count] "+r" (count)
                  __IF_64(, [count2] "=&r" (count2))
                : [dst] "r" (dst), [src] "r" (src),
                  [MASK] "r" (normalize_const)
                  __IF_32(, [count2] "m" (count2))
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE_PMATH_NORMALIZE_H_ */
