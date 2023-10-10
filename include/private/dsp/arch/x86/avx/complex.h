/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_COMPLEX_H_
#define PRIVATE_DSP_ARCH_X86_AVX_COMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        #define FMA_OFF(a, b)       a
        #define FMA_ON(a, b)        b
        #define FMA_PASS(x)         __ASM_EMIT(x)
        #define FMA_SKIP(x)

        IF_ARCH_X86(
            static uint32_t complex_div_const[] __lsp_aligned32 =
            {
                LSP_DSP_VEC8(0x3f800000),   // 1.0
                LSP_DSP_VEC8(0x80000000)    // signbit
            };
        )

        #define COMPLEX_MUL3_CORE(DST, SRC1, SRC2, SEL) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x8 blocks */ \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_re], %[off]), %%ymm4")              /* ymm4 = ar */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_im], %[off]), %%ymm5")              /* ymm5 = ai */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%ymm4")                   /* ymm4 = ar */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%ymm5")                   /* ymm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_re], %[off]), %%ymm0")              /* ymm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_im], %[off]), %%ymm1")              /* ymm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm5, %%ymm2")                            /* ymm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%ymm1, %%ymm5, %%ymm3")                            /* ymm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm0, %%ymm4, %%ymm0", ""))                   /* ymm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm1, %%ymm4, %%ymm1", ""))                   /* ymm1 = ar*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  (SEL("vsubps    %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps %%ymm4, %%ymm3, %%ymm0")) /* ymm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddps    %%ymm2, %%ymm1, %%ymm1", "vfmadd132ps %%ymm4, %%ymm2, %%ymm1")) /* ymm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovups       %%ymm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%ymm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_re], %[off]), %%xmm4")              /* xmm4 = ar */ \
            __ASM_EMIT64("vmovups       0x00(%[" SRC1 "_im], %[off]), %%xmm5")              /* xmm5 = ai */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%xmm4")                   /* xmm4 = ar */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%xmm5")                   /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_re], %[off]), %%xmm0")              /* xmm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC2 "_im], %[off]), %%xmm1")              /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulps    %%xmm0, %%xmm4, %%xmm0", ""))                   /* xmm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulps    %%xmm1, %%xmm4, %%xmm1", ""))                   /* xmm1 = ar*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  (SEL("vsubps    %%xmm3, %%xmm0, %%xmm0", "vfmsub132ps %%xmm4, %%xmm3, %%xmm0")) /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddps    %%xmm2, %%xmm1, %%xmm1", "vfmadd132ps %%xmm4, %%xmm2, %%xmm1")) /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovups       %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("4:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("5:") \
            __ASM_EMIT32("mov           %[" SRC1 "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" SRC1 "_im], %[ptr_im]") \
            __ASM_EMIT64("vmovss        0x00(%[" SRC1 "_re], %[off]), %%xmm4")              /* xmm4 = ar */ \
            __ASM_EMIT64("vmovss        0x00(%[" SRC1 "_im], %[off]), %%xmm5")              /* xmm5 = ai */ \
            __ASM_EMIT32("vmovss        0x00(%[ptr_re], %[off]), %%xmm4")                   /* xmm4 = ar */ \
            __ASM_EMIT32("vmovss        0x00(%[ptr_im], %[off]), %%xmm5")                   /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC2 "_re], %[off]), %%xmm0")              /* xmm0 = br */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC2 "_im], %[off]), %%xmm1")              /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulss    %%xmm0, %%xmm4, %%xmm0", ""))                   /* xmm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulss    %%xmm1, %%xmm4, %%xmm1", ""))                   /* xmm1 = ar*bi */ \
            __ASM_EMIT32("mov           %[" DST "_re], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DST "_im], %[ptr_im]") \
            __ASM_EMIT  (SEL("vsubss    %%xmm3, %%xmm0, %%xmm0", "vfmsub132ss %%xmm4, %%xmm3, %%xmm0")) /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddss    %%xmm2, %%xmm1, %%xmm1", "vfmadd132ss %%xmm4, %%xmm2, %%xmm1")) /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT32("vmovss        %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovss        %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           5b") \
            __ASM_EMIT  ("6:")

        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            IF_ARCH_I386(float *ptr_re, *ptr_im);
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MUL3_CORE("dst", "src1", "src2", FMA_OFF)
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

        void complex_mul3_fma3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            IF_ARCH_I386(float *ptr_re, *ptr_im);
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MUL3_CORE("dst", "src1", "src2", FMA_ON)
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

        #undef COMPLEX_MUL3_CORE

        #define COMPLEX_MUL2_CORE(DST, SRC, SEL) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x8 blocks */ \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT  ("vmovups       0x00(%[" DST "_re], %[off]), %%ymm4")               /* ymm4 = ar */ \
            __ASM_EMIT  ("vmovups       0x00(%[" DST "_im], %[off]), %%ymm5")               /* ymm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm1")               /* ymm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm5, %%ymm2")                            /* ymm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%ymm1, %%ymm5, %%ymm3")                            /* ymm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm0, %%ymm4, %%ymm0", ""))                   /* ymm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm1, %%ymm4, %%ymm1", ""))                   /* ymm1 = ar*bi */ \
            __ASM_EMIT  (SEL("vsubps    %%ymm3, %%ymm0, %%ymm0", "vfmsub132ps %%ymm4, %%ymm3, %%ymm0")) /* ymm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddps    %%ymm2, %%ymm1, %%ymm1", "vfmadd132ps %%ymm4, %%ymm2, %%ymm1")) /* ymm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT  ("vmovups       0x00(%[" DST "_re], %[off]), %%xmm4")               /* xmm4 = ar */ \
            __ASM_EMIT  ("vmovups       0x00(%[" DST "_im], %[off]), %%xmm5")               /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%xmm1")               /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulps    %%xmm0, %%xmm4, %%xmm0", ""))                   /* xmm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulps    %%xmm1, %%xmm4, %%xmm1", ""))                   /* xmm1 = ar*bi */ \
            __ASM_EMIT  (SEL("vsubps    %%xmm3, %%xmm0, %%xmm0", "vfmsub132ps %%xmm4, %%xmm3, %%xmm0")) /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddps    %%xmm2, %%xmm1, %%xmm1", "vfmadd132ps %%xmm4, %%xmm2, %%xmm1")) /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT  ("vmovups       %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("4:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("5:") \
            __ASM_EMIT  ("vmovss        0x00(%[" DST "_re], %[off]), %%xmm4")               /* xmm4 = ar */ \
            __ASM_EMIT  ("vmovss        0x00(%[" DST "_im], %[off]), %%xmm5")               /* xmm5 = ai */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = br */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_im], %[off]), %%xmm1")               /* xmm1 = bi */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm5, %%xmm2")                            /* xmm2 = ai*br */ \
            __ASM_EMIT  ("vmulps        %%xmm1, %%xmm5, %%xmm3")                            /* xmm3 = ai*bi */ \
            __ASM_EMIT  (SEL("vmulss    %%xmm0, %%xmm4, %%xmm0", ""))                   /* xmm0 = ar*br */ \
            __ASM_EMIT  (SEL("vmulss    %%xmm1, %%xmm4, %%xmm1", ""))                   /* xmm1 = ar*bi */ \
            __ASM_EMIT  (SEL("vsubss    %%xmm3, %%xmm0, %%xmm0", "vfmsub132ss %%xmm4, %%xmm3, %%xmm0")) /* xmm0 = ar*br - ai*bi = r */ \
            __ASM_EMIT  (SEL("vaddss    %%xmm2, %%xmm1, %%xmm1", "vfmadd132ss %%xmm4, %%xmm2, %%xmm1")) /* xmm1 = ar*bi + ai*br = i */ \
            __ASM_EMIT  ("vmovss        %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovss        %%xmm1, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           5b") \
            __ASM_EMIT  ("6:")

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

        void complex_mul2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MUL2_CORE("dst", "src", FMA_ON)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5"
            );
        }

        #undef COMPLEX_MUL2_CORE

        #define COMPLEX_MOD_CORE(DST, SRC, SEL) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x16 blocks */ \
            __ASM_EMIT  ("sub           $16, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = r */ \
            __ASM_EMIT  ("vmovups       0x20(%[" SRC "_re], %[off]), %%ymm1") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm2")               /* ymm2 = i */ \
            __ASM_EMIT  ("vmovups       0x20(%[" SRC "_im], %[off]), %%ymm3") \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm0, %%ymm0")                            /* ymm0 = r*r */ \
            __ASM_EMIT  ("vmulps        %%ymm1, %%ymm1, %%ymm1") \
            __ASM_EMIT  (SEL("vmulps        %%ymm2, %%ymm2, %%ymm2", ""))                   /* ymm2 = i*i */ \
            __ASM_EMIT  (SEL("vmulps        %%ymm3, %%ymm3, %%ymm3", "")) \
            __ASM_EMIT  (SEL("vaddps        %%ymm2, %%ymm0, %%ymm0", "vfmadd231ps %%ymm2, %%ymm2, %%ymm0")) /* ymm0 = r*r + i*i */ \
            __ASM_EMIT  (SEL("vaddps        %%ymm3, %%ymm1, %%ymm1", "vfmadd231ps %%ymm3, %%ymm3, %%ymm1")) \
            __ASM_EMIT  ("vsqrtps       %%ymm0, %%ymm0")                                    /* ymm0 = sqrt(r*r + i*i) */ \
            __ASM_EMIT  ("vsqrtps       %%ymm1, %%ymm1") \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm1, 0x20(%[" DST "], %[off])") \
            __ASM_EMIT  ("add           $0x40, %[off]") \
            __ASM_EMIT  ("sub           $16, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* x8 block */ \
            __ASM_EMIT  ("add           $8, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = r */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm2")               /* ymm2 = i */ \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm0, %%ymm0")                            /* ymm0 = r*r */ \
            __ASM_EMIT  (SEL("vmulps        %%ymm2, %%ymm2, %%ymm2", ""))                   /* ymm2 = i*i */ \
            __ASM_EMIT  (SEL("vaddps        %%ymm2, %%ymm0, %%ymm0", "vfmadd231ps %%ymm2, %%ymm2, %%ymm0")) /* ymm0 = r*r + i*i */ \
            __ASM_EMIT  ("vsqrtps       %%ymm0, %%ymm0")                                    /* ymm0 = sqrt(r*r + i*i) */ \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT  ("sub           $8, %[count]") \
            __ASM_EMIT  ("4:") \
            /* x4 block */ \
            __ASM_EMIT  ("add           $4, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = r */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%xmm2")               /* xmm2 = i */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm0, %%xmm0")                            /* xmm0 = r*r */ \
            __ASM_EMIT  (SEL("vmulps        %%xmm2, %%xmm2, %%xmm2", ""))                   /* xmm2 = i*i */ \
            __ASM_EMIT  (SEL("vaddps        %%xmm2, %%xmm0, %%xmm0", "vfmadd231ps %%xmm2, %%xmm2, %%xmm0")) /* xmm0 = r*r + i*i */ \
            __ASM_EMIT  ("vsqrtps       %%xmm0, %%xmm0")                                    /* xmm0 = sqrt(r*r + i*i) */ \
            __ASM_EMIT  ("vmovups       %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT  ("sub           $4, %[count]") \
            __ASM_EMIT  ("6:") \
            /* x1 block */ \
            __ASM_EMIT  ("add           $3, %[count]") \
            __ASM_EMIT  ("jl            8f") \
            __ASM_EMIT  ("7:") \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = r */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_im], %[off]), %%xmm2")               /* xmm2 = i */ \
            __ASM_EMIT  ("vmulss        %%xmm0, %%xmm0, %%xmm0")                            /* xmm0 = r*r */ \
            __ASM_EMIT  (SEL("vmulss        %%xmm2, %%xmm2, %%xmm2", ""))                   /* xmm2 = i*i */ \
            __ASM_EMIT  (SEL("vaddss        %%xmm2, %%xmm0, %%xmm0", "vfmadd231ps %%xmm2, %%xmm2, %%xmm0")) /* xmm0 = r*r + i*i */ \
            __ASM_EMIT  ("vsqrtss       %%xmm0, %%xmm0, %%xmm0")                            /* xmm0 = sqrt(r*r + i*i) */ \
            __ASM_EMIT  ("vmovss        %%xmm0, 0x00(%[" DST "], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT  ("dec           %[count]") \
            __ASM_EMIT  ("jge           7b") \
            __ASM_EMIT  ("8:") \

        void complex_mod(float *dst, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MOD_CORE("dst", "src", FMA_OFF)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src_re] "r" (src_re), [src_im] "r" (src_im)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void complex_mod_fma3(float *dst, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_MOD_CORE("dst", "src", FMA_ON)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst] "r" (dst), [src_re] "r" (src_re), [src_im] "r" (src_im)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        #undef COMPLEX_MOD_CORE

        #define COMPLEX_DIV3_CORE(DSTR, DSTI, TR, TI, BR, BI, __ASM_NOFMA, __ASM_FMA) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x8 blocks */ \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT32("mov           %[" TR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" TI "], %[ptr_im]") \
            __ASM_EMIT  ("vmovups       0x00(%[" BR "], %[off]), %%ymm2")   /*  ymm2 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" BI "], %[off]), %%ymm3")   /*  ymm3 = bi */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%ymm0")   /*  ymm0 = tr */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%ymm1")   /*  ymm1 = ti */ \
            __ASM_EMIT64("vmovups       0x00(%[" TR "], %[off]), %%ymm0")   /*  ymm0 = tr */ \
            __ASM_EMIT64("vmovups       0x00(%[" TI "], %[off]), %%ymm1")   /*  ymm1 = ti */ \
            __ASM_EMIT32("mov           %[" DSTR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DSTI "], %[ptr_im]") \
            __ASM_EMIT  ("vmulps        %%ymm2, %%ymm2, %%ymm7")            /*  ymm7 = br*br */ \
            __ASM_NOFMA ("vmulps        %%ymm3, %%ymm3, %%ymm6")            /*  ymm6 = bi*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm3, %%ymm1, %%ymm4")            /*  ymm4 = ti*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm3, %%ymm0, %%ymm5")            /*  ymm5 = tr*bi */ \
            __ASM_NOFMA ("vaddps        %%ymm6, %%ymm7, %%ymm7")            /*  ymm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ps   %%ymm3, %%ymm3, %%ymm7")            /*  ymm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%ymm6")              /*  ymm6 = 1 */ \
            __ASM_NOFMA ("vmulps        %%ymm2, %%ymm0, %%ymm0")            /*  ymm0 = tr*br */ \
            __ASM_NOFMA ("vmulps        %%ymm2, %%ymm1, %%ymm1")            /*  ymm1 = ti*br */ \
            __ASM_EMIT  ("vdivps        %%ymm7, %%ymm6, %%ymm7")            /*  ymm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddps        %%ymm4, %%ymm0, %%ymm0")            /*  ymm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubps        %%ymm5, %%ymm1, %%ymm1")            /*  ymm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ps   %%ymm2, %%ymm4, %%ymm0")            /*  ymm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ps   %%ymm2, %%ymm5, %%ymm1")            /*  ymm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm7, %%ymm0, %%ymm0")            /*  ymm0 = r/R */ \
            __ASM_EMIT  ("vmulps        %%ymm7, %%ymm1, %%ymm1")            /*  ymm1 = i/R */ \
            __ASM_EMIT32("vmovups       %%ymm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%ymm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT64("vmovups       %%ymm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT32("mov           %[" TR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" TI "], %[ptr_im]") \
            __ASM_EMIT  ("vmovups       0x00(%[" BR "], %[off]), %%xmm2")   /*  xmm2 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" BI "], %[off]), %%xmm3")   /*  xmm3 = bi */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_re], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT32("vmovups       0x00(%[ptr_im], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT64("vmovups       0x00(%[" TR "], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT64("vmovups       0x00(%[" TI "], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT32("mov           %[" DSTR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DSTI "], %[ptr_im]") \
            __ASM_EMIT  ("vmulps        %%xmm2, %%xmm2, %%xmm7")            /*  xmm7 = br*br */ \
            __ASM_NOFMA ("vmulps        %%xmm3, %%xmm3, %%xmm6")            /*  xmm6 = bi*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm3, %%xmm1, %%xmm4")            /*  xmm4 = ti*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm3, %%xmm0, %%xmm5")            /*  xmm5 = tr*bi */ \
            __ASM_NOFMA ("vaddps        %%xmm6, %%xmm7, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ps   %%xmm3, %%xmm3, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")              /*  xmm6 = 1 */ \
            __ASM_NOFMA ("vmulps        %%xmm2, %%xmm0, %%xmm0")            /*  xmm0 = tr*br */ \
            __ASM_NOFMA ("vmulps        %%xmm2, %%xmm1, %%xmm1")            /*  xmm1 = ti*br */ \
            __ASM_EMIT  ("vdivps        %%xmm7, %%xmm6, %%xmm7")            /*  xmm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddps        %%xmm4, %%xmm0, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubps        %%xmm5, %%xmm1, %%xmm1")            /*  xmm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ps   %%xmm2, %%xmm4, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ps   %%xmm2, %%xmm5, %%xmm1")            /*  xmm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm7, %%xmm0, %%xmm0")            /*  xmm0 = r/R */ \
            __ASM_EMIT  ("vmulps        %%xmm7, %%xmm1, %%xmm1")            /*  xmm1 = i/R */ \
            __ASM_EMIT32("vmovups       %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovups       %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT64("vmovups       %%xmm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT  ("4:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("5:") \
            __ASM_EMIT32("mov           %[" TR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" TI "], %[ptr_im]") \
            __ASM_EMIT  ("vmovss        0x00(%[" BR "], %[off]), %%xmm2")   /*  xmm2 = br */ \
            __ASM_EMIT  ("vmovss        0x00(%[" BI "], %[off]), %%xmm3")   /*  xmm3 = bi */ \
            __ASM_EMIT32("vmovss        0x00(%[ptr_re], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT32("vmovss        0x00(%[ptr_im], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT64("vmovss        0x00(%[" TR "], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT64("vmovss        0x00(%[" TI "], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT32("mov           %[" DSTR "], %[ptr_re]") \
            __ASM_EMIT32("mov           %[" DSTI "], %[ptr_im]") \
            __ASM_EMIT  ("vmulss        %%xmm2, %%xmm2, %%xmm7")            /*  xmm7 = br*br */ \
            __ASM_NOFMA ("vmulss        %%xmm3, %%xmm3, %%xmm6")            /*  xmm6 = bi*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm3, %%xmm1, %%xmm4")            /*  xmm4 = ti*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm3, %%xmm0, %%xmm5")            /*  xmm5 = tr*bi */ \
            __ASM_NOFMA ("vaddss        %%xmm6, %%xmm7, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ss   %%xmm3, %%xmm3, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")              /*  xmm6 = 1 */ \
            __ASM_NOFMA ("vmulss        %%xmm2, %%xmm0, %%xmm0")            /*  xmm0 = tr*br */ \
            __ASM_NOFMA ("vmulss        %%xmm2, %%xmm1, %%xmm1")            /*  xmm1 = ti*br */ \
            __ASM_EMIT  ("vdivss        %%xmm7, %%xmm6, %%xmm7")            /*  xmm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddss        %%xmm4, %%xmm0, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubss        %%xmm5, %%xmm1, %%xmm1")            /*  xmm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ss   %%xmm2, %%xmm4, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ss   %%xmm2, %%xmm5, %%xmm1")            /*  xmm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm7, %%xmm0, %%xmm0")            /*  xmm0 = r/R */ \
            __ASM_EMIT  ("vmulss        %%xmm7, %%xmm1, %%xmm1")            /*  xmm1 = i/R */ \
            __ASM_EMIT32("vmovss        %%xmm0, 0x00(%[ptr_re], %[off])") \
            __ASM_EMIT32("vmovss        %%xmm1, 0x00(%[ptr_im], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT64("vmovss        %%xmm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           5b") \
            __ASM_EMIT  ("6:")

        void complex_div3(float *dst_re, float *dst_im,
                const float *src1_re, const float *src1_im,
                const float *src2_re, const float *src2_im,
                size_t count)
        {
            IF_ARCH_I386(float *ptr_re, *ptr_im);
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV3_CORE("dst_re", "dst_im", "src1_re", "src1_im", "src2_re", "src2_im", FMA_PASS, FMA_SKIP)
                : __IF_32(
                    [ptr_re] "=&r" (ptr_re), [ptr_im] "=&r" (ptr_im),
                  )
                  [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] X86_GREG (dst_re), [dst_im] X86_GREG (dst_im),
                  [src1_re] X86_GREG (src1_re), [src1_im] X86_GREG (src1_im),
                  [src2_re] "r" (src2_re), [src2_im] "r" (src2_im),
                  [CC] "o" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_div3_fma3(float *dst_re, float *dst_im,
                const float *src1_re, const float *src1_im,
                const float *src2_re, const float *src2_im,
                size_t count)
        {
            IF_ARCH_I386(float *ptr_re, *ptr_im);
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV3_CORE("dst_re", "dst_im", "src1_re", "src1_im", "src2_re", "src2_im", FMA_SKIP, FMA_PASS)
                : __IF_32(
                    [ptr_re] "=&r" (ptr_re), [ptr_im] "=&r" (ptr_im),
                  )
                  [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] X86_GREG (dst_re), [dst_im] X86_GREG (dst_im),
                  [src1_re] X86_GREG (src1_re), [src1_im] X86_GREG (src1_im),
                  [src2_re] "r" (src2_re), [src2_im] "r" (src2_im),
                  [CC] "o" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef COMPLEX_DIV3_CORE

        #define COMPLEX_DIV2_CORE(DSTR, DSTI, TR, TI, BR, BI, __ASM_NOFMA, __ASM_FMA) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x8 blocks */ \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT  ("vmovups       0x00(%[" BR "], %[off]), %%ymm2")   /*  ymm2 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" BI "], %[off]), %%ymm3")   /*  ymm3 = bi */ \
            __ASM_EMIT  ("vmovups       0x00(%[" TR "], %[off]), %%ymm0")   /*  ymm0 = tr */ \
            __ASM_EMIT  ("vmovups       0x00(%[" TI "], %[off]), %%ymm1")   /*  ymm1 = ti */ \
            __ASM_EMIT  ("vmulps        %%ymm2, %%ymm2, %%ymm7")            /*  ymm7 = br*br */ \
            __ASM_NOFMA ("vmulps        %%ymm3, %%ymm3, %%ymm6")            /*  ymm6 = bi*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm3, %%ymm1, %%ymm4")            /*  ymm4 = ti*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm3, %%ymm0, %%ymm5")            /*  ymm5 = tr*bi */ \
            __ASM_NOFMA ("vaddps        %%ymm6, %%ymm7, %%ymm7")            /*  ymm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ps   %%ymm3, %%ymm3, %%ymm7")            /*  ymm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%ymm6")              /*  ymm6 = 1 */ \
            __ASM_NOFMA ("vmulps        %%ymm2, %%ymm0, %%ymm0")            /*  ymm0 = tr*br */ \
            __ASM_NOFMA ("vmulps        %%ymm2, %%ymm1, %%ymm1")            /*  ymm1 = ti*br */ \
            __ASM_EMIT  ("vdivps        %%ymm7, %%ymm6, %%ymm7")            /*  ymm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddps        %%ymm4, %%ymm0, %%ymm0")            /*  ymm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubps        %%ymm5, %%ymm1, %%ymm1")            /*  ymm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ps   %%ymm2, %%ymm4, %%ymm0")            /*  ymm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ps   %%ymm2, %%ymm5, %%ymm1")            /*  ymm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulps        %%ymm7, %%ymm0, %%ymm0")            /*  ymm0 = r/R */ \
            __ASM_EMIT  ("vmulps        %%ymm7, %%ymm1, %%ymm1")            /*  ymm1 = i/R */ \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT  ("vmovups       0x00(%[" BR "], %[off]), %%xmm2")   /*  xmm2 = br */ \
            __ASM_EMIT  ("vmovups       0x00(%[" BI "], %[off]), %%xmm3")   /*  xmm3 = bi */ \
            __ASM_EMIT  ("vmovups       0x00(%[" TR "], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT  ("vmovups       0x00(%[" TI "], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT  ("vmulps        %%xmm2, %%xmm2, %%xmm7")            /*  xmm7 = br*br */ \
            __ASM_NOFMA ("vmulps        %%xmm3, %%xmm3, %%xmm6")            /*  xmm6 = bi*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm3, %%xmm1, %%xmm4")            /*  xmm4 = ti*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm3, %%xmm0, %%xmm5")            /*  xmm5 = tr*bi */ \
            __ASM_NOFMA ("vaddps        %%xmm6, %%xmm7, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ps   %%xmm3, %%xmm3, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")              /*  xmm6 = 1 */ \
            __ASM_NOFMA ("vmulps        %%xmm2, %%xmm0, %%xmm0")            /*  xmm0 = tr*br */ \
            __ASM_NOFMA ("vmulps        %%xmm2, %%xmm1, %%xmm1")            /*  xmm1 = ti*br */ \
            __ASM_EMIT  ("vdivps        %%xmm7, %%xmm6, %%xmm7")            /*  xmm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddps        %%xmm4, %%xmm0, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubps        %%xmm5, %%xmm1, %%xmm1")            /*  xmm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ps   %%xmm2, %%xmm4, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ps   %%xmm2, %%xmm5, %%xmm1")            /*  xmm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulps        %%xmm7, %%xmm0, %%xmm0")            /*  xmm0 = r/R */ \
            __ASM_EMIT  ("vmulps        %%xmm7, %%xmm1, %%xmm1")            /*  xmm1 = i/R */ \
            __ASM_EMIT  ("vmovups       %%xmm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT  ("vmovups       %%xmm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT  ("4:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("5:") \
            __ASM_EMIT  ("vmovss        0x00(%[" BR "], %[off]), %%xmm2")   /*  xmm2 = br */ \
            __ASM_EMIT  ("vmovss        0x00(%[" BI "], %[off]), %%xmm3")   /*  xmm3 = bi */ \
            __ASM_EMIT  ("vmovss        0x00(%[" TR "], %[off]), %%xmm0")   /*  xmm0 = tr */ \
            __ASM_EMIT  ("vmovss        0x00(%[" TI "], %[off]), %%xmm1")   /*  xmm1 = ti */ \
            __ASM_EMIT  ("vmulss        %%xmm2, %%xmm2, %%xmm7")            /*  xmm7 = br*br */ \
            __ASM_NOFMA ("vmulss        %%xmm3, %%xmm3, %%xmm6")            /*  xmm6 = bi*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm3, %%xmm1, %%xmm4")            /*  xmm4 = ti*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm3, %%xmm0, %%xmm5")            /*  xmm5 = tr*bi */ \
            __ASM_NOFMA ("vaddss        %%xmm6, %%xmm7, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_FMA   ("vfmadd231ss   %%xmm3, %%xmm3, %%xmm7")            /*  xmm7 = M = br*br + bi*bi */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")              /*  xmm6 = 1 */ \
            __ASM_NOFMA ("vmulss        %%xmm2, %%xmm0, %%xmm0")            /*  xmm0 = tr*br */ \
            __ASM_NOFMA ("vmulss        %%xmm2, %%xmm1, %%xmm1")            /*  xmm1 = ti*br */ \
            __ASM_EMIT  ("vdivss        %%xmm7, %%xmm6, %%xmm7")            /*  xmm7 = R = 1/M */ \
            __ASM_NOFMA ("vaddss        %%xmm4, %%xmm0, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_NOFMA ("vsubss        %%xmm5, %%xmm1, %%xmm1")            /*  xmm1 = i = ti*br - tr*bi */ \
            __ASM_FMA   ("vfmadd132ss   %%xmm2, %%xmm4, %%xmm0")            /*  xmm0 = r = tr*br + ti*bi */ \
            __ASM_FMA   ("vfmsub132ss   %%xmm2, %%xmm5, %%xmm1")            /*  xmm1 = i = tr*br - tr*bi */ \
            __ASM_EMIT  ("vmulss        %%xmm7, %%xmm0, %%xmm0")            /*  xmm0 = r/R */ \
            __ASM_EMIT  ("vmulss        %%xmm7, %%xmm1, %%xmm1")            /*  xmm1 = i/R */ \
            __ASM_EMIT  ("vmovss        %%xmm0, 0x00(%[" DSTR "], %[off])") \
            __ASM_EMIT  ("vmovss        %%xmm1, 0x00(%[" DSTI "], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           5b") \
            __ASM_EMIT  ("6:")

        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV2_CORE("dst_re", "dst_im", "dst_re", "dst_im", "src_re", "src_im", FMA_PASS, FMA_SKIP)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_div2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV2_CORE("dst_re", "dst_im", "dst_re", "dst_im", "src_re", "src_im", FMA_SKIP, FMA_PASS)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV2_CORE("dst_re", "dst_im", "src_re", "src_im", "dst_re", "dst_im", FMA_PASS, FMA_SKIP)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_rdiv2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_DIV2_CORE("dst_re", "dst_im", "src_re", "src_im", "dst_re", "dst_im", FMA_SKIP, FMA_PASS)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef COMPLEX_DIV2_CORE

        #define COMPLEX_RCP_CORE(DST, SRC, SEL) \
            __ASM_EMIT  ("xor           %[off], %[off]") \
            /* x16 blocks */ \
            __ASM_EMIT32("subl          $16, %[count]") \
            __ASM_EMIT64("sub           $16, %[count]") \
            __ASM_EMIT  ("jb            2f") \
            __ASM_EMIT  ("1:") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = ar */ \
            __ASM_EMIT  ("vmovups       0x20(%[" SRC "_re], %[off]), %%ymm1") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm2")               /* ymm2 = ai */ \
            __ASM_EMIT  ("vmovups       0x20(%[" SRC "_im], %[off]), %%ymm3") \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm0, %%ymm4")                            /* ymm4 = ar*ar */ \
            __ASM_EMIT  ("vmulps        %%ymm1, %%ymm1, %%ymm5") \
            __ASM_EMIT  (SEL("vmulps    %%ymm2, %%ymm2, %%ymm6", ""))                       /* ymm6 = ai*ai */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm3, %%ymm3, %%ymm7", "")) \
            __ASM_EMIT  (SEL("vaddps    %%ymm6, %%ymm4, %%ymm4", "vfmadd231ps %%ymm2, %%ymm2, %%ymm4")) /* ymm4 = R = ar*ar+ai*ai */ \
            __ASM_EMIT  (SEL("vaddps    %%ymm7, %%ymm5, %%ymm5", "vfmadd231ps %%ymm3, %%ymm3, %%ymm5")) \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%ymm6")                              /* ymm6 = 1 */ \
            __ASM_EMIT  ("vxorps        0x20 + %[CC], %%ymm2, %%ymm2")                      /* ymm1 = -ai */ \
            __ASM_EMIT  ("vxorps        0x20 + %[CC], %%ymm3, %%ymm3") \
            __ASM_EMIT  ("vdivps        %%ymm4, %%ymm6, %%ymm4")                            /* ymm4 = 1/R */ \
            __ASM_EMIT  ("vdivps        %%ymm5, %%ymm6, %%ymm5") \
            __ASM_EMIT  ("vmulps        %%ymm4, %%ymm0, %%ymm0")                            /* ymm0 = ar/R */ \
            __ASM_EMIT  ("vmulps        %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT  ("vmulps        %%ymm4, %%ymm2, %%ymm2")                            /* ymm1 = -ai/R */ \
            __ASM_EMIT  ("vmulps        %%ymm5, %%ymm3, %%ymm3") \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm1, 0x20(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm2, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm3, 0x20(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x40, %[off]") \
            __ASM_EMIT32("subl          $16, %[count]") \
            __ASM_EMIT64("sub           $16, %[count]") \
            __ASM_EMIT  ("jae           1b") \
            __ASM_EMIT  ("2:") \
            /* 8x block */ \
            __ASM_EMIT32("addl          $8, %[count]") \
            __ASM_EMIT64("add           $8, %[count]") \
            __ASM_EMIT  ("jl            4f") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%ymm0")               /* ymm0 = ar */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%ymm2")               /* ymm2 = ai */ \
            __ASM_EMIT  ("vmulps        %%ymm0, %%ymm0, %%ymm4")                            /* ymm4 = ar*ar */ \
            __ASM_EMIT  (SEL("vmulps    %%ymm2, %%ymm2, %%ymm6", ""))                       /* ymm6 = ai*ai */ \
            __ASM_EMIT  (SEL("vaddps    %%ymm6, %%ymm4, %%ymm4", "vfmadd231ps %%ymm2, %%ymm2, %%ymm4")) /* ymm4 = R = ar*ar+ai*ai */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%ymm6")                              /* ymm6 = 1 */ \
            __ASM_EMIT  ("vxorps        0x20 + %[CC], %%ymm2, %%ymm2")                      /* ymm1 = -ai */ \
            __ASM_EMIT  ("vdivps        %%ymm4, %%ymm6, %%ymm4")                            /* ymm4 = 1/R */ \
            __ASM_EMIT  ("vmulps        %%ymm4, %%ymm0, %%ymm0")                            /* ymm0 = ar/R */ \
            __ASM_EMIT  ("vmulps        %%ymm4, %%ymm2, %%ymm2")                            /* ymm1 = -ai/R */ \
            __ASM_EMIT  ("vmovups       %%ymm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%ymm2, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x20, %[off]") \
            __ASM_EMIT32("subl          $8, %[count]") \
            __ASM_EMIT64("sub           $8, %[count]") \
            __ASM_EMIT  ("4:") \
            /* 4x block */ \
            __ASM_EMIT32("addl          $4, %[count]") \
            __ASM_EMIT64("add           $4, %[count]") \
            __ASM_EMIT  ("jl            6f") \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = ar */ \
            __ASM_EMIT  ("vmovups       0x00(%[" SRC "_im], %[off]), %%xmm2")               /* xmm2 = ai */ \
            __ASM_EMIT  ("vmulps        %%xmm0, %%xmm0, %%xmm4")                            /* xmm4 = ar*ar */ \
            __ASM_EMIT  (SEL("vmulps    %%xmm2, %%xmm2, %%xmm6", ""))                       /* xmm6 = ai*ai */ \
            __ASM_EMIT  (SEL("vaddps    %%xmm6, %%xmm4, %%xmm4", "vfmadd231ps %%xmm2, %%xmm2, %%xmm4")) /* xmm4 = R = ar*ar+ai*ai */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")                              /* xmm6 = 1 */ \
            __ASM_EMIT  ("vxorps        0x20 + %[CC], %%xmm2, %%xmm2")                      /* xmm1 = -ai */ \
            __ASM_EMIT  ("vdivps        %%xmm4, %%xmm6, %%xmm4")                            /* xmm4 = 1/R */ \
            __ASM_EMIT  ("vmulps        %%xmm4, %%xmm0, %%xmm0")                            /* xmm0 = ar/R */ \
            __ASM_EMIT  ("vmulps        %%xmm4, %%xmm2, %%xmm2")                            /* xmm1 = -ai/R */ \
            __ASM_EMIT  ("vmovups       %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovups       %%xmm2, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x10, %[off]") \
            __ASM_EMIT32("subl          $4, %[count]") \
            __ASM_EMIT64("sub           $4, %[count]") \
            __ASM_EMIT  ("6:") \
            /* 1x blocks */ \
            __ASM_EMIT32("addl          $3, %[count]") \
            __ASM_EMIT64("add           $3, %[count]") \
            __ASM_EMIT  ("jl            8f") \
            __ASM_EMIT  ("7:") \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_re], %[off]), %%xmm0")               /* xmm0 = ar */ \
            __ASM_EMIT  ("vmovss        0x00(%[" SRC "_im], %[off]), %%xmm2")               /* xmm2 = ai */ \
            __ASM_EMIT  ("vmulss        %%xmm0, %%xmm0, %%xmm4")                            /* xmm4 = ar*ar */ \
            __ASM_EMIT  (SEL("vmulss    %%xmm2, %%xmm2, %%xmm6", ""))                       /* xmm6 = ai*ai */ \
            __ASM_EMIT  (SEL("vaddss    %%xmm6, %%xmm4, %%xmm4", "vfmadd231ss %%xmm2, %%xmm2, %%xmm4")) /* xmm4 = R = ar*ar+ai*ai */ \
            __ASM_EMIT  ("vmovaps       0x00 + %[CC], %%xmm6")                              /* xmm6 = 1 */ \
            __ASM_EMIT  ("vxorps        0x20 + %[CC], %%xmm2, %%xmm2")                      /* xmm1 = -ai */ \
            __ASM_EMIT  ("vdivss        %%xmm4, %%xmm6, %%xmm4")                            /* xmm4 = 1/R */ \
            __ASM_EMIT  ("vmulss        %%xmm4, %%xmm0, %%xmm0")                            /* xmm0 = ar/R */ \
            __ASM_EMIT  ("vmulss        %%xmm4, %%xmm2, %%xmm2")                            /* xmm1 = -ai/R */ \
            __ASM_EMIT  ("vmovss        %%xmm0, 0x00(%[" DST "_re], %[off])") \
            __ASM_EMIT  ("vmovss        %%xmm2, 0x00(%[" DST "_im], %[off])") \
            __ASM_EMIT  ("add           $0x04, %[off]") \
            __ASM_EMIT32("decl          %[count]") \
            __ASM_EMIT64("dec           %[count]") \
            __ASM_EMIT  ("jge           7b") \
            __ASM_EMIT  ("8:")

        void complex_rcp1(float *dst_re, float *dst_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_RCP_CORE("dst", "dst", FMA_OFF)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_rcp1_fma3(float *dst_re, float *dst_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_RCP_CORE("dst", "dst", FMA_ON)
                : [count] "+r" (count), [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_RCP_CORE("dst", "src", FMA_OFF)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void complex_rcp2_fma3(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            IF_ARCH_X86(size_t off);
            ARCH_X86_ASM
            (
                COMPLEX_RCP_CORE("dst", "src", FMA_ON)
                : [count] X86_PGREG (count),
                  [off] "=&r" (off)
                : [dst_re] "r" (dst_re), [dst_im] "r" (dst_im),
                  [src_re] "r" (src_re), [src_im] "r" (src_im),
                  [CC] "m" (complex_div_const)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        #undef COMPLEX_RCP_CORE

        #undef FMA_OFF
        #undef FMA_ON
        #undef FMA_PASS
        #undef FMA_SKIP
    } /* namespace avx */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX_COMPLEX_H_ */
