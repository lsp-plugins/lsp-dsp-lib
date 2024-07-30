/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 29 июл. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_SEARCH_IMINMAX_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_SEARCH_IMINMAX_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t minmax_incr[] __lsp_aligned64 =
            {
                // +0x00 Index
                0, 1, 2, 3,
                4, 5, 6, 7,
                8, 9, 10, 11,
                12, 13, 14, 15,
                // Increments
                LSP_DSP_VEC16(16),          // +0x40
                LSP_DSP_VEC8(8),            // +0x80
                LSP_DSP_VEC4(4),            // +0xa0
                LSP_DSP_VEC4(1),            // +0xb0
                // Absolute mask
                LSP_DSP_VEC16(0x7fffffff)   // +0xc0
            }
        );

        #define SEL_NONE(x, y)          x
        #define SEL_ABS(x, y)           y

        /*
         * Register allocation:
         *  zmm0    = imin
         *  zmm1    = vmin
         *  zmm2    = cind
         *  zmm3    = cval
         *  zmm4/k4 = cmask
         *  zmm5    = temp
         *  zmm6    = amask
         *  zmm7    = incr
         */
        #define IMINMAX_CORE(COND, SEL) \
            __ASM_EMIT("vpxord              %%zmm0, %%zmm0, %%zmm0") \
            __ASM_EMIT("test                %[count], %[count]") \
            __ASM_EMIT("jz                  8f") \
            /* 16x block */ \
            __ASM_EMIT("vbroadcastss        0x00(%[src]), %%zmm1") \
            __ASM_EMIT(SEL("", "vmovaps     0xc0 + %[INCR], %%zmm6")) \
            __ASM_EMIT("vmovdqa32           0x00 + %[INCR], %%zmm2") \
            __ASM_EMIT(SEL("", "vandps      %%zmm6, %%zmm1, %%zmm1")) \
            __ASM_EMIT("vmovdqa32           0x40 + %[INCR], %%zmm7") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT(SEL("vmovups         0x00(%[src]), %%zmm3", "vandps 0x00(%[src]), %%zmm6, %%zmm3")) \
            __ASM_EMIT("vcmpps              " COND ", %%zmm1, %%zmm3, %%k4") \
            __ASM_EMIT("vmovdqa32           %%zmm2, %%zmm0 %{%%k4%}") \
            __ASM_EMIT("vmovaps             %%zmm3, %%zmm1 %{%%k4%}") \
            __ASM_EMIT("vpaddd              %%zmm7, %%zmm2, %%zmm2") \
            __ASM_EMIT("add                 $0x40, %[src]") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("vextracti64x4       $1, %%zmm0, %%ymm5") \
            __ASM_EMIT("vextractf64x4       $1, %%zmm1, %%ymm3") \
            __ASM_EMIT("vcmpps              " COND ", %%ymm1, %%ymm3, %%ymm4") \
            __ASM_EMIT("vpblendvb           %%ymm4, %%ymm5, %%ymm0, %%ymm0") \
            __ASM_EMIT("vblendvps           %%ymm4, %%ymm3, %%ymm1, %%ymm1") \
            __ASM_EMIT("2:") \
            /* 8x block */ \
            __ASM_EMIT("add                 $8, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT(SEL("vmovups         0x00(%[src]), %%ymm3", "vandps 0x00(%[src]), %%ymm6, %%ymm3")) \
            __ASM_EMIT("vcmpps              " COND ", %%ymm1, %%ymm3, %%ymm4") \
            __ASM_EMIT("vpblendvb           %%ymm4, %%ymm2, %%ymm0, %%ymm0") \
            __ASM_EMIT("vblendvps           %%ymm4, %%ymm3, %%ymm1, %%ymm1") \
            __ASM_EMIT("vpaddd              0x80 + %[INCR], %%ymm2, %%ymm2") \
            __ASM_EMIT("add                 $0x20, %[src]") \
            __ASM_EMIT("sub                 $8, %[count]") \
            __ASM_EMIT("4:") \
            __ASM_EMIT("vextracti128        $1, %%ymm0, %%xmm5") \
            __ASM_EMIT("vextractf128        $1, %%ymm1, %%xmm3") \
            __ASM_EMIT("vcmpps              " COND ", %%xmm1, %%xmm3, %%xmm4") \
            __ASM_EMIT("vpblendvb           %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
            /* 4x block */ \
            __ASM_EMIT("add                 $4, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT(SEL("vmovups         0x00(%[src]), %%xmm3", "vandps 0x00(%[src]), %%xmm6, %%xmm3")) \
            __ASM_EMIT("vcmpps              " COND ", %%xmm1, %%xmm3, %%xmm4") \
            __ASM_EMIT("vpblendvb           %%xmm4, %%xmm2, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpaddd              0xa0 + %[INCR], %%xmm2, %%xmm2") \
            __ASM_EMIT("sub                 $4, %[count]") \
            __ASM_EMIT("add                 $0x10, %[src]") \
            __ASM_EMIT("6:") \
            __ASM_EMIT("vpshufd             $0x39, %%xmm0, %%xmm5") \
            __ASM_EMIT("vshufps             $0x39, %%xmm1, %%xmm1, %%xmm3") \
            __ASM_EMIT("vcmpps              " COND ", %%xmm1, %%xmm3, %%xmm4") \
            __ASM_EMIT("vpblendvb           %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpunpckhdq          %%xmm0, %%xmm0, %%xmm5") \
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm1, %%xmm3") \
            __ASM_EMIT("vcmpps              " COND ", %%xmm1, %%xmm3, %%xmm4") \
            __ASM_EMIT("vpblendvb           %%xmm4, %%xmm5, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
            /* 1x block */ \
            __ASM_EMIT("add                 $3, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("movss               0x00(%[src]), %%xmm3") \
            __ASM_EMIT(SEL("", "vandps      %%xmm6, %%xmm3, %%xmm3")) \
            __ASM_EMIT("vcmpps              " COND ", %%xmm1, %%xmm3, %%xmm4") \
            __ASM_EMIT("vpblendvb           %%xmm4, %%xmm2, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm4, %%xmm3, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpaddd              0xb0 + %[INCR], %%xmm2, %%xmm2") \
            __ASM_EMIT("add                 $0x04, %[src]") \
            __ASM_EMIT("dec                 %[count]") \
            __ASM_EMIT("jge                 7b") \
            __ASM_EMIT("8:") \
            /* end */


        size_t min_index(const float *src, size_t count)
        {
            uint32_t index;

            ARCH_X86_ASM(
                IMINMAX_CORE("$1", SEL_NONE)
                : [src] "+r" (src), [count] "+r" (count),
                  [index] "=Yz" (index)
                : [INCR] "o" (minmax_incr)
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );

            return index;
        }

        size_t max_index(const float *src, size_t count)
        {
            uint32_t index;

            ARCH_X86_ASM(
                IMINMAX_CORE("$6", SEL_NONE)
                : [src] "+r" (src), [count] "+r" (count),
                  [index] "=Yz" (index)
                : [INCR] "o" (minmax_incr)
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );

            return index;
        }

        size_t abs_min_index(const float *src, size_t count)
        {
            uint32_t index;

            ARCH_X86_ASM(
                IMINMAX_CORE("$1", SEL_ABS)
                : [src] "+r" (src), [count] "+r" (count),
                  [index] "=Yz" (index)
                : [INCR] "o" (minmax_incr)
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );

            return index;
        }

        size_t abs_max_index(const float *src, size_t count)
        {
            uint32_t index;

            ARCH_X86_ASM(
                IMINMAX_CORE("$6", SEL_ABS)
                : [src] "+r" (src), [count] "+r" (count),
                  [index] "=Yz" (index)
                : [INCR] "o" (minmax_incr)
                : "cc",
                  "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k4"
            );

            return index;
        }

        #undef IMINMAX_CORE

        /*
         * Register allocation:
         *  ymm0    = imin
         *  ymm1    = vmin
         *  ymm2    = imax
         *  ymm3    = vmax
         *  ymm4    = cind
         *  ymm5    = cval
         *  ymm6/k6 = cmask
         *  ymm7/k7 = temp
         */
        #define IMINMAX2_CORE(SEL) \
            __ASM_EMIT("vpxord              %%zmm0, %%zmm0, %%zmm0")    /* imin */ \
            __ASM_EMIT("vpxord              %%zmm2, %%zmm2, %%zmm2")    /* imax */ \
            __ASM_EMIT("test                %[count], %[count]") \
            __ASM_EMIT("jz                  8f") \
            /* 16x block */ \
            __ASM_EMIT("vbroadcastss        0x00(%[src]), %%zmm1")      /* vmin */ \
            __ASM_EMIT("vmovdqa32           0x00 + %[INCR], %%zmm4")    /* cind */ \
            __ASM_EMIT(SEL("", "vandps      0xc0 + %[INCR], %%zmm1, %%zmm1")) \
            __ASM_EMIT("vmovdqa32           %%zmm1, %%zmm3")            /* vmax */ \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jb                  2f") \
            __ASM_EMIT("1:") \
            __ASM_EMIT("vmovups             0x00(%[src]), %%zmm5") \
            __ASM_EMIT(SEL("", "vandps      0xc0 + %[INCR], %%zmm5, %%zmm5")) \
            __ASM_EMIT("vcmpps              $1, %%zmm1, %%zmm5, %%k6") /* cmpltps */ \
            __ASM_EMIT("vcmpps              $6, %%zmm3, %%zmm5, %%k7") /* cmpgtps */ \
            __ASM_EMIT("vmovdqa32           %%zmm4, %%zmm0 %{%%k6%}") \
            __ASM_EMIT("vmovaps             %%zmm5, %%zmm1 %{%%k6%}") \
            __ASM_EMIT("vmovdqa32           %%zmm4, %%zmm2 %{%%k7%}") \
            __ASM_EMIT("vmovaps             %%zmm5, %%zmm3 %{%%k7%}") \
            __ASM_EMIT("vpaddd              0x40 + %[INCR], %%zmm4, %%zmm4") \
            __ASM_EMIT("add                 $0x40, %[src]") \
            __ASM_EMIT("sub                 $16, %[count]") \
            __ASM_EMIT("jae                 1b") \
            __ASM_EMIT("vextracti64x4       $1, %%zmm0, %%ymm7") \
            __ASM_EMIT("vextractf64x4       $1, %%zmm1, %%ymm5") \
            __ASM_EMIT("vcmpps              $1, %%ymm1, %%ymm5, %%ymm6") \
            __ASM_EMIT("vpblendvb           %%ymm6, %%ymm7, %%ymm0, %%ymm0") \
            __ASM_EMIT("vblendvps           %%ymm6, %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT("vextracti64x4       $1, %%zmm2, %%ymm7") \
            __ASM_EMIT("vextractf64x4       $1, %%zmm3, %%ymm5") \
            __ASM_EMIT("vcmpps              $6, %%ymm3, %%ymm5, %%ymm6") \
            __ASM_EMIT("vpblendvb           %%ymm6, %%ymm7, %%ymm2, %%ymm2") \
            __ASM_EMIT("vblendvps           %%ymm6, %%ymm5, %%ymm3, %%ymm3") \
            __ASM_EMIT("2:") \
            /* 8x block */ \
            __ASM_EMIT("add                 $8, %[count]") \
            __ASM_EMIT("jl                  4f") \
            __ASM_EMIT("vmovups             0x00(%[src]), %%ymm5") \
            __ASM_EMIT(SEL("", "vandps      0xc0 + %[INCR], %%ymm5, %%ymm5")) \
            __ASM_EMIT("vcmpps              $1, %%ymm1, %%ymm5, %%ymm6") /* cmpltps */ \
            __ASM_EMIT("vcmpps              $6, %%ymm3, %%ymm5, %%ymm7") /* cmpgtps */ \
            __ASM_EMIT("vpblendvb           %%ymm6, %%ymm4, %%ymm0, %%ymm0") \
            __ASM_EMIT("vblendvps           %%ymm6, %%ymm5, %%ymm1, %%ymm1") \
            __ASM_EMIT("vpblendvb           %%ymm7, %%ymm4, %%ymm2, %%ymm2") \
            __ASM_EMIT("vblendvps           %%ymm7, %%ymm5, %%ymm3, %%ymm3") \
            __ASM_EMIT("vpaddd              0x80 + %[INCR], %%ymm4, %%ymm4") \
            __ASM_EMIT("add                 $0x20, %[src]") \
            __ASM_EMIT("sub                 $8, %[count]") \
            __ASM_EMIT("4:") \
            __ASM_EMIT("vextracti128        $1, %%ymm0, %%xmm7") \
            __ASM_EMIT("vextractf128        $1, %%ymm1, %%xmm5") \
            __ASM_EMIT("vcmpps              $1, %%xmm1, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm1, %%xmm1") \
            __ASM_EMIT("vextracti128        $1, %%ymm2, %%xmm7") \
            __ASM_EMIT("vextractf128        $1, %%ymm3, %%xmm5") \
            __ASM_EMIT("vcmpps              $6, %%xmm3, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm3, %%xmm3") \
            /* 4x block */ \
            __ASM_EMIT("add                 $4, %[count]") \
            __ASM_EMIT("jl                  6f") \
            __ASM_EMIT("vmovups             0x00(%[src]), %%xmm5") \
            __ASM_EMIT(SEL("", "vandps      0xc0 + %[INCR], %%xmm5, %%xmm5")) \
            __ASM_EMIT("vcmpps              $1, %%xmm1, %%xmm5, %%xmm6") /* cmpltps */ \
            __ASM_EMIT("vcmpps              $6, %%xmm3, %%xmm5, %%xmm7") /* cmpgtps */ \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm4, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpblendvb           %%xmm7, %%xmm4, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps           %%xmm7, %%xmm5, %%xmm3, %%xmm3") \
            __ASM_EMIT("vpaddd              0xa0 + %[INCR], %%xmm4, %%xmm4") \
            __ASM_EMIT("sub                 $4, %[count]") \
            __ASM_EMIT("add                 $0x10, %[src]") \
            __ASM_EMIT("6:") \
            __ASM_EMIT("vpshufd             $0x39, %%xmm0, %%xmm7") \
            __ASM_EMIT("vshufps             $0x39, %%xmm1, %%xmm1, %%xmm5") \
            __ASM_EMIT("vcmpps              $1, %%xmm1, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpunpckhdq          %%xmm0, %%xmm0, %%xmm7") \
            __ASM_EMIT("vunpckhps           %%xmm1, %%xmm1, %%xmm5") \
            __ASM_EMIT("vcmpps              $1, %%xmm1, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpshufd             $0x39, %%xmm2, %%xmm7") \
            __ASM_EMIT("vshufps             $0x39, %%xmm3, %%xmm3, %%xmm5") \
            __ASM_EMIT("vcmpps              $6, %%xmm3, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm3, %%xmm3") \
            __ASM_EMIT("vpunpckhdq          %%xmm2, %%xmm2, %%xmm7") \
            __ASM_EMIT("vunpckhps           %%xmm3, %%xmm3, %%xmm5") \
            __ASM_EMIT("vcmpps              $6, %%xmm3, %%xmm5, %%xmm6") \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm7, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm3, %%xmm3") \
            /* 1x block */ \
            __ASM_EMIT("add                 $3, %[count]") \
            __ASM_EMIT("jl                  8f") \
            __ASM_EMIT("7:") \
            __ASM_EMIT("movss               0x00(%[src]), %%xmm5") \
            __ASM_EMIT(SEL("", "vandps      0xc0 + %[INCR], %%xmm5, %%xmm5")) \
            __ASM_EMIT("vcmpps              $1, %%xmm1, %%xmm5, %%xmm6") /* cmpltps */ \
            __ASM_EMIT("vcmpps              $6, %%xmm3, %%xmm5, %%xmm7") /* cmpgtps */ \
            __ASM_EMIT("vpblendvb           %%xmm6, %%xmm4, %%xmm0, %%xmm0") \
            __ASM_EMIT("vblendvps           %%xmm6, %%xmm5, %%xmm1, %%xmm1") \
            __ASM_EMIT("vpblendvb           %%xmm7, %%xmm4, %%xmm2, %%xmm2") \
            __ASM_EMIT("vblendvps           %%xmm7, %%xmm5, %%xmm3, %%xmm3") \
            __ASM_EMIT("vpaddd              0xb0 + %[INCR], %%xmm4, %%xmm4") \
            __ASM_EMIT("add                 $0x04, %[src]") \
            __ASM_EMIT("dec                 %[count]") \
            __ASM_EMIT("jge                 7b") \
            __ASM_EMIT("8:") \
            /* end */ \
            __ASM_EMIT("movss               %%xmm0, (%[min])") \
            __ASM_EMIT("movss               %%xmm2, (%[max])")

        void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
        {
            ARCH_X86_ASM(
                IMINMAX2_CORE(SEL_NONE)
                : [src] "+r" (src), [count] "+r" (count)
                : [min] "r" (min), [max] "r" (max),
                  [INCR] "o" (minmax_incr)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k6", "%k7"
            );
        }

         void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
         {
             ARCH_X86_ASM(
                 IMINMAX2_CORE(SEL_ABS)
                 : [src] "+r" (src), [count] "+r" (count)
                 : [min] "r" (min), [max] "r" (max),
                   [INCR] "o" (minmax_incr)
                 : "cc", "memory",
                   "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                   "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                   "%k6", "%k7"
             );
         }

        #undef IMINMAX2_CORE
        #undef SEL_NONE
        #undef SEL_ABS
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_SEARCH_IMINMAX_H_ */
