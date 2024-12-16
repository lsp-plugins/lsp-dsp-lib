/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */

namespace lsp
{
    namespace sse3
    {
        IF_ARCH_X86(
            static const uint32_t pcomplex_div_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x3f800000),
                LSP_DSP_VEC4(0x80000000)
            };
        );

    #define PCOMPLEX_MUL_CORE(DST, T, B) \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* x4 blocks */ \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("lddqu       0x00(%[" T "], %[off]), %%xmm0")    /* xmm0 = ar0 ai0 ar1 ai1 */ \
        __ASM_EMIT("lddqu       0x10(%[" T "], %[off]), %%xmm4")    /* xmm4 = ar2 ai2 ar3 ai3 */ \
        __ASM_EMIT("lddqu       0x00(%[" B "], %[off]), %%xmm1")    /* xmm1 = br0 bi0 br1 bi1 */ \
        __ASM_EMIT("lddqu       0x10(%[" B "], %[off]), %%xmm5")    /* xmm5 = br2 bi2 br3 bi3 */ \
        __ASM_EMIT("movsldup    %%xmm0, %%xmm2")                    /* xmm2 = ar0 ar0 ar1 ar1 */ \
        __ASM_EMIT("movsldup    %%xmm4, %%xmm6")                    /* xmm6 = ar2 ar2 ar3 ar3 */ \
        __ASM_EMIT("movshdup    %%xmm0, %%xmm0")                    /* xmm0 = ai0 ai0 ai1 ai1 */ \
        __ASM_EMIT("movshdup    %%xmm4, %%xmm4")                    /* xmm4 = ai2 ai2 ai3 ai3 */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm0")                    /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm4")                    /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm2")                    /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm6")                    /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */ \
        __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")             /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */ \
        __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")             /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */ \
        __ASM_EMIT("addsubps    %%xmm0, %%xmm2")                    /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */ \
        __ASM_EMIT("addsubps    %%xmm4, %%xmm6")                    /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */ \
        __ASM_EMIT("movdqu      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movdqu      %%xmm6, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* Process scalar data */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movlps      0x00(%[" T "], %[off]), %%xmm0")    /* xmm0 = ar0 ai0 ar1 ai1 */ \
        __ASM_EMIT("movlps      0x00(%[" B "], %[off]), %%xmm1")    /* xmm1 = br0 bi0 br1 bi1 */ \
        __ASM_EMIT("movsldup    %%xmm0, %%xmm2")                    /* xmm2 = ar0 ar0 ar1 ar1 */ \
        __ASM_EMIT("movshdup    %%xmm0, %%xmm0")                    /* xmm0 = ai0 ai0 ai1 ai1 */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm0")                    /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm2")                    /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */ \
        __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")             /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */ \
        __ASM_EMIT("addsubps    %%xmm0, %%xmm2")                    /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */ \
        __ASM_EMIT("movlps      %%xmm2, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x08, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:")

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                PCOMPLEX_MUL_CORE("dst", "src1", "src2")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4",
                  "%xmm5", "%xmm6"
            );
        }

        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                PCOMPLEX_MUL_CORE("dst", "dst", "src")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4",
                  "%xmm5", "%xmm6"
            );
        }

    #undef PCOMPLEX_MUL_CORE

    #define PCOMPLEX_DIV_CORE(DST, T, B) \
        __ASM_EMIT("xor         %[off], %[off]") \
        /* x4 blocks */ \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jb          2f") \
        __ASM_EMIT("1:") \
        __ASM_EMIT("lddqu       0x00(%[" T "], %[off]), %%xmm0")    /* xmm0 = tr0 ti0 tr1 ti1 */ \
        __ASM_EMIT("lddqu       0x10(%[" T "], %[off]), %%xmm4") \
        __ASM_EMIT("lddqu       0x00(%[" B "], %[off]), %%xmm1")    /* xmm1 = br0 bi0 br1 bi1 */ \
        __ASM_EMIT("lddqu       0x10(%[" B "], %[off]), %%xmm5") \
        __ASM_EMIT("movsldup    %%xmm0, %%xmm2")                    /* xmm2 = tr0 tr0 tr1 tr1 */ \
        __ASM_EMIT("movsldup    %%xmm4, %%xmm6") \
        __ASM_EMIT("movshdup    %%xmm0, %%xmm0")                    /* xmm0 = ti0 ti0 ti1 ti1 */ \
        __ASM_EMIT("movshdup    %%xmm4, %%xmm4") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm2")                    /* xmm2 = tr0*br0 tr0*bi0 tr1*br1 tr1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm6") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm0")                    /* xmm0 = ti0*br0 ti0*bi0 ti1*br1 ti1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm4") \
        __ASM_EMIT("xorps       0x10 + %[CC], %%xmm2")              /* xmm2 = -tr0*br0 -tr0*bi0 -tr1*br1 -tr1*bi1 */ \
        __ASM_EMIT("xorps       0x10 + %[CC], %%xmm6") \
        __ASM_EMIT("mulps       %%xmm1, %%xmm1")                    /* xmm1 = br0*br0 bi0*bi0 br1*br1 bi1*bi1 */ \
        __ASM_EMIT("mulps       %%xmm5, %%xmm5") \
        __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")             /* xmm0 = ti0*bi0 ti0*br0 ti1*bi1 ti1*br1 */ \
        __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4") \
        __ASM_EMIT("movaps      0x00 + %[CC], %%xmm3")              /* xmm3 = 1 */ \
        __ASM_EMIT("haddps      %%xmm5, %%xmm1")                    /* xmm1 = R0 R1 R2 R3 = br0*br0+bi0*bi0 br1*br1+bi1*bi1 */ \
        __ASM_EMIT("divps       %%xmm1, %%xmm3")                    /* xmm3 = 1/R0 1/R2 1/R2 1/R3 */ \
        __ASM_EMIT("addsubps    %%xmm2, %%xmm0")                    /* xmm0 = r0 i0 r1 i1 = ti0*bi0+tr0*br0 ti0*br0-tr0*bi0 ti1*bi1+tr1*br1 ti1*br1-tr1*bi1 */ \
        __ASM_EMIT("addsubps    %%xmm6, %%xmm4") \
        __ASM_EMIT("movaps      %%xmm3, %%xmm7")                    /* xmm7 = 1/R0 1/R2 1/R2 1/R3 */ \
        __ASM_EMIT("unpcklps    %%xmm3, %%xmm3")                    /* xmm3 = 1/R0 1/R0 1/R2 1/R2 */ \
        __ASM_EMIT("unpckhps    %%xmm7, %%xmm7")                    /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
        __ASM_EMIT("mulps       %%xmm3, %%xmm0")                    /* xmm0 = r0' i0' r1' i1' */ \
        __ASM_EMIT("mulps       %%xmm7, %%xmm4") \
        __ASM_EMIT("movdqu      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("movdqu      %%xmm4, 0x10(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x20, %[off]") \
        __ASM_EMIT("sub         $4, %[count]") \
        __ASM_EMIT("jae         1b") \
        __ASM_EMIT("2:") \
        /* Process scalar data */ \
        __ASM_EMIT("add         $3, %[count]") \
        __ASM_EMIT("jl          4f") \
        __ASM_EMIT("3:") \
        __ASM_EMIT("movlps      0x00(%[" T "], %[off]), %%xmm0")    /* xmm0 = tr ti */ \
        __ASM_EMIT("movlps      0x00(%[" B "], %[off]), %%xmm1")    /* xmm1 = br bi */ \
        __ASM_EMIT("movsldup    %%xmm0, %%xmm2")                    /* xmm2 = tr tr */ \
        __ASM_EMIT("movshdup    %%xmm0, %%xmm0")                    /* xmm0 = ti ti */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm2")                    /* xmm2 = tr*br tr*bi */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm0")                    /* xmm0 = ti*br ti*bi */ \
        __ASM_EMIT("xorps       0x10 + %[CC], %%xmm2")              /* xmm2 = -tr*br -tr*bi */ \
        __ASM_EMIT("mulps       %%xmm1, %%xmm1")                    /* xmm1 = br*br bi*bi */ \
        __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")             /* xmm0 = ti*bi ti*br */ \
        __ASM_EMIT("haddps      %%xmm1, %%xmm1")                    /* xmm1 = R = br*br+bi*bi */ \
        __ASM_EMIT("addsubps    %%xmm2, %%xmm0")                    /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
        __ASM_EMIT("unpcklps    %%xmm1, %%xmm1")                    /* xmm1 = R R */ \
        __ASM_EMIT("divps       %%xmm1, %%xmm0")                    /* xmm0 = r' i' */ \
        __ASM_EMIT("movlps      %%xmm0, 0x00(%[" DST "], %[off])") \
        __ASM_EMIT("add         $0x08, %[off]") \
        __ASM_EMIT("dec         %[count]") \
        __ASM_EMIT("jge         3b") \
        __ASM_EMIT("4:")

        void pcomplex_div3(float *dst, const float *src1, const float *src2, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "src1", "src2")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "dst", "src")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "src", "dst")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    #undef PCOMPLEX_DIV_CORE

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jb          2f")

                // 16x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
                __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
                __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
                __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")

                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("mulps       %%xmm6, %%xmm6")
                __ASM_EMIT("mulps       %%xmm7, %%xmm7")
                __ASM_EMIT("haddps      %%xmm1, %%xmm0")
                __ASM_EMIT("haddps      %%xmm3, %%xmm2")
                __ASM_EMIT("haddps      %%xmm5, %%xmm4")
                __ASM_EMIT("haddps      %%xmm7, %%xmm6")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
                __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")

                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $12, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("haddps      %%xmm1, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movlps      0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("haddps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")

                // End
                __ASM_EMIT("6:")
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count), [off] "=&r" (off)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void x64_pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);

            ARCH_X86_64_ASM
            (
                /* Do block processing */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          3f")

                __ASM_EMIT(".align 16")
                __ASM_EMIT("2:")
                /* Process vectorized data */
                __ASM_EMIT("lddqu       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("lddqu       0x10(%[src1], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("lddqu       0x20(%[src1], %[off]), %%xmm8")
                __ASM_EMIT("lddqu       0x30(%[src1], %[off]), %%xmm12")
                __ASM_EMIT("lddqu       0x00(%[src2], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
                __ASM_EMIT("lddqu       0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                __ASM_EMIT("lddqu       0x20(%[src2], %[off]), %%xmm9")
                __ASM_EMIT("lddqu       0x30(%[src2], %[off]), %%xmm13")
                /* Calc multiplication */
                __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
                __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
                __ASM_EMIT("movsldup    %%xmm8, %%xmm10")
                __ASM_EMIT("movsldup    %%xmm12, %%xmm14")
                __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
                __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
                __ASM_EMIT("movshdup    %%xmm8, %%xmm8")
                __ASM_EMIT("movshdup    %%xmm12, %%xmm12")
                __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm9, %%xmm8")
                __ASM_EMIT("mulps       %%xmm13, %%xmm12")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
                __ASM_EMIT("mulps       %%xmm9, %%xmm10")
                __ASM_EMIT("mulps       %%xmm13, %%xmm14")
                __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
                __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
                __ASM_EMIT("shufps      $0xb1, %%xmm8, %%xmm8")
                __ASM_EMIT("shufps      $0xb1, %%xmm12, %%xmm12")
                __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
                __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
                __ASM_EMIT("addsubps    %%xmm8, %%xmm10")
                __ASM_EMIT("addsubps    %%xmm12, %%xmm14")
                /* Store */
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm10, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm14, 0x30(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         2b")

                /* 4-element block */
                __ASM_EMIT("3:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                /* Process vectorized data */
                __ASM_EMIT("lddqu       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("lddqu       0x10(%[src1], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("lddqu       0x00(%[src2], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
                __ASM_EMIT("lddqu       0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                /* Calc multiplication */
                __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
                __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
                __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
                __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
                __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
                __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
                __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
                __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
                /* Store */
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                /* Check count again */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jle         6f")
                /* Process scalar data */
                __ASM_EMIT("5:")
                /* Load */
                __ASM_EMIT("movss       0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar */
                __ASM_EMIT("movss       0x04(%[src1], %[off]), %%xmm2")     /* xmm2 = ai */
                __ASM_EMIT("movss       0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br */
                __ASM_EMIT("movss       0x04(%[src2], %[off]), %%xmm6")     /* xmm6 = bi */
                /* Calculate multiplication */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
                __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
                __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
                /* Store */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x08, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                /* Exit */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src1] "+r" (src1), [src2] "+r" (src2),
                  [off] "=&r" (off),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2",
                  "%xmm4", "%xmm5", "%xmm6",
                  "%xmm8", "%xmm9", "%xmm10",
                  "%xmm12", "%xmm13", "%xmm14"
            );
        }

        void x64_pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);

            ARCH_X86_64_ASM
            (
                /* Do block processing */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          3f")

                __ASM_EMIT(".align 16")
                __ASM_EMIT("2:")
                /* Process vectorized data */
                __ASM_EMIT("lddqu       0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("lddqu       0x10(%[dst], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("lddqu       0x20(%[dst], %[off]), %%xmm8")
                __ASM_EMIT("lddqu       0x30(%[dst], %[off]), %%xmm12")
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                __ASM_EMIT("lddqu       0x20(%[src], %[off]), %%xmm9")
                __ASM_EMIT("lddqu       0x30(%[src], %[off]), %%xmm13")
                /* Calc multiplication */
                __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
                __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
                __ASM_EMIT("movsldup    %%xmm8, %%xmm10")
                __ASM_EMIT("movsldup    %%xmm12, %%xmm14")
                __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
                __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
                __ASM_EMIT("movshdup    %%xmm8, %%xmm8")
                __ASM_EMIT("movshdup    %%xmm12, %%xmm12")
                __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm9, %%xmm8")
                __ASM_EMIT("mulps       %%xmm13, %%xmm12")
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
                __ASM_EMIT("mulps       %%xmm9, %%xmm10")
                __ASM_EMIT("mulps       %%xmm13, %%xmm14")
                __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
                __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
                __ASM_EMIT("shufps      $0xb1, %%xmm8, %%xmm8")
                __ASM_EMIT("shufps      $0xb1, %%xmm12, %%xmm12")
                __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
                __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
                __ASM_EMIT("addsubps    %%xmm8, %%xmm10")
                __ASM_EMIT("addsubps    %%xmm12, %%xmm14")
                /* Store */
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm10, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm14, 0x30(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         2b")

                /* 4-element block */
                __ASM_EMIT("3:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                /* Process vectorized data */
                __ASM_EMIT("lddqu       0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("lddqu       0x10(%[dst], %[off]), %%xmm4")     /* xmm4 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("lddqu       0x00(%[src], %[off]), %%xmm1")     /* xmm1 = br0 bi0 br1 bi1 */
                __ASM_EMIT("lddqu       0x10(%[src], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                /* Calc multiplication */
                __ASM_EMIT("movsldup    %%xmm0, %%xmm2")            /* xmm2 = ar0 ar0 ar1, ar1 */
                __ASM_EMIT("movsldup    %%xmm4, %%xmm6")            /* xmm6 = ar2 ar2 ar3, ar3 */
                __ASM_EMIT("movshdup    %%xmm0, %%xmm0")            /* xmm0 = ai0 ai0 ai1, ai1 */
                __ASM_EMIT("movshdup    %%xmm4, %%xmm4")            /* xmm4 = ai2 ai2 ai3, ai3 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm0")            /* xmm0 = ai0*br0 ai0*bi0 ai1*br1 ai1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm4")            /* xmm4 = ai2*br2 ai2*bi2 ai3*br3 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm2")            /* xmm2 = ar0*br0 ar0*bi0 ar1*br1 ar1*bi1 */
                __ASM_EMIT("mulps       %%xmm5, %%xmm6")            /* xmm6 = ar2*br2 ar2*bi2 ar3*br3 ar3*bi3 */
                __ASM_EMIT("shufps      $0xb1, %%xmm0, %%xmm0")     /* xmm0 = ai0*bi0 ai0*br0 ai1*bi1 ai1*br1 */
                __ASM_EMIT("shufps      $0xb1, %%xmm4, %%xmm4")     /* xmm4 = ai2*bi2 ai2*br2 ai3*bi3 ai3*br3 */
                __ASM_EMIT("addsubps    %%xmm0, %%xmm2")            /* xmm2 = ar0*br0 - ai0*bi0 ar0*bi0 + ai0*br0 ar1*br1 - ai1*bi1 ar1*bi1 + ai1*br1 */
                __ASM_EMIT("addsubps    %%xmm4, %%xmm6")            /* xmm6 = ar2*br2 - ai2*bi2 ar2*bi2 + ai2*br2 ar3*br3 - ai3*bi3 ar3*bi3 + ai3*br3 */
                /* Store */
                __ASM_EMIT("movdqu      %%xmm2, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x10(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $4, %[count]")

                /* Check count again */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jle         6f")
                /* Process scalar data */
                __ASM_EMIT("5:")
                /* Load */
                __ASM_EMIT("movss       0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar */
                __ASM_EMIT("movss       0x04(%[dst], %[off]), %%xmm2")     /* xmm2 = ai */
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm4")     /* xmm4 = br */
                __ASM_EMIT("movss       0x04(%[src], %[off]), %%xmm6")     /* xmm6 = bi */
                /* Calculate multiplication */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar */
                __ASM_EMIT("mulss       %%xmm4, %%xmm0")            /* xmm0 = ar*br */
                __ASM_EMIT("mulss       %%xmm6, %%xmm1")            /* xmm1 = ar*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm6")            /* xmm6 = ai*bi */
                __ASM_EMIT("mulss       %%xmm2, %%xmm4")            /* xmm4 = ai*br */
                __ASM_EMIT("addss       %%xmm4, %%xmm1")            /* xmm1 = ar*bi + ai*br = i */
                __ASM_EMIT("subss       %%xmm6, %%xmm0")            /* xmm0 = ar*br - ai*bi = r */
                /* Store */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x08, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                /* Exit */
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src),
                  [off] "=&r" (off),
                  [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2",
                  "%xmm4", "%xmm5", "%xmm6",
                  "%xmm8", "%xmm9", "%xmm10",
                  "%xmm12", "%xmm13", "%xmm14"
            );
        }

        void x64_pcomplex_mod(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86_64(size_t off);

            ARCH_X86_64_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $32, %[count]")
                __ASM_EMIT("jb          2f")

                // 32x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
                __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
                __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
                __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")
                __ASM_EMIT("lddqu       0x80(%[src], %[off], 2), %%xmm8")
                __ASM_EMIT("lddqu       0x90(%[src], %[off], 2), %%xmm9")
                __ASM_EMIT("lddqu       0xa0(%[src], %[off], 2), %%xmm10")
                __ASM_EMIT("lddqu       0xb0(%[src], %[off], 2), %%xmm11")
                __ASM_EMIT("lddqu       0xc0(%[src], %[off], 2), %%xmm12")
                __ASM_EMIT("lddqu       0xd0(%[src], %[off], 2), %%xmm13")
                __ASM_EMIT("lddqu       0xe0(%[src], %[off], 2), %%xmm14")
                __ASM_EMIT("lddqu       0xf0(%[src], %[off], 2), %%xmm15")

                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("mulps       %%xmm6, %%xmm6")
                __ASM_EMIT("mulps       %%xmm7, %%xmm7")
                __ASM_EMIT("mulps       %%xmm8, %%xmm8")
                __ASM_EMIT("mulps       %%xmm9, %%xmm9")
                __ASM_EMIT("mulps       %%xmm10, %%xmm10")
                __ASM_EMIT("mulps       %%xmm11, %%xmm11")
                __ASM_EMIT("mulps       %%xmm12, %%xmm12")
                __ASM_EMIT("mulps       %%xmm13, %%xmm13")
                __ASM_EMIT("mulps       %%xmm14, %%xmm14")
                __ASM_EMIT("mulps       %%xmm15, %%xmm15")

                __ASM_EMIT("haddps      %%xmm1, %%xmm0")
                __ASM_EMIT("haddps      %%xmm3, %%xmm2")
                __ASM_EMIT("haddps      %%xmm5, %%xmm4")
                __ASM_EMIT("haddps      %%xmm7, %%xmm6")
                __ASM_EMIT("haddps      %%xmm9, %%xmm8")
                __ASM_EMIT("haddps      %%xmm11, %%xmm10")
                __ASM_EMIT("haddps      %%xmm13, %%xmm12")
                __ASM_EMIT("haddps      %%xmm15, %%xmm14")

                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
                __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")
                __ASM_EMIT("sqrtps      %%xmm8, %%xmm8")
                __ASM_EMIT("sqrtps      %%xmm10, %%xmm10")
                __ASM_EMIT("sqrtps      %%xmm12, %%xmm12")
                __ASM_EMIT("sqrtps      %%xmm14, %%xmm14")

                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm8, 0x40(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm10, 0x50(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm12, 0x60(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm14, 0x70(%[dst], %[off])")

                __ASM_EMIT("add         $0x80, %[off]")
                __ASM_EMIT("sub         $32, %[count]")
                __ASM_EMIT("jae         1b")

                // 16x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $16, %[count]")
                __ASM_EMIT("jl          4f")

                __ASM_EMIT("3:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("lddqu       0x20(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("lddqu       0x30(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("lddqu       0x40(%[src], %[off], 2), %%xmm4")
                __ASM_EMIT("lddqu       0x50(%[src], %[off], 2), %%xmm5")
                __ASM_EMIT("lddqu       0x60(%[src], %[off], 2), %%xmm6")
                __ASM_EMIT("lddqu       0x70(%[src], %[off], 2), %%xmm7")

                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("mulps       %%xmm6, %%xmm6")
                __ASM_EMIT("mulps       %%xmm7, %%xmm7")
                __ASM_EMIT("haddps      %%xmm1, %%xmm0")
                __ASM_EMIT("haddps      %%xmm3, %%xmm2")
                __ASM_EMIT("haddps      %%xmm5, %%xmm4")
                __ASM_EMIT("haddps      %%xmm7, %%xmm6")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm4, %%xmm4")
                __ASM_EMIT("sqrtps      %%xmm6, %%xmm6")

                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm4, 0x20(%[dst], %[off])")
                __ASM_EMIT("movdqu      %%xmm6, 0x30(%[dst], %[off])")
                __ASM_EMIT("add         $0x40, %[off]")
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("jae         3b")

                // 4x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $12, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("lddqu       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("lddqu       0x10(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("haddps      %%xmm1, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("movdqu      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jge         5b")

                // 1x blocks
                __ASM_EMIT("6:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("movlps      0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("haddps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         7b")

                // End
                __ASM_EMIT("8:")
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count), [off] "=&r" (off)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10",
                  "%xmm12", "%xmm13", "%xmm14"
            );
        }

        IF_ARCH_X86_64(
            static const float pcomplex_corr_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-36f),
            };
        );

        void x64_pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86_64(size_t off);

            /*
             * src1[i] = a + j*b, src2[i] = c + j*d
             * den  = (a*a + b*b)*(c*c + d*d)
             * nom  = a*c + b*d
             * corr[i] = (den > threshold) ? nom / sqrt(den) : 0.0
             */
            ARCH_X86_64_ASM (
                __ASM_EMIT("xor         %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub         $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("movaps      %[CC], %%xmm7")                         /* xmm7  = threshold */
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("movups      0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("movups      0x20(%[src1], %[off], 2), %%xmm8")      /* xmm8  = a4 b4 a5 b5 */
                __ASM_EMIT("movups      0x30(%[src1], %[off], 2), %%xmm12")     /* xmm12 = a6 b6 a7 b7 */
                __ASM_EMIT("movups      0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("movups      0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                __ASM_EMIT("movups      0x20(%[src2], %[off], 2), %%xmm10")     /* xmm10 = c4 d4 c5 d5 */
                __ASM_EMIT("movups      0x30(%[src2], %[off], 2), %%xmm14")     /* xmm14 = c6 d6 c7 d7 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        /* xmm1  = a0 b0 a1 b1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")                        /* xmm3  = c0 d0 c1 d1 */
                __ASM_EMIT("movaps      %%xmm8, %%xmm9")                        /* xmm9  = a4 b4 a5 b5 */
                __ASM_EMIT("movaps      %%xmm10, %%xmm11")                      /* xmm11 = c4 d4 c5 d5 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")                 /* xmm0  = A0 = a0 a1 a2 a3 */
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")                 /* xmm2  = C0 = c0 c1 c2 c3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")                 /* xmm1  = B0 = b0 b1 b2 b3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")                 /* xmm3  = D0 = d0 d1 d2 d3 */
                __ASM_EMIT("shufps      $0x88, %%xmm12, %%xmm8")                /* xmm8  = A1 = a0 a1 a2 a3 */
                __ASM_EMIT("shufps      $0x88, %%xmm14, %%xmm10")               /* xmm10 = C1 = c0 c1 c2 c3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm12, %%xmm9")                /* xmm9  = B1 = b0 b1 b2 b3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm14, %%xmm11")               /* xmm11 = D1 = d0 d1 d2 d3 */
                /* xmm0 = A0, xmm1 = B0, xmm2 = C0, xmm3 = D0   */
                /* xmm8 = A1, xmm9 = B1, xmm10 = C1, xmm11 = D1 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = A0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = B0 */
                __ASM_EMIT("movaps      %%xmm8, %%xmm12")                       /* xmm12 = A1 */
                __ASM_EMIT("movaps      %%xmm9, %%xmm13")                       /* xmm13 = B1 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")                        /* xmm4  = A0*C0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")                        /* xmm5  = B0*D0 */
                __ASM_EMIT("mulps       %%xmm10, %%xmm12")                      /* xmm10 = A1*C1 */
                __ASM_EMIT("mulps       %%xmm11, %%xmm13")                      /* xmm13 = B1*D1 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                        /* xmm0  = A0*A0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                        /* xmm1  = B0*B0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")                        /* xmm2  = C0*C0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                        /* xmm3  = D0*D0 */
                __ASM_EMIT("mulps       %%xmm8, %%xmm8")                        /* xmm8  = A1*A1 */
                __ASM_EMIT("mulps       %%xmm9, %%xmm9")                        /* xmm9  = B1*B1 */
                __ASM_EMIT("mulps       %%xmm10, %%xmm10")                      /* xmm10 = C1*C1 */
                __ASM_EMIT("mulps       %%xmm11, %%xmm11")                      /* xmm11 = D1*D1 */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")                        /* xmm0  = A0*A0 + B0*B0 */
                __ASM_EMIT("addps       %%xmm3, %%xmm2")                        /* xmm2  = C0*C0 + D0*D0 */
                __ASM_EMIT("addps       %%xmm9, %%xmm8")                        /* xmm8  = A1*A1 + B1*B1 */
                __ASM_EMIT("addps       %%xmm11, %%xmm10")                      /* xmm10 = C1*C1 + D1*D1 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm0")                        /* xmm0  = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("mulps       %%xmm10, %%xmm8")                       /* xmm8  = den1 = (A1*A1 + B1*B1)*(C1*C1 + D1*D1) */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                        /* xmm4  = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("addps       %%xmm13, %%xmm12")                      /* xmm12 = nom1 = A1*C1 + B1*D1 */
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den0) */
                __ASM_EMIT("sqrtps      %%xmm8, %%xmm9")                        /* xmm9  = sqrt(den1) */
                __ASM_EMIT("divps       %%xmm1, %%xmm4")                        /* xmm4  = nom0 / sqrt(den0) */
                __ASM_EMIT("divps       %%xmm9, %%xmm12")                       /* xmm12 = nom1 / sqrt(den1) */
                __ASM_EMIT("cmpps       $5, %%xmm7, %%xmm0")                    /* xmm0  = [den0 >= threshold] */
                __ASM_EMIT("cmpps       $5, %%xmm7, %%xmm8")                    /* xmm8  = [den1 >= threshold] */
                __ASM_EMIT("andps       %%xmm4, %%xmm0")                        /* xmm0  = (den0 >= threshold) ? nom0 / sqrt(den0) : 0.0f */
                __ASM_EMIT("andps       %%xmm12, %%xmm8")                       /* xmm8  = (den0 >= threshold) ? nom0 / sqrt(den0) : 0.0f */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 */
                __ASM_EMIT("movups      %%xmm8, 0x10(%[dst], %[off])")          /* dst[1]  = v4 v5 v6 v7 */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("jae         1b")
                /* x4 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")                          /* count += 4 */
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("movups      0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("movups      0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("movups      0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        /* xmm1  = a0 b0 a1 b1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")                        /* xmm3  = c0 d0 c1 d1 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")                 /* xmm0  = A0 = a0 a1 a2 a3 */
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm2")                 /* xmm2  = C0 = c0 c1 c2 c3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")                 /* xmm1  = B0 = b0 b1 b2 b3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm3")                 /* xmm3  = D0 = d0 d1 d2 d3 */
                /* xmm0 = A0, xmm1 = B0, xmm2 = C0, xmm3 = D0   */
                /* xmm8 = A1, xmm9 = B1, xmm10 = C1, xmm11 = D1 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = A0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = B0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")                        /* xmm4  = A0*C0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")                        /* xmm5  = B0*D0 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                        /* xmm0  = A0*A0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                        /* xmm1  = B0*B0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")                        /* xmm2  = C0*C0 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                        /* xmm3  = D0*D0 */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")                        /* xmm0  = A0*A0 + B0*B0 */
                __ASM_EMIT("addps       %%xmm3, %%xmm2")                        /* xmm2  = C0*C0 + D0*D0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm0")                        /* xmm0  = den0 = (A0*A0 + B0*B0)*(C0*C0 + D0*D0) */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                        /* xmm4  = nom0 = A0*C0 + B0*D0 */
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den0) */
                __ASM_EMIT("divps       %%xmm1, %%xmm4")                        /* xmm4  = nom0 / sqrt(den0) */
                __ASM_EMIT("cmpps       $5, %%xmm7, %%xmm0")                    /* xmm0  = [den0 >= threshold] */
                __ASM_EMIT("andps       %%xmm4, %%xmm0")                        /* xmm0  = (den0 >= threshold) ? nom0 / sqrt(den0) : 0.0f */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 */
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")                          /* count -= 4 */
                /* x1 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a */
                __ASM_EMIT("movss       0x04(%[src1], %[off], 2), %%xmm1")      /* xmm1  = b */
                __ASM_EMIT("movss       0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c */
                __ASM_EMIT("movss       0x04(%[src2], %[off], 2), %%xmm3")      /* xmm3  = d */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = a */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = b */
                __ASM_EMIT("mulss       %%xmm2, %%xmm4")                        /* xmm4  = a*c */
                __ASM_EMIT("mulss       %%xmm3, %%xmm5")                        /* xmm5  = b*d */
                __ASM_EMIT("mulss       %%xmm0, %%xmm0")                        /* xmm0  = a*a */
                __ASM_EMIT("mulss       %%xmm1, %%xmm1")                        /* xmm1  = b*b */
                __ASM_EMIT("mulss       %%xmm2, %%xmm2")                        /* xmm2  = c*c */
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")                        /* xmm3  = d*d */
                __ASM_EMIT("addss       %%xmm1, %%xmm0")                        /* xmm0  = a*a + b*b */
                __ASM_EMIT("addss       %%xmm3, %%xmm2")                        /* xmm2  = c*c + d*d */
                __ASM_EMIT("mulss       %%xmm2, %%xmm0")                        /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("addss       %%xmm5, %%xmm4")                        /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("sqrtss      %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den) */
                __ASM_EMIT("divss       %%xmm1, %%xmm4")                        /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("cmpps       $5, %%xmm7, %%xmm0")                    /* xmm0  = [den >= threshold] */
                __ASM_EMIT("andps       %%xmm4, %%xmm0")                        /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 */
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")
                /* End */
                __ASM_EMIT("6:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst_corr), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "m" (pcomplex_corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }


    } /* namespace sse3 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE3_PCOMPLEX_H_ */
