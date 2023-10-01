/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_PCOMPLEX_H_ */
