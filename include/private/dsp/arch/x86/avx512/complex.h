/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_COMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_COMPLEX_H_

namespace lsp
{
    namespace avx512
    {
        #define COMPLEX_MUL3_CORE(DST, SRC1, SRC2) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x16 blocks */ \
            __ASM_EMIT32("subl          $16, %[count]") \
            __ASM_EMIT64("sub           $16, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%zmm4")                   /* zmm4 = ar */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%zmm5")                   /* zmm5 = ai */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_re], %[off]), %%zmm4")              /* zmm4 = ar */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_im], %[off]), %%zmm5")              /* zmm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_re], %[off]), %%zmm0")              /* zmm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_im], %[off]), %%zmm1")              /* zmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%zmm0, %%zmm5, %%zmm2")                            /* zmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%zmm1, %%zmm5, %%zmm3")                            /* zmm3 = ai*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  ("vfmsub132ps   %%zmm4, %%zmm3, %%zmm0")                            /* ymm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  ("vfmadd132ps   %%zmm4, %%zmm2, %%zmm1")                            /* ymm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovups       %%zmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%zmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%zmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovups       %%zmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x40, %[off]") \
            __ASM_EMIT32("subl          $16, %[count]") \
            __ASM_EMIT64("sub           $16, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* x8 blocks */ \
            __ASM_EMIT32("addl          $8, %[count]") \
            __ASM_EMIT64("add           $8, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%ymm4")                   /* ymm4 = ar */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%ymm5")                   /* ymm5 = ai */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_re], %[off]), %%ymm4")              /* ymm4 = ar */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_im], %[off]), %%ymm5")              /* ymm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_re], %[off]), %%ymm0")              /* ymm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_im], %[off]), %%ymm1")              /* ymm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm5, %%ymm2")                            /* ymm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%ymm1, %%ymm5, %%ymm3")                            /* ymm3 = ai*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  ("vfmsub132ps   %%ymm4, %%ymm3, %%ymm0")                            /* ymm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  ("vfmadd132ps   %%ymm4, %%ymm2, %%ymm1")                            /* ymm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovups       %%ymm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%ymm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("4:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%xmm4")                   /* xmm4 = ar */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%xmm5")                   /* xmm5 = ai */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_re], %[off]), %%xmm4")              /* xmm4 = ar */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_im], %[off]), %%xmm5")              /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_re], %[off]), %%xmm0")              /* xmm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_im], %[off]), %%xmm1")              /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  ("vfmsub132ps   %%xmm4, %%xmm3, %%xmm0")                            /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  ("vfmadd132ps   %%xmm4, %%xmm2, %%xmm1")                            /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovups       %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("6:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            8f") \
            __ASM_EMIT  ("7:") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT32("vmovss        0x00(%[ptr_re], %[off]), %%xmm4")                   /* xmm4 = ar */ \
            __ASM_EMIT32("vmovss        0x00(%[ptr_im], %[off]), %%xmm5")                   /* xmm5 = ai */ \
            __ASM_EMIT64("vmovss        0x00(%[" SRC1 "_re], %[off]), %%xmm4")              /* xmm4 = ar */ \
            __ASM_EMIT64("vmovss        0x00(%[" SRC1 "_im], %[off]), %%xmm5")              /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC2 "_re], %[off]), %%xmm0")              /* xmm0 = br */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC2 "_im], %[off]), %%xmm1")              /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  ("vfmsub132ss   %%xmm4, %%xmm3, %%xmm0")                            /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  ("vfmadd132ss   %%xmm4, %%xmm2, %%xmm1")                            /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovss        %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovss        %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           7b") \
            __ASM_EMIT  ("8:")

        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            IF_ARCH_I386(float *ptr_re, *ptr_im);
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MUL3_CORE("dst", "src1", "src2")
                : __IF_32(
                    [ptr_re] "=&r" (ptr_re), [ptr_im] "=&r" (ptr_im),
                  )
                  [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] X86_GREG (dst_re), [dst_im] X86_GREG (dst_im),
                  [src1_re] X86_GREG (src1_re), [src1_im] X86_GREG (src1_im),
                  [src2_re] "r" (src2_re), [src2_im] "r" (src2_im)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

    #define COMPLEX_MUL2_CORE(DST, SRC, SEL) \
        __ASM_EMIT  ("xor           %[off], %[off]") \
        /* x16 blocks */ \
        __ASM_EMIT32("subl          $16, %[count]") \
        __ASM_EMIT64("sub           $16, %[count]") \
        __ASM_EMIT  ("jb            2f") \
        __ASM_EMIT  ("1:") \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_re], %[off]), %%zmm4")               /* zmm4 = ar */ \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_im], %[off]), %%zmm5")               /* zmm5 = ai */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%zmm0")               /* zmm0 = br */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%zmm1")               /* zmm1 = bi */ \
        __ASM_EMIT  ("vmulps        %%zmm0, %%zmm5, %%zmm2")                            /* ymm2 = ai*br */ \
        __ASM_EMIT  ("vmulps        %%zmm1, %%zmm5, %%zmm3")                            /* ymm3 = ai*bi */ \
        __ASM_EMIT  ("vfmsub132ps   %%zmm4, %%zmm3, %%zmm0")                            /* ymm0 = ar*br - ai*bi = r */ \
        __ASM_EMIT  ("vfmadd132ps   %%zmm4, %%zmm2, %%zmm1")                            /* ymm1 = ar*bi + ai*br = i */ \
        __ASM_EMIT  ("vmovups       %%zmm0, 0x00(%[" DST "_re], %[off])") \
        __ASM_EMIT  ("vmovups       %%zmm1, 0x00(%[" DST "_im], %[off])") \
        __ASM_EMIT  ("add           $0x40, %[off]") \
        __ASM_EMIT32("subl          $16, %[count]") \
        __ASM_EMIT64("sub           $16, %[count]") \
        __ASM_EMIT  ("jae           1b") \
        __ASM_EMIT  ("2:") \
        /* x8 blocks */ \
        __ASM_EMIT32("addl          $8, %[count]") \
        __ASM_EMIT64("add           $8, %[count]") \
        __ASM_EMIT  ("jl            4f") \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_re], %[off]), %%ymm4")               /* ymm4 = ar */ \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_im], %[off]), %%ymm5")               /* ymm5 = ai */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = br */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm1")               /* ymm1 = bi */ \
        __ASM_EMIT  ("vmulps        %%ymm0, %%ymm5, %%ymm2")                            /* ymm2 = ai*br */ \
        __ASM_EMIT  ("vmulps        %%ymm1, %%ymm5, %%ymm3")                            /* ymm3 = ai*bi */ \
        __ASM_EMIT  ("vfmsub132ps   %%ymm4, %%ymm3, %%ymm0")                            /* ymm0 = ar*br - ai*bi = r */ \
        __ASM_EMIT  ("vfmadd132ps   %%ymm4, %%ymm2, %%ymm1")                            /* ymm1 = ar*bi + ai*br = i */ \
        __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
        __ASM_EMIT  ("vmovups       %%ymm1, 0x00(%[" DST "_im], %[off])") \
        __ASM_EMIT  ("add           $0x20, %[off]") \
        __ASM_EMIT32("subl          $8, %[count]") \
        __ASM_EMIT64("sub           $8, %[count]") \
        __ASM_EMIT  ("jae           1b") \
        __ASM_EMIT  ("4:") \
        /* 4x block */ \
        __ASM_EMIT32("addl          $4, %[count]") \
        __ASM_EMIT64("add           $4, %[count]") \
        __ASM_EMIT  ("jl            6f") \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_re], %[off]), %%xmm4")               /* xmm4 = ar */ \
        __ASM_EMIT  ("vmovups       0x00(%[" DST "_im], %[off]), %%xmm5")               /* xmm5 = ai */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = br */ \
        __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%xmm1")               /* xmm1 = bi */ \
        __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
        __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
        __ASM_EMIT  ("vfmsub132ps   %%xmm4, %%xmm3, %%xmm0")                            /* xmm0 = ar*br - ai*bi = r */ \
        __ASM_EMIT  ("vfmadd132ps   %%xmm4, %%xmm2, %%xmm1")                            /* xmm1 = ar*bi + ai*br = i */ \
        __ASM_EMIT  ("vmovups       %%xmm0, 0x00(%[" DST "_re], %[off])") \
        __ASM_EMIT  ("vmovups       %%xmm1, 0x00(%[" DST "_im], %[off])") \
        __ASM_EMIT  ("add           $0x10, %[off]") \
        __ASM_EMIT32("subl          $4, %[count]") \
        __ASM_EMIT64("sub           $4, %[count]") \
        __ASM_EMIT  ("6:") \
        /* 1x blocks */ \
        __ASM_EMIT32("addl          $3, %[count]") \
        __ASM_EMIT64("add           $3, %[count]") \
        __ASM_EMIT  ("jl            8f") \
        __ASM_EMIT  ("7:") \
        __ASM_EMIT  ("vmovss        0x00(%[" DST "_re], %[off]), %%xmm4")               /* xmm4 = ar */ \
        __ASM_EMIT  ("vmovss        0x00(%[" DST "_im], %[off]), %%xmm5")               /* xmm5 = ai */ \
        __ASM_EMIT  ("vmovss        0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = br */ \
        __ASM_EMIT  ("vmovss        0x00(%[" SRC "_im], %[off]), %%xmm1")               /* xmm1 = bi */ \
        __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
        __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
        __ASM_EMIT  ("vfmsub132ss   %%xmm4, %%xmm3, %%xmm0")                            /* xmm0 = ar*br - ai*bi = r */ \
        __ASM_EMIT  ("vfmadd132ss   %%xmm4, %%xmm2, %%xmm1")                            /* xmm1 = ar*bi + ai*br = i */ \
        __ASM_EMIT  ("vmovss        %%xmm0, 0x00(%[" DST "_re], %[off])") \
        __ASM_EMIT  ("vmovss        %%xmm1, 0x00(%[" DST "_im], %[off])") \
        __ASM_EMIT  ("add           $0x04, %[off]") \
        __ASM_EMIT32("decl          %[count]") \
        __ASM_EMIT64("dec           %[count]") \
        __ASM_EMIT  ("jge           7b") \
        __ASM_EMIT  ("8:")

        void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MUL2_CORE("dst", "src", FMA_OFF)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_COMPLEX_H_ */
