/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 мар. 2024 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_CORRELATION_H_
#define PRIVATE_DSP_ARCH_X86_AVX_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {

        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")        /* xv = 0 */
                __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")        /* xa = 0 */
                __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")        /* xb = 0 */
                /* 16x blocks */
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm3")    /* ymm3 = a0 */
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm4")    /* ymm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm5")    /* ymm5 = b0 */
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm6")    /* ymm6 = b1 */
                __ASM_EMIT("vmulps          %%ymm3, %%ymm5, %%ymm7")        /* ymm7 = a0*b0 */
                __ASM_EMIT("vmulps          %%ymm3, %%ymm3, %%ymm3")        /* ymm3 = a0*a0 */
                __ASM_EMIT("vmulps          %%ymm5, %%ymm5, %%ymm5")        /* ymm5 = b0*b0 */
                __ASM_EMIT("vaddps          %%ymm7, %%ymm0, %%ymm0")        /* ymm0 = xv + a0*b0 */
                __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")        /* ymm1 = xa + a0*a0 */
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")        /* ymm2 = xb + b0*b0 */
                __ASM_EMIT("vmulps          %%ymm4, %%ymm6, %%ymm7")        /* ymm7 = a0*b0 */
                __ASM_EMIT("vmulps          %%ymm4, %%ymm4, %%ymm4")        /* ymm4 = a0*a0 */
                __ASM_EMIT("vmulps          %%ymm6, %%ymm6, %%ymm6")        /* ymm6 = b0*b0 */
                __ASM_EMIT("vaddps          %%ymm7, %%ymm0, %%ymm0")        /* ymm0 = xv + a1*b1 */
                __ASM_EMIT("vaddps          %%ymm4, %%ymm1, %%ymm1")        /* ymm1 = xa + a1*a1 */
                __ASM_EMIT("vaddps          %%ymm6, %%ymm2, %%ymm2")        /* ymm2 = xb + b1*b1 */
                __ASM_EMIT("add             $0x40, %[off]")                 /* ++off */
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")
                __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm5")
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm6")
                __ASM_EMIT("vaddps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          %%xmm5, %%xmm1, %%xmm1")
                __ASM_EMIT("vaddps          %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x10(%[a], %[off]), %%xmm4")    /* xmm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vmovups         0x10(%[b], %[off]), %%xmm6")    /* xmm6 = b1 */
                __ASM_EMIT("vmulps          %%xmm3, %%xmm5, %%xmm7")        /* xmm7 = a0*b0 */
                __ASM_EMIT("vmulps          %%xmm3, %%xmm3, %%xmm3")        /* xmm3 = a0*a0 */
                __ASM_EMIT("vmulps          %%xmm5, %%xmm5, %%xmm5")        /* xmm5 = b0*b0 */
                __ASM_EMIT("vaddps          %%xmm7, %%xmm0, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vaddps          %%xmm3, %%xmm1, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("vmulps          %%xmm4, %%xmm6, %%xmm7")        /* xmm7 = a0*b0 */
                __ASM_EMIT("vmulps          %%xmm4, %%xmm4, %%xmm4")        /* xmm4 = a0*a0 */
                __ASM_EMIT("vmulps          %%xmm6, %%xmm6, %%xmm6")        /* xmm6 = b0*b0 */
                __ASM_EMIT("vaddps          %%xmm7, %%xmm0, %%xmm0")        /* xmm0 = xv + a1*b1 */
                __ASM_EMIT("vaddps          %%xmm4, %%xmm1, %%xmm1")        /* xmm1 = xa + a1*a1 */
                __ASM_EMIT("vaddps          %%xmm6, %%xmm2, %%xmm2")        /* xmm2 = xb + b1*b1 */
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")                 /* ++off */
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vmulps          %%xmm3, %%xmm5, %%xmm7")        /* xmm7 = a0*b0 */
                __ASM_EMIT("vmulps          %%xmm3, %%xmm3, %%xmm3")        /* xmm3 = a0*a0 */
                __ASM_EMIT("vmulps          %%xmm5, %%xmm5, %%xmm5")        /* xmm5 = b0*b0 */
                __ASM_EMIT("vaddps          %%xmm7, %%xmm0, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vaddps          %%xmm3, %%xmm1, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")                 /* ++off */
                __ASM_EMIT("6:")
                /* Do horizontal sum */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1+xv2+xv3 */
                /* 1x blocks */
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vmulss          %%xmm3, %%xmm5, %%xmm7")        /* xmm7 = a0*b0 */
                __ASM_EMIT("vmulss          %%xmm3, %%xmm3, %%xmm3")        /* xmm3 = a0*a0 */
                __ASM_EMIT("vmulss          %%xmm5, %%xmm5, %%xmm5")        /* xmm5 = b0*b0 */
                __ASM_EMIT("vaddss          %%xmm7, %%xmm0, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vaddss          %%xmm3, %%xmm1, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vaddss          %%xmm5, %%xmm2, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("add             $0x04, %[off]")                 /* ++off */
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                __ASM_EMIT("8:")
                /* Store result */
                __ASM_EMIT("vaddss          0x00(%[corr]), %%xmm0, %%xmm0")
                __ASM_EMIT("vaddss          0x04(%[corr]), %%xmm1, %%xmm1")
                __ASM_EMIT("vaddss          0x08(%[corr]), %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[corr])")
                __ASM_EMIT("vmovss          %%xmm1, 0x04(%[corr])")
                __ASM_EMIT("vmovss          %%xmm2, 0x08(%[corr])")

                : [corr] "+r" (corr), [off] "=&r" (off), [count] "+r" (count)
                : [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void corr_init_fma3(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%ymm0, %%ymm0, %%ymm0")        /* xv = 0 */
                __ASM_EMIT("vxorps          %%ymm1, %%ymm1, %%ymm1")        /* xa = 0 */
                __ASM_EMIT("vxorps          %%ymm2, %%ymm2, %%ymm2")        /* xb = 0 */
                /* 16x blocks */
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm3")    /* ymm3 = a0 */
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm4")    /* ymm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm5")    /* ymm5 = b0 */
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm6")    /* ymm6 = b1 */
                __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm5, %%ymm0")        /* ymm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm1")        /* ymm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%ymm5, %%ymm5, %%ymm2")        /* ymm2 = xv + b0*b0 */
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm6, %%ymm0")        /* ymm0 = xv + a1*b1 */
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm4, %%ymm1")        /* ymm1 = xa + a1*a1 */
                __ASM_EMIT("vfmadd231ps     %%ymm6, %%ymm6, %%ymm2")        /* ymm2 = xv + b1*b1 */
                __ASM_EMIT("add             $0x40, %[off]")                 /* ++off */
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")
                __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm5")
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm6")
                __ASM_EMIT("vaddps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          %%xmm5, %%xmm1, %%xmm1")
                __ASM_EMIT("vaddps          %%xmm6, %%xmm2, %%xmm2")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x10(%[a], %[off]), %%xmm4")    /* xmm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vmovups         0x10(%[b], %[off]), %%xmm6")    /* xmm6 = b1 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xv + b0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm6, %%xmm0")        /* xmm0 = xv + a1*b1 */
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm4, %%xmm1")        /* xmm1 = xa + a1*a1 */
                __ASM_EMIT("vfmadd231ps     %%xmm6, %%xmm6, %%xmm2")        /* xmm2 = xv + b1*b1 */
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")                 /* ++off */
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xv + b0*b0 */
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")                 /* ++off */
                __ASM_EMIT("6:")
                /* Do horizontal sum */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1+xv2+xv3 */
                /* 1x blocks */
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vfmadd231ss     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ss     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ss     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xv + b0*b0 */
                __ASM_EMIT("add             $0x04, %[off]")                 /* ++off */
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             7b")
                __ASM_EMIT("8:")
                /* Store result */
                __ASM_EMIT("vaddss          0x00(%[corr]), %%xmm0, %%xmm0")
                __ASM_EMIT("vaddss          0x04(%[corr]), %%xmm1, %%xmm1")
                __ASM_EMIT("vaddss          0x08(%[corr]), %%xmm2, %%xmm2")
                __ASM_EMIT("vmovss          %%xmm0, 0x00(%[corr])")
                __ASM_EMIT("vmovss          %%xmm1, 0x04(%[corr])")
                __ASM_EMIT("vmovss          %%xmm2, 0x08(%[corr])")

                : [corr] "+r" (corr), [off] "=&r" (off), [count] "+r" (count)
                : [a] "r" (a), [b] "r" (b)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_CORRELATION_H_ */
