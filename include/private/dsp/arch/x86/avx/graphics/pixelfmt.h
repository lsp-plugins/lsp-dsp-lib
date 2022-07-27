/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 июл. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_PIXELFMT_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        IF_ARCH_X86(
            static const uint32_t pabc32_set_alpha_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x00ffffff)
            };
        )

        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
                uint32_t value  = uint32_t(alpha) << 24;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovdqa     %[MASK], %%xmm1")                   // xmm1 = 00 ff ff ff
                __ASM_EMIT("vpshufd     $0x00, %%xmm0, %%xmm0")             // xmm0 = vv vv vv vv
                __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0

                // 24-element blocks
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vpand       0x00(%[src], %[off]), %%xmm1, %%xmm2")  // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand       0x10(%[src], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vpand       0x20(%[src], %[off]), %%xmm1, %%xmm4")
                __ASM_EMIT("vpor        %%xmm0, %%xmm2, %%xmm2")                // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vpand       0x30(%[src], %[off]), %%xmm1, %%xmm5")
                __ASM_EMIT("vpor        %%xmm0, %%xmm3, %%xmm3")
                __ASM_EMIT("vpand       0x40(%[src], %[off]), %%xmm1, %%xmm6")
                __ASM_EMIT("vpor        %%xmm0, %%xmm4, %%xmm4")
                __ASM_EMIT("vpand       0x50(%[src], %[off]), %%xmm1, %%xmm7")
                __ASM_EMIT("vmovdqu     %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpor        %%xmm0, %%xmm5, %%xmm5")
                __ASM_EMIT("vmovdqu     %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("vpor        %%xmm0, %%xmm6, %%xmm6")
                __ASM_EMIT("vmovdqu     %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpor        %%xmm0, %%xmm7, %%xmm7")
                __ASM_EMIT("vmovdqu     %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm6, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm7, 0x50(%[dst], %[off])")
                __ASM_EMIT("add         $0x60, %[off]")
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jae         1b")

                // 16-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("vpand       0x00(%[src], %[off]), %%xmm1, %%xmm2")  // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand       0x10(%[src], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vpand       0x20(%[src], %[off]), %%xmm1, %%xmm4")
                __ASM_EMIT("vpand       0x30(%[src], %[off]), %%xmm1, %%xmm5")
                __ASM_EMIT("vpor        %%xmm0, %%xmm2, %%xmm2")                // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vpor        %%xmm0, %%xmm3, %%xmm3")
                __ASM_EMIT("vpor        %%xmm0, %%xmm4, %%xmm4")
                __ASM_EMIT("vpor        %%xmm0, %%xmm5, %%xmm5")
                __ASM_EMIT("vmovdqu     %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")

                // 8-element block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("vpand       0x00(%[src], %[off]), %%xmm1, %%xmm2")  // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand       0x10(%[src], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vpor        %%xmm0, %%xmm2, %%xmm2")                // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vpor        %%xmm0, %%xmm3, %%xmm3")
                __ASM_EMIT("vmovdqu     %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")

                // 4-element block
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("vpand       0x00(%[src], %[off]), %%xmm1, %%xmm2")  // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpor        %%xmm0, %%xmm2, %%xmm2")                // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vmovdqu     %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // Tail
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovd       0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("vpand       %%xmm1, %%xmm2, %%xmm2")            // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpor        %%xmm0, %%xmm2, %%xmm2")            // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vmovd       %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                // End
                __ASM_EMIT("10:")

                : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
                  [off] "=&r" (off), [value] "+Yz" (value)
                : [MASK] "m" (pabc32_set_alpha_const)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }
    } /* namespace avx */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX_GRAPHICS_PIXELFMT_H_ */
