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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_AVX_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        IF_ARCH_X86(
            static uint32_t pcomplex_div_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x3f800000),   // 1.0
                LSP_DSP_VEC8(0x80000000)    // signbit
            };
        )

        #define FMA_OFF(a, b)       a
        #define FMA_ON(a, b)        b

        #define PCOMPLEX_MULX_CORE(DST, SRC1, SRC2, SEL) \
            /* Init */ \
            __ASM_EMIT("xor         %[off], %[off]") \
            /* x8 block */ \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jb          2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%ymm0")     /* ymm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovsldup   0x20(%[" SRC1 "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
            __ASM_EMIT("vmovups     0x20(%[" SRC2 "], %[off]), %%ymm7") \
            __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* ymm4 = bi0 br0 */ \
            __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%ymm2")     /* ymm2 = ai0 ai0 */ \
            __ASM_EMIT("vmovshdup   0x20(%[" SRC1 "], %[off]), %%ymm3") \
            __ASM_EMIT("vshufps     $0xb1, %%ymm7, %%ymm7, %%ymm5") \
            __ASM_EMIT(SEL("vmulps      %%ymm6, %%ymm0, %%ymm0", ""))       /* ymm0 = ar0*br0 ar0*bi0 */ \
            __ASM_EMIT(SEL("vmulps      %%ymm7, %%ymm1, %%ymm1", "")) \
            __ASM_EMIT("vmulps      %%ymm4, %%ymm2, %%ymm2")                /* ymm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vmulps      %%ymm5, %%ymm3, %%ymm3") \
            __ASM_EMIT(SEL("vaddsubps   %%ymm2, %%ymm0, %%ymm0", "vfmaddsub132ps %%ymm6, %%ymm2, %%ymm0"))   /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT(SEL("vaddsubps   %%ymm3, %%ymm1, %%ymm1", "vfmaddsub132ps %%ymm7, %%ymm3, %%ymm1")) \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* x4 block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
            __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%ymm0")     /* ymm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%ymm2")     /* ymm2 = ai0 ai0 */ \
            __ASM_EMIT("vshufps     $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT(SEL("vmulps      %%ymm6, %%ymm0, %%ymm0", ""))       /* ymm0 = ar0*br0 ar0*bi0 */ \
            __ASM_EMIT("vmulps      %%ymm4, %%ymm2, %%ymm2")                /* ymm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT(SEL("vaddsubps   %%ymm2, %%ymm0, %%ymm0", "vfmaddsub132ps %%ymm6, %%ymm2, %%ymm0"))   /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("4:") \
            /* x2 block */ \
            __ASM_EMIT("add         $2, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x00(%[" SRC2 "], %[off]), %%xmm6")     /* xmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovsldup   0x00(%[" SRC1 "], %[off]), %%xmm0")     /* xmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovshdup   0x00(%[" SRC1 "], %[off]), %%xmm2")     /* xmm2 = ai0 ai0 */ \
            __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT(SEL("vmulps      %%xmm6, %%xmm0, %%xmm0", ""))       /* xmm0 = ar0*br0 ar0*bi0 */ \
            __ASM_EMIT("vmulps      %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT(SEL("vaddsubps   %%xmm2, %%xmm0, %%xmm0", "vfmaddsub132ps %%xmm6, %%xmm2, %%xmm0"))   /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $2, %[count]") \
            __ASM_EMIT("6:") \
            /* x1 block */ \
            __ASM_EMIT("add         $1, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovlps     0x00(%[" SRC1 "], %[off]), %%xmm4, %%xmm4")     /* xmm4 = ar0 ai0 */ \
            __ASM_EMIT("vmovlps     0x00(%[" SRC2 "], %[off]), %%xmm6, %%xmm6")     /* xmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovsldup   %%xmm4, %%xmm0")                        /* xmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovshdup   %%xmm4, %%xmm2")                        /* xmm2 = ai0 ai0 */ \
            __ASM_EMIT("vshufps     $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT(SEL("vmulps      %%xmm6, %%xmm0, %%xmm0", ""))       /* xmm0 = ar0*br0 ar0*bi0 */ \
            __ASM_EMIT("vmulps      %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT(SEL("vaddsubps   %%xmm2, %%xmm0, %%xmm0", "vfmaddsub132ps %%xmm6, %%xmm2, %%xmm0"))   /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("8:") \
            /* end */

        /*
         * Register allocation:
         * ymm4     = a1
         * ymm5     = a2
         * ymm6     = b1
         * ymm7     = b2
         */
        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_MULX_CORE("dst", "src1", "src2", FMA_OFF)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_MULX_CORE("dst", "dst", "src", FMA_OFF)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_mul3_fma3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_MULX_CORE("dst", "src1", "src2", FMA_ON)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src1] "r" (src1), [src2] "r" (src2)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_mul2_fma3(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_MULX_CORE("dst", "dst", "src", FMA_ON)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef PCOMPLEX_MULX_CORE

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            // Strided-load technique is used to improve performance
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                // 16x blocks
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vinsertf128     $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
                __ASM_EMIT("vinsertf128     $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vmovups         0x040(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("vmovups         0x050(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("vinsertf128     $1, 0x060(%[src], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vinsertf128     $1, 0x070(%[src], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vhaddps         %%ymm2, %%ymm0, %%ymm0")
                __ASM_EMIT("vhaddps         %%ymm3, %%ymm1, %%ymm1")
                __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
                __ASM_EMIT("vsqrtps         %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups         %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                // 8x block
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vinsertf128     $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
                __ASM_EMIT("vinsertf128     $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vhaddps         %%ymm2, %%ymm0, %%ymm0")
                __ASM_EMIT("vsqrtps         %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("4:")
                // 4x block
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups         0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("6:")
                // 2x block
                __ASM_EMIT("add             $2, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
                __ASM_EMIT("vmovlps         %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub             $2, %[count]")
                __ASM_EMIT("add             $0x08, %[off]")
                __ASM_EMIT("8:")
                // 1x block
                __ASM_EMIT("add             $1, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("vmovlps         0x000(%[src], %[off], 2), %%xmm0, %%xmm0")
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps         %%xmm0, %%xmm0")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("10:")
                // End

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3", "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #define PCOMPLEX_DIV_CORE(DST, T, B, SEL) \
            /* Init */ \
            __ASM_EMIT("xor                     %[off], %[off]") \
            /* x8 block */ \
            __ASM_EMIT("sub                     $8, %[count]") \
            __ASM_EMIT("jb                      2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups                 0x00(%[" T "], %[off]), %%ymm0")        /* ymm0 = tr ti */ \
            __ASM_EMIT("vmovups                 0x20(%[" T "], %[off]), %%ymm4") \
            __ASM_EMIT("vmovups                 0x00(%[" B "], %[off]), %%ymm1")        /* ymm1 = br bi */ \
            __ASM_EMIT("vmovups                 0x20(%[" B "], %[off]), %%ymm5") \
            __ASM_EMIT("vmovshdup               %%ymm0, %%ymm2")                        /* ymm2 = ti ti */ \
            __ASM_EMIT("vmovshdup               %%ymm4, %%ymm6") \
            __ASM_EMIT("vmovsldup               %%ymm0, %%ymm0")                        /* ymm0 = tr tr */ \
            __ASM_EMIT("vmovsldup               %%ymm4, %%ymm4") \
            __ASM_EMIT("vmulps                  %%ymm1, %%ymm1, %%ymm3")                /* ymm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%ymm5, %%ymm5, %%ymm7") \
            __ASM_EMIT("vmulps                  %%ymm1, %%ymm0, %%ymm0")                /* ymm0 = tr*br tr*bi */ \
            __ASM_EMIT("vmulps                  %%ymm5, %%ymm4, %%ymm4") \
            __ASM_EMIT("vhaddps                 %%ymm7, %%ymm3, %%ymm3")                /* ymm3 = R0 R1 R4 R5 R2 R3 R6 R7 */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%ymm0, %%ymm0", ""))     /* ymm0 = -tr*br -tr*bi */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%ymm4, %%ymm4", ""))  \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%ymm7")                  /* ymm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%ymm3, %%ymm7, %%ymm3")                /* ymm3 = 1/R0 1/R1 1/R4 1/R5 1/R2 1/R3 1/R6 1/R7 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%ymm1, %%ymm1, %%ymm1")         /* ymm1 = bi br */ \
            __ASM_EMIT("vshufps                 $0xb1, %%ymm5, %%ymm5, %%ymm5") \
            __ASM_EMIT("vunpckhps               %%ymm3, %%ymm3, %%ymm7")                /* ymm7 = 1/R4 1/R4 1/R5 1/R5 1/R6 1/R6 1/R7 1/R7 */ \
            __ASM_EMIT("vunpcklps               %%ymm3, %%ymm3, %%ymm3")                /* ymm3 = 1/R0 1/R0 1/R1 1/R1 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT(SEL("vmulps              %%ymm1, %%ymm2, %%ymm2", ""))           /* ymm2 = ti*bi ti*br */ \
            __ASM_EMIT(SEL("vmulps              %%ymm5, %%ymm6, %%ymm6", "")) \
            __ASM_EMIT(SEL("vaddsubps           %%ymm0, %%ymm2, %%ymm0", ""))           /* ymm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("vaddsubps           %%ymm4, %%ymm6, %%ymm4", "")) \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%ymm1, %%ymm2, %%ymm0"))               /* ymm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%ymm5, %%ymm6, %%ymm4")) \
            __ASM_EMIT("vmulps                  %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = r' i' */ \
            __ASM_EMIT("vmulps                  %%ymm7, %%ymm4, %%ymm4") \
            __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups                 %%ymm4, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x40, %[off]") \
            __ASM_EMIT("sub                     $8, %[count]") \
            __ASM_EMIT("jae                     1b") \
            __ASM_EMIT("2:") \
            /* x4 block */ \
            __ASM_EMIT("add                     $4, %[count]") \
            __ASM_EMIT("jl                      4f") \
            __ASM_EMIT("vmovups                 0x00(%[" T "], %[off]), %%xmm0")        /* xmm0 = tr ti */ \
            __ASM_EMIT("vmovups                 0x10(%[" T "], %[off]), %%xmm4") \
            __ASM_EMIT("vmovups                 0x00(%[" B "], %[off]), %%xmm1")        /* xmm1 = br bi */ \
            __ASM_EMIT("vmovups                 0x10(%[" B "], %[off]), %%xmm5") \
            __ASM_EMIT("vmovshdup               %%xmm0, %%xmm2")                        /* xmm2 = ti ti */ \
            __ASM_EMIT("vmovshdup               %%xmm4, %%xmm6") \
            __ASM_EMIT("vmovsldup               %%xmm0, %%xmm0")                        /* xmm0 = tr tr */ \
            __ASM_EMIT("vmovsldup               %%xmm4, %%xmm4") \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm1, %%xmm3")                /* xmm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%xmm5, %%xmm5, %%xmm7") \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vmulps                  %%xmm5, %%xmm4, %%xmm4") \
            __ASM_EMIT("vhaddps                 %%xmm7, %%xmm3, %%xmm3")                /* xmm3 = R0 R1 R2 R3 */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%xmm0, %%xmm0", ""))     /* xmm0 = -tr*br -tr*bi */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%xmm4, %%xmm4", ""))  \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm5, %%xmm5, %%xmm5") \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT(SEL("vmulps              %%xmm1, %%xmm2, %%xmm2", ""))           /* xmm2 = ti*bi ti*br */ \
            __ASM_EMIT(SEL("vmulps              %%xmm5, %%xmm6, %%xmm6", "")) \
            __ASM_EMIT(SEL("vaddsubps           %%xmm0, %%xmm2, %%xmm0", ""))           /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("vaddsubps           %%xmm4, %%xmm6, %%xmm4", "")) \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%xmm1, %%xmm2, %%xmm0"))               /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%xmm5, %%xmm6, %%xmm4")) \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmulps                  %%xmm7, %%xmm4, %%xmm4") \
            __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups                 %%xmm4, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x20, %[off]") \
            __ASM_EMIT("sub                     $4, %[count]") \
            __ASM_EMIT("4:") \
            /* x2 block */ \
            __ASM_EMIT("add                     $2, %[count]") \
            __ASM_EMIT("jl                      6f") \
            __ASM_EMIT("vmovups                 0x00(%[" T "], %[off]), %%xmm0")        /* xmm0 = tr ti */ \
            __ASM_EMIT("vmovups                 0x00(%[" B "], %[off]), %%xmm1")        /* xmm1 = br bi */ \
            __ASM_EMIT("vmovshdup               %%xmm0, %%xmm2")                        /* xmm2 = ti ti */ \
            __ASM_EMIT("vmovsldup               %%xmm0, %%xmm0")                        /* xmm0 = tr tr */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm1, %%xmm3")                /* xmm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vhaddps                 %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = R0 R1 */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%xmm0, %%xmm0", ""))     /* xmm0 = -tr*br -tr*bi */ \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT(SEL("vmulps              %%xmm1, %%xmm2, %%xmm2", ""))           /* xmm2 = ti*bi ti*br */ \
            __ASM_EMIT(SEL("vaddsubps           %%xmm0, %%xmm2, %%xmm0", ""))           /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%xmm1, %%xmm2, %%xmm0"))               /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x10, %[off]") \
            __ASM_EMIT("sub                     $2, %[count]") \
            __ASM_EMIT("6:") \
            /* x1 block */ \
            __ASM_EMIT("add                     $1, %[count]") \
            __ASM_EMIT("jl                      8f") \
            __ASM_EMIT("vmovlps                 0x00(%[" T "], %[off]), %%xmm0, %%xmm0")/* xmm0 = tr ti */ \
            __ASM_EMIT("vmovlps                 0x00(%[" B "], %[off]), %%xmm1, %%xmm1")/* xmm1 = br bi */ \
            __ASM_EMIT("vmovshdup               %%xmm0, %%xmm2")                        /* xmm2 = ti ti */ \
            __ASM_EMIT("vmovsldup               %%xmm0, %%xmm0")                        /* xmm0 = tr tr */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm1, %%xmm3")                /* xmm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vhaddps                 %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = R0 R1 */ \
            __ASM_EMIT(SEL("vxorps              0x20 + %[CC], %%xmm0, %%xmm0", ""))     /* xmm0 = -tr*br -tr*bi */ \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT(SEL("vmulps              %%xmm1, %%xmm2, %%xmm2", ""))           /* xmm2 = ti*bi ti*br */ \
            __ASM_EMIT(SEL("vaddsubps           %%xmm0, %%xmm2, %%xmm0", ""))           /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT(SEL("", "vfmsubadd231ps  %%xmm1, %%xmm2, %%xmm0"))               /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("8:") \
            /* end */

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "t", "b", FMA_OFF)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [t] "r" (t), [b] "r" (b),
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
                PCOMPLEX_DIV_CORE("dst", "dst", "src", FMA_OFF)
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
                PCOMPLEX_DIV_CORE("dst", "src", "dst", FMA_OFF)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_div3_fma3(float *dst, const float *t, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "t", "b", FMA_ON)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [t] "r" (t), [b] "r" (b),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_div2_fma3(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "dst", "src", FMA_ON)
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_rdiv2_fma3(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "src", "dst", FMA_ON)
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

        #define PCOMPLEX_RCP_CORE(DST, SRC) \
            /* Init */ \
            __ASM_EMIT("xor         %[off], %[off]") \
            /* x16 block */ \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jb          2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
            __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups     0x40(%[" SRC "], %[off]), %%ymm2") \
            __ASM_EMIT("vmovups     0x60(%[" SRC "], %[off]), %%ymm3") \
            __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
            __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm5") \
            __ASM_EMIT("vmulps      %%ymm2, %%ymm2, %%ymm6") \
            __ASM_EMIT("vmulps      %%ymm3, %%ymm3, %%ymm7") \
            __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
            __ASM_EMIT("vhaddps     %%ymm7, %%ymm6, %%ymm6") \
            __ASM_EMIT("vmovaps     0x00(%[CC]), %%ymm5")                   /* ymm5 = 1 */ \
            __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
            __ASM_EMIT("vdivps      %%ymm6, %%ymm5, %%ymm6") \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%ymm4, %%ymm5")           /* ymm5 = -1/R */ \
            __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm7")                /* ymm7 = 1/R -1/R */ \
            __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5") \
            __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%ymm6, %%ymm5")           /* ymm5 = -1/R2 */ \
            __ASM_EMIT("vunpckhps   %%ymm5, %%ymm6, %%ymm7")                /* ymm7 = 1/R2 -1/R2 */ \
            __ASM_EMIT("vunpcklps   %%ymm5, %%ymm6, %%ymm5") \
            __ASM_EMIT("vmulps      %%ymm5, %%ymm2, %%ymm2")                /* ymm0 = r1/R2 -i1/R2 */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm3, %%ymm3") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm2, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm3, 0x60(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x80, %[off]") \
            __ASM_EMIT("sub         $16, %[count]") \
            __ASM_EMIT("jae         1b") \
            __ASM_EMIT("2:") \
            /* x8 block */ \
            __ASM_EMIT("add         $8, %[count]") \
            __ASM_EMIT("jl          4f") \
            __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
            __ASM_EMIT("vmovups     0x20(%[" SRC "], %[off]), %%ymm1") \
            __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
            __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm5") \
            __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
            __ASM_EMIT("vmovaps     0x00(%[CC]), %%ymm5")                   /* ymm5 = 1 */ \
            __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%ymm4, %%ymm5")           /* ymm5 = -1/R */ \
            __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm7")                /* ymm7 = 1/R -1/R */ \
            __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5") \
            __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
            __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1") \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups     %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x40, %[off]") \
            __ASM_EMIT("sub         $8, %[count]") \
            __ASM_EMIT("4:") \
            /* x4 block */ \
            __ASM_EMIT("add         $4, %[count]") \
            __ASM_EMIT("jl          6f") \
            __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%ymm0")      /* ymm0 = r0 i0 */ \
            __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */ \
            __ASM_EMIT("vhaddps     %%ymm4, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */ \
            __ASM_EMIT("vmovaps     0x00(%[CC]), %%ymm5")                   /* ymm5 = 1 */ \
            __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */ \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%ymm4, %%ymm5")           /* ymm5 = -1/R */ \
            __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5")                /* ymm7 = 1/R -1/R */ \
            __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */ \
            __ASM_EMIT("vmovups     %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x20, %[off]") \
            __ASM_EMIT("sub         $4, %[count]") \
            __ASM_EMIT("6:") \
            /* x2 block */ \
            __ASM_EMIT("add         $2, %[count]") \
            __ASM_EMIT("jl          8f") \
            __ASM_EMIT("vmovups     0x00(%[" SRC "], %[off]), %%xmm0")      /* xmm0 = r0 i0 */ \
            __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */ \
            __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */ \
            __ASM_EMIT("vmovaps     0x00(%[CC]), %%xmm5")                   /* xmm5 = 1 */ \
            __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = 1/R */ \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%xmm4, %%xmm5")           /* xmm5 = -1/R */ \
            __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = 1/R -1/R */ \
            __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = r0/R -i0/R */ \
            __ASM_EMIT("vmovups     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add         $0x10, %[off]") \
            __ASM_EMIT("sub         $2, %[count]") \
            __ASM_EMIT("8:") \
            /* x1 block */ \
            __ASM_EMIT("add         $1, %[count]") \
            __ASM_EMIT("jl          10f") \
            __ASM_EMIT("vmovlps     0x00(%[" SRC "], %[off]), %%xmm0, %%xmm0")  /* xmm0 = r0 i0 */ \
            __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */ \
            __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */ \
            __ASM_EMIT("vmovaps     0x00(%[CC]), %%xmm5")                   /* xmm5 = 1 */ \
            __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = 1/R */ \
            __ASM_EMIT("vxorps      0x20(%[CC]), %%xmm4, %%xmm5")           /* xmm5 = -1/R */ \
            __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = 1/R -1/R */ \
            __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = r0/R -i0/R */ \
            __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("10:") \
            /* end */

        void pcomplex_rcp1(float *dst, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_RCP_CORE("dst", "dst")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst),
                  [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_rcp2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_RCP_CORE("dst", "src")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef PCOMPLEX_RCP_CORE

        #undef FMA_OFF
        #undef FMA_ON

        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x16 blocks */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%ymm0")              /* ymm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm0, %%ymm1")                    /* ymm1 = s2 0 s3 0 s6 0 s7 0 */
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm2, %%ymm3")
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = s0 0 s1 0 s4 0 s5 0 */
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vextractf128            $1, %%ymm0, %%xmm4")                        /* xmm4 = s4 0 s5 0 */
                __ASM_EMIT("vextractf128            $1, %%ymm2, %%xmm5")
                __ASM_EMIT("vinsertf128             $1, %%xmm1, %%ymm0, %%ymm0")                /* ymm0 = s0 0 s1 0 s2 0 s3 0 */
                __ASM_EMIT("vinsertf128             $1, %%xmm3, %%ymm2, %%ymm2")
                __ASM_EMIT("vinsertf128             $0, %%xmm4, %%ymm1, %%ymm1")                /* ymm1 = s4 0 s5 0 s6 0 s7 0 */
                __ASM_EMIT("vinsertf128             $0, %%xmm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vaddps                  0x00(%[dst], %[off], 2), %%ymm0, %%ymm0")   /* ymm0 = r0+s0 i0 r1+s1 i1 r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("vaddps                  0x20(%[dst], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vaddps                  0x40(%[dst], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vaddps                  0x60(%[dst], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vaddps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0+s0 i0 r1+s1 i1 r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("vaddps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vaddps                  0x20(%[dst], %[off], 2), %%xmm2, %%xmm2")
                __ASM_EMIT("vaddps                  0x30(%[dst], %[off], 2), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                /* x4 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vaddps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0+s0 i0 r1+s1 i1 r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("vaddps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 */
                __ASM_EMIT("vaddss                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0 + s0 */
                __ASM_EMIT("vmovss                  %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x16 blocks */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%ymm0")              /* ymm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm0, %%ymm1")                    /* ymm1 = s2 0 s3 0 s6 0 s7 0 */
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm2, %%ymm3")
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = s0 0 s1 0 s4 0 s5 0 */
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vextractf128            $1, %%ymm0, %%xmm4")                        /* xmm4 = s4 0 s5 0 */
                __ASM_EMIT("vextractf128            $1, %%ymm2, %%xmm5")
                __ASM_EMIT("vinsertf128             $1, %%xmm1, %%ymm0, %%ymm0")                /* ymm0 = s0 0 s1 0 s2 0 s3 0 */
                __ASM_EMIT("vinsertf128             $1, %%xmm3, %%ymm2, %%ymm2")
                __ASM_EMIT("vinsertf128             $0, %%xmm4, %%ymm1, %%ymm1")                /* ymm1 = s4 0 s5 0 s6 0 s7 0 */
                __ASM_EMIT("vinsertf128             $0, %%xmm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%ymm0, %%ymm0")   /* ymm0 = r0-s0 i0 r1-s1 i1 r2-s2 i2 r3-s3 i3 */
                __ASM_EMIT("vsubps                  0x20(%[dst], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps                  0x40(%[dst], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vsubps                  0x60(%[dst], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0-s0 i0 r1-s1 i1 r2-s2 i2 r3-s3 i3 */
                __ASM_EMIT("vsubps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vsubps                  0x20(%[dst], %[off], 2), %%xmm2, %%xmm2")
                __ASM_EMIT("vsubps                  0x30(%[dst], %[off], 2), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                /* x4 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0-s0 i0 r1-s1 i1 r2-s2 i2 r3-s3 i3 */
                __ASM_EMIT("vsubps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")
                __ASM_EMIT("vsubps                  %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = s0-r0 -i0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x16 blocks */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%ymm0")              /* ymm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm0, %%ymm1")                    /* ymm1 = s2 0 s3 0 s6 0 s7 0 */
                __ASM_EMIT("vunpckhps               %%ymm6, %%ymm2, %%ymm3")
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm0, %%ymm0")                    /* ymm0 = s0 0 s1 0 s4 0 s5 0 */
                __ASM_EMIT("vunpcklps               %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("vextractf128            $1, %%ymm0, %%xmm4")                        /* xmm4 = s4 0 s5 0 */
                __ASM_EMIT("vextractf128            $1, %%ymm2, %%xmm5")
                __ASM_EMIT("vinsertf128             $1, %%xmm1, %%ymm0, %%ymm0")                /* ymm0 = s0 0 s1 0 s2 0 s3 0 */
                __ASM_EMIT("vinsertf128             $1, %%xmm3, %%ymm2, %%ymm2")
                __ASM_EMIT("vinsertf128             $0, %%xmm4, %%ymm1, %%ymm1")                /* ymm1 = s4 0 s5 0 s6 0 s7 0 */
                __ASM_EMIT("vinsertf128             $0, %%xmm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%ymm4")           /* ymm4 = r0 i0 r1 i1 r2 i2 r3 i3 */
                __ASM_EMIT("vmovups                 0x20(%[dst], %[off], 2), %%ymm5")
                __ASM_EMIT("vsubps                  %%ymm0, %%ymm4, %%ymm0")                    /* ymm0 = s0-r0 -i0 s1-r1 -i1 s2-r2 -i2 s3-r3 -i3 */
                __ASM_EMIT("vsubps                  %%ymm1, %%ymm5, %%ymm1")
                __ASM_EMIT("vmovups                 0x40(%[dst], %[off], 2), %%ymm4")
                __ASM_EMIT("vmovups                 0x60(%[dst], %[off], 2), %%ymm5")
                __ASM_EMIT("vsubps                  %%ymm2, %%ymm4, %%ymm2")
                __ASM_EMIT("vsubps                  %%ymm3, %%ymm5, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%xmm4")           /* xmm4 = r0 i0 r1 i1 r2 i2 r3 i3 */
                __ASM_EMIT("vmovups                 0x10(%[dst], %[off], 2), %%xmm5")
                __ASM_EMIT("vsubps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = s0-r0 -i0 s1-r1 -i1 s2-r2 -i2 s3-r3 -i3 */
                __ASM_EMIT("vsubps                  %%xmm1, %%xmm5, %%xmm1")
                __ASM_EMIT("vmovups                 0x20(%[dst], %[off], 2), %%xmm4")
                __ASM_EMIT("vmovups                 0x30(%[dst], %[off], 2), %%xmm5")
                __ASM_EMIT("vsubps                  %%xmm2, %%xmm4, %%xmm2")
                __ASM_EMIT("vsubps                  %%xmm3, %%xmm5, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                /* x4 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%xmm4")           /* xmm4 = r0 i0 r1 i1 r2 i2 r3 i3 */
                __ASM_EMIT("vmovups                 0x10(%[dst], %[off], 2), %%xmm5")
                __ASM_EMIT("vsubps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = s0-r0 -i0 s1-r1 -i1 s2-r2 -i2 s3-r3 -i3 */
                __ASM_EMIT("vsubps                  %%xmm1, %%xmm5, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")   /* xmm4 = r0 i0 */
                __ASM_EMIT("vsubps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = s0-r0 -i0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x16 blocks */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%ymm0")              /* ymm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vunpckhps               %%ymm0, %%ymm0, %%ymm1")                    /* ymm1 = s2 s2 s3 s3 s6 s6 s7 s7 */
                __ASM_EMIT("vunpckhps               %%ymm2, %%ymm2, %%ymm3")
                __ASM_EMIT("vunpcklps               %%ymm0, %%ymm0, %%ymm0")                    /* ymm0 = s0 s0 s1 s1 s4 s4 s5 s5 */
                __ASM_EMIT("vunpcklps               %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vextractf128            $1, %%ymm0, %%xmm4")                        /* xmm4 = s4 s4 s5 s5 */
                __ASM_EMIT("vextractf128            $1, %%ymm2, %%xmm5")
                __ASM_EMIT("vinsertf128             $1, %%xmm1, %%ymm0, %%ymm0")                /* ymm0 = s0 s0 s1 s1 s2 s2 s3 s3 */
                __ASM_EMIT("vinsertf128             $1, %%xmm3, %%ymm2, %%ymm2")
                __ASM_EMIT("vinsertf128             $0, %%xmm4, %%ymm1, %%ymm1")                /* ymm1 = s4 s4 s5 s5 s6 s6 s7 s7 */
                __ASM_EMIT("vinsertf128             $0, %%xmm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%ymm0, %%ymm0")   /* ymm0 = r0*s0 i0*s0 r1*s1 i1*s1 r2*s2 i2*s2 r3*s3 i3*s3 */
                __ASM_EMIT("vmulps                  0x20(%[dst], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps                  0x40(%[dst], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps                  0x60(%[dst], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm0, %%xmm0, %%xmm1")                    /* xmm1 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpckhps               %%xmm2, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 s1 s1 */
                __ASM_EMIT("vunpcklps               %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("vmulps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmulps                  0x20(%[dst], %[off], 2), %%xmm2, %%xmm2")
                __ASM_EMIT("vmulps                  0x30(%[dst], %[off], 2), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                /* x4 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm0, %%xmm0, %%xmm1")                    /* xmm1 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 s1 s1 */
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm4 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("vmulps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 */
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0*s0 i0*s0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

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
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x16 blocks */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%ymm0")              /* ymm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off]), %%ymm2")
                __ASM_EMIT("vunpckhps               %%ymm0, %%ymm0, %%ymm1")                    /* ymm1 = s2 s2 s3 s3 s6 s6 s7 s7 */
                __ASM_EMIT("vunpckhps               %%ymm2, %%ymm2, %%ymm3")
                __ASM_EMIT("vunpcklps               %%ymm0, %%ymm0, %%ymm0")                    /* ymm0 = s0 s0 s1 s1 s4 s4 s5 s5 */
                __ASM_EMIT("vunpcklps               %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vextractf128            $1, %%ymm0, %%xmm4")                        /* xmm4 = s4 s4 s5 s5 */
                __ASM_EMIT("vextractf128            $1, %%ymm2, %%xmm5")
                __ASM_EMIT("vinsertf128             $1, %%xmm1, %%ymm0, %%ymm0")                /* ymm0 = s0 s0 s1 s1 s2 s2 s3 s3 */
                __ASM_EMIT("vinsertf128             $1, %%xmm3, %%ymm2, %%ymm2")
                __ASM_EMIT("vinsertf128             $0, %%xmm4, %%ymm1, %%ymm1")                /* ymm1 = s4 s4 s5 s5 s6 s6 s7 s7 */
                __ASM_EMIT("vinsertf128             $0, %%xmm5, %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%ymm4")
                __ASM_EMIT("vmovups                 0x20(%[dst], %[off], 2), %%ymm5")
                __ASM_EMIT("vmovups                 0x40(%[dst], %[off], 2), %%ymm6")
                __ASM_EMIT("vmovups                 0x60(%[dst], %[off], 2), %%ymm7")
                __ASM_EMIT("vdivps                  %%ymm0, %%ymm4, %%ymm0")                    /* ymm0 = r0/s0 i0/s0 r1/s1 i1/s1 r2/s2 i2/s2 r3/s3 i3/s3 */
                __ASM_EMIT("vdivps                  %%ymm1, %%ymm5, %%ymm1")
                __ASM_EMIT("vdivps                  %%ymm2, %%ymm6, %%ymm2")
                __ASM_EMIT("vdivps                  %%ymm3, %%ymm7, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                /* x8 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm0, %%xmm0, %%xmm1")                    /* xmm1 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpckhps               %%xmm2, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 s1 s1 */
                __ASM_EMIT("vunpcklps               %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%xmm4")
                __ASM_EMIT("vmovups                 0x10(%[dst], %[off], 2), %%xmm5")
                __ASM_EMIT("vmovups                 0x20(%[dst], %[off], 2), %%xmm6")
                __ASM_EMIT("vmovups                 0x30(%[dst], %[off], 2), %%xmm7")
                __ASM_EMIT("vdivps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = r0/s0 i0/s0 r1/s1 i1/s1 */
                __ASM_EMIT("vdivps                  %%xmm1, %%xmm5, %%xmm1")
                __ASM_EMIT("vdivps                  %%xmm2, %%xmm6, %%xmm2")
                __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                /* x4 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm0, %%xmm0, %%xmm1")                    /* xmm1 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 s1 s1 */
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%xmm4")
                __ASM_EMIT("vmovups                 0x10(%[dst], %[off], 2), %%xmm5")
                __ASM_EMIT("vdivps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = r0/s0 i0/s0 r1/s1 i1/s1 */
                __ASM_EMIT("vdivps                  %%xmm1, %%xmm5, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")   /* xmm4 = r0 i0 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 */
                __ASM_EMIT("vdivps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = r0/s0 i0/s0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

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
            ARCH_X86_ASM
            (
                /* Init */
                __ASM_EMIT("xor         %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jb          2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups     0x00(%[dst], %[off], 2), %%ymm0")       /* ymm0 = r0 i0 */
                __ASM_EMIT("vmovups     0x20(%[dst], %[off], 2), %%ymm1")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%ymm2")          /* ymm2 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vmulps      %%ymm0, %%ymm0, %%ymm4")                /* ymm0 = r0*r0 + i0*i0 */
                __ASM_EMIT("vmulps      %%ymm1, %%ymm1, %%ymm5")
                __ASM_EMIT("vunpckhps   %%ymm2, %%ymm2, %%ymm3")                /* ymm3 = s2 s2 s3 s3 s6 s6 s7 s7 */
                __ASM_EMIT("vunpcklps   %%ymm2, %%ymm2, %%ymm2")                /* ymm2 = s0 s0 s1 s1 s4 s4 s5 s5 */
                __ASM_EMIT("vhaddps     %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = R = r0*r0+i0*i0 */
                __ASM_EMIT("vmovups     0x00(%[CC]), %%ymm5")                   /* ymm5 = 1 */
                __ASM_EMIT("vextractf128 $1, %%ymm2, %%xmm6")                   /* xmm6 = s4 s4 s5 s5 */
                __ASM_EMIT("vdivps      %%ymm4, %%ymm5, %%ymm4")                /* ymm4 = 1/R */
                __ASM_EMIT("vinsertf128  $1, %%xmm3, %%ymm2, %%ymm2")           /* ymm2 = s0 s0 s1 s1 s2 s2 s3 s3 */
                __ASM_EMIT("vxorps      0x20(%[CC]), %%ymm4, %%ymm5")           /* ymm5 = -1/R */
                __ASM_EMIT("vinsertf128  $0, %%xmm6, %%ymm3, %%ymm3")           /* ymm3 = s4 s4 s5 s5 s6 s6 s7 s7 */
                __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm7")                /* ymm7 = 1/R -1/R */
                __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm5")
                __ASM_EMIT("vmulps      %%ymm5, %%ymm0, %%ymm0")                /* ymm0 = r0/R -i0/R */
                __ASM_EMIT("vmulps      %%ymm7, %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps      %%ymm2, %%ymm0, %%ymm0")                /* ymm0 = (r0*s0)/R -(i0*s0)/R */
                __ASM_EMIT("vmulps      %%ymm3, %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups     %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups     %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("add         $0x20, %[off]")
                __ASM_EMIT("sub         $8, %[count]")
                __ASM_EMIT("jae         1b")
                __ASM_EMIT("2:")
                /* x4 block */
                __ASM_EMIT("add         $4, %[count]")
                __ASM_EMIT("jl          4f")
                __ASM_EMIT("vmovups     0x00(%[dst], %[off], 2), %%ymm0")       /* xmm0 = r0 i0 */
                __ASM_EMIT("vmovups     0x10(%[dst], %[off], 2), %%xmm1")
                __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */
                __ASM_EMIT("vmulps      %%xmm1, %%xmm1, %%xmm5")
                __ASM_EMIT("vhaddps     %%xmm5, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */
                __ASM_EMIT("vmovups     0x00(%[CC]), %%xmm5")                   /* xmm5 = 1 */
                __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = 1/R */
                __ASM_EMIT("vxorps      0x20(%[CC]), %%xmm4, %%xmm5")           /* xmm5 = -1/R */
                __ASM_EMIT("vunpckhps   %%xmm5, %%xmm4, %%xmm7")                /* xmm7 = 1/R -1/R */
                __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")
                __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = r0/R -i0/R */
                __ASM_EMIT("vmulps      %%xmm7, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups     0x00(%[src], %[off]), %%xmm4")          /* xmm4 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps   %%xmm4, %%xmm4, %%xmm6")                /* xmm6 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpcklps   %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = s0 s0 s1 s1 */
                __ASM_EMIT("vmulps      %%xmm4, %%xmm0, %%xmm0")                /* xmm0 = (r0*s0)/R -(i0*s0)/R */
                __ASM_EMIT("vmulps      %%xmm6, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups     %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("add         $0x10, %[off]")
                __ASM_EMIT("sub         $4, %[count]")
                __ASM_EMIT("4:")
                /* x2 block */
                __ASM_EMIT("add         $2, %[count]")
                __ASM_EMIT("jl          6f")
                __ASM_EMIT("vmovups     0x00(%[dst], %[off], 2), %%xmm0")       /* xmm0 = r0 i0 */
                __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */
                __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */
                __ASM_EMIT("vmovlps     0x00(%[src], %[off]), %%xmm5, %%xmm5")  /* xmm5 = s0 */
                __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = s0/R */
                __ASM_EMIT("vxorps      0x20(%[CC]), %%xmm4, %%xmm5")           /* xmm5 = -s0/R */
                __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = s0/R -s0/R */
                __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = (s0*r0)/R -(s0*i0)/R */
                __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add         $0x08, %[off]")
                __ASM_EMIT("sub         $2, %[count]")
                __ASM_EMIT("6:")
                /* x1 block */
                __ASM_EMIT("add         $1, %[count]")
                __ASM_EMIT("jl          8f")
                __ASM_EMIT("vmovlps     0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")/* xmm0 = r0 i0 */
                __ASM_EMIT("vmulps      %%xmm0, %%xmm0, %%xmm4")                /* xmm0 = r0*r0 + i0*i0 */
                __ASM_EMIT("vhaddps     %%xmm4, %%xmm4, %%xmm4")                /* xmm4 = R = r0*r0+i0*i0 */
                __ASM_EMIT("vmovss      0x00(%[src], %[off]), %%xmm5")          /* xmm5 = s0 */
                __ASM_EMIT("vdivps      %%xmm4, %%xmm5, %%xmm4")                /* xmm4 = s0/R */
                __ASM_EMIT("vxorps      0x20(%[CC]), %%xmm4, %%xmm5")           /* xmm5 = -s0/R */
                __ASM_EMIT("vunpcklps   %%xmm5, %%xmm4, %%xmm5")                /* xmm7 = s0/R -s0/R */
                __ASM_EMIT("vmulps      %%xmm5, %%xmm0, %%xmm0")                /* xmm0 = (r0*s0)/R -(i0*s0)/R */
                __ASM_EMIT("vmovlps     %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("8:")
                /* end */

                : [off] "=&r" (off), [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "r" (pcomplex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            ARCH_X86_ASM
            (
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")
                /* x16 block */
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups                 0x00(%[src]), %%xmm0")                      /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src]), %%xmm1")
                __ASM_EMIT("vmovups                 0x20(%[src]), %%xmm2")
                __ASM_EMIT("vmovups                 0x30(%[src]), %%xmm3")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm4")                    /* xmm4 = s2 0 s3 0 s6 0 s7 0 */
                __ASM_EMIT("vunpckhps               %%xmm7, %%xmm1, %%xmm5")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 s4 0 s5 0 */
                __ASM_EMIT("vunpcklps               %%xmm7, %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm1, 0x20(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm5, 0x30(%[dst])")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm4")                    /* xmm4 = s2 0 s3 0 s6 0 s7 0 */
                __ASM_EMIT("vunpckhps               %%xmm7, %%xmm3, %%xmm5")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")                    /* xmm0 = s0 0 s1 0 s4 0 s5 0 */
                __ASM_EMIT("vunpcklps               %%xmm7, %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm2, 0x40(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm4, 0x50(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm3, 0x60(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm5, 0x70(%[dst])")
                __ASM_EMIT("add                     $0x40, %[src]")
                __ASM_EMIT("add                     $0x80, %[dst]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("vmovups                 0x00(%[src]), %%xmm0")                      /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm7, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm7, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst])")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[src]")
                __ASM_EMIT("add                     $0x40, %[dst]")
                __ASM_EMIT("4:")
                /* x4 block */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src]), %%xmm0")                      /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm7, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst])")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[src]")
                __ASM_EMIT("add                     $0x20, %[dst]")
                __ASM_EMIT("6:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss                  0x00(%[src]), %%xmm0")
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add                     $0x4, %[src]")
                __ASM_EMIT("add                     $0x8, %[dst]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     7b")
                __ASM_EMIT("8:")

                : [dst] "+r" (dst), [src] "+r" (src), [count] "+r" (count)
                :
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        IF_ARCH_X86(
            static const float pcomplex_corr_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(1e-36f),
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
                __ASM_EMIT("xor             %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub             $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("vmovaps         %[CC], %%ymm7")                         /* ymm7  = threshold */
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("vmovups         0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("vmovups         0x20(%[src1], %[off], 2), %%xmm1")      /* xmm1  = a4 b4 a5 b5 */
                __ASM_EMIT("vmovups         0x30(%[src1], %[off], 2), %%xmm5")      /* xmm5  = a6 b6 a7 b7 */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm0, %%ymm0")            /* ymm0  = a0 b0 a1 b1 a4 b4 a5 b5 */
                __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm4, %%ymm4")            /* ymm4  = a2 b2 a3 b3 a6 b6 a7 b7 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("vmovups         0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                __ASM_EMIT("vmovups         0x20(%[src2], %[off], 2), %%xmm1")      /* xmm1  = c4 d4 c5 d5 */
                __ASM_EMIT("vmovups         0x30(%[src2], %[off], 2), %%xmm5")      /* xmm5  = c6 d6 c7 d7 */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm2, %%ymm2")            /* ymm2  = c0 d0 c1 d1 c4 d4 c5 d5 */
                __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm6, %%ymm6")            /* ymm6  = c2 d2 c3 d3 c6 d6 c7 d7 */
                /* Do shuffle */
                __ASM_EMIT("vshufps         $0xdd, %%ymm4, %%ymm0, %%ymm1")         /* ymm1  = b = b0 b1 b2 b3 b4 b5 b6 b7 */
                __ASM_EMIT("vshufps         $0xdd, %%ymm6, %%ymm2, %%ymm3")         /* ymm3  = d = d0 d1 d2 d3 d4 d5 d6 d7 */
                __ASM_EMIT("vshufps         $0x88, %%ymm4, %%ymm0, %%ymm0")         /* ymm0  = a = a0 a1 a2 a3 a4 a5 a6 a7 */
                __ASM_EMIT("vshufps         $0x88, %%ymm6, %%ymm2, %%ymm2")         /* ymm2  = c = c0 c1 c2 c3 c4 c5 c6 c7 */
                /* ymm0 = a, ymm1 = b, ymm2 = c, ymm3 = d */
                __ASM_EMIT("vmulps          %%ymm0, %%ymm2, %%ymm4")                /* ymm4  = a*c */
                __ASM_EMIT("vmulps          %%ymm1, %%ymm3, %%ymm5")                /* ymm5  = b*d */
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")                /* ymm0  = a*a */
                __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")                /* ymm1  = b*b */
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")                /* ymm2  = c*c */
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")                /* ymm3  = d*d */
                __ASM_EMIT("vaddps          %%ymm1, %%ymm0, %%ymm0")                /* ymm0  = a*a + b*b */
                __ASM_EMIT("vaddps          %%ymm3, %%ymm2, %%ymm2")                /* ymm2  = c*c + d*d */
                __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vaddps          %%ymm5, %%ymm4, %%ymm4")                /* ymm4  = nom = a*c + b*d */
                __ASM_EMIT("vsqrtps         %%ymm0, %%ymm1")                        /* ymm1  = sqrt(den) */
                __ASM_EMIT("vdivps          %%ymm1, %%ymm4, %%ymm4")                /* ymm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%ymm7, %%ymm0, %%ymm0")            /* ymm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%ymm4, %%ymm0, %%ymm0")                /* ymm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 v4 v5 v6 v7 */
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("jae             1b")
                /* x4 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $4, %[count]")                          /* count += 4 */
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("vmovups         0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("vmovups         0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                /* Do shuffle */
                __ASM_EMIT("vshufps         $0xdd, %%xmm4, %%xmm0, %%xmm1")         /* xmm1  = b = b0 b1 b2 b3 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm6, %%xmm2, %%xmm3")         /* xmm3  = d = d0 d1 d2 d3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm4, %%xmm0, %%xmm0")         /* xmm0  = a = a0 a1 a2 a3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm6, %%xmm2, %%xmm2")         /* xmm2  = c = c0 c1 c2 c3 */
                /* xmm0 = a, xmm1 = b, xmm2 = c, xmm3 = d */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm2, %%xmm4")                /* xmm4  = a*c */
                __ASM_EMIT("vmulps          %%xmm1, %%xmm3, %%xmm5")                /* xmm5  = b*d */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")                /* xmm0  = a*a */
                __ASM_EMIT("vmulps          %%xmm1, %%xmm1, %%xmm1")                /* xmm1  = b*b */
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")                /* xmm2  = c*c */
                __ASM_EMIT("vmulps          %%xmm3, %%xmm3, %%xmm3")                /* xmm3  = d*d */
                __ASM_EMIT("vaddps          %%xmm1, %%xmm0, %%xmm0")                /* xmm0  = a*a + b*b */
                __ASM_EMIT("vaddps          %%xmm3, %%xmm2, %%xmm2")                /* xmm2  = c*c + d*d */
                __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vaddps          %%xmm5, %%xmm4, %%xmm4")                /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("vsqrtps         %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den) */
                __ASM_EMIT("vdivps          %%xmm1, %%xmm4, %%xmm4")                /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%xmm7, %%xmm0, %%xmm0")            /* xmm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%xmm4, %%xmm0, %%xmm0")                /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 v4 v5 v6 v7 */
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")                          /* count -= 4 */
                /* x1 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmovss          0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a */
                __ASM_EMIT("vmovss          0x04(%[src1], %[off], 2), %%xmm1")      /* xmm1  = b */
                __ASM_EMIT("vmovss          0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c */
                __ASM_EMIT("vmovss          0x04(%[src2], %[off], 2), %%xmm3")      /* xmm3  = d */
                __ASM_EMIT("vmulss          %%xmm0, %%xmm2, %%xmm4")                /* xmm4  = a*c */
                __ASM_EMIT("vmulss          %%xmm1, %%xmm3, %%xmm5")                /* xmm5  = b*d */
                __ASM_EMIT("vmulss          %%xmm0, %%xmm0, %%xmm0")                /* xmm0  = a*a */
                __ASM_EMIT("vmulss          %%xmm1, %%xmm1, %%xmm1")                /* xmm1  = b*b */
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")                /* xmm2  = c*c */
                __ASM_EMIT("vmulss          %%xmm3, %%xmm3, %%xmm3")                /* xmm3  = d*d */
                __ASM_EMIT("vaddss          %%xmm1, %%xmm0, %%xmm0")                /* xmm0  = a*a + b*b */
                __ASM_EMIT("vaddss          %%xmm3, %%xmm2, %%xmm2")                /* xmm2  = c*c + d*d */
                __ASM_EMIT("vmulss          %%xmm2, %%xmm0, %%xmm0")                /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vaddss          %%xmm5, %%xmm4, %%xmm4")                /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("vsqrtss         %%xmm0, %%xmm1, %%xmm1")                /* xmm1  = sqrt(den) */
                __ASM_EMIT("vdivss          %%xmm1, %%xmm4, %%xmm4")                /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%xmm7, %%xmm0, %%xmm0")            /* xmm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%xmm4, %%xmm0, %%xmm0")                /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 */
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             5b")
                /* End */
                __ASM_EMIT("6:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst_corr), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "m" (pcomplex_corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void pcomplex_corr_fma3(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);

            /*
             * src1[i] = a + j*b, src2[i] = c + j*d
             * den  = (a*a + b*b)*(c*c + d*d)
             * nom  = a*c + b*d
             * corr[i] = (den > threshold) ? nom / sqrt(den) : 0.0
             */
            ARCH_X86_ASM (
                __ASM_EMIT("xor             %[off], %[off]")
                /* x8 blocks */
                __ASM_EMIT("sub             $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("vmovaps         %[CC], %%ymm7")                         /* ymm7  = threshold */
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("vmovups         0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("vmovups         0x20(%[src1], %[off], 2), %%xmm1")      /* xmm1  = a4 b4 a5 b5 */
                __ASM_EMIT("vmovups         0x30(%[src1], %[off], 2), %%xmm5")      /* xmm5  = a6 b6 a7 b7 */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm0, %%ymm0")            /* ymm0  = a0 b0 a1 b1 a4 b4 a5 b5 */
                __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm4, %%ymm4")            /* ymm4  = a2 b2 a3 b3 a6 b6 a7 b7 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("vmovups         0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                __ASM_EMIT("vmovups         0x20(%[src2], %[off], 2), %%xmm1")      /* xmm1  = c4 d4 c5 d5 */
                __ASM_EMIT("vmovups         0x30(%[src2], %[off], 2), %%xmm5")      /* xmm5  = c6 d6 c7 d7 */
                __ASM_EMIT("vinsertf128     $1, %%xmm1, %%ymm2, %%ymm2")            /* ymm2  = c0 d0 c1 d1 c4 d4 c5 d5 */
                __ASM_EMIT("vinsertf128     $1, %%xmm5, %%ymm6, %%ymm6")            /* ymm6  = c2 d2 c3 d3 c6 d6 c7 d7 */
                /* Do shuffle */
                __ASM_EMIT("vshufps         $0xdd, %%ymm4, %%ymm0, %%ymm1")         /* ymm1  = b = b0 b1 b2 b3 b4 b5 b6 b7 */
                __ASM_EMIT("vshufps         $0xdd, %%ymm6, %%ymm2, %%ymm3")         /* ymm3  = d = d0 d1 d2 d3 d4 d5 d6 d7 */
                __ASM_EMIT("vshufps         $0x88, %%ymm4, %%ymm0, %%ymm0")         /* ymm0  = a = a0 a1 a2 a3 a4 a5 a6 a7 */
                __ASM_EMIT("vshufps         $0x88, %%ymm6, %%ymm2, %%ymm2")         /* ymm2  = c = c0 c1 c2 c3 c4 c5 c6 c7 */
                /* ymm0 = a, ymm1 = b, ymm2 = c, ymm3 = d */
                __ASM_EMIT("vmulps          %%ymm0, %%ymm2, %%ymm4")                /* ymm4  = a*c */
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")                /* ymm0  = a*a */
                __ASM_EMIT("vmulps          %%ymm2, %%ymm2, %%ymm2")                /* ymm2  = c*c */
                __ASM_EMIT("vfmadd231ps     %%ymm1, %%ymm3, %%ymm4")                /* ymm4  = nom = a*c + b*d */
                __ASM_EMIT("vfmadd231ps     %%ymm1, %%ymm1, %%ymm0")                /* ymm0  = a*a + b*b */
                __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm2")                /* ymm2  = c*c + d*d */
                __ASM_EMIT("vmulps          %%ymm2, %%ymm0, %%ymm0")                /* ymm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vsqrtps         %%ymm0, %%ymm1")                        /* ymm1  = sqrt(den) */
                __ASM_EMIT("vdivps          %%ymm1, %%ymm4, %%ymm4")                /* ymm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%ymm7, %%ymm0, %%ymm0")            /* ymm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%ymm4, %%ymm0, %%ymm0")                /* ymm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovups         %%ymm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 v4 v5 v6 v7 */
                __ASM_EMIT("add             $0x20, %[off]")
                __ASM_EMIT("sub             $8, %[count]")                          /* count -= 8 */
                __ASM_EMIT("jae             1b")
                /* x4 block */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $4, %[count]")                          /* count += 4 */
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("vmovups         0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("vmovups         0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
                /* Do shuffle */
                __ASM_EMIT("vshufps         $0xdd, %%xmm4, %%xmm0, %%xmm1")         /* xmm1  = b = b0 b1 b2 b3 */
                __ASM_EMIT("vshufps         $0xdd, %%xmm6, %%xmm2, %%xmm3")         /* xmm3  = d = d0 d1 d2 d3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm4, %%xmm0, %%xmm0")         /* xmm0  = a = a0 a1 a2 a3 */
                __ASM_EMIT("vshufps         $0x88, %%xmm6, %%xmm2, %%xmm2")         /* xmm2  = c = c0 c1 c2 c3 */
                /* xmm0 = a, xmm1 = b, xmm2 = c, xmm3 = d */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm2, %%xmm4")                /* xmm4  = a*c */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")                /* xmm0  = a*a */
                __ASM_EMIT("vmulps          %%xmm2, %%xmm2, %%xmm2")                /* xmm2  = c*c */
                __ASM_EMIT("vfmadd231ps     %%xmm1, %%xmm3, %%xmm4")                /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("vfmadd231ps     %%xmm1, %%xmm1, %%xmm0")                /* xmm0  = a*a + b*b */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm3, %%xmm2")                /* xmm2  = c*c + d*d */
                __ASM_EMIT("vmulps          %%xmm2, %%xmm0, %%xmm0")                /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vsqrtps         %%xmm0, %%xmm1")                        /* xmm1  = sqrt(den) */
                __ASM_EMIT("vdivps          %%xmm1, %%xmm4, %%xmm4")                /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%xmm7, %%xmm0, %%xmm0")            /* xmm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%xmm4, %%xmm0, %%xmm0")                /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovups         %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 v1 v2 v3 v4 v5 v6 v7 */
                __ASM_EMIT("add             $0x10, %[off]")
                __ASM_EMIT("sub             $4, %[count]")                          /* count -= 4 */
                /* x1 blocks */
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("5:")
                __ASM_EMIT("vmovss          0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a */
                __ASM_EMIT("vmovss          0x04(%[src1], %[off], 2), %%xmm1")      /* xmm1  = b */
                __ASM_EMIT("vmovss          0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c */
                __ASM_EMIT("vmovss          0x04(%[src2], %[off], 2), %%xmm3")      /* xmm3  = d */
                __ASM_EMIT("vmulss          %%xmm0, %%xmm2, %%xmm4")                /* xmm4  = a*c */
                __ASM_EMIT("vmulss          %%xmm0, %%xmm0, %%xmm0")                /* xmm0  = a*a */
                __ASM_EMIT("vmulss          %%xmm2, %%xmm2, %%xmm2")                /* xmm2  = c*c */
                __ASM_EMIT("vfmadd231ss     %%xmm1, %%xmm3, %%xmm4")                /* xmm4  = nom = a*c + b*d */
                __ASM_EMIT("vfmadd231ss     %%xmm1, %%xmm1, %%xmm0")                /* xmm0  = a*a + b*b */
                __ASM_EMIT("vfmadd231ss     %%xmm3, %%xmm3, %%xmm2")                /* xmm2  = c*c + d*d */
                __ASM_EMIT("vmulss          %%xmm2, %%xmm0, %%xmm0")                /* xmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vsqrtss         %%xmm0, %%xmm1, %%xmm1")                /* xmm1  = sqrt(den) */
                __ASM_EMIT("vdivss          %%xmm1, %%xmm4, %%xmm4")                /* xmm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%xmm7, %%xmm0, %%xmm0")            /* xmm0  = [den >= threshold] */
                __ASM_EMIT("vandps          %%xmm4, %%xmm0, %%xmm0")                /* xmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[dst], %[off])")          /* dst[0]  = v0 */
                __ASM_EMIT("add             $0x04, %[off]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             5b")
                /* End */
                __ASM_EMIT("6:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst_corr), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "m" (pcomplex_corr_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX_PCOMPLEX_H_ */
