/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_COPY_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_COPY_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */

namespace lsp
{
    namespace sse3
    {
        void copy(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                __ASM_EMIT("cmp         %[dst], %[src]")
                __ASM_EMIT("je          10f")

                /* Destination is aligned */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("jb          2f")

                /* Block of 32 items */
                __ASM_EMIT("1:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("lddqu       0x40(%[src], %[off]), %%xmm4")
                __ASM_EMIT("lddqu       0x50(%[src], %[off]), %%xmm5")
                __ASM_EMIT("lddqu       0x60(%[src], %[off]), %%xmm6")
                __ASM_EMIT("lddqu       0x70(%[src], %[off]), %%xmm7")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm5, 0x50(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x60(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm7, 0x70(%[dst], %[off])")
                __ASM_EMIT("add         $0x80, %[off]")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("jae         1b")
                /* Block of 16 items */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $0x10, %[count]")
                /* Block of 8 items */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x08, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $0x08, %[count]")
                /* Block of 4 items */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $0x04, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $0x04, %[count]")
                /* 1x items */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $0x03, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                __ASM_EMIT("10:")

                : [src] "+r" (src), [dst] "+r"(dst),
                  [count] "+r" (count), [off] "=&r" (off)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void copy_backward(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                __ASM_EMIT("cmp         %[dst], %[src]")
                __ASM_EMIT("je          10f")

                __ASM_EMIT("lea         (,%[count], 4), %[off]")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("jb          2f")

                /* Block of 32 items */
                __ASM_EMIT("1:")
                __ASM_EMIT("sub         $0x80, %[off]")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("lddqu       0x40(%[src], %[off]), %%xmm4")
                __ASM_EMIT("lddqu       0x50(%[src], %[off]), %%xmm5")
                __ASM_EMIT("lddqu       0x60(%[src], %[off]), %%xmm6")
                __ASM_EMIT("lddqu       0x70(%[src], %[off]), %%xmm7")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm5, 0x50(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x60(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm7, 0x70(%[dst], %[off])")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("jae         1b")
                /* Block of 16 items */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("sub         $0x40, %[off]")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("sub         $0x10, %[count]")
                /* Block of 8 items */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x08, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("sub         $0x20, %[off]")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub         $0x08, %[count]")
                /* Block of 4 items */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $0x04, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("sub         $0x10, %[off]")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub         $0x04, %[count]")
                /* 1x items */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $0x03, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("sub         $0x04, %[off]")
                __ASM_EMIT("movd        0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("movd        %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                __ASM_EMIT("10:")

                : [src] "+r" (src), [dst] "+r"(dst),
                  [count] "+r" (count), [off] "=&r" (off)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            #undef RCOPY_CORE
        }

        void move(float *dst, const float *src, size_t count)
        {
            if (dst < src)
                sse3::copy(dst, src, count);
            else if (dst > src)
                sse3::copy_backward(dst, src, count);
        }

    } /* namespace sse3 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE3_COPY_H_ */
