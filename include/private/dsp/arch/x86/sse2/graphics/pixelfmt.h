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

#ifndef PRIVATE_DSP_ARCH_X86_SSE2_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_X86_SSE2_PIXELFMT_H_

namespace lsp
{
    namespace sse2
    {
        IF_ARCH_X86(
            static const uint32_t rgba_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x00ff00ff)
            };
        )

        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movdqa      %[MASK], %%xmm6")                   // xmm6 = 00 ff 00 ff
                __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0
                __ASM_EMIT("movdqa      %%xmm6, %%xmm7")                    // xmm7 = 00 ff 00 ff
                __ASM_EMIT("pslld       $8, %%xmm6")                        // xmm6 = ff 00 ff 00

                // 8-element blocks
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")      // xmm1 = A2 R2 G2 B2
                __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = A1 R1 G1 B1
                __ASM_EMIT("movdqa      %%xmm1, %%xmm3")                    // xmm3 = A2 R2 G2 B2
                __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
                __ASM_EMIT("pand        %%xmm6, %%xmm2")                    // xmm2 = A1 00 G1 00
                __ASM_EMIT("pand        %%xmm7, %%xmm1")                    // xmm1 = 00 R2 00 B2
                __ASM_EMIT("pand        %%xmm6, %%xmm3")                    // xmm3 = A2 00 G2 00
                __ASM_EMIT("movdqa      %%xmm0, %%xmm4")                    // xmm4 = A1 00 G1 00
                __ASM_EMIT("movdqa      %%xmm1, %%xmm5")                    // xmm5 = A2 00 G2 00
                __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
                __ASM_EMIT("pslld       $16, %%xmm1")                       // xmm1 = 00 B2 00 00
                __ASM_EMIT("psrld       $16, %%xmm4")                       // xmm4 = 00 00 00 R1
                __ASM_EMIT("psrld       $16, %%xmm5")                       // xmm5 = 00 00 00 R2
                __ASM_EMIT("por         %%xmm2, %%xmm0")                    // xmm0 = A1 B1 G1 00
                __ASM_EMIT("por         %%xmm3, %%xmm1")                    // xmm1 = A2 B2 G2 00
                __ASM_EMIT("por         %%xmm4, %%xmm0")                    // xmm0 = A1 B1 G1 R1
                __ASM_EMIT("por         %%xmm5, %%xmm1")                    // xmm1 = A2 B2 G2 R2
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")

                // 4-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = A1 R1 G1 B1
                __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = A1 R1 G1 B1
                __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 R1 00 B1
                __ASM_EMIT("pand        %%xmm6, %%xmm2")                    // xmm2 = A1 00 G1 00
                __ASM_EMIT("movdqa      %%xmm0, %%xmm4")                    // xmm4 = A1 00 G1 00
                __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 B1 00 00
                __ASM_EMIT("psrld       $16, %%xmm4")                       // xmm4 = 00 00 00 R1
                __ASM_EMIT("por         %%xmm2, %%xmm0")                    // xmm0 = A1 B1 G1 00
                __ASM_EMIT("por         %%xmm4, %%xmm0")                    // xmm0 = A1 B1 G1 R1
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // Tail
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movd        0x00(%[src], %[off]), %%xmm0")      // xmm0 = AA RR GG BB
                __ASM_EMIT("movdqa      %%xmm0, %%xmm1")                    // xmm1 = AA RR GG BB
                __ASM_EMIT("pand        %%xmm7, %%xmm0")                    // xmm0 = 00 RR 00 BB
                __ASM_EMIT("pand        %%xmm6, %%xmm1")                    // xmm1 = AA 00 GG 00
                __ASM_EMIT("movdqa      %%xmm0, %%xmm2")                    // xmm2 = 00 RR 00 BB
                __ASM_EMIT("pslld       $16, %%xmm0")                       // xmm0 = 00 BB 00 00
                __ASM_EMIT("psrld       $16, %%xmm2")                       // xmm2 = 00 00 00 RR
                __ASM_EMIT("por         %%xmm1, %%xmm0")                    // xmm0 = AA 00 GG RR
                __ASM_EMIT("por         %%xmm2, %%xmm0")                    // xmm0 = AA BB GG RR
                __ASM_EMIT("movd        %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")

                // End
                __ASM_EMIT("6:")

                : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
                  [off] "=&r" (off)
                : [MASK] "m" (rgba_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const uint32_t abgr32_to_bgrff32_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0xff000000)
            };
        )

        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("movdqa      %[MASK], %%xmm6")                   // xmm6 = ff 00 00 00
                __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0
                __ASM_EMIT("movdqa      %%xmm6, %%xmm7")                    // xmm7 = ff 00 00 00

                // 24-element blocks
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movdqu      0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movdqu      0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movdqu      0x40(%[src], %[off]), %%xmm4")
                __ASM_EMIT("movdqu      0x50(%[src], %[off]), %%xmm5")
                __ASM_EMIT("psrld       $8, %%xmm0")                        // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("psrld       $8, %%xmm1")
                __ASM_EMIT("psrld       $8, %%xmm2")
                __ASM_EMIT("psrld       $8, %%xmm3")
                __ASM_EMIT("psrld       $8, %%xmm4")
                __ASM_EMIT("psrld       $8, %%xmm5")
                __ASM_EMIT("por         %%xmm6, %%xmm0")                    // xmm6 = FF R1 G1 B1
                __ASM_EMIT("por         %%xmm7, %%xmm1")
                __ASM_EMIT("por         %%xmm6, %%xmm2")
                __ASM_EMIT("por         %%xmm7, %%xmm3")
                __ASM_EMIT("por         %%xmm6, %%xmm4")
                __ASM_EMIT("por         %%xmm7, %%xmm5")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x40(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm5, 0x50(%[dst], %[off])")
                __ASM_EMIT("add         $0x60, %[off]")
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jae         1b")

                // 16-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("movdqu      0x20(%[src], %[off]), %%xmm2")
                __ASM_EMIT("movdqu      0x30(%[src], %[off]), %%xmm3")
                __ASM_EMIT("psrld       $8, %%xmm0")                        // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("psrld       $8, %%xmm1")
                __ASM_EMIT("psrld       $8, %%xmm2")
                __ASM_EMIT("psrld       $8, %%xmm3")
                __ASM_EMIT("por         %%xmm6, %%xmm0")                    // xmm6 = FF R1 G1 B1
                __ASM_EMIT("por         %%xmm7, %%xmm1")
                __ASM_EMIT("por         %%xmm6, %%xmm2")
                __ASM_EMIT("por         %%xmm7, %%xmm3")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")

                // 8-element block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm1")
                __ASM_EMIT("psrld       $8, %%xmm0")                        // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("psrld       $8, %%xmm1")
                __ASM_EMIT("por         %%xmm6, %%xmm0")                    // xmm6 = FF R1 G1 B1
                __ASM_EMIT("por         %%xmm7, %%xmm1")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")

                // 4-element block
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("psrld       $8, %%xmm0")                        // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("por         %%xmm6, %%xmm0")                    // xmm6 = FF R1 G1 B1
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // Tail
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movd        0x00(%[src], %[off]), %%xmm0")      // xmm0 = R1 G1 B1 A1
                __ASM_EMIT("psrld       $8, %%xmm0")                        // xmm0 = 00 R1 G1 B1
                __ASM_EMIT("por         %%xmm6, %%xmm0")                    // xmm6 = FF R1 G1 B1
                __ASM_EMIT("movd        %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         9b")

                // End
                __ASM_EMIT("10:")

                : [dst] "+r"(dst), [src] "+r"(src), [count] "+r" (count),
                  [off] "=&r" (off)
                : [MASK] "m" (abgr32_to_bgrff32_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

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
                __ASM_EMIT("movdqa      %[MASK], %%xmm1")                   // xmm1 = 00 ff ff ff
                __ASM_EMIT("pshufd      $0x00, %%xmm0, %%xmm0")             // xmm0 = vv vv vv vv
                __ASM_EMIT("xor         %[off], %[off]")                    // off  = 0

                // 24-element blocks
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movdqu      0x20(%[src], %[off]), %%xmm4")
                __ASM_EMIT("pand        %%xmm1, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("movdqu      0x30(%[src], %[off]), %%xmm5")
                __ASM_EMIT("pand        %%xmm1, %%xmm3")
                __ASM_EMIT("movdqu      0x40(%[src], %[off]), %%xmm6")
                __ASM_EMIT("pand        %%xmm1, %%xmm4")
                __ASM_EMIT("por         %%xmm0, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("movdqu      0x50(%[src], %[off]), %%xmm7")
                __ASM_EMIT("por         %%xmm0, %%xmm3")
                __ASM_EMIT("pand        %%xmm1, %%xmm5")
                __ASM_EMIT("por         %%xmm0, %%xmm4")
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("pand        %%xmm1, %%xmm6")
                __ASM_EMIT("por         %%xmm0, %%xmm5")
                __ASM_EMIT("movdqu      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("pand        %%xmm1, %%xmm7")
                __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("por         %%xmm0, %%xmm6")
                __ASM_EMIT("movdqu      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("por         %%xmm0, %%xmm7")
                __ASM_EMIT("movdqu      %%xmm6, 0x40(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm7, 0x50(%[dst], %[off])")
                __ASM_EMIT("add         $0x60, %[off]")
                __ASM_EMIT("sub         $24, %[count]")
                __ASM_EMIT("jae         1b")

                // 16-element block
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("movdqu      0x20(%[src], %[off]), %%xmm4")
                __ASM_EMIT("movdqu      0x30(%[src], %[off]), %%xmm5")
                __ASM_EMIT("pand        %%xmm1, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("pand        %%xmm1, %%xmm3")
                __ASM_EMIT("pand        %%xmm1, %%xmm4")
                __ASM_EMIT("pand        %%xmm1, %%xmm5")
                __ASM_EMIT("por         %%xmm0, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("por         %%xmm0, %%xmm3")
                __ASM_EMIT("por         %%xmm0, %%xmm4")
                __ASM_EMIT("por         %%xmm0, %%xmm5")
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm5, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")

                // 8-element block
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("movdqu      0x10(%[src], %[off]), %%xmm3")
                __ASM_EMIT("pand        %%xmm1, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("pand        %%xmm1, %%xmm3")
                __ASM_EMIT("por         %%xmm0, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("por         %%xmm0, %%xmm3")
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm3, 0x10(%[dst], %[off])")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")

                // 4-element block
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("movdqu      0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("pand        %%xmm1, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("por         %%xmm0, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                // Tail
                __ASM_EMIT("8:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("movd        0x00(%[src], %[off]), %%xmm2")      // xmm2 = AA XX YY ZZ
                __ASM_EMIT("pand        %%xmm1, %%xmm2")                    // xmm2 = 00 XX YY ZZ
                __ASM_EMIT("por         %%xmm0, %%xmm2")                    // xmm2 = VV XX YY ZZ
                __ASM_EMIT("movd        %%xmm2, 0x00(%[dst], %[off])")
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

    } /* namespace sse2 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE2_PIXELFMT_H_ */
