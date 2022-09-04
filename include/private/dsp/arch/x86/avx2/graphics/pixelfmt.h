/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 окт. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_PIXELFMT_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX2_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX2_IMPL */

namespace lsp
{
    namespace avx2
    {
        IF_ARCH_X86(
            static const uint32_t abgr32_to_bgrff32_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC8(0xff000000)
            };
        )

        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("vmovdqa     %[MASK], %%ymm6")                   // ymm6 = ff 00 00 00
                __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0
                __ASM_EMIT("vmovdqa     %%ymm6, %%ymm7")                    // ymm7 = ff 00 00 00

                // 48-element blocks
                __ASM_EMIT("sub         $48, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovdqu     0x00(%[src], %[off]), %%ymm0")      // ymm0 = R1 G1 B1 A1
                __ASM_EMIT("vmovdqu     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovdqu     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovdqu     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("vmovdqu     0x80(%[src], %[off]), %%ymm4")
                __ASM_EMIT("vmovdqu     0xa0(%[src], %[off]), %%ymm5")
                __ASM_EMIT("vpsrld      $8, %%ymm0, %%ymm0")                // ymm0 = 00 R1 G1 B1
                __ASM_EMIT("vpsrld      $8, %%ymm1, %%ymm1")
                __ASM_EMIT("vpsrld      $8, %%ymm2, %%ymm2")
                __ASM_EMIT("vpsrld      $8, %%ymm3, %%ymm3")
                __ASM_EMIT("vpsrld      $8, %%ymm4, %%ymm4")
                __ASM_EMIT("vpsrld      $8, %%ymm5, %%ymm5")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm0, %%ymm0")            // ymm6 = FF R1 G1 B1
                __ASM_EMIT("vorpd       %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vorpd       %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm4, %%ymm4")
                __ASM_EMIT("vorpd       %%ymm7, %%ymm5, %%ymm5")
                __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm4, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm5, 0xa0(%[dst], %[off])")
                __ASM_EMIT("add         $0xc0, %[off]")
                __ASM_EMIT("sub         $48, %[count]")
                __ASM_EMIT("jae         1b")

                // 32-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $16, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("vmovdqu     0x00(%[src], %[off]), %%ymm0")      // ymm0 = R1 G1 B1 A1
                __ASM_EMIT("vmovdqu     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vmovdqu     0x40(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vmovdqu     0x60(%[src], %[off]), %%ymm3")
                __ASM_EMIT("vpsrld      $8, %%ymm0, %%ymm0")                // ymm0 = 00 R1 G1 B1
                __ASM_EMIT("vpsrld      $8, %%ymm1, %%ymm1")
                __ASM_EMIT("vpsrld      $8, %%ymm2, %%ymm2")
                __ASM_EMIT("vpsrld      $8, %%ymm3, %%ymm3")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm0, %%ymm0")            // ymm0 = FF R1 G1 B1
                __ASM_EMIT("vorpd       %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vorpd       %%ymm7, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm3, 0x60(%[dst], %[off])")
                __ASM_EMIT("add         $0x80, %[off]")
                __ASM_EMIT("sub         $32, %[count]")

                // 16-element block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $16, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("vmovdqu     0x00(%[src], %[off]), %%ymm0")      // ymm0 = R1 G1 B1 A1
                __ASM_EMIT("vmovdqu     0x20(%[src], %[off]), %%ymm1")
                __ASM_EMIT("vpsrld      $8, %%ymm0, %%ymm0")                // ymm0 = 00 R1 G1 B1
                __ASM_EMIT("vpsrld      $8, %%ymm1, %%ymm1")
                __ASM_EMIT("vorpd       %%ymm6, %%ymm0, %%ymm0")            // ymm0 = FF R1 G1 B1
                __ASM_EMIT("vorpd       %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovdqu     %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")

                // 8-element block
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("vmovdqu     0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("vmovdqu     0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("vpsrld      $8, %%xmm0, %%xmm0")                // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("vpsrld      $8, %%xmm1, %%xmm1")
                __ASM_EMIT("vorpd       %%xmm6, %%xmm0, %%xmm0")            // xmm0 = FF R1 G1 B1
                __ASM_EMIT("vorpd       %%xmm7, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovdqu     %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu     %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")

                // 4-element block
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("vmovdqu     0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("vpsrld      $8, %%xmm0 ,%%xmm0")                // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("vorpd       %%xmm6, %%xmm0, %%xmm0")            // xmm0 = FF R1 G1 B1
                __ASM_EMIT("vmovdqu     %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // Tail
                __ASM_EMIT("10:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovd       0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("vpsrld      $8, %%xmm0, %%xmm0")                // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("vorpd       %%xmm6, %%xmm0, %%xmm0")            // xmm0 = FF R1 G1 B1
                __ASM_EMIT("vmovd       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         11b")

                // End
                __ASM_EMIT("12:")

                : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
                  [off] "=&r" (off)
                : [MASK] "m" (abgr32_to_bgrff32_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const uint32_t pabc32_set_alpha_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x00ffffff)
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
                __ASM_EMIT("vmovdqa         %[MASK], %%ymm1")               // ymm1 = 00 ff ff ff
                __ASM_EMIT("vpbroadcastd    %%xmm0, %%ymm0")                // ymm0 = vv vv vv vv
                __ASM_EMIT("xor             %[off], %[off]")                // off  = 0

                // 48-element blocks
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vpand           0x00(%[src], %[off]), %%ymm1, %%ymm2")  // ymm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand           0x20(%[src], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vpand           0x40(%[src], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vpor            %%ymm0, %%ymm2, %%ymm2")                // ymm2 = VV XX YY ZZ
                __ASM_EMIT("vpand           0x60(%[src], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vpor            %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vpand           0x80(%[src], %[off]), %%ymm1, %%ymm6")
                __ASM_EMIT("vpor            %%ymm0, %%ymm4, %%ymm4")
                __ASM_EMIT("vpand           0xa0(%[src], %[off]), %%ymm1, %%ymm7")
                __ASM_EMIT("vmovdqu         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vpor            %%ymm0, %%ymm5, %%ymm5")
                __ASM_EMIT("vmovdqu         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vpor            %%ymm0, %%ymm6, %%ymm6")
                __ASM_EMIT("vmovdqu         %%ymm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("vpor            %%ymm0, %%ymm7, %%ymm7")
                __ASM_EMIT("vmovdqu         %%ymm5, 0x60(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm6, 0x80(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm7, 0xa0(%[dst], %[off])")
                __ASM_EMIT("add             $0xc0, %[off]")
                __ASM_EMIT("sub             $48, %[count]")
                __ASM_EMIT("jae             1b")

                // 32-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vpand           0x00(%[src], %[off]), %%ymm1, %%ymm2")      // ymm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand           0x20(%[src], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vpand           0x40(%[src], %[off]), %%ymm1, %%ymm4")
                __ASM_EMIT("vpand           0x60(%[src], %[off]), %%ymm1, %%ymm5")
                __ASM_EMIT("vpor            %%ymm0, %%ymm2, %%ymm2")                    // ymm2 = VV XX YY ZZ
                __ASM_EMIT("vpor            %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vpor            %%ymm0, %%ymm4, %%ymm4")
                __ASM_EMIT("vpor            %%ymm0, %%ymm5, %%ymm5")
                __ASM_EMIT("vmovdqu         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm5, 0x60(%[dst], %[off])")
                __ASM_EMIT("add             $0x80, %[off]")
                __ASM_EMIT("sub             $32, %[count]")

                // 16-element block
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vpand           0x00(%[src], %[off]), %%ymm1, %%ymm2")      // ymm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand           0x20(%[src], %[off]), %%ymm1, %%ymm3")
                __ASM_EMIT("vpor            %%ymm0, %%ymm2, %%ymm2")                    // ymm2 = VV XX YY ZZ
                __ASM_EMIT("vpor            %%ymm0, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovdqu         %%ymm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%ymm3, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")

                // 8-element block
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vpand           0x00(%[src], %[off]), %%xmm1, %%xmm2")      // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpand           0x10(%[src], %[off]), %%xmm1, %%xmm3")
                __ASM_EMIT("vpor            %%xmm0, %%xmm2, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vpor            %%xmm0, %%xmm3, %%xmm3")
                __ASM_EMIT("vmovdqu         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovdqu         %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")

                // 4-element block
                __ASM_EMIT("8:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vpand           0x00(%[src], %[off]), %%xmm1, %%xmm2")      // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpor            %%xmm0, %%xmm2, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vmovdqu         %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")

                // Tail
                __ASM_EMIT("10:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              12f")
                __ASM_EMIT("11:")
                __ASM_EMIT("vmovd           0x00(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vpand           %%xmm1, %%xmm2, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("vpor            %%xmm0, %%xmm2, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("vmovd           %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add             $4, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             11b")

                // End
                __ASM_EMIT("12:")

                : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
                  [off] "=&r" (off), [value] "+Yz" (value)
                : [MASK] "m" (pabc32_set_alpha_const)
                : "cc", "memory",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX2_GRAPHICS_PIXELFMT_H_ */
