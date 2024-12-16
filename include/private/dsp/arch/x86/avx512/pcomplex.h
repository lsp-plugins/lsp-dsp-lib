/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        #define FMA_OFF(a, b)       a
        #define FMA_ON(a, b)        b

        #define PCOMPLEX_MULX_CORE(DST, SRC1, SRC2) \
            /* Init */ \
            __ASM_EMIT("xor                 %[off], %[off]") \
            /* x16 blocks */ \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovsldup           0x00(%[" SRC1 "], %[off]), %%zmm0")     /* zmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovsldup           0x40(%[" SRC1 "], %[off]), %%zmm1") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%zmm6")     /* zmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovups             0x40(%[" SRC2 "], %[off]), %%zmm7") \
            __ASM_EMIT("vshufps             $0xb1, %%zmm6, %%zmm6, %%zmm4")         /* zmm4 = bi0 br0 */ \
            __ASM_EMIT("vmovshdup           0x00(%[" SRC1 "], %[off]), %%zmm2")     /* zmm2 = ai0 ai0 */ \
            __ASM_EMIT("vmovshdup           0x40(%[" SRC1 "], %[off]), %%zmm3") \
            __ASM_EMIT("vshufps             $0xb1, %%zmm7, %%zmm7, %%zmm5") \
            __ASM_EMIT("vmulps              %%zmm4, %%zmm2, %%zmm2")                /* zmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vmulps              %%zmm5, %%zmm3, %%zmm3") \
            __ASM_EMIT("vfmaddsub132ps      %%zmm6, %%zmm2, %%zmm0")                /* zmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vfmaddsub132ps      %%zmm7, %%zmm3, %%zmm1") \
            __ASM_EMIT("vmovups             %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%zmm1, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x80, %[off]") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("2:") \
            /* x8 block */ \
            __ASM_EMIT("add                 $8, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("vmovsldup           0x00(%[" SRC1 "], %[off]), %%ymm0")     /* ymm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovsldup           0x20(%[" SRC1 "], %[off]), %%ymm1") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%ymm6")     /* ymm6 = br0 bi0 */ \
            __ASM_EMIT("vmovups             0x20(%[" SRC2 "], %[off]), %%ymm7") \
            __ASM_EMIT("vshufps             $0xb1, %%ymm6, %%ymm6, %%ymm4")         /* ymm4 = bi0 br0 */ \
            __ASM_EMIT("vmovshdup           0x00(%[" SRC1 "], %[off]), %%ymm2")     /* ymm2 = ai0 ai0 */ \
            __ASM_EMIT("vmovshdup           0x20(%[" SRC1 "], %[off]), %%ymm3") \
            __ASM_EMIT("vshufps             $0xb1, %%ymm7, %%ymm7, %%ymm5") \
            __ASM_EMIT("vmulps              %%ymm4, %%ymm2, %%ymm2")                /* ymm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vmulps              %%ymm5, %%ymm3, %%ymm3") \
            __ASM_EMIT("vfmaddsub132ps      %%ymm6, %%ymm2, %%ymm0")                /* ymm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vfmaddsub132ps      %%ymm7, %%ymm3, %%ymm1") \
            __ASM_EMIT("vmovups             %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("sub                 $8, %[count]") \
            __ASM_EMIT("add                 $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /* x4 block */ \
            __ASM_EMIT("add                 $4, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT("vmovsldup           0x00(%[" SRC1 "], %[off]), %%xmm0")     /* xmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovsldup           0x10(%[" SRC1 "], %[off]), %%xmm1") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%xmm6")     /* xmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovups             0x10(%[" SRC2 "], %[off]), %%xmm7") \
            __ASM_EMIT("vshufps             $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT("vmovshdup           0x00(%[" SRC1 "], %[off]), %%xmm2")     /* xmm2 = ai0 ai0 */ \
            __ASM_EMIT("vmovshdup           0x10(%[" SRC1 "], %[off]), %%xmm3") \
            __ASM_EMIT("vshufps             $0xb1, %%xmm7, %%xmm7, %%xmm5") \
            __ASM_EMIT("vmulps              %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vmulps              %%xmm5, %%xmm3, %%xmm3") \
            __ASM_EMIT("vfmaddsub132ps      %%xmm6, %%xmm2, %%xmm0")                /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vfmaddsub132ps      %%xmm7, %%xmm3, %%xmm1") \
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups             %%xmm1, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x20, %[off]") \
            __ASM_EMIT("sub                 $4, %[count]") \
            __ASM_EMIT("6:") \
            /* x2 block */ \
            __ASM_EMIT("add                 $2, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("vmovups             0x00(%[" SRC2 "], %[off]), %%xmm6")     /* xmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovsldup           0x00(%[" SRC1 "], %[off]), %%xmm0")     /* xmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovshdup           0x00(%[" SRC1 "], %[off]), %%xmm2")     /* xmm2 = ai0 ai0 */ \
            __ASM_EMIT("vshufps             $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT("vmulps              %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vfmaddsub132ps      %%xmm6, %%xmm2, %%xmm0")                /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vmovups             %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                 $0x10, %[off]") \
            __ASM_EMIT("sub                 $2, %[count]") \
            __ASM_EMIT("8:") \
            /* x1 block */ \
            __ASM_EMIT("add                 $1, %[count]") \
            __ASM_EMIT("jl                  10f") \
            __ASM_EMIT("vmovlps             0x00(%[" SRC1 "], %[off]), %%xmm4, %%xmm4")     /* xmm4 = ar0 ai0 */ \
            __ASM_EMIT("vmovlps             0x00(%[" SRC2 "], %[off]), %%xmm6, %%xmm6")     /* xmm6 = br0 bi0 */ \
            __ASM_EMIT("vmovsldup           %%xmm4, %%xmm0")                        /* xmm0 = ar0 ar0 */ \
            __ASM_EMIT("vmovshdup           %%xmm4, %%xmm2")                        /* xmm2 = ai0 ai0 */ \
            __ASM_EMIT("vshufps             $0xb1, %%xmm6, %%xmm6, %%xmm4")         /* xmm4 = bi0 br0 */ \
            __ASM_EMIT("vmulps              %%xmm4, %%xmm2, %%xmm2")                /* xmm2 = ai0*bi0 ai0*br0 */ \
            __ASM_EMIT("vfmaddsub132ps      %%xmm6, %%xmm2, %%xmm0")                /* xmm0 = ar0*br0-ai0*bi0 ar0*bi0+ai0*br0 ... */ \
            __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("10:") \
            /* end */

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_MULX_CORE("dst", "src1", "src2")
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
                PCOMPLEX_MULX_CORE("dst", "dst", "src")
                : [off] "=&r" (off),
                  [count] "+r" (count)
                : [dst] "r" (dst), [src] "r" (src)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef PCOMPLEX_MULX_CORE

        static const uint16_t pcomplex_mod_kmask[] =
        {
            0x5555,
            0xaaaa
        };

        void pcomplex_mod(float *dst, const float *src, size_t count)
        {
            // Strided-load technique is used to improve performance
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                 %[off], %[off]")
                // 32x blocks
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jb                  2f")
                __ASM_EMIT("kmovw               0x00 + %[kmask], %%k4")                         // k4   = 0x5555 /* even indices */
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups             0x000(%[src], %[off], 2), %%zmm0")              // zmm0 = r00 i00 ...
                __ASM_EMIT("vmovups             0x040(%[src], %[off], 2), %%zmm1")
                __ASM_EMIT("vmovups             0x080(%[src], %[off], 2), %%zmm2")
                __ASM_EMIT("vmovups             0x0c0(%[src], %[off], 2), %%zmm3")
                __ASM_EMIT("vmulps              %%zmm0, %%zmm0, %%zmm0")                        // zmm0 = r00*r00 i00*i00
                __ASM_EMIT("vmulps              %%zmm1, %%zmm1, %%zmm1")
                __ASM_EMIT("vmulps              %%zmm2, %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps              %%zmm3, %%zmm3, %%zmm3")
                __ASM_EMIT("vmovshdup           %%zmm0, %%zmm4")                                // zmm4 = i00*i00 i00*i00
                __ASM_EMIT("vmovshdup           %%zmm1, %%zmm5")
                __ASM_EMIT("vmovshdup           %%zmm2, %%zmm6")
                __ASM_EMIT("vmovshdup           %%zmm3, %%zmm7")
                __ASM_EMIT("vaddps              %%zmm4, %%zmm0, %%zmm0")                        // zmm0 = r00*r00+i00*i00 i00*i00+i00*i00
                __ASM_EMIT("vaddps              %%zmm5, %%zmm1, %%zmm1")
                __ASM_EMIT("vaddps              %%zmm6, %%zmm2, %%zmm2")
                __ASM_EMIT("vaddps              %%zmm7, %%zmm3, %%zmm3")
                __ASM_EMIT("vcompressps         %%zmm0, %%zmm0 %{%%k4%}")                       // ymm0 = r00 ...
                __ASM_EMIT("vcompressps         %%zmm1, %%zmm1 %{%%k4%}")
                __ASM_EMIT("vcompressps         %%zmm2, %%zmm2 %{%%k4%}")
                __ASM_EMIT("vcompressps         %%zmm3, %%zmm3 %{%%k4%}")
                __ASM_EMIT("vinsertf32x8        $1, %%ymm1, %%zmm0, %%zmm0")                    // zmm0 = q00 ...
                __ASM_EMIT("vinsertf32x8        $1, %%ymm3, %%zmm2, %%zmm2")
                __ASM_EMIT("vsqrtps             %%zmm0, %%zmm0")
                __ASM_EMIT("vsqrtps             %%zmm2, %%zmm2")
                __ASM_EMIT("vmovups             %%zmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups             %%zmm2, 0x40(%[dst], %[off])")
                __ASM_EMIT("add                 $0x80, %[off]")
                __ASM_EMIT("sub                 $32, %[count]")
                __ASM_EMIT("jae                 1b")
                __ASM_EMIT("2:")
                // 16x blocks
                __ASM_EMIT("add                 $16, %[count]")
                __ASM_EMIT("jl                  4f")
                __ASM_EMIT("vmovups             0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups             0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vinsertf128         $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
                __ASM_EMIT("vinsertf128         $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vmovups             0x040(%[src], %[off], 2), %%xmm1")
                __ASM_EMIT("vmovups             0x050(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("vinsertf128         $1, 0x060(%[src], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vinsertf128         $1, 0x070(%[src], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmulps              %%ymm0, %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps              %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps              %%ymm1, %%ymm1, %%ymm1")
                __ASM_EMIT("vmulps              %%ymm3, %%ymm3, %%ymm3")
                __ASM_EMIT("vhaddps             %%ymm2, %%ymm0, %%ymm0")
                __ASM_EMIT("vhaddps             %%ymm3, %%ymm1, %%ymm1")
                __ASM_EMIT("vsqrtps             %%ymm0, %%ymm0")
                __ASM_EMIT("vsqrtps             %%ymm1, %%ymm1")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups             %%ymm1, 0x20(%[dst], %[off])")
                __ASM_EMIT("sub                 $16, %[count]")
                __ASM_EMIT("add                 $0x40, %[off]")
                __ASM_EMIT("4:")
                // 8x block
                __ASM_EMIT("add                 $8, %[count]")
                __ASM_EMIT("jl                  6f")
                __ASM_EMIT("vmovups             0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups             0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vinsertf128         $1, 0x020(%[src], %[off], 2), %%ymm0, %%ymm0")
                __ASM_EMIT("vinsertf128         $1, 0x030(%[src], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vmulps              %%ymm0, %%ymm0, %%ymm0")
                __ASM_EMIT("vmulps              %%ymm2, %%ymm2, %%ymm2")
                __ASM_EMIT("vhaddps             %%ymm2, %%ymm0, %%ymm0")
                __ASM_EMIT("vsqrtps             %%ymm0, %%ymm0")
                __ASM_EMIT("vmovups             %%ymm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub                 $8, %[count]")
                __ASM_EMIT("add                 $0x20, %[off]")
                __ASM_EMIT("6:")
                // 4x block
                __ASM_EMIT("add                 $4, %[count]")
                __ASM_EMIT("jl                  8f")
                __ASM_EMIT("vmovups             0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmovups             0x010(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vmulps              %%xmm2, %%xmm2, %%xmm2")
                __ASM_EMIT("vhaddps             %%xmm2, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps             %%xmm0, %%xmm0")
                __ASM_EMIT("vmovups             %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub                 $4, %[count]")
                __ASM_EMIT("add                 $0x10, %[off]")
                __ASM_EMIT("8:")
                // 2x block
                __ASM_EMIT("add                 $2, %[count]")
                __ASM_EMIT("jl                  10f")
                __ASM_EMIT("vmovups             0x000(%[src], %[off], 2), %%xmm0")
                __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vhaddps             %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps             %%xmm0, %%xmm0")
                __ASM_EMIT("vmovlps             %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub                 $2, %[count]")
                __ASM_EMIT("add                 $0x08, %[off]")
                __ASM_EMIT("10:")
                // 1x block
                __ASM_EMIT("add                 $1, %[count]")
                __ASM_EMIT("jl                  12f")
                __ASM_EMIT("vmovlps             0x000(%[src], %[off], 2), %%xmm0, %%xmm0")
                __ASM_EMIT("vmulps              %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vhaddps             %%xmm0, %%xmm0, %%xmm0")
                __ASM_EMIT("vsqrtps             %%xmm0, %%xmm0")
                __ASM_EMIT("vmovss              %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("12:")
                // End

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [kmask] "o" (pcomplex_mod_kmask)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        IF_ARCH_X86(
            static uint32_t pcomplex_div_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC8(0x3f800000)    // 1.0
            };
        )

        #define PCOMPLEX_DIV_CORE(DST, T, B) \
            /* Init */ \
            __ASM_EMIT("xor                     %[off], %[off]") \
            /* x16 block */ \
            __ASM_EMIT("sub                     $16, %[count]") \
            __ASM_EMIT("jb                      2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups                 0x00(%[" T "], %[off]), %%zmm0")        /* zmm0 = tr ti */ \
            __ASM_EMIT("vmovups                 0x40(%[" T "], %[off]), %%zmm4") \
            __ASM_EMIT("vmovups                 0x00(%[" B "], %[off]), %%zmm1")        /* zmm1 = br bi */ \
            __ASM_EMIT("vmovups                 0x40(%[" B "], %[off]), %%zmm5") \
            __ASM_EMIT("vmovshdup               %%zmm0, %%zmm2")                        /* zmm2 = ti ti */ \
            __ASM_EMIT("vmovshdup               %%zmm4, %%zmm6") \
            __ASM_EMIT("vmovsldup               %%zmm0, %%zmm0")                        /* zmm0 = tr tr */ \
            __ASM_EMIT("vmovsldup               %%zmm4, %%zmm4") \
            __ASM_EMIT("vmulps                  %%zmm1, %%zmm0, %%zmm0")                /* zmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vmulps                  %%zmm5, %%zmm4, %%zmm4") \
            __ASM_EMIT("vshufps                 $0xb1, %%zmm1, %%zmm1, %%zmm1")         /* zmm1 = bi br */ \
            __ASM_EMIT("vshufps                 $0xb1, %%zmm5, %%zmm5, %%zmm5") \
            __ASM_EMIT("vfmsubadd231ps          %%zmm1, %%zmm2, %%zmm0")                /* zmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vfmsubadd231ps          %%zmm5, %%zmm6, %%zmm4") \
            __ASM_EMIT("vmulps                  %%zmm1, %%zmm1, %%zmm1")                /* zmm1 = bi*bi br*br */ \
            __ASM_EMIT("vmulps                  %%zmm5, %%zmm5, %%zmm5") \
            __ASM_EMIT("vmovshdup               %%zmm1, %%zmm3")                        /* zmm3 = br*br br*br */ \
            __ASM_EMIT("vmovshdup               %%zmm5, %%zmm7") \
            __ASM_EMIT("vaddps                  %%zmm1, %%zmm3, %%zmm1")                /* zmm1 = br*br+bi*bi br*br+br*br */ \
            __ASM_EMIT("vaddps                  %%zmm5, %%zmm7, %%zmm5") \
            __ASM_EMIT("vmovsldup               %%zmm1, %%zmm1")                        /* zmm1 = br*br+bi*bi br*br+bi*bi */ \
            __ASM_EMIT("vmovsldup               %%zmm5, %%zmm5") \
            __ASM_EMIT("vdivps                  %%zmm1, %%zmm0, %%zmm0")                /* zmm0 = r' i' */ \
            __ASM_EMIT("vdivps                  %%zmm5, %%zmm4, %%zmm4") \
            __ASM_EMIT("vmovups                 %%zmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups                 %%zmm4, 0x40(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x80, %[off]") \
            __ASM_EMIT("sub                     $16, %[count]") \
            __ASM_EMIT("jae                     1b") \
            __ASM_EMIT("2:") \
            /* x8 block */ \
            __ASM_EMIT("add                     $8, %[count]") \
            __ASM_EMIT("jl                      4f") \
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
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%ymm7")                  /* ymm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%ymm3, %%ymm7, %%ymm3")                /* ymm3 = 1/R0 1/R1 1/R4 1/R5 1/R2 1/R3 1/R6 1/R7 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%ymm1, %%ymm1, %%ymm1")         /* ymm1 = bi br */ \
            __ASM_EMIT("vshufps                 $0xb1, %%ymm5, %%ymm5, %%ymm5") \
            __ASM_EMIT("vunpckhps               %%ymm3, %%ymm3, %%ymm7")                /* ymm7 = 1/R4 1/R4 1/R5 1/R5 1/R6 1/R6 1/R7 1/R7 */ \
            __ASM_EMIT("vunpcklps               %%ymm3, %%ymm3, %%ymm3")                /* ymm3 = 1/R0 1/R0 1/R1 1/R1 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vfmsubadd231ps          %%ymm1, %%ymm2, %%ymm0")                /* ymm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vfmsubadd231ps          %%ymm5, %%ymm6, %%ymm4") \
            __ASM_EMIT("vmulps                  %%ymm3, %%ymm0, %%ymm0")                /* ymm0 = r' i' */ \
            __ASM_EMIT("vmulps                  %%ymm7, %%ymm4, %%ymm4") \
            __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups                 %%ymm4, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT("sub                     $8, %[count]") \
            __ASM_EMIT("add                     $0x40, %[off]") \
            __ASM_EMIT("4:") \
            /* x4 block */ \
            __ASM_EMIT("add                     $4, %[count]") \
            __ASM_EMIT("jl                      6f") \
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
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm5, %%xmm5, %%xmm5") \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT("vfmsubadd231ps          %%xmm1, %%xmm2, %%xmm0")                /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vfmsubadd231ps          %%xmm5, %%xmm6, %%xmm4") \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmulps                  %%xmm7, %%xmm4, %%xmm4") \
            __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("vmovups                 %%xmm4, 0x10(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x20, %[off]") \
            __ASM_EMIT("sub                     $4, %[count]") \
            __ASM_EMIT("6:") \
            /* x2 block */ \
            __ASM_EMIT("add                     $2, %[count]") \
            __ASM_EMIT("jl                      8f") \
            __ASM_EMIT("vmovups                 0x00(%[" T "], %[off]), %%xmm0")        /* xmm0 = tr ti */ \
            __ASM_EMIT("vmovups                 0x00(%[" B "], %[off]), %%xmm1")        /* xmm1 = br bi */ \
            __ASM_EMIT("vmovshdup               %%xmm0, %%xmm2")                        /* xmm2 = ti ti */ \
            __ASM_EMIT("vmovsldup               %%xmm0, %%xmm0")                        /* xmm0 = tr tr */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm1, %%xmm3")                /* xmm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vhaddps                 %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = R0 R1 */ \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT("vfmsubadd231ps          %%xmm1, %%xmm2, %%xmm0")                /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("add                     $0x10, %[off]") \
            __ASM_EMIT("sub                     $2, %[count]") \
            __ASM_EMIT("8:") \
            /* x1 block */ \
            __ASM_EMIT("add                     $1, %[count]") \
            __ASM_EMIT("jl                      10f") \
            __ASM_EMIT("vmovlps                 0x00(%[" T "], %[off]), %%xmm0, %%xmm0")/* xmm0 = tr ti */ \
            __ASM_EMIT("vmovlps                 0x00(%[" B "], %[off]), %%xmm1, %%xmm1")/* xmm1 = br bi */ \
            __ASM_EMIT("vmovshdup               %%xmm0, %%xmm2")                        /* xmm2 = ti ti */ \
            __ASM_EMIT("vmovsldup               %%xmm0, %%xmm0")                        /* xmm0 = tr tr */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm1, %%xmm3")                /* xmm3 = br*br bi*bi */ \
            __ASM_EMIT("vmulps                  %%xmm1, %%xmm0, %%xmm0")                /* xmm0 = tr*br tr*bi */ \
            __ASM_EMIT("vhaddps                 %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = R0 R1 */ \
            __ASM_EMIT("vmovaps                 0x00 + %[CC], %%xmm7")                  /* xmm7 = 1 */ \
            __ASM_EMIT("vdivps                  %%xmm3, %%xmm7, %%xmm3")                /* xmm3 = 1/R0 1/R1 1/R2 1/R3 */ \
            __ASM_EMIT("vshufps                 $0xb1, %%xmm1, %%xmm1, %%xmm1")         /* xmm1 = bi br */ \
            __ASM_EMIT("vunpckhps               %%xmm3, %%xmm3, %%xmm7")                /* xmm7 = 1/R2 1/R2 1/R3 1/R3 */ \
            __ASM_EMIT("vunpcklps               %%xmm3, %%xmm3, %%xmm3")                /* xmm3 = 1/R0 1/R0 1/R1 1/R1 */ \
            __ASM_EMIT("vfmsubadd231ps          %%xmm1, %%xmm2, %%xmm0")                /* xmm0 = r i = ti*bi+tr*br ti*br-tr*bi */ \
            __ASM_EMIT("vmulps                  %%xmm3, %%xmm0, %%xmm0")                /* xmm0 = r' i' */ \
            __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT("10:") \
            /* end */

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                PCOMPLEX_DIV_CORE("dst", "t", "b")
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

        static const uint16_t pcomplex_r2c_expand[] =
        {
            0x5555
        };

        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x64 blocks */
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 s1 0 s2 0 s3 0 s4 0 s5 0 s6 0 s7 0 */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x80(%[src], %[off]), %%zmm4 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xa0(%[src], %[off]), %%zmm5 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xc0(%[src], %[off]), %%zmm6 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xe0(%[src], %[off]), %%zmm7 %{%%k4%}%{z%}")
                __ASM_EMIT("vaddps                  0x000(%[dst], %[off], 2), %%zmm0, %%zmm0")  /* zmm0 = r0+s0 i0 r1+s1 i1 r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("vaddps                  0x040(%[dst], %[off], 2), %%zmm1, %%zmm1")
                __ASM_EMIT("vaddps                  0x080(%[dst], %[off], 2), %%zmm2, %%zmm2")
                __ASM_EMIT("vaddps                  0x0c0(%[dst], %[off], 2), %%zmm3, %%zmm3")
                __ASM_EMIT("vaddps                  0x100(%[dst], %[off], 2), %%zmm4, %%zmm4")
                __ASM_EMIT("vaddps                  0x140(%[dst], %[off], 2), %%zmm5, %%zmm5")
                __ASM_EMIT("vaddps                  0x180(%[dst], %[off], 2), %%zmm6, %%zmm6")
                __ASM_EMIT("vaddps                  0x1c0(%[dst], %[off], 2), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm4, 0x100(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm5, 0x140(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm6, 0x180(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm7, 0x1c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x100, %[off]")
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 blocks */
                __ASM_EMIT("add                     $48, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jl                      4f") \
                __ASM_EMIT("3:") \
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
                __ASM_EMIT("jge                     3b")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
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
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vaddps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0+s0 i0 r1+s1 i1 r2+s2 i2 r3+s3 i3 */
                __ASM_EMIT("vaddps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 */
                __ASM_EMIT("vaddss                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0 + s0 */
                __ASM_EMIT("vmovss                  %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x64 blocks */
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 ... */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x80(%[src], %[off]), %%zmm4 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xa0(%[src], %[off]), %%zmm5 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xc0(%[src], %[off]), %%zmm6 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xe0(%[src], %[off]), %%zmm7 %{%%k4%}%{z%}")
                __ASM_EMIT("vsubps                  0x000(%[dst], %[off], 2), %%zmm0, %%zmm0")  /* zmm0 = s0-r0 i0 ... */
                __ASM_EMIT("vsubps                  0x040(%[dst], %[off], 2), %%zmm1, %%zmm1")
                __ASM_EMIT("vsubps                  0x080(%[dst], %[off], 2), %%zmm2, %%zmm2")
                __ASM_EMIT("vsubps                  0x0c0(%[dst], %[off], 2), %%zmm3, %%zmm3")
                __ASM_EMIT("vsubps                  0x100(%[dst], %[off], 2), %%zmm4, %%zmm4")
                __ASM_EMIT("vsubps                  0x140(%[dst], %[off], 2), %%zmm5, %%zmm5")
                __ASM_EMIT("vsubps                  0x180(%[dst], %[off], 2), %%zmm6, %%zmm6")
                __ASM_EMIT("vsubps                  0x1c0(%[dst], %[off], 2), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm4, 0x100(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm5, 0x140(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm6, 0x180(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm7, 0x1c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x100, %[off]")
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 blocks */
                __ASM_EMIT("add                     $48, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jl                      4f") \
                __ASM_EMIT("3:") \
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
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%ymm0, %%ymm0")   /* ymm0 = s0-r0 i0 ... */
                __ASM_EMIT("vsubps                  0x20(%[dst], %[off], 2), %%ymm1, %%ymm1")
                __ASM_EMIT("vsubps                  0x40(%[dst], %[off], 2), %%ymm2, %%ymm2")
                __ASM_EMIT("vsubps                  0x60(%[dst], %[off], 2), %%ymm3, %%ymm3")
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jge                     3b")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = s0-r0 i0 ... */
                __ASM_EMIT("vsubps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vsubps                  0x20(%[dst], %[off], 2), %%xmm2, %%xmm2")
                __ASM_EMIT("vsubps                  0x30(%[dst], %[off], 2), %%xmm3, %%xmm3")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vsubps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = s0-r0 i0 ... */
                __ASM_EMIT("vsubps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")
                __ASM_EMIT("vsubps                  %%xmm4, %%xmm0, %%xmm0")                    /* xmm0 = s0-r0 -i0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x32 blocks */
                __ASM_EMIT("sub                     $32, %[count]")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 ... */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovups                 0x00(%[dst], %[off], 2), %%zmm4")           /* zmm4 = r0 i0 ... */
                __ASM_EMIT("vmovups                 0x40(%[dst], %[off], 2), %%zmm5")
                __ASM_EMIT("vmovups                 0x80(%[dst], %[off], 2), %%zmm6")
                __ASM_EMIT("vmovups                 0xc0(%[dst], %[off], 2), %%zmm7")
                __ASM_EMIT("vsubps                  %%zmm0, %%zmm4, %%zmm0")                    /* zmm0 = r0-s0 i0 ... */
                __ASM_EMIT("vsubps                  %%zmm1, %%zmm5, %%zmm1")
                __ASM_EMIT("vsubps                  %%zmm2, %%zmm6, %%zmm2")
                __ASM_EMIT("vsubps                  %%zmm3, %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x80, %[off]")
                __ASM_EMIT("sub                     $32, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 block */
                __ASM_EMIT("add                     $16, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jl                      4f")
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
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
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
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
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
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")   /* xmm4 = r0 i0 */
                __ASM_EMIT("vsubps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = s0-r0 -i0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x64 blocks */
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 ... */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x80(%[src], %[off]), %%zmm4 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xa0(%[src], %[off]), %%zmm5 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xc0(%[src], %[off]), %%zmm6 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xe0(%[src], %[off]), %%zmm7 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovsldup               %%zmm0, %%zmm0")                            /* zmm0 = s0 s0 ... */
                __ASM_EMIT("vmovsldup               %%zmm1, %%zmm1")
                __ASM_EMIT("vmovsldup               %%zmm2, %%zmm2")
                __ASM_EMIT("vmovsldup               %%zmm3, %%zmm3")
                __ASM_EMIT("vmovsldup               %%zmm4, %%zmm4")
                __ASM_EMIT("vmovsldup               %%zmm5, %%zmm5")
                __ASM_EMIT("vmovsldup               %%zmm6, %%zmm6")
                __ASM_EMIT("vmovsldup               %%zmm7, %%zmm7")
                __ASM_EMIT("vmulps                  0x000(%[dst], %[off], 2), %%zmm0, %%zmm0")   /* zmm4 = s0*r0 s0*i0 ... */
                __ASM_EMIT("vmulps                  0x040(%[dst], %[off], 2), %%zmm1, %%zmm1")
                __ASM_EMIT("vmulps                  0x080(%[dst], %[off], 2), %%zmm2, %%zmm2")
                __ASM_EMIT("vmulps                  0x0c0(%[dst], %[off], 2), %%zmm3, %%zmm3")
                __ASM_EMIT("vmulps                  0x100(%[dst], %[off], 2), %%zmm4, %%zmm4")
                __ASM_EMIT("vmulps                  0x140(%[dst], %[off], 2), %%zmm5, %%zmm5")
                __ASM_EMIT("vmulps                  0x180(%[dst], %[off], 2), %%zmm6, %%zmm6")
                __ASM_EMIT("vmulps                  0x1c0(%[dst], %[off], 2), %%zmm7, %%zmm7")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm4, 0x100(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm5, 0x140(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm6, 0x180(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm7, 0x1c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x100, %[off]")
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 block */
                __ASM_EMIT("add                     $48, %[count]")
                __ASM_EMIT("jl                      4f")
                __ASM_EMIT("3:")
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
                __ASM_EMIT("jge                     3b")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
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
                __ASM_EMIT("6:")
                /* x4 block */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm0, %%xmm0, %%xmm1")                    /* xmm1 = s2 s2 s3 s3 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 s1 s1 */
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm4 = r0*s0 i0*s0 r1*s1 i1*s1 */
                __ASM_EMIT("vmulps                  0x10(%[dst], %[off], 2), %%xmm1, %%xmm1")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 */
                __ASM_EMIT("vmulps                  0x00(%[dst], %[off], 2), %%xmm0, %%xmm0")   /* xmm0 = r0*s0 i0*s0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_r2c_div2(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x32 blocks */
                __ASM_EMIT("sub                     $32, %[count]")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("jb                      2f") \
                __ASM_EMIT("1:") \
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 ... */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovups                 0x000(%[dst], %[off], 2), %%zmm4")          /* zmm4 = r0 i0 ... */
                __ASM_EMIT("vmovups                 0x040(%[dst], %[off], 2), %%zmm5")
                __ASM_EMIT("vmovups                 0x080(%[dst], %[off], 2), %%zmm6")
                __ASM_EMIT("vmovups                 0x0c0(%[dst], %[off], 2), %%zmm7")
                __ASM_EMIT("vmovsldup               %%zmm0, %%zmm0")                            /* zmm0 = s0 s0 ... */
                __ASM_EMIT("vmovsldup               %%zmm1, %%zmm1")
                __ASM_EMIT("vmovsldup               %%zmm2, %%zmm2")
                __ASM_EMIT("vmovsldup               %%zmm3, %%zmm3")
                __ASM_EMIT("vdivps                  %%zmm0, %%zmm4, %%zmm0")                    /* ymm0 = r0/s0 i0/s0 r1/s1 i1/s1 r2/s2 i2/s2 r3/s3 i3/s3 */
                __ASM_EMIT("vdivps                  %%zmm1, %%zmm5, %%zmm1")
                __ASM_EMIT("vdivps                  %%zmm2, %%zmm6, %%zmm2")
                __ASM_EMIT("vdivps                  %%zmm3, %%zmm7, %%zmm3")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x80, %[off]")
                __ASM_EMIT("sub                     $32, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 block */
                __ASM_EMIT("add                     $16, %[count]")
                __ASM_EMIT("jl                      4f") \
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
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
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
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
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
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 0 */
                __ASM_EMIT("vmovlps                 0x00(%[dst], %[off], 2), %%xmm4, %%xmm4")   /* xmm4 = r0 i0 */
                __ASM_EMIT("vunpcklps               %%xmm0, %%xmm0, %%xmm0")                    /* xmm0 = s0 s0 */
                __ASM_EMIT("vdivps                  %%xmm0, %%xmm4, %%xmm0")                    /* xmm0 = r0/s0 i0/s0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x64 blocks */
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("1:")
                __ASM_EMIT("vexpandps               0x00(%[src], %[off]), %%zmm0 %{%%k4%}%{z%}")/* zmm0 = s0 0 s1 0 s2 0 s3 0 s4 0 s5 0 s6 0 s7 0 */
                __ASM_EMIT("vexpandps               0x20(%[src], %[off]), %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x40(%[src], %[off]), %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x60(%[src], %[off]), %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0x80(%[src], %[off]), %%zmm4 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xa0(%[src], %[off]), %%zmm5 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xc0(%[src], %[off]), %%zmm6 %{%%k4%}%{z%}")
                __ASM_EMIT("vexpandps               0xe0(%[src], %[off]), %%zmm7 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovups                 %%zmm0, 0x000(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm1, 0x040(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm2, 0x080(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm3, 0x0c0(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm4, 0x100(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm5, 0x140(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm6, 0x180(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%zmm7, 0x1c0(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x100, %[off]")
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 blocks */
                __ASM_EMIT("add                     $48, %[count]")
                __ASM_EMIT("vxorps                  %%ymm6, %%ymm6, %%ymm6")                    /* ymm6 = 0 */
                __ASM_EMIT("vxorps                  %%ymm7, %%ymm7, %%ymm7")                    /* ymm7 = 0 */
                __ASM_EMIT("jl                      4f") \
                __ASM_EMIT("3:") \
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
                __ASM_EMIT("vmovups                 %%ymm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm1, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm2, 0x40(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%ymm3, 0x60(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jge                     3b")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off]), %%xmm2")
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm2, %%xmm3")
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm2, 0x20(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm3, 0x30(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vunpckhps               %%xmm6, %%xmm0, %%xmm1")                    /* xmm1 = s2 0 s3 0 */
                __ASM_EMIT("vunpcklps               %%xmm6, %%xmm0, %%xmm0")                    /* xmm0 = s0 0 s1 0 */
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("vmovups                 %%xmm1, 0x10(%[dst], %[off], 2)")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off]), %%xmm0")              /* xmm0 = s0 */
                __ASM_EMIT("vmovlps                 %%xmm0, 0x00(%[dst], %[off], 2)")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        void pcomplex_c2r(float *dst, const float *src, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                __ASM_EMIT("xor                     %[off], %[off]")
                /* x64 blocks */
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("kmovw                   %[CC], %%k4")                               /* k4 = 0x5555 */
                __ASM_EMIT("jb                      2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups                 0x000(%[src], %[off], 2), %%zmm0")          /* zmm0 = s0 i0 s1 i0 s2 i2 s3 i3 s4 i4 s5 i5 s6 i6 s7 i7 */
                __ASM_EMIT("vmovups                 0x040(%[src], %[off], 2), %%zmm1")
                __ASM_EMIT("vmovups                 0x080(%[src], %[off], 2), %%zmm2")
                __ASM_EMIT("vmovups                 0x0c0(%[src], %[off], 2), %%zmm3")
                __ASM_EMIT("vmovups                 0x100(%[src], %[off], 2), %%zmm4")
                __ASM_EMIT("vmovups                 0x140(%[src], %[off], 2), %%zmm5")
                __ASM_EMIT("vmovups                 0x180(%[src], %[off], 2), %%zmm6")
                __ASM_EMIT("vmovups                 0x1c0(%[src], %[off], 2), %%zmm7")
                __ASM_EMIT("vcompressps             %%zmm0, %%zmm0 %{%%k4%}%{z%}")              /* zmm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vcompressps             %%zmm1, %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm2, %%zmm2 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm3, %%zmm3 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm4, %%zmm4 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm5, %%zmm5 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm6, %%zmm6 %{%%k4%}%{z%}")
                __ASM_EMIT("vcompressps             %%zmm7, %%zmm7 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovups                 %%ymm0, 0x000(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm1, 0x020(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm2, 0x040(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm3, 0x060(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm4, 0x080(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm5, 0x0a0(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm6, 0x0c0(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm7, 0x0e0(%[dst], %[off])")
                __ASM_EMIT("add                     $0x100, %[off]")
                __ASM_EMIT("sub                     $64, %[count]")
                __ASM_EMIT("jae                     1b")
                __ASM_EMIT("2:")
                /* x16 blocks */
                __ASM_EMIT("add                     $48, %[count]")
                __ASM_EMIT("jl                      4f") \
                __ASM_EMIT("3:") \
                __ASM_EMIT("vmovups                 0x000(%[src], %[off], 2), %%zmm0")          /* zmm0 = s0 i0 s1 i1 s2 i2 s3 i3 s4 i4 s5 i5 s6 i6 s7 i7 */
                __ASM_EMIT("vmovups                 0x040(%[src], %[off], 2), %%zmm1")
                __ASM_EMIT("vcompressps             %%zmm0, %%zmm0 %{%%k4%}%{z%}")              /* zmm0 = s0 s1 s2 s3 s4 s5 s6 s7 */
                __ASM_EMIT("vcompressps             %%zmm1, %%zmm1 %{%%k4%}%{z%}")
                __ASM_EMIT("vmovups                 %%ymm0, 0x000(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%ymm1, 0x020(%[dst], %[off])")
                __ASM_EMIT("add                     $0x40, %[off]")
                __ASM_EMIT("sub                     $16, %[count]")
                __ASM_EMIT("jge                     3b")
                __ASM_EMIT("4:")
                /* x8 block */
                __ASM_EMIT("add                     $8, %[count]")
                __ASM_EMIT("jl                      6f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off], 2), %%xmm0")           /* xmm0 = s0 i0 s1 i1 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off], 2), %%xmm1")           /* xmm1 = s2 i2 s3 i3 */
                __ASM_EMIT("vmovups                 0x20(%[src], %[off], 2), %%xmm2")
                __ASM_EMIT("vmovups                 0x30(%[src], %[off], 2), %%xmm3")
                __ASM_EMIT("vshufps                 $0x88, %%xmm1, %%xmm0, %%xmm0")             /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vshufps                 $0x88, %%xmm3, %%xmm2, %%xmm2")
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("vmovups                 %%xmm2, 0x10(%[dst], %[off])")
                __ASM_EMIT("sub                     $8, %[count]")
                __ASM_EMIT("add                     $0x20, %[off]")
                __ASM_EMIT("6:")
                /* x4 blocks */
                __ASM_EMIT("add                     $4, %[count]")
                __ASM_EMIT("jl                      8f")
                __ASM_EMIT("vmovups                 0x00(%[src], %[off], 2), %%xmm0")           /* xmm0 = s0 i0 s1 i1 */
                __ASM_EMIT("vmovups                 0x10(%[src], %[off], 2), %%xmm1")           /* xmm1 = s2 i2 s3 i3 */
                __ASM_EMIT("vshufps                 $0x88, %%xmm1, %%xmm0, %%xmm0")             /* xmm0 = s0 s1 s2 s3 */
                __ASM_EMIT("vmovups                 %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("sub                     $4, %[count]")
                __ASM_EMIT("add                     $0x10, %[off]")
                __ASM_EMIT("8:")
                /* x1 blocks */
                __ASM_EMIT("add                     $3, %[count]")
                __ASM_EMIT("jl                      10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss                  0x00(%[src], %[off], 2), %%xmm0")           /* xmm0 = s0 */
                __ASM_EMIT("vmovss                  %%xmm0, 0x00(%[dst], %[off])")
                __ASM_EMIT("add                     $0x04, %[off]")
                __ASM_EMIT("dec                     %[count]")
                __ASM_EMIT("jge                     9b")
                __ASM_EMIT("10:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src] "r" (src),
                  [CC] "o" (pcomplex_r2c_expand)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );
        }

        IF_ARCH_X86(
            static const float pcomplex_corr_const[] __lsp_aligned64 =
            {
                LSP_DSP_VEC16(1e-36f),
            };
        );

        IF_ARCH_X86(
            static const uint32_t pcomplex_corr_perm_const[] __lsp_aligned64 =
            {
                0, 2, 4, 6,  8, 10, 12, 14,  16, 18, 20, 22,  24, 26, 28, 30,
                1, 3, 5, 7,  9, 11, 13, 15,  17, 19, 21, 23,  25, 27, 29, 31,
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
                /* x16 blocks */
                __ASM_EMIT("sub             $16, %[count]")                         /* count -= 16 */
                __ASM_EMIT("vmovaps         %[CC], %%zmm7")                         /* zmm7  = threshold */
                __ASM_EMIT64("vmovaps       0x00 + %[PP], %%zmm8")                  /* zmm8  = even indices */
                __ASM_EMIT64("vmovaps       0x40 + %[PP], %%zmm9")                  /* zmm9  = odd indices */
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%zmm2")      /* zmm2  = a0 b0 a1 b1 a2 b2 a3 b3 a4 b4 a5 b5 a6 b6 a7 b7 */
                __ASM_EMIT("vmovups         0x40(%[src1], %[off], 2), %%zmm3")      /* zmm3  = a8 b8 a9 b9 a10 b10 a11 b11 a12 b12 a13 b13 a14 b14 a15 b15 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%zmm4")      /* zmm4  = c0 d0 c1 d1 c2 d2 c3 d3 c4 d4 c5 d5 c6 d6 c7 d7 */
                __ASM_EMIT("vmovups         0x40(%[src2], %[off], 2), %%zmm5")      /* zmm5  = c8 d8 c9 d9 c10 d10 c11 d11 c12 d12 c13 d13 c14 d14 c15 d15 */
                __ASM_EMIT32("vmovaps       0x00 + %[PP], %%zmm0")                  /* zmm0  = even indices */
                __ASM_EMIT32("vmovaps       0x40 + %[PP], %%zmm1")                  /* zmm1  = odd indices */
                __ASM_EMIT64("vmovaps       %%zmm8, %%zmm0")                        /* zmm0  = even indices */
                __ASM_EMIT64("vmovaps       %%zmm9, %%zmm1")                        /* zmm1  = odd indices */
                __ASM_EMIT("vpermi2ps       %%zmm3, %%zmm2, %%zmm0")                /* zmm0  = a = a0 a1 a2 a3 a4 a5 a6 a7 a8 a9 a10 a11 a12 a13 a14 a15 */
                __ASM_EMIT("vpermi2ps       %%zmm3, %%zmm2, %%zmm1")                /* zmm1  = b = b0 b1 b2 b3 b4 b5 b6 b7 b8 b9 b10 b11 b12 b13 b14 b15 */
                __ASM_EMIT32("vmovaps       0x00 + %[PP], %%zmm2")                  /* zmm2  = even indices */
                __ASM_EMIT32("vmovaps       0x40 + %[PP], %%zmm3")                  /* zmm3  = odd indices */
                __ASM_EMIT64("vmovaps       %%zmm8, %%zmm2")                        /* zmm2  = even indices */
                __ASM_EMIT64("vmovaps       %%zmm9, %%zmm3")                        /* zmm3  = odd indices */
                __ASM_EMIT("vpermi2ps       %%zmm5, %%zmm4, %%zmm2")                /* zmm2  = c = c0 c1 c2 c3 c4 c5 c6 c7 c8 c9 c10 c11 c12 c13 c14 c15 */
                __ASM_EMIT("vpermi2ps       %%zmm5, %%zmm4, %%zmm3")                /* zmm3  = d = d0 d1 d2 d3 d4 d5 d6 d7 d8 d9 d10 d11 d12 d13 d14 d15 */
                /* zmm0 = a, zmm1 = b, zmm2 = c, zmm3 = d */
                __ASM_EMIT("vmulps          %%zmm0, %%zmm2, %%zmm4")                /* zmm4  = a*c */
                __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm0")                /* zmm0  = a*a */
                __ASM_EMIT("vmulps          %%zmm2, %%zmm2, %%zmm2")                /* zmm2  = c*c */
                __ASM_EMIT("vfmadd231ps     %%zmm1, %%zmm3, %%zmm4")                /* zmm4  = nom = a*c + b*d */
                __ASM_EMIT("vfmadd231ps     %%zmm1, %%zmm1, %%zmm0")                /* zmm0  = a*a + b*b */
                __ASM_EMIT("vfmadd231ps     %%zmm3, %%zmm3, %%zmm2")                /* zmm2  = c*c + d*d */
                __ASM_EMIT("vmulps          %%zmm2, %%zmm0, %%zmm0")                /* zmm0  = den = (a*a + b*b)*(c*c + d*d) */
                __ASM_EMIT("vsqrtps         %%zmm0, %%zmm1")                        /* zmm1  = sqrt(den) */
                __ASM_EMIT("vdivps          %%zmm1, %%zmm4, %%zmm4")                /* zmm4  = nom / sqrt(den) */
                __ASM_EMIT("vcmpps          $5, %%zmm7, %%zmm0, %%k1")              /* k1    = [den >= threshold] */
                __ASM_EMIT("vmovaps         %%zmm4, %%zmm0 %{%%k1%}%{z%}")          /* zmm0  = (den >= threshold) ? nom / sqrt(den) : 0.0f */
                __ASM_EMIT("vmovups         %%zmm0, 0x00(%[dst], %[off])")          /* dst[0]= v0 v1 v2 v3 v4 v5 v6 v7 v8 v9 v10 v11 v12 v13 v14 v15 */
                __ASM_EMIT("add             $0x40, %[off]")
                __ASM_EMIT("sub             $16, %[count]")                         /* count -= 16 */
                __ASM_EMIT("jae             1b")
                /* x8 blocks */
                __ASM_EMIT("2:")
                __ASM_EMIT("add             $8, %[count]")                          /* count += 8 */
                __ASM_EMIT("jl              4f")
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
                __ASM_EMIT("4:")
                __ASM_EMIT("add             $4, %[count]")                          /* count += 4 */
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[src1], %[off], 2), %%xmm0")      /* xmm0  = a0 b0 a1 b1 */
                __ASM_EMIT("vmovups         0x10(%[src1], %[off], 2), %%xmm4")      /* xmm4  = a2 b2 a3 b3 */
                __ASM_EMIT("vmovups         0x00(%[src2], %[off], 2), %%xmm2")      /* xmm2  = c0 d0 c1 d1 */
                __ASM_EMIT("vmovups         0x10(%[src2], %[off], 2), %%xmm6")      /* xmm6  = c2 d2 c3 d3 */
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
                __ASM_EMIT("sub             $4, %[count]")                          /* count -= 8 */
                /* x1 blocks */
                __ASM_EMIT("6:")
                __ASM_EMIT("add             $3, %[count]")                          /* count += 3 */
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("7:")
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
                __ASM_EMIT("jge             7b")
                /* End */
                __ASM_EMIT("8:")

                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst_corr), [src1] "r" (src1), [src2] "r" (src2),
                  [CC] "m" (pcomplex_corr_const),
                  [PP] "m" (pcomplex_corr_perm_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  __IF_64( "xmm8", "xmm9", )
                  "%k1"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PCOMPLEX_H_ */
