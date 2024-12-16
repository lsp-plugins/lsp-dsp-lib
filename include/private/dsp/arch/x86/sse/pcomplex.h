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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_SSE_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const uint32_t pcomplex_div_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x3f800000),
                LSP_DSP_VEC4(0x80000000)
            };
        );

        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                /* Check count */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jb          3f")

                __ASM_EMIT(".align 16")
                __ASM_EMIT("2:")
                /* Process vectorized data */
                __ASM_EMIT("movups      0x00(%[dst], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("movups      0x10(%[dst], %[off]), %%xmm1")     /* xmm1 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm4")     /* xmm4 = br0 bi0 br1 bi1 */
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = br0 bi0 br1 bi1 */
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0 = ar0 ar1 ar2 ar3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4 = br0 br1 br2 br3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2 = ai0 ai1 ai2 ai3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6 = bi0 bi1 bi2 bi3 */
                /* Calc multiplication */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
                __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
                /* Re-shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
                /* Store */
                __ASM_EMIT("movups       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups       %%xmm2, 0x10(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         2b")

                __ASM_EMIT("3:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jle         5f")
                /* Process scalar data */
                __ASM_EMIT("4:")
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
                __ASM_EMIT("jnz         4b")

                /* Exit */
                __ASM_EMIT("5:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                /* Check count */
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jb          3f")

                __ASM_EMIT(".align 16")
                __ASM_EMIT("2:")
                /* Process vectorized data */
                __ASM_EMIT("movups      0x00(%[src1], %[off]), %%xmm0")     /* xmm0 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("movups      0x10(%[src1], %[off]), %%xmm1")     /* xmm1 = ar2 ai2 ar3 ai3 */
                __ASM_EMIT("movups      0x00(%[src2], %[off]), %%xmm4")     /* xmm4 = br0 bi0 br1 bi1 */
                __ASM_EMIT("movups      0x10(%[src2], %[off]), %%xmm5")     /* xmm5 = br2 bi2 br3 bi3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = ar0 ai0 ar1 ai1 */
                __ASM_EMIT("movaps      %%xmm4, %%xmm6")            /* xmm6 = br0 bi0 br1 bi1 */
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0")     /* xmm0 = ar0 ar1 ar2 ar3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4")     /* xmm4 = br0 br1 br2 br3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm1, %%xmm2")     /* xmm2 = ai0 ai1 ai2 ai3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm6")     /* xmm6 = bi0 bi1 bi2 bi3 */
                /* Calc multiplication */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1 = ar0 ar1 ar2 ar3 */
                __ASM_EMIT("mulps       %%xmm4, %%xmm0")            /* xmm0 = ar0*br0 ar1*br1 ar2*br2 ar3*br3 */
                __ASM_EMIT("mulps       %%xmm6, %%xmm1")            /* xmm1 = ar0*bi0 ar1*bi1 ar2*bi2 ar3*bi3 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6 = ai0*bi0 ai1*bi1 ai2*bi2 ai3*bi3 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")            /* xmm4 = ai0*br0 ai1*br1 ai2*br2 ai3*br3 */
                __ASM_EMIT("addps       %%xmm4, %%xmm1")            /* xmm1 = ar[i]*bi[i] + ai[i]*br[i] = i0 i1 i2 i3 */
                __ASM_EMIT("subps       %%xmm6, %%xmm0")            /* xmm0 = ar[i]*br[i] - ai[i]*bi[i] = r0 r1 r2 r3 */
                /* Re-shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm2")            /* xmm2 = r0 r1 r2 r3 */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm2")            /* xmm2 = r2 i2 r3 i3 */
                /* Store */
                __ASM_EMIT("movups       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups       %%xmm2, 0x10(%[dst], %[off])")
                /* Repeat loop */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         2b")

                __ASM_EMIT("3:")
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jle         5f")
                /* Process scalar data */
                __ASM_EMIT("4:")
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
                __ASM_EMIT("jnz         4b")

                /* Exit */
                __ASM_EMIT("5:")

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm4", "%xmm5", "%xmm6"
            );
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("xorps       %%xmm6, %%xmm6")
                __ASM_EMIT("xorps       %%xmm7, %%xmm7")

                /* Do block processing */
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jb          2f")
                /* Do with 16x blocks */
                __ASM_EMIT(".align 16")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  /* xmm0  = r0  r1  r2  r3  */
                __ASM_EMIT("movups      0x10(%[src]), %%xmm1")  /* xmm1  = r4  r5  r6  r7  */
                __ASM_EMIT("movups      0x20(%[src]), %%xmm2")  /* xmm2  = r8  r9  r10 r11 */
                __ASM_EMIT("movups      0x30(%[src]), %%xmm3")  /* xmm3  = r12 r13 r14 r15 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0")
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm1")
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4")
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")
                __ASM_EMIT("movaps      %%xmm2, %%xmm4")
                __ASM_EMIT("movaps      %%xmm3, %%xmm5")
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm2")
                __ASM_EMIT("unpcklps    %%xmm7, %%xmm3")
                __ASM_EMIT("unpckhps    %%xmm6, %%xmm4")
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm5")
                __ASM_EMIT("movups      %%xmm2, 0x40(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x50(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x60(%[dst])")
                __ASM_EMIT("movups      %%xmm5, 0x70(%[dst])")
                /* Repeat loop */
                __ASM_EMIT("sub         $16, %[count]")
                __ASM_EMIT("add         $0x40, %[src]")
                __ASM_EMIT("add         $0x80, %[dst]")
                __ASM_EMIT("cmp         $16, %[count]")
                __ASM_EMIT("jae         1b")
                /* 4x iterations */
                __ASM_EMIT("2:")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jb          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")
                __ASM_EMIT("unpcklps    %%xmm6, %%xmm0")
                __ASM_EMIT("unpckhps    %%xmm7, %%xmm4")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")

                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("add         $0x10, %[src]")
                __ASM_EMIT("add         $0x20, %[dst]")
                __ASM_EMIT("cmp         $4, %[count]")
                __ASM_EMIT("jae         3b")
                __ASM_EMIT("jmp         4f")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("movlps      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x4, %[src]")
                __ASM_EMIT("add         $0x8, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_c2r(float *dst, const float *src, size_t count)
        {
            #define complex_to_real_core(MV_DST, MV_SRC) \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jb          2f") \
                /* Do with 16x blocks */ \
                __ASM_EMIT(".align 16") \
                __ASM_EMIT("1:") \
                /* Load data */ \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0")  /* xmm0  = r0 i0 r1 i1 */ \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1")  /* xmm1  = r2 i2 r3 i3 */ \
                __ASM_EMIT(MV_SRC "     0x20(%[src]), %%xmm2")  \
                __ASM_EMIT(MV_SRC "     0x30(%[src]), %%xmm3")  \
                __ASM_EMIT(MV_SRC "     0x40(%[src]), %%xmm4")  \
                __ASM_EMIT(MV_SRC "     0x50(%[src]), %%xmm5")  \
                __ASM_EMIT(MV_SRC "     0x60(%[src]), %%xmm6")  \
                __ASM_EMIT(MV_SRC "     0x70(%[src]), %%xmm7")  \
                /* Pack data */ \
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0") /* xmm0  = r0 r1 r2 r3 */ \
                __ASM_EMIT("shufps      $0x88, %%xmm3, %%xmm2") \
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm4") \
                __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm6") \
                /* Store data */ \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm2, 0x10(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm4, 0x20(%[dst])") \
                __ASM_EMIT(MV_DST "     %%xmm6, 0x30(%[dst])") \
                /* Repeat loop */ \
                __ASM_EMIT("sub         $16, %[count]") \
                __ASM_EMIT("add         $0x80, %[src]") \
                __ASM_EMIT("add         $0x40, %[dst]") \
                __ASM_EMIT("cmp         $16, %[count]") \
                __ASM_EMIT("jae         1b") \
                /* 4x iterations */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jb          4f") \
                \
                __ASM_EMIT("3:") \
                __ASM_EMIT(MV_SRC "     0x00(%[src]), %%xmm0") \
                __ASM_EMIT(MV_SRC "     0x10(%[src]), %%xmm1") \
                __ASM_EMIT("shufps      $0x88, %%xmm1, %%xmm0") \
                __ASM_EMIT(MV_DST "     %%xmm0, 0x00(%[dst])") \
                \
                __ASM_EMIT("sub         $4, %[count]") \
                __ASM_EMIT("add         $0x20, %[src]") \
                __ASM_EMIT("add         $0x10, %[dst]") \
                __ASM_EMIT("cmp         $4, %[count]") \
                __ASM_EMIT("jae         3b") \
                __ASM_EMIT("jmp         4f")


            ARCH_X86_ASM
            (
                /* Do block processing */
                __ASM_EMIT("test $0x0f, %[dst]")
                __ASM_EMIT("jnz 110f")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 101f")
                        complex_to_real_core("movaps", "movaps")
                    __ASM_EMIT("101:")
                        complex_to_real_core("movaps", "movups")

                __ASM_EMIT("110:")
                    __ASM_EMIT("test $0x0f, %[src]")
                    __ASM_EMIT("jnz 111f")
                        complex_to_real_core("movups", "movaps")
                    __ASM_EMIT("111:")
                        complex_to_real_core("movups", "movups")

                // 1x iterations
                __ASM_EMIT("4:")
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          6f")

                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src]), %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add         $0x8, %[src]")
                __ASM_EMIT("add         $0x4, %[dst]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jnz         5b")

                // End of routine
                __ASM_EMIT("6:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );

            #undef complex_to_real_core
        }

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            size_t off;

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor         %[off], %[off]")
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jb          2f")

                // 12x blocks
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("movups      0x10(%[src], %[off], 2), %%xmm5")
                __ASM_EMIT("movups      0x20(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("movups      0x30(%[src], %[off], 2), %%xmm6")
                __ASM_EMIT("movups      0x40(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("movups      0x50(%[src], %[off], 2), %%xmm7")

                __ASM_EMIT("movaps      %%xmm0, %%xmm3")
                __ASM_EMIT("movaps      %%xmm1, %%xmm4")
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm0") /* xmm0 = re[0..3] */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3") /* xmm3 = im[0..3] */
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")
                __ASM_EMIT("shufps      $0x88, %%xmm6, %%xmm1") /* xmm1 = re[4..7] */
                __ASM_EMIT("shufps      $0xdd, %%xmm6, %%xmm4") /* xmm4 = im[4..7] */
                __ASM_EMIT("shufps      $0x88, %%xmm7, %%xmm2") /* xmm2 = re[8..11] */
                __ASM_EMIT("shufps      $0xdd, %%xmm7, %%xmm5") /* xmm5 = im[8..11] */

                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("mulps       %%xmm4, %%xmm4")
                __ASM_EMIT("mulps       %%xmm5, %%xmm5")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")
                __ASM_EMIT("addps       %%xmm4, %%xmm1")
                __ASM_EMIT("addps       %%xmm5, %%xmm2")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm1, %%xmm1")
                __ASM_EMIT("sqrtps      %%xmm2, %%xmm2")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm1, 0x10(%[dst], %[off])")
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off])")
                __ASM_EMIT("add         $0x30, %[off]")
                __ASM_EMIT("sub         $12, %[count]")
                __ASM_EMIT("jae         1b")

                // 4x blocks
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $8, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movups      0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("movups      0x10(%[src], %[off], 2), %%xmm5")
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm0") /* xmm0 = re[0..3] */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3") /* xmm3 = im[0..3] */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")
                __ASM_EMIT("addps       %%xmm3, %%xmm0")
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm0")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("jae         3b")

                // 1x blocks
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("movss       0x04(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("mulss       %%xmm0, %%xmm0")
                __ASM_EMIT("mulss       %%xmm3, %%xmm3")
                __ASM_EMIT("addss       %%xmm3, %%xmm0")
                __ASM_EMIT("sqrtss      %%xmm0, %%xmm0")
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add         $0x4, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")

                // End
                __ASM_EMIT("6:")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_rcp2(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM (
                /* Do block processing */
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          50f")

                __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                /* Perform 8x RCP */
                __ASM_EMIT("movups      0x00(%[src]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[src]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
                __ASM_EMIT("movups      0x20(%[src]),  %%xmm2")     /* xmm2  = r4 i4 r5 i5 */
                __ASM_EMIT("movups      0x30(%[src]),  %%xmm5")     /* xmm5  = r6 i6 r7 i7 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3  = r4 i4 r5 i5 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")     /* xmm2  = r4 r5 r6 r7 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")     /* xmm3  = i4 i5 i6 i7 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm6  = r1 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* xmm7  = i1 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = r1*r1 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm7")            /* xmm7  = i1*i1 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("addps       %%xmm7, %%xmm6")            /* xmm6  = r1*r1 + i1*i1 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("movaps      %%xmm5, %%xmm7")            /* xmm7  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm3")       /* xmm3  = -i1 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("divps       %%xmm6, %%xmm7")            /* xmm7  = 1 / (r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2  = r1' = r1 / (r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")            /* xmm3  = i1' = -i1 / (r1*r1 + i1*i1) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")            /* xmm5 = r4' r5' r6' r7' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")            /* xmm2 = r4' i4' r5' i5' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */
                __ASM_EMIT("unpckhps    %%xmm3, %%xmm5")            /* xmm5 = r6' i6' r7' i7' */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3' */
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")      /* dst[2]  = r4' i4' r5' i5' */
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")      /* dst[3]  = r6' i6' r7' i7' */
                __ASM_EMIT("add         $0x40, %[src]")             /* src += 16 */
                __ASM_EMIT("add         $0x40, %[dst]")             /* dst += 16 */
                __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
                __ASM_EMIT("jae         1b")
                /* Next step */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
                __ASM_EMIT("jl          10f")
                /* Perform 4x RCP */
                __ASM_EMIT("movups      0x00(%[src]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[src]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3'  */
                __ASM_EMIT("add         $0x20, %[src]")             /* src += 8 */
                __ASM_EMIT("add         $0x20, %[dst]")             /* dst += 8 */
                __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */

                /* 1x RCP */
                __ASM_EMIT("10:")
                __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
                __ASM_EMIT("jle         50f")
                __ASM_EMIT("40:")
                __ASM_EMIT("movss       0x00(%[src]),  %%xmm0")     /* xmm0  = r0 */
                __ASM_EMIT("movss       0x04(%[src]),  %%xmm1")     /* xmm1  = i0 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("mulss       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulss       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("addss       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("divss       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' */
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst])")      /* dst[1]  = i0' */
                __ASM_EMIT("add         $0x08, %[src]")             /* src += 2 */
                __ASM_EMIT("add         $0x08, %[dst]")             /* dst += 2 */
                __ASM_EMIT("dec         %[count]")                  /* count-- */
                __ASM_EMIT("jnz         40b")

                __ASM_EMIT("50:")
                : [dst] "+r" (dst), [src] "+r" (src),
                  [count] "+r" (count)
                : [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_rcp1(float *dst, size_t count)
        {
            ARCH_X86_ASM (
                /* Do block processing */
                __ASM_EMIT("test        %[count], %[count]")
                __ASM_EMIT("jz          50f")

                __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                /* Perform 8x RCP */
                __ASM_EMIT("movups      0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[dst]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
                __ASM_EMIT("movups      0x20(%[dst]),  %%xmm2")     /* xmm2  = r4 i4 r5 i5 */
                __ASM_EMIT("movups      0x30(%[dst]),  %%xmm5")     /* xmm5  = r6 i6 r7 i7 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")            /* xmm3  = r4 i4 r5 i5 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")     /* xmm2  = r4 r5 r6 r7 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")     /* xmm3  = i4 i5 i6 i7 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")            /* xmm6  = r1 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")            /* xmm7  = i1 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")            /* xmm6  = r1*r1 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm7")            /* xmm7  = i1*i1 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("addps       %%xmm7, %%xmm6")            /* xmm6  = r1*r1 + i1*i1 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("movaps      %%xmm5, %%xmm7")            /* xmm7  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm3")       /* xmm3  = -i1 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("divps       %%xmm6, %%xmm7")            /* xmm7  = 1 / (r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")            /* xmm2  = r1' = r1 / (r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")            /* xmm3  = i1' = -i1 / (r1*r1 + i1*i1) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")            /* xmm5 = r4' r5' r6' r7' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")            /* xmm2 = r4' i4' r5' i5' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */
                __ASM_EMIT("unpckhps    %%xmm3, %%xmm5")            /* xmm5 = r6' i6' r7' i7' */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3' */
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst])")      /* dst[2]  = r4' i4' r5' i5' */
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst])")      /* dst[3]  = r6' i6' r7' i7' */
                __ASM_EMIT("add         $0x40, %[dst]")             /* dst += 16 */
                __ASM_EMIT("sub         $8, %[count]")              /* count -= 8 */
                __ASM_EMIT("jae         1b")
                /* Next step */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
                __ASM_EMIT("jl          10f")
                /* Perform 4x RCP */
                __ASM_EMIT("movups      0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[dst]),  %%xmm4")     /* xmm4  = r2 i2 r3 i3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")            /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")     /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")     /* xmm1  = i0 i1 i2 i3 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")            /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")            /* xmm4 = r2' i2' r3' i3' */

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst])")      /* dst[1]  = r2' i2' r3' i3'  */
                __ASM_EMIT("add         $0x20, %[dst]")             /* dst += 8 */
                __ASM_EMIT("sub         $4, %[count]")              /* count -= 4 */
                /* 1x RCP */
                __ASM_EMIT("10:")
                __ASM_EMIT("add         $4, %[count]")              /* count += 4 */
                __ASM_EMIT("jle         50f")
                __ASM_EMIT("40:")
                __ASM_EMIT("movss       0x00(%[dst]),  %%xmm0")     /* xmm0  = r0 */
                __ASM_EMIT("movss       0x04(%[dst]),  %%xmm1")     /* xmm1  = i0 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")            /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")            /* xmm5  = i0 */
                __ASM_EMIT("mulss       %%xmm0, %%xmm4")            /* xmm4  = r0*r0 */
                __ASM_EMIT("mulss       %%xmm1, %%xmm5")            /* xmm5  = i0*i0 */
                __ASM_EMIT("addss       %%xmm5, %%xmm4")            /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movaps      0x00(%[CC]), %%xmm5")       /* xmm5  = 1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")       /* xmm1  = -i0 */
                __ASM_EMIT("divss       %%xmm4, %%xmm5")            /* xmm5  = 1 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm0")            /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm1")            /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst])")      /* dst[0]  = r0' */
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst])")      /* dst[1]  = i0' */
                __ASM_EMIT("add         $0x08, %[dst]")             /* dst += 2 */
                __ASM_EMIT("dec         %[count]")                  /* count-- */
                __ASM_EMIT("jnz         40b")

                __ASM_EMIT("50:")
                : [dst] "+r" (dst),
                  [count] "+r" (count)
                : [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #define PCOMPLEX_DIV_CORE(DST, T, B) \
            __ASM_EMIT("xor             %[off], %[off]") \
            /* x4 blocks */ \
            __ASM_EMIT("sub             $4, %[count]") \
            __ASM_EMIT("jb              2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("movups          0x00(%[" B "], %[off]), %%xmm0")    /* xmm0 = br0 bi0 br1 bi1 */ \
            __ASM_EMIT("movups          0x10(%[" B "], %[off]), %%xmm4")    /* xmm4 = br2 bi2 br3 bi3 */ \
            __ASM_EMIT("movups          0x00(%[" T "], %[off]), %%xmm2")    /* xmm2 = tr0 ti0 tr1 ti1 */ \
            __ASM_EMIT("movups          0x10(%[" T "], %[off]), %%xmm6")    /* xmm6 = tr2 ti2 tr3 ti3 */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm1")                    /* xmm1 = br0 bi0 br1 bi1 */ \
            __ASM_EMIT("movaps          %%xmm2, %%xmm3")                    /* xmm3 = tr0 ti0 tr1 ti1 */ \
            __ASM_EMIT("shufps          $0x88, %%xmm4, %%xmm0")             /* xmm0 = br */ \
            __ASM_EMIT("shufps          $0x88, %%xmm6, %%xmm2")             /* xmm2 = tr */ \
            __ASM_EMIT("shufps          $0xdd, %%xmm4, %%xmm1")             /* xmm1 = bi */ \
            __ASM_EMIT("shufps          $0xdd, %%xmm6, %%xmm3")             /* xmm3 = ti */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm7")                    /* xmm7 = br */ \
            __ASM_EMIT("movaps          %%xmm1, %%xmm6")                    /* xmm6 = bi */ \
            __ASM_EMIT("movaps          %%xmm2, %%xmm4")                    /* xmm4 = tr */ \
            __ASM_EMIT("movaps          %%xmm3, %%xmm5")                    /* xmm5 = ti */ \
            __ASM_EMIT("mulps           %%xmm7, %%xmm7")                    /* xmm7 = br*br */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm6")                    /* xmm6 = bi*bi */ \
            __ASM_EMIT("mulps           %%xmm0, %%xmm2")                    /* xmm2 = tr*br */ \
            __ASM_EMIT("mulps           %%xmm0, %%xmm3")                    /* xmm3 = ti*br */ \
            __ASM_EMIT("addps           %%xmm6, %%xmm7")                    /* xmm7 = R = br*br + bi*bi */ \
            __ASM_EMIT("movaps          0x00 + %[CC], %%xmm6")              /* xmm6 = 1 */ \
            __ASM_EMIT("mulps           %%xmm1, %%xmm5")                    /* xmm5 = ti*bi */ \
            __ASM_EMIT("mulps           %%xmm1, %%xmm4")                    /* xmm4 = tr*bi */ \
            __ASM_EMIT("divps           %%xmm7, %%xmm6")                    /* xmm6 = 1/R */ \
            __ASM_EMIT("addps           %%xmm5, %%xmm2")                    /* xmm2 = r = tr*br + ti*bi */ \
            __ASM_EMIT("subps           %%xmm4, %%xmm3")                    /* xmm3 = i = ti*br - tr*bi */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm2")                    /* xmm2 = r/R */ \
            __ASM_EMIT("mulps           %%xmm6, %%xmm3")                    /* xmm3 = i/R */ \
            __ASM_EMIT("movaps          %%xmm2, %%xmm4")                    /* xmm2 = r/R */ \
            __ASM_EMIT("unpcklps        %%xmm3, %%xmm2")                    /* xmm2 = r0' i0' r1' i1' */ \
            __ASM_EMIT("unpckhps        %%xmm3, %%xmm4")                    /* xmm4 = r2' i2' r3' i3' */ \
            __ASM_EMIT("movups          %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movups          %%xmm4, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add             $0x20, %[off]") \
            __ASM_EMIT("sub             $4, %[count]") \
            __ASM_EMIT("jae             1b") \
            /* x1 blocks */ \
            __ASM_EMIT("2:") \
            __ASM_EMIT("add             $3, %[count]") \
            __ASM_EMIT("jl              4f") \
            __ASM_EMIT("3:") \
            __ASM_EMIT("movss           0x00(%[" B "], %[off]), %%xmm0")    /* xmm0 = br */ \
            __ASM_EMIT("movss           0x04(%[" B "], %[off]), %%xmm1")    /* xmm1 = bi */ \
            __ASM_EMIT("movss           0x00(%[" T "], %[off]), %%xmm2")    /* xmm2 = tr */ \
            __ASM_EMIT("movss           0x04(%[" T "], %[off]), %%xmm3")    /* xmm3 = ti */ \
            __ASM_EMIT("movaps          %%xmm0, %%xmm7")                    /* xmm7 = br */ \
            __ASM_EMIT("movaps          %%xmm1, %%xmm6")                    /* xmm6 = bi */ \
            __ASM_EMIT("movaps          %%xmm2, %%xmm4")                    /* xmm4 = tr */ \
            __ASM_EMIT("movaps          %%xmm3, %%xmm5")                    /* xmm5 = ti */ \
            __ASM_EMIT("mulss           %%xmm7, %%xmm7")                    /* xmm7 = br*br */ \
            __ASM_EMIT("mulss           %%xmm6, %%xmm6")                    /* xmm6 = bi*bi */ \
            __ASM_EMIT("mulss           %%xmm0, %%xmm2")                    /* xmm2 = tr*br */ \
            __ASM_EMIT("mulss           %%xmm0, %%xmm3")                    /* xmm3 = ti*br */ \
            __ASM_EMIT("addss           %%xmm6, %%xmm7")                    /* xmm7 = R = br*br + bi*bi */ \
            __ASM_EMIT("movaps          0x00 + %[CC], %%xmm6")              /* xmm6 = 1 */ \
            __ASM_EMIT("mulss           %%xmm1, %%xmm5")                    /* xmm5 = ti*bi */ \
            __ASM_EMIT("mulss           %%xmm1, %%xmm4")                    /* xmm4 = tr*bi */ \
            __ASM_EMIT("divss           %%xmm7, %%xmm6")                    /* xmm6 = 1/R */ \
            __ASM_EMIT("addss           %%xmm5, %%xmm2")                    /* xmm2 = r = tr*br + ti*bi */ \
            __ASM_EMIT("subss           %%xmm4, %%xmm3")                    /* xmm3 = i = ti*br - tr*bi */ \
            __ASM_EMIT("mulss           %%xmm6, %%xmm2")                    /* xmm2 = r/R */ \
            __ASM_EMIT("mulss           %%xmm6, %%xmm3")                    /* xmm3 = i/R */ \
            __ASM_EMIT("movss           %%xmm2, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("movss           %%xmm3, 0x04(%[" DST "], %[off])") \
            __ASM_EMIT("add             $0x08, %[off]") \
            __ASM_EMIT("dec             %[count]") \
            __ASM_EMIT("jge             3b") \
            __ASM_EMIT("4:") \
            /* end */

        void pcomplex_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "dst", "src")
                : [count] "+r" (count), [off] "=&r" (off)
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
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "t", "b")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [t] "r" (t), [b] "r" (b),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                /* x4 blocks */
                __ASM_EMIT("sub             $4, %[count]") \
                __ASM_EMIT("jb              2f") \
                __ASM_EMIT("xorps           %%xmm6, %%xmm6")                    /* xmm6 = 0 */
                __ASM_EMIT("xorps           %%xmm7, %%xmm7")                    /* xmm7 = 0 */
                __ASM_EMIT("1:") \
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm4")                    /* xmm4 = s0 0 s1 0 */
                __ASM_EMIT("unpckhps        %%xmm7, %%xmm5")                    /* xmm5 = s2 0 s3 0 */
                __ASM_EMIT("addps           %%xmm4, %%xmm0")                    /* xmm0 = r0+s0 i0 r1+s1 i1 */
                __ASM_EMIT("addps           %%xmm5, %%xmm1")                    /* xmm1 = r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x10, %[off]") \
                __ASM_EMIT("sub             $4, %[count]") \
                __ASM_EMIT("jae             1b") \
                /* x1 blocks */ \
                __ASM_EMIT("2:") \
                __ASM_EMIT("add             $3, %[count]") \
                __ASM_EMIT("jl              4f") \
                __ASM_EMIT("3:") \
                __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 */
                __ASM_EMIT("movss           0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 */
                __ASM_EMIT("addss           %%xmm4, %%xmm0")                    /* xmm0 = r0+s0 */
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x04, %[off]") \
                __ASM_EMIT("dec             %[count]") \
                __ASM_EMIT("jge             3b") \
                __ASM_EMIT("4:") \
                /* end */

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                /* x4 blocks */
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("xorps           %%xmm6, %%xmm6")                    /* xmm6 = 0 */
                __ASM_EMIT("xorps           %%xmm7, %%xmm7")                    /* xmm7 = 0 */
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm4")                    /* xmm4 = s0 0 s1 0 */
                __ASM_EMIT("unpckhps        %%xmm7, %%xmm5")                    /* xmm5 = s2 0 s3 0 */
                __ASM_EMIT("subps           %%xmm4, %%xmm0")                    /* xmm0 = r0-s0 i0 r1-s1 i1 */
                __ASM_EMIT("subps           %%xmm5, %%xmm1")                    /* xmm1 = r2-s2 i2 r3-s3 i3 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jae             1b")
                /* x1 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 */
                __ASM_EMIT("movss           0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 */
                __ASM_EMIT("subss           %%xmm4, %%xmm0")                    /* xmm0 = r0-s0 */
                __ASM_EMIT("movss           %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             3b")
                __ASM_EMIT("4:")
                /* end */

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("xorps           %%xmm6, %%xmm6")                    /* xmm6 = 0 */
                /* x4 blocks */
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("xorps           %%xmm7, %%xmm7")                    /* xmm7 = 0 */
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm4")                    /* xmm4 = s0 0 s1 0 */
                __ASM_EMIT("unpckhps        %%xmm7, %%xmm5")                    /* xmm5 = s2 0 s3 0 */
                __ASM_EMIT("subps           %%xmm0, %%xmm4")                    /* xmm4 = s0-r0 -i0 s1-r1 */
                __ASM_EMIT("subps           %%xmm1, %%xmm5")                    /* xmm5 = s2-r2 -i2 s3-r3 */
                __ASM_EMIT("movups          %%xmm4, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm5, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("jae             1b")
                /* x1 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("3:")
                __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 */
                __ASM_EMIT("movlps          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm4")                    /* xmm4 = s0 0 */
                __ASM_EMIT("subps           %%xmm0, %%xmm4")                    /* xmm4 = s0-r0 -i0 */
                __ASM_EMIT("movlps          %%xmm4, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             3b")
                __ASM_EMIT("4:")
                /* end */

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x10(%[src], %[off]), %%xmm6")      /* xmm6 = s4 s5 s6 s7 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("movups          0x20(%[dst], %[off], 2), %%xmm2")   /* xmm2 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x30(%[dst], %[off], 2), %%xmm3")   /* xmm3 = r2 i2 r3 i3 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("movaps          %%xmm6, %%xmm7")                    /* xmm7 = s4 s5 s6 s7 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 s1 s1 */
                __ASM_EMIT("unpckhps        %%xmm5, %%xmm5")                    /* xmm5 = s2 s2 s3 s3 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm6")                    /* xmm6 = s4 s4 s5 s5 */
                __ASM_EMIT("unpckhps        %%xmm7, %%xmm7")                    /* xmm7 = s6 s6 s7 s7 */
                __ASM_EMIT("mulps           %%xmm4, %%xmm0")                    /* xmm0 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("mulps           %%xmm5, %%xmm1")                    /* xmm1 = r2*s2 i2*s2 r3*s3 i3*s3 */
                __ASM_EMIT("mulps           %%xmm6, %%xmm2")                    /* xmm2 = r4*s4 i4*s4 r5*s5 i5*s5 */
                __ASM_EMIT("mulps           %%xmm7, %%xmm3")                    /* xmm3 = r6*s6 i6*s6 r7*s7 i7*s7 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                /* x4 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 s1 s1 */
                __ASM_EMIT("unpckhps        %%xmm5, %%xmm5")                    /* xmm5 = s2 s2 s3 s3 */
                __ASM_EMIT("mulps           %%xmm4, %%xmm0")                    /* xmm0 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("mulps           %%xmm5, %%xmm1")                    /* xmm1 = r2*s2 i2*s2 r3*s3 i3*s3 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                /* x1 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 */
                __ASM_EMIT("movlps          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 */
                __ASM_EMIT("mulps           %%xmm4, %%xmm0")                    /* xmm0 = r0*s0 i0*s0 */
                __ASM_EMIT("movlps          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             5b")
                __ASM_EMIT("6:")
                /* end */

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x10(%[src], %[off]), %%xmm6")      /* xmm6 = s4 s5 s6 s7 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("movups          0x20(%[dst], %[off], 2), %%xmm2")   /* xmm2 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x30(%[dst], %[off], 2), %%xmm3")   /* xmm3 = r2 i2 r3 i3 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("movaps          %%xmm6, %%xmm7")                    /* xmm7 = s4 s5 s6 s7 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 s1 s1 */
                __ASM_EMIT("unpckhps        %%xmm5, %%xmm5")                    /* xmm5 = s2 s2 s3 s3 */
                __ASM_EMIT("unpcklps        %%xmm6, %%xmm6")                    /* xmm6 = s4 s4 s5 s5 */
                __ASM_EMIT("unpckhps        %%xmm7, %%xmm7")                    /* xmm7 = s6 s6 s7 s7 */
                __ASM_EMIT("divps           %%xmm4, %%xmm0")                    /* xmm0 = r0/s0 i0/s0 r1/s1 i1/s1 */
                __ASM_EMIT("divps           %%xmm5, %%xmm1")                    /* xmm1 = r2/s2 i2/s2 r3/s3 i3/s3 */
                __ASM_EMIT("divps           %%xmm6, %%xmm2")                    /* xmm2 = r4/s4 i4/s4 r5/s5 i5/s5 */
                __ASM_EMIT("divps           %%xmm7, %%xmm3")                    /* xmm3 = r6/s6 i6/s6 r7/s7 i7/s7 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("jae             1b")
                /* x4 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("movups          0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("movups          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 r1 i1 */
                __ASM_EMIT("movups          0x10(%[dst], %[off], 2), %%xmm1")   /* xmm1 = r2 i2 r3 i3 */
                __ASM_EMIT("movaps          %%xmm4, %%xmm5")                    /* xmm5 = s0 s1 s2 s3 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 s1 s1 */
                __ASM_EMIT("unpckhps        %%xmm5, %%xmm5")                    /* xmm5 = s2 s2 s3 s3 */
                __ASM_EMIT("divps           %%xmm4, %%xmm0")                    /* xmm0 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("divps           %%xmm5, %%xmm1")                    /* xmm1 = r2*s2 i2*s2 r3*s3 i3*s3 */
                __ASM_EMIT("movups          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("movups          %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")
                /* x1 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss           0x00(%[src], %[off]), %%xmm4")      /* xmm4 = s0 */
                __ASM_EMIT("movlps          0x00(%[dst], %[off], 2), %%xmm0")   /* xmm0 = r0 i0 */
                __ASM_EMIT("unpcklps        %%xmm4, %%xmm4")                    /* xmm4 = s0 s0 */
                __ASM_EMIT("divps           %%xmm4, %%xmm0")                    /* xmm0 = r0*s0 i0*s0 */
                __ASM_EMIT("movlps          %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             5b")
                __ASM_EMIT("6:")
                /* end */

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_rdiv2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);

            ARCH_X86_ASM (
                __ASM_EMIT("xor         %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub         $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[dst], %[off], 2), %%xmm0")       /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[dst], %[off], 2), %%xmm4")       /* xmm4  = r2 i2 r3 i3 */
                __ASM_EMIT("movups      0x20(%[dst], %[off], 2), %%xmm2")       /* xmm2  = r4 i4 r5 i5 */
                __ASM_EMIT("movups      0x30(%[dst], %[off], 2), %%xmm5")       /* xmm5  = r6 i6 r7 i7 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")                        /* xmm3  = r4 i4 r5 i5 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")                 /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")                 /* xmm2  = r4 r5 r6 r7 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")                 /* xmm1  = i0 i1 i2 i3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")                 /* xmm3  = i4 i5 i6 i7 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = i0 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm6")                        /* xmm6  = r1 */
                __ASM_EMIT("movaps      %%xmm3, %%xmm7")                        /* xmm7  = i1 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")                        /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")                        /* xmm5  = i0*i0 */
                __ASM_EMIT("mulps       %%xmm2, %%xmm6")                        /* xmm6  = r1*r1 */
                __ASM_EMIT("mulps       %%xmm3, %%xmm7")                        /* xmm7  = i1*i1 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                        /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("addps       %%xmm7, %%xmm6")                        /* xmm6  = r1*r1 + i1*i1 */
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm5")          /* xmm5  = s0 */
                __ASM_EMIT("movups      0x10(%[src], %[off]), %%xmm7")          /* xmm7  = s1 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")                   /* xmm1  = -i0 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm3")                   /* xmm3  = -i1 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")                        /* xmm5  = s0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("divps       %%xmm6, %%xmm7")                        /* xmm7  = s1 / (r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")                        /* xmm0  = r0' = (s0*r0)/(r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm2")                        /* xmm2  = r1' = (s1*r1)/(r1*r1 + i1*i1) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")                        /* xmm1  = i0' = -(s0*i0)/(r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm7, %%xmm3")                        /* xmm3  = i1' = -(s1*i1)/(r1*r1 + i1*i1) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("movaps      %%xmm2, %%xmm5")                        /* xmm5 = r4' r5' r6' r7' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")                        /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpcklps    %%xmm3, %%xmm2")                        /* xmm2 = r4' i4' r5' i5' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")                        /* xmm4 = r2' i2' r3' i3' */
                __ASM_EMIT("unpckhps    %%xmm3, %%xmm5")                        /* xmm5 = r6' i6' r7' i7' */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off], 2)")       /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst], %[off], 2)")       /* dst[1]  = r2' i2' r3' i3' */
                __ASM_EMIT("movups      %%xmm2, 0x20(%[dst], %[off], 2)")       /* dst[2]  = r4' i4' r5' i5' */
                __ASM_EMIT("movups      %%xmm5, 0x30(%[dst], %[off], 2)")       /* dst[3]  = r6' i6' r7' i7' */
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("jae         1b")

                /* x4 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $4, %[count]")                          /* count += 4 */
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("movups      0x00(%[dst], %[off], 2), %%xmm0")       /* xmm0  = r0 i0 r1 i1 */
                __ASM_EMIT("movups      0x10(%[dst], %[off], 2), %%xmm4")       /* xmm4  = r2 i2 r3 i3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        /* xmm1  = r0 i0 r1 i1 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")                 /* xmm0  = r0 r1 r2 r3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")                 /* xmm1  = i0 i1 i2 i3 */
                /* Perform operations */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = i0 */
                __ASM_EMIT("mulps       %%xmm0, %%xmm4")                        /* xmm4  = r0*r0 */
                __ASM_EMIT("mulps       %%xmm1, %%xmm5")                        /* xmm5  = i0*i0 */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                        /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movups      0x00(%[src], %[off]), %%xmm5")          /* xmm5  = s0 */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")                   /* xmm1  = -i0 */
                __ASM_EMIT("divps       %%xmm4, %%xmm5")                        /* xmm5  = s0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm0")                        /* xmm0  = r0' = (s0*r0) / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulps       %%xmm5, %%xmm1")                        /* xmm1  = i0' = -(s0*i0) / (r0*r0 + i0*i0) */
                /* Do shuffle back */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4 = r0' r1' r2' r3' */
                __ASM_EMIT("unpcklps    %%xmm1, %%xmm0")                        /* xmm0 = r0' i0' r1' i1' */
                __ASM_EMIT("unpckhps    %%xmm1, %%xmm4")                        /* xmm4 = r2' i2' r3' i3' */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off], 2)")       /* dst[0]  = r0' i0' r1' i1' */
                __ASM_EMIT("movups      %%xmm4, 0x10(%[dst], %[off], 2)")       /* dst[1]  = r2' i2' r3' i3' */
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")                          /* count -= 4 */
                /* 1x RCP */
                __ASM_EMIT("4:")
                __ASM_EMIT("add         $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("movss       0x00(%[dst], %[off], 2), %%xmm0")       /* xmm0  = r0 */
                __ASM_EMIT("movss       0x04(%[dst], %[off], 2), %%xmm1")       /* xmm1  = i0 */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = r0 */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = i0 */
                __ASM_EMIT("mulss       %%xmm0, %%xmm4")                        /* xmm4  = r0*r0 */
                __ASM_EMIT("mulss       %%xmm1, %%xmm5")                        /* xmm5  = i0*i0 */
                __ASM_EMIT("addss       %%xmm5, %%xmm4")                        /* xmm4  = r0*r0 + i0*i0 */
                __ASM_EMIT("movss       0x00(%[src], %[off]), %%xmm5")          /* xmm5  = s0  */
                __ASM_EMIT("xorps       0x10(%[CC]), %%xmm1")                   /* xmm1  = -i0 */
                __ASM_EMIT("divss       %%xmm4, %%xmm5")                        /* xmm5  = s0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm0")                        /* xmm0  = r0' = r0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("mulss       %%xmm5, %%xmm1")                        /* xmm1  = i0' = -i0 / (r0*r0 + i0*i0) */
                __ASM_EMIT("movss       %%xmm0, 0x00(%[dst], %[off], 2)")       /* dst[0]  = r0' */
                __ASM_EMIT("movss       %%xmm1, 0x04(%[dst], %[off], 2)")       /* dst[1]  = i0' */
                __ASM_EMIT("add         $0x04, %[off]")
                __ASM_EMIT("dec         %[count]")
                __ASM_EMIT("jge         5b")

                __ASM_EMIT("6:")
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const float pcomplex_corr_const[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-36f),
            };
        );

        void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);

            /*
             * src1[i] = a + j*b, src2[i] = c + j*d
             * den  = (a*a + b*b)*(c*c + d*d)
             * nom  = a*c + b*d
             * corr[i] = (den > threshold) ? nom / sqrt(den) : 0.0
             */
            ARCH_X86_ASM (
                __ASM_EMIT("xor         %[off], %[off]")
                /* x4 blocks */
                __ASM_EMIT("sub         $4, %[count]")                          /* count -= 4 */
                __ASM_EMIT("movaps      %[CC], %%xmm7")                         /* xmm7  = threshold */
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("movups      0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("movups      0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("movups      0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("movups      0x10(%[src2], %[off], 2), %%xmm5")      /* xmm5  = c2 d2 c3 d3 */
                /* Do shuffle */
                __ASM_EMIT("movaps      %%xmm0, %%xmm1")                        /* xmm1  = a0 b0 a1 b1 */
                __ASM_EMIT("movaps      %%xmm2, %%xmm3")                        /* xmm3  = c0 d0 c1 d1 */
                __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm0")                 /* xmm0  = a = a0 a1 a2 a3 */
                __ASM_EMIT("shufps      $0x88, %%xmm5, %%xmm2")                 /* xmm2  = c = c0 c1 c2 c3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm4, %%xmm1")                 /* xmm1  = b = b0 b1 b2 b3 */
                __ASM_EMIT("shufps      $0xdd, %%xmm5, %%xmm3")                 /* xmm3  = d = d0 d1 d2 d3 */
                /* xmm0 = a, xmm1 = b, xmm2 = c, xmm3 = d */
                __ASM_EMIT("movaps      %%xmm0, %%xmm4")                        /* xmm4  = a */
                __ASM_EMIT("movaps      %%xmm1, %%xmm5")                        /* xmm5  = b */
                __ASM_EMIT("mulps       %%xmm2, %%xmm4")                        /* xmm4  = a*c */
                __ASM_EMIT("mulps       %%xmm3, %%xmm5")                        /* xmm5  = b*d */
                __ASM_EMIT("mulps       %%xmm0, %%xmm0")                        /* xmm0  = a*a */
                __ASM_EMIT("mulps       %%xmm1, %%xmm1")                        /* xmm1  = b*b */
                __ASM_EMIT("mulps       %%xmm2, %%xmm2")                        /* xmm2  = c*c */
                __ASM_EMIT("mulps       %%xmm3, %%xmm3")                        /* xmm3  = d*d */
                __ASM_EMIT("addps       %%xmm1, %%xmm0")                        /* xmm0  = a*a + b*b */
                __ASM_EMIT("addps       %%xmm3, %%xmm2")                        /* xmm2  = c*c + d*d */
                __ASM_EMIT("mulps       %%xmm2, %%xmm0")                        /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("addps       %%xmm5, %%xmm4")                        /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("sqrtps      %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den) */
                __ASM_EMIT("divps       %%xmm1, %%xmm4")                        /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("cmpps       $5, %%xmm7, %%xmm0")                    /* xmm0  = [den >= threshold] */
                __ASM_EMIT("andps       %%xmm4, %%xmm0")                        /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 */
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")                          /* count -= 4 */
                __ASM_EMIT("jae         1b")
                /* x1 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add         $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("3:")
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
                __ASM_EMIT("jge         3b")
                /* End */
                __ASM_EMIT("4:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst_corr), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "m" (pcomplex_corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace sse */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_SSE_PCOMPLEX_H_ */
