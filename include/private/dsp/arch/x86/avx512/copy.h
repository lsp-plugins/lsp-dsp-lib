/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 мая 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_COPY_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_COPY_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        void copy(float *dst, const float *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("cmp         %[dst], %[src]")
                __ASM_EMIT("je          12f")

                __ASM_EMIT("xor         %[off], %[off]")

                /* Block of 128 items */
                __ASM_EMIT("sub         $0x80, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%zmm0")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%zmm1")
                __ASM_EMIT("vmovups     0x80(%[src], %[off]), %%zmm2")
                __ASM_EMIT("vmovups     0xc0(%[src], %[off]), %%zmm3")
                __ASM_EMIT("vmovups     0x100(%[src], %[off]), %%zmm4")
                __ASM_EMIT("vmovups     0x140(%[src], %[off]), %%zmm5")
                __ASM_EMIT("vmovups     0x180(%[src], %[off]), %%zmm6")
                __ASM_EMIT("vmovups     0x1c0(%[src], %[off]), %%zmm7")
                __ASM_EMIT("vmovups     %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm4, 0x100(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm5, 0x140(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm6, 0x180(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm7, 0x1c0(%[dst], %[off])")
                __ASM_EMIT("add         $0x200, %[off]")
                __ASM_EMIT("sub         $0x80, %[count]")
                __ASM_EMIT("jae         1b")
                /* Block of 64 items */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $0x40, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovups     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("vmovups     0x80(%[src], %[off]), %%ymm4")
                __ASM_EMIT("vmovups     0xa0(%[src], %[off]), %%ymm5")
                __ASM_EMIT("vmovups     0xc0(%[src], %[off]), %%ymm6")
                __ASM_EMIT("vmovups     0xe0(%[src], %[off]), %%ymm7")
                __ASM_EMIT("sub         $0x40, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm4, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm5, 0xa0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm6, 0xc0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm7, 0xe0(%[dst], %[off])")
                __ASM_EMIT("add         $0x100, %[off]")
                /* Block of 32 items */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x20, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovups     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add         $0x80, %[off]")
                /* Block of 16 items */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                /* Block of 8 items */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $0x8, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("sub         $0x8, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                /* 1x items */
                __ASM_EMIT("10:")
                __ASM_EMIT("add         $0x07, %[count]")
                __ASM_EMIT("jl          12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovd       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("vmovd       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         11b")

                __ASM_EMIT("12:")

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
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("cmp         %[dst], %[src]")
                __ASM_EMIT("je          12f")

                __ASM_EMIT("lea         (,%[count], 4), %[off]")
                /* Block of 128 items */
                __ASM_EMIT("sub         $0x80, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("sub         $0x200, %[off]")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%zmm0")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%zmm1")
                __ASM_EMIT("vmovups     0x80(%[src], %[off]), %%zmm2")
                __ASM_EMIT("vmovups     0xc0(%[src], %[off]), %%zmm3")
                __ASM_EMIT("vmovups     0x100(%[src], %[off]), %%zmm4")
                __ASM_EMIT("vmovups     0x140(%[src], %[off]), %%zmm5")
                __ASM_EMIT("vmovups     0x180(%[src], %[off]), %%zmm6")
                __ASM_EMIT("vmovups     0x1c0(%[src], %[off]), %%zmm7")
                __ASM_EMIT("vmovups     %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm1, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm2, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm3, 0xc0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm4, 0x100(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm5, 0x140(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm6, 0x180(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%zmm7, 0x1c0(%[dst], %[off])")
                __ASM_EMIT("sub         $0x80, %[count]")
                __ASM_EMIT("jae         1b")
                /* Block of 64 items */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $0x40, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("sub         $0x100, %[off]")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovups     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("vmovups     0x80(%[src], %[off]), %%ymm4")
                __ASM_EMIT("vmovups     0xa0(%[src], %[off]), %%ymm5")
                __ASM_EMIT("vmovups     0xc0(%[src], %[off]), %%ymm6")
                __ASM_EMIT("vmovups     0xe0(%[src], %[off]), %%ymm7")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm4, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm5, 0xa0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm6, 0xc0(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm7, 0xe0(%[dst], %[off])")
                __ASM_EMIT("sub         $0x40, %[count]")
                /* Block of 32 items */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $0x20, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("sub         $0x80, %[off]")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovups     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovups     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("sub         $0x20, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm3, 0x60(%[dst], %[off])")
                /* Block of 16 items */
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $0x10, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("sub         $0x40, %[off]")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vmovups     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("sub         $0x10, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off])")
                /* Block of 8 items */
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $0x8, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("sub         $0x20, %[off]")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("sub         $0x8, %[count]")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off])")
                /* 1x items */
                __ASM_EMIT("10:")
                __ASM_EMIT("add         $0x07, %[count]")
                __ASM_EMIT("jl          12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("sub         $0x04, %[off]")
                __ASM_EMIT("vmovd       0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("vmovd       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         11b")

                __ASM_EMIT("12:")

                : [src] "+r" (src), [dst] "+r"(dst),
                  [count] "+r" (count), [off] "=&r" (off)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void move(float *dst, const float *src, size_t count)
        {
            if (dst < src)
                avx512::copy(dst, src, count);
            else if (dst > src)
                avx512::copy_backward(dst, src, count);
        }

    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_COPY_H_ */
