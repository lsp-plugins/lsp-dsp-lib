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

#ifndef INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_NORMALIZE_H_
#define INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_NORMALIZE_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {

        IF_ARCH_X86(
            static const uint32_t normalize_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x7fffffff),
                LSP_DSP_VEC8(0x3f800000)    /* 1.0f */
            };
        );

        void normalize1(float *dst, size_t count)
        {
            IF_ARCH_X86( size_t off, count2 );

            ARCH_X86_ASM
            (
                __ASM_EMIT("test            %[count], %[count]")
                __ASM_EMIT("jz              2000f")

                /* Search minimum */
                __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")        /* ymm1 = 0         */
                __ASM_EMIT("vmovaps         0x00(%[MASK]), %%ymm2")         /* ymm2 = abs mask  */
                __ASM_EMIT("xor             %[off], %[off]")                /* off  = 0         */
                __ASM_EMIT("mov             %[count], %[count2]")

                /* Perform search of absolute max value */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %%ymm1, %%ymm0")                /* ymm0 = 0         */
                __ASM_EMIT("vmovaps         %%ymm2, %%ymm3")                /* ymm3 = abs mask  */
                __ASM_EMIT("jb              2f")
                /* x32 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vandps          0x00(%[dst], %[off]), %%ymm2, %%ymm4")
                __ASM_EMIT("vandps          0x20(%[dst], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT("vandps          0x40(%[dst], %[off]), %%ymm2, %%ymm6")
                __ASM_EMIT("vandps          0x60(%[dst], %[off]), %%ymm2, %%ymm7")
                __ASM_EMIT("vmaxps          %%ymm4, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps          %%ymm6, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vandps          0x00(%[dst], %[off]), %%ymm2, %%ymm4")
                __ASM_EMIT("vandps          0x20(%[dst], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT("vmaxps          %%ymm4, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("vmaxps          %%ymm0, %%ymm1, %%ymm0")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm1")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vandps          0x00(%[dst], %[off]), %%xmm2, %%xmm4")
                __ASM_EMIT("vandps          0x10(%[dst], %[off]), %%xmm3, %%xmm5")
                __ASM_EMIT("vmaxps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vandps          0x00(%[dst], %[off]), %%xmm2, %%xmm4")
                __ASM_EMIT("vmaxps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("vmaxps          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovhlps        %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxps          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vunpcklps       %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovhlps        %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxss          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[dst], %[off]), %%xmm4")
                __ASM_EMIT("vandps          %%xmm2, %%xmm4, %%xmm4")
                __ASM_EMIT("vmaxss          %%xmm4, %%xmm1, %%xmm1")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")
                /* xmm1 = max { abs{ src } } */
                __ASM_EMIT("mov             %[count2], %[count]")           /* restore count    */
                __ASM_EMIT("vxorps          %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = 0.0f      */
                __ASM_EMIT("xor             %[off], %[off]")                /* off  = 0         */
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm0")                /* abs mask <=> 0.0f */
                __ASM_EMIT("jle             2000f")
                __ASM_EMIT("vshufps         $0x00, %%xmm1, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovaps         0x20(%[MASK]), %%ymm0")         /* ymm0 = 1.0f      */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm1, %%ymm1")
                __ASM_EMIT("vdivps          %%ymm1, %%ymm0, %%ymm0")        /* ymm0 = 1.0f / max { abs { src } } */

                /* Scale data by the coefficient stored in xmm0 and store to destination */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
                __ASM_EMIT("jb              2f")
                /* x32 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x00(%[dst], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vmulps          0x20(%[dst], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmulps          0x40(%[dst], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT("vmulps          0x60(%[dst], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x00(%[dst], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vmulps          0x20(%[dst], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x00(%[dst], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmulps          0x10(%[dst], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x00(%[dst], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[dst], %[off]), %%xmm2")
                __ASM_EMIT("vmulss          %%xmm0, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")

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
                __ASM_EMIT("test            %[count], %[count]")
                __ASM_EMIT("jz              2000f")

                /* Search minimum */
                __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")        /* ymm1 = 0         */
                __ASM_EMIT("vmovaps         0x00(%[MASK]), %%ymm2")         /* ymm2 = abs mask  */
                __ASM_EMIT("xor             %[off], %[off]")                /* off  = 0         */
                __ASM_EMIT("mov             %[count], %[count2]")

                /* Perform search of absolute max value */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %%ymm1, %%ymm0")                /* ymm0 = 0         */
                __ASM_EMIT("vmovaps         %%ymm2, %%ymm3")                /* ymm3 = abs mask  */
                __ASM_EMIT("jb              2f")
                /* x32 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vandps          0x00(%[src], %[off]), %%ymm2, %%ymm4")
                __ASM_EMIT("vandps          0x20(%[src], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT("vandps          0x40(%[src], %[off]), %%ymm2, %%ymm6")
                __ASM_EMIT("vandps          0x60(%[src], %[off]), %%ymm2, %%ymm7")
                __ASM_EMIT("vmaxps          %%ymm4, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("vmaxps          %%ymm6, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vandps          0x00(%[src], %[off]), %%ymm2, %%ymm4")
                __ASM_EMIT("vandps          0x20(%[src], %[off]), %%ymm2, %%ymm5")
                __ASM_EMIT("vmaxps          %%ymm4, %%ymm0, %%ymm0")
                __ASM_EMIT("vmaxps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("vmaxps          %%ymm0, %%ymm1, %%ymm0")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm1")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vandps          0x00(%[src], %[off]), %%xmm2, %%xmm4")
                __ASM_EMIT("vandps          0x10(%[src], %[off]), %%xmm3, %%xmm5")
                __ASM_EMIT("vmaxps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxps          %%xmm5, %%xmm1, %%xmm1")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vandps          0x00(%[src], %[off]), %%xmm2, %%xmm4")
                __ASM_EMIT("vmaxps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("vmaxps          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovhlps        %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxps          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vunpcklps       %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovhlps        %%xmm1, %%xmm0, %%xmm0")
                __ASM_EMIT("vmaxss          %%xmm0, %%xmm1, %%xmm1")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[src], %[off]), %%xmm4")
                __ASM_EMIT("vandps          %%xmm2, %%xmm4, %%xmm4")
                __ASM_EMIT("vmaxss          %%xmm4, %%xmm1, %%xmm1")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")
                /* xmm1 = max { abs{ src } } */
                __ASM_EMIT("mov             %[count2], %[count]")           /* restore count    */
                __ASM_EMIT("vxorps          %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = 0.0f      */
                __ASM_EMIT("xor             %[off], %[off]")                /* off  = 0         */
                __ASM_EMIT("vucomiss        %%xmm1, %%xmm0")                /* abs mask <=> 0.0f */
                __ASM_EMIT("jle             1000f")
                __ASM_EMIT("vshufps         $0x00, %%xmm1, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovaps         0x20(%[MASK]), %%ymm0")         /* ymm0 = 1.0f      */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm1, %%ymm1")
                __ASM_EMIT("vdivps          %%ymm1, %%ymm0, %%ymm0")        /* ymm0 = 1.0f / max { abs { src } } */

                /* Scale data by the coefficient stored in xmm0 and store to destination */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("vmovaps         %%ymm0, %%ymm1")
                __ASM_EMIT("jb              2f")
                /* x32 blocks */
                __ASM_EMIT("1:")
                __ASM_EMIT("vmulps          0x00(%[src], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vmulps          0x20(%[src], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmulps          0x40(%[src], %[off]), %%ymm0, %%ymm4")
                __ASM_EMIT("vmulps          0x60(%[src], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm5, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                /* x16 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmulps          0x00(%[src], %[off]), %%ymm0, %%ymm2")
                __ASM_EMIT("vmulps          0x20(%[src], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vmovups         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmulps          0x00(%[src], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmulps          0x10(%[src], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                /* x4 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmulps          0x00(%[src], %[off]), %%xmm0, %%xmm2")
                __ASM_EMIT("vmovups         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                /* x1 block */
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vmulss          %%xmm0, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("jmp             2000f")

                /* Copy data to destination */
                __ASM_EMIT("1000:")
                __ASM_EMIT("cmp             %[src], %[dst]")
                __ASM_EMIT("je              2000f")
                __ASM_EMIT("sub             $0x40, %[count]")
                __ASM_EMIT("jb              2f")

                /* x64 block */
                __ASM_EMIT("1:")
                __ASM_EMIT("vlddqu          0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vlddqu          0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vlddqu          0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vlddqu          0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("vlddqu          0x80(%[src], %[off]), %%ymm4")
                __ASM_EMIT("vlddqu          0xa0(%[src], %[off]), %%ymm5")
                __ASM_EMIT("vlddqu          0xc0(%[src], %[off]), %%ymm6")
                __ASM_EMIT("vlddqu          0xe0(%[src], %[off]), %%ymm7")
                __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm4, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm5, 0xa0(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm6, 0xc0(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm7, 0xe0(%[dst], %[off])")
                __ASM_EMIT("add             $0x100, %[off]")
                __ASM_EMIT("sub             $0x40, %[count]")
                __ASM_EMIT("jae             1b")
                /* x32 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $0x20, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vlddqu          0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vlddqu          0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vlddqu          0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vlddqu          0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("sub             $0x20, %[count]")
                __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                /* x16 block */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $0x10, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vlddqu          0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("vlddqu          0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("sub             $0x10, %[count]")
                __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                /* x8 block */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $0x8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vlddqu          0x00(%[src], %[off]), %%ymm0")
                __ASM_EMIT("sub             $0x8, %[count]")
                __ASM_EMIT("vmovdqu         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                /* x1 items */
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $0x07, %[count]")
                __ASM_EMIT("jl              2000f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovd           0x00(%[src], %[off]), %%xmm0")
                __ASM_EMIT("vmovd           %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")

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

    } /* namespace avx */
} /* namespace lsp */



#endif /* INCLUDE_PRIVATE_DSP_ARCH_X86_AVX_PMATH_NORMALIZE_H_ */
