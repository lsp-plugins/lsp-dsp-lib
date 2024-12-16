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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_CORRELATION_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_CORRELATION_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        void corr_init(dsp::correlation_t *corr, const float *a, const float *b, size_t count)
        {
            IF_ARCH_X86(
                size_t off;
            );

            ARCH_X86_ASM
            (
                __ASM_EMIT("xor             %[off], %[off]")
                __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0")        /* xv = 0 */
                __ASM_EMIT("vxorps          %%zmm1, %%zmm1, %%zmm1")        /* xa = 0 */
                __ASM_EMIT("vxorps          %%zmm2, %%zmm2, %%zmm2")        /* xb = 0 */
                /* 32x blocks */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%zmm3")    /* zmm3 = a0 */
                __ASM_EMIT("vmovups         0x40(%[a], %[off]), %%zmm4")    /* zmm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%zmm5")    /* zmm5 = b0 */
                __ASM_EMIT("vmovups         0x40(%[b], %[off]), %%zmm6")    /* zmm6 = b1 */
                __ASM_EMIT("vfmadd231ps     %%zmm3, %%zmm5, %%zmm0")        /* zmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%zmm3, %%zmm3, %%zmm1")        /* zmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%zmm5, %%zmm5, %%zmm2")        /* zmm2 = xb + b0*b0 */
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm6, %%zmm0")        /* zmm0 = xv + a1*b1 */
                __ASM_EMIT("vfmadd231ps     %%zmm4, %%zmm4, %%zmm1")        /* zmm1 = xa + a1*a1 */
                __ASM_EMIT("vfmadd231ps     %%zmm6, %%zmm6, %%zmm2")        /* zmm2 = xb + b1*b1 */
                __ASM_EMIT("add             $0x80, %[off]")                 /* ++off */
                __ASM_EMIT("sub             $32, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("vextractf64x4   $1, %%zmm0, %%ymm4")
                __ASM_EMIT("vextractf64x4   $1, %%zmm1, %%ymm5")
                __ASM_EMIT("vextractf64x4   $1, %%zmm2, %%ymm6")
                __ASM_EMIT("vaddps          %%ymm4, %%ymm0, %%ymm0")
                __ASM_EMIT("vaddps          %%ymm5, %%ymm1, %%ymm1")
                __ASM_EMIT("vaddps          %%ymm6, %%ymm2, %%ymm2")
                __ASM_EMIT("2:")
                /* 16x blocks */
                __ASM_EMIT("add             $16, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%ymm3")    /* ymm3 = a0 */
                __ASM_EMIT("vmovups         0x20(%[a], %[off]), %%ymm4")    /* ymm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%ymm5")    /* ymm5 = b0 */
                __ASM_EMIT("vmovups         0x20(%[b], %[off]), %%ymm6")    /* ymm6 = b1 */
                __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm5, %%ymm0")        /* ymm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%ymm3, %%ymm3, %%ymm1")        /* ymm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%ymm5, %%ymm5, %%ymm2")        /* ymm2 = xb + b0*b0 */
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm6, %%ymm0")        /* ymm0 = xv + a1*b1 */
                __ASM_EMIT("vfmadd231ps     %%ymm4, %%ymm4, %%ymm1")        /* ymm1 = xa + a1*a1 */
                __ASM_EMIT("vfmadd231ps     %%ymm6, %%ymm6, %%ymm2")        /* ymm2 = xb + b1*b1 */
                __ASM_EMIT("sub             $16, %[count]")
                __ASM_EMIT("add             $0x40, %[off]")                 /* ++off */
                __ASM_EMIT("4:")
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm4")
                __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm5")
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm6")
                __ASM_EMIT("vaddps          %%xmm4, %%xmm0, %%xmm0")
                __ASM_EMIT("vaddps          %%xmm5, %%xmm1, %%xmm1")
                __ASM_EMIT("vaddps          %%xmm6, %%xmm2, %%xmm2")
                /* 8x block */
                __ASM_EMIT("add             $8, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x10(%[a], %[off]), %%xmm4")    /* xmm4 = a1 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vmovups         0x10(%[b], %[off]), %%xmm6")    /* xmm6 = b1 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm6, %%xmm0")        /* xmm0 = xv + a1*b1 */
                __ASM_EMIT("vfmadd231ps     %%xmm4, %%xmm4, %%xmm1")        /* xmm1 = xa + a1*a1 */
                __ASM_EMIT("vfmadd231ps     %%xmm6, %%xmm6, %%xmm2")        /* xmm2 = xb + b1*b1 */
                __ASM_EMIT("sub             $8, %[count]")
                __ASM_EMIT("add             $0x20, %[off]")                 /* ++off */
                __ASM_EMIT("6:")
                /* 4x block */
                __ASM_EMIT("add             $4, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("vmovups         0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovups         0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ps     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ps     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("sub             $4, %[count]")
                __ASM_EMIT("add             $0x10, %[off]")                 /* ++off */
                __ASM_EMIT("8:")
                /* Do horizontal sum */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1 xv2+xv3 xv0+xv1 xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm0, %%xmm0, %%xmm0")        /* xmm0 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm1, %%xmm1, %%xmm1")        /* xmm1 = xv0+xv1+xv2+xv3 */
                __ASM_EMIT("vhaddps         %%xmm2, %%xmm2, %%xmm2")        /* xmm2 = xv0+xv1+xv2+xv3 */
                /* 1x blocks */
                __ASM_EMIT("add             $3, %[count]")
                __ASM_EMIT("jl              10f")
                __ASM_EMIT("9:")
                __ASM_EMIT("vmovss          0x00(%[a], %[off]), %%xmm3")    /* xmm3 = a0 */
                __ASM_EMIT("vmovss          0x00(%[b], %[off]), %%xmm5")    /* xmm5 = b0 */
                __ASM_EMIT("vfmadd231ss     %%xmm3, %%xmm5, %%xmm0")        /* xmm0 = xv + a0*b0 */
                __ASM_EMIT("vfmadd231ss     %%xmm3, %%xmm3, %%xmm1")        /* xmm1 = xa + a0*a0 */
                __ASM_EMIT("vfmadd231ss     %%xmm5, %%xmm5, %%xmm2")        /* xmm2 = xb + b0*b0 */
                __ASM_EMIT("add             $0x04, %[off]")                 /* ++off */
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jge             9b")
                __ASM_EMIT("10:")
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

        static const float corr_const[] __lsp_aligned64 =
        {
            LSP_DSP_VEC16(1e-18f)
        };

        static const uint32_t corr_idx[] __lsp_aligned64 =
        {
            0, 0, 0, 0,  0, 0, 0, 0,  3, 3, 3, 3,  7, 7, 7, 7,                  // 0 0 A0 A1
            0, 0, 0, 0,  3, 3, 3, 3,  7, 7, 7, 7,  11, 11, 11, 11,              // 0 A0 A1 A2
            15, 15, 15, 15,  15, 15, 15, 15,  15, 15, 15, 15, 15, 15, 15, 15
        };

        static const uint16_t corr_kmask[] =
        {
            0xff00,
            0xfff0
        };

        void corr_incr(dsp::correlation_t *corr, float *dst,
            const float *a_head, const float *b_head,
            const float *a_tail, const float *b_tail,
            size_t count)
        {
            IF_ARCH_I386(
                void *ptr;
            );

            ARCH_X86_ASM
            (
                /* 16x blocks */
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT("jb              2f")
                __ASM_EMIT("kmovw           0x00 + %[CORR_KMASK], %%k1")
                __ASM_EMIT("kmovw           0x02 + %[CORR_KMASK], %%k2")
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovups         0x00(%[a_head]), %%zmm0")           /* zmm0 = ah */
                __ASM_EMIT("vmovups         0x00(%[b_head]), %%zmm1")           /* zmm1 = bh */
                __ASM_EMIT("vmovups         0x00(%[a_tail]), %%zmm3")           /* zmm3 = at */
                __ASM_EMIT("vmovups         0x00(%[b_tail]), %%zmm4")           /* zmm4 = bt */
                __ASM_EMIT("vmulps          %%zmm1, %%zmm0, %%zmm2")            /* zmm2 = ah*bh */
                __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm0")            /* zmm0 = ah*ah */
                __ASM_EMIT("vmulps          %%zmm1, %%zmm1, %%zmm1")            /* zmm1 = bh*bh */
                __ASM_EMIT("vxorps          %%zmm6, %%zmm6, %%zmm6")            /* zmm6 = 0 0 0 0 0 0 0 0 */
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm3, %%zmm2")            /* zmm2 = DV = ah*bh - at*bt */
                __ASM_EMIT("vfnmadd231ps    %%zmm3, %%zmm3, %%zmm0")            /* zmm0 = DA = ah*ah - at*at */
                __ASM_EMIT("vfnmadd231ps    %%zmm4, %%zmm4, %%zmm1")            /* zmm1 = DB = bh*bh - bt*bt */

                __ASM_EMIT("vshufps         $0x44, %%zmm0, %%zmm6, %%zmm3")     /* zmm3 = 0 0 DA[0] DA[1] ... */
                __ASM_EMIT("vshufps         $0x44, %%zmm1, %%zmm6, %%zmm4")     /* zmm4 = 0 0 DB[0] DB[1] ... */
                __ASM_EMIT("vshufps         $0x44, %%zmm2, %%zmm6, %%zmm5")     /* zmm5 = 0 0 DV[0] DV[1] ... */
                __ASM_EMIT("vaddps          %%zmm3, %%zmm0, %%zmm0")            /* zmm0 = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] ... */
                __ASM_EMIT("vaddps          %%zmm4, %%zmm1, %%zmm1")            /* zmm1 = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] ... */
                __ASM_EMIT("vaddps          %%zmm5, %%zmm2, %%zmm2")            /* zmm2 = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] ... */
                __ASM_EMIT("vmovaps         0x00 + %[CORR_IDX], %%zmm6")        /* zmm6 = permute mask */
                __ASM_EMIT("vmovaps         0x40 + %[CORR_IDX], %%zmm7")        /* zmm7 = permute mask */
                __ASM_EMIT("vshufps         $0x99, %%zmm0, %%zmm3, %%zmm3")     /* zmm3 = 0 DA[0] DA[1] DA[0]+DA[2] ... */
                __ASM_EMIT("vshufps         $0x99, %%zmm1, %%zmm4, %%zmm4")     /* zmm4 = 0 DB[0] DB[1] DB[0]+DB[2] ... */
                __ASM_EMIT("vshufps         $0x99, %%zmm2, %%zmm5, %%zmm5")     /* zmm5 = 0 DV[0] DV[1] DV[0]+DV[2] ... */
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm3")            /* zmm3 = A = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] ... */
                __ASM_EMIT("vaddps          %%zmm1, %%zmm4, %%zmm4")            /* zmm4 = B = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] ... */
                __ASM_EMIT("vaddps          %%zmm2, %%zmm5, %%zmm5")            /* zmm5 = V = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] ... */
                __ASM_EMIT("vpermps         %%zmm3, %%zmm6, %%zmm0")            /* zmm0 = A0 A0 A0 A1 */
                __ASM_EMIT("vpermps         %%zmm4, %%zmm6, %%zmm1")            /* zmm1 = B0 B0 B0 B1 */
                __ASM_EMIT("vpermps         %%zmm5, %%zmm6, %%zmm2")            /* zmm2 = V0 V0 V0 V1 */
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm3 %{%%k1%}")   /* zmm3 = A0 A1 A0+A2 A1+A3 */
                __ASM_EMIT("vaddps          %%zmm1, %%zmm4, %%zmm4 %{%%k1%}")   /* zmm4 = B0 B1 B0+B2 B1+B3 */
                __ASM_EMIT("vaddps          %%zmm2, %%zmm5, %%zmm5 %{%%k1%}")   /* zmm5 = V0 V1 V0+V2 V1+V3 */
                __ASM_EMIT("vpermps         %%zmm3, %%zmm7, %%zmm0")            /* zmm0 = A0 A0 A1 A0+A2 */
                __ASM_EMIT("vpermps         %%zmm4, %%zmm7, %%zmm1")            /* zmm1 = B0 B0 B1 B0+B2 */
                __ASM_EMIT("vpermps         %%zmm5, %%zmm7, %%zmm2")            /* zmm2 = V0 V0 V1 V0+V2 */
                __ASM_EMIT("vaddps          %%zmm0, %%zmm3, %%zmm3 %{%%k2%}")   /* zmm3 = A0 A0+A1 A0+A1+A2 A0+A1+A2+A3 */
                __ASM_EMIT("vaddps          %%zmm1, %%zmm4, %%zmm4 %{%%k2%}")   /* zmm4 = B0 B0+B1 B0+B1+B2 B0+B1+B2+B3 */
                __ASM_EMIT("vaddps          %%zmm2, %%zmm5, %%zmm5 %{%%k2%}")   /* zmm5 = V0 V0+V1 V0+V1+V2 V0+V1+V2+V3 */

                __ASM_EMIT32("mov           %[corr], %[ptr]")
                __ASM_EMIT32("vbroadcastss  0x00(%[ptr]), %%zmm0")
                __ASM_EMIT32("vbroadcastss  0x04(%[ptr]), %%zmm1")
                __ASM_EMIT32("vbroadcastss  0x08(%[ptr]), %%zmm2")
                __ASM_EMIT64("vbroadcastss  0x00(%[corr]), %%zmm0")
                __ASM_EMIT64("vbroadcastss  0x04(%[corr]), %%zmm1")
                __ASM_EMIT64("vbroadcastss  0x08(%[corr]), %%zmm2")

                __ASM_EMIT("vaddps          %%zmm3, %%zmm1, %%zmm1")            /* zmm1 = BA = xa + A */
                __ASM_EMIT("vaddps          %%zmm4, %%zmm2, %%zmm2")            /* zmm2 = BB = xb + B */
                __ASM_EMIT("vaddps          %%zmm5, %%zmm0, %%zmm0")            /* zmm0 = T = xv + V */
                __ASM_EMIT("vmovaps         0x80 + %[CORR_IDX], %%zmm6")        /* zmm6 = permute mask */
                __ASM_EMIT("vmulps          %%zmm2, %%zmm1, %%zmm7")            /* zmm7 = B = BA*BB */
                __ASM_EMIT("vpermps         %%zmm0, %%zmm6, %%zmm3")            /* zmm3 = xv' */
                __ASM_EMIT("vpermps         %%zmm1, %%zmm6, %%zmm4")            /* zmm4 = xa' */
                __ASM_EMIT("vpermps         %%zmm2, %%zmm6, %%zmm5")            /* zmm5 = xb' */

                __ASM_EMIT32("vmovss        %%xmm3, 0x00(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm4, 0x04(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm5, 0x08(%[ptr])")
                __ASM_EMIT64("vmovss        %%xmm3, 0x00(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm4, 0x04(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm5, 0x08(%[corr])")

                __ASM_EMIT("vsqrtps         %%zmm7, %%zmm6")                    /* zmm6 = sqrtf(B) */
                __ASM_EMIT("vcmpps          $1, %[CORR_CC], %%zmm7, %%k3")      /* k3   = B < threshold */
                __ASM_EMIT("vdivps          %%zmm6, %%zmm0, %%zmm0")            /* zmm0 = T/sqrtf(B) */
                __ASM_EMIT32("mov           %[dst], %[ptr]")
                __ASM_EMIT("vxorps          %%zmm0, %%zmm0, %%zmm0 %{%%k3%}")   /* zmm0 = (B < threshold) ? 0 : T/sqrtf(B) */
                __ASM_EMIT("add             $0x40, %[a_head]")                  /* ++a_head */
                __ASM_EMIT("add             $0x40, %[b_head]")                  /* ++b_head */
                __ASM_EMIT32("vmovups       %%zmm0, 0x00(%[ptr])")
                __ASM_EMIT64("vmovups       %%zmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x40, %[a_tail]")                  /* ++a_tail */
                __ASM_EMIT("add             $0x40, %[b_tail]")                  /* ++b_tail */
                __ASM_EMIT32("add           $0x40, %[ptr]")
                __ASM_EMIT64("add           $0x40, %[dst]")
                __ASM_EMIT32("mov           %[ptr], %[dst]")
                __ASM_EMIT32("subl          $16, %[count]")
                __ASM_EMIT64("sub           $16, %[count]")
                __ASM_EMIT("jae             1b")
                __ASM_EMIT("2:")
                /* 8x block */
                __ASM_EMIT32("addl          $8, %[count]")
                __ASM_EMIT64("add           $8, %[count]")
                __ASM_EMIT("jl              4f")
                __ASM_EMIT("vmovups         0x00(%[a_head]), %%ymm0")           /* ymm0 = ah */
                __ASM_EMIT("vmovups         0x00(%[b_head]), %%ymm1")           /* ymm1 = bh */
                __ASM_EMIT("vmovups         0x00(%[a_tail]), %%ymm3")           /* ymm3 = at */
                __ASM_EMIT("vmovups         0x00(%[b_tail]), %%ymm4")           /* ymm4 = bt */
                __ASM_EMIT("vmulps          %%ymm1, %%ymm0, %%ymm2")            /* ymm2 = ah*bh */
                __ASM_EMIT("vmulps          %%ymm0, %%ymm0, %%ymm0")            /* ymm0 = ah*ah */
                __ASM_EMIT("vmulps          %%ymm1, %%ymm1, %%ymm1")            /* ymm1 = bh*bh */
                __ASM_EMIT("vxorps          %%ymm6, %%ymm6, %%ymm6")            /* ymm6 = 0 0 0 0 0 0 0 0 */
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm3, %%ymm2")            /* ymm2 = DV = ah*bh - at*bt */
                __ASM_EMIT("vfnmadd231ps    %%ymm3, %%ymm3, %%ymm0")            /* ymm0 = DA = ah*ah - at*at */
                __ASM_EMIT("vfnmadd231ps    %%ymm4, %%ymm4, %%ymm1")            /* ymm1 = DB = bh*bh - bt*bt */

                __ASM_EMIT("vshufps         $0x44, %%ymm0, %%ymm6, %%ymm3")     /* ymm3 = 0 0 DA[0] DA[1] 0 0 DA[4] DA[5] */
                __ASM_EMIT("vshufps         $0x44, %%ymm1, %%ymm6, %%ymm4")     /* ymm4 = 0 0 DB[0] DB[1] 0 0 DB[4] DB[5] */
                __ASM_EMIT("vshufps         $0x44, %%ymm2, %%ymm6, %%ymm5")     /* ymm5 = 0 0 DV[0] DV[1] 0 0 DV[4] DV[5] */
                __ASM_EMIT("vaddps          %%ymm3, %%ymm0, %%ymm0")            /* ymm0 = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] DA[4] DA[5] DA[4]+DA[6] DA[5]+DA[7] */
                __ASM_EMIT("vaddps          %%ymm4, %%ymm1, %%ymm1")            /* ymm1 = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] DB[4] DB[5] DB[4]+DB[6] DB[5]+DB[7] */
                __ASM_EMIT("vaddps          %%ymm5, %%ymm2, %%ymm2")            /* ymm2 = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] DV[4] DV[5] DV[4]+DV[6] DV[5]+DV[7] */
                __ASM_EMIT("vshufps         $0x99, %%ymm0, %%ymm3, %%ymm3")     /* ymm3 = 0 DA[0] DA[1] DA[0]+DA[2] 0 DA[4] DA[5] DA[4]+DA[6] */
                __ASM_EMIT("vshufps         $0x99, %%ymm1, %%ymm4, %%ymm4")     /* ymm4 = 0 DB[0] DB[1] DB[0]+DB[2] 0 DB[4] DB[5] DB[4]+DB[6] */
                __ASM_EMIT("vshufps         $0x99, %%ymm2, %%ymm5, %%ymm5")     /* ymm5 = 0 DV[0] DV[1] DV[0]+DV[2] 0 DV[4] DV[5] DV[4]+DV[6] */
                __ASM_EMIT("vaddps          %%ymm0, %%ymm3, %%ymm3")            /* ymm3 = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] DA[4] DA[4]+DA[5] DA[4]+DA[5]+DA[6] DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%ymm1, %%ymm4, %%ymm4")            /* ymm4 = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] DB[4] DB[4]+DB[5] DB[4]+DB[5]+DB[6] DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vaddps          %%ymm2, %%ymm5, %%ymm5")            /* ymm5 = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] DV[4] DV[4]+DV[5] DV[4]+DV[5]+DV[6] DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm3, %%xmm3, %%xmm0")     /* xmm0 = DA[0]+DA[1]+DA[2]+DA[3] DA[0]+DA[1]+DA[2]+DA[3] DA[0]+DA[1]+DA[2]+DA[3] DA[0]+DA[1]+DA[2]+DA[3] */
                __ASM_EMIT("vshufps         $0xff, %%xmm4, %%xmm4, %%xmm1")     /* xmm1 = DB[0]+DB[1]+DB[2]+DB[3] DB[0]+DB[1]+DB[2]+DB[3] DB[0]+DB[1]+DB[2]+DB[3] DB[0]+DB[1]+DB[2]+DB[3] */
                __ASM_EMIT("vshufps         $0xff, %%xmm5, %%xmm5, %%xmm2")     /* xmm2 = DV[0]+DV[1]+DV[2]+DV[3] DV[0]+DV[1]+DV[2]+DV[3] DV[0]+DV[1]+DV[2]+DV[3] DV[0]+DV[1]+DV[2]+DV[3] */
                __ASM_EMIT("vextractf128    $1, %%ymm3, %%xmm6")                /* xmm6 = DA[4] DA[4]+DA[5] DA[4]+DA[5]+DA[6] DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vextractf128    $1, %%ymm4, %%xmm7")                /* xmm7 = DB[4] DB[4]+DB[5] DB[4]+DB[5]+DB[6] DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vaddps          %%xmm0, %%xmm6, %%xmm6")            /* xmm6 = DA[0]+DA[1]+DA[2]+DA[3]+DA[4] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%xmm1, %%xmm7, %%xmm7")            /* xmm7 = DB[0]+DB[1]+DB[2]+DB[3]+DB[4] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vextractf128    $1, %%ymm5, %%xmm0")                /* xmm0 = DV[4] DV[4]+DV[5] DV[4]+DV[5]+DV[6] DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vinsertf128     $1, %%xmm6, %%ymm3, %%ymm3")        /* ymm3 = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] DA[0]+DA[1]+DA[2]+DA[3]+DA[4] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6] DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%xmm2, %%xmm0, %%xmm0")            /* xmm0 = DV[0]+DV[1]+DV[2]+DV[3]+DV[4] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vinsertf128     $1, %%xmm7, %%ymm4, %%ymm4")        /* ymm4 = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] DB[0]+DB[1]+DB[2]+DB[3]+DB[4] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6] DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vinsertf128     $1, %%xmm0, %%ymm5, %%ymm5")        /* ymm5 = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] DV[0]+DV[1]+DV[2]+DV[3]+DV[4] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6] DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6]+DV[7] */

                __ASM_EMIT32("mov           %[corr], %[ptr]")
                __ASM_EMIT32("vbroadcastss  0x00(%[ptr]), %%ymm0")
                __ASM_EMIT32("vbroadcastss  0x04(%[ptr]), %%ymm1")
                __ASM_EMIT32("vbroadcastss  0x08(%[ptr]), %%ymm2")
                __ASM_EMIT64("vbroadcastss  0x00(%[corr]), %%ymm0")
                __ASM_EMIT64("vbroadcastss  0x04(%[corr]), %%ymm1")
                __ASM_EMIT64("vbroadcastss  0x08(%[corr]), %%ymm2")

                __ASM_EMIT("vaddps          %%ymm3, %%ymm1, %%ymm1")            /* ymm1 = BA = xa+DA[0] xa+DA[0]+DA[1] xa+DA[0]+DA[1]+DA[2] xa+DA[0]+DA[1]+DA[2]+DA[3] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%ymm4, %%ymm2, %%ymm2")            /* ymm2 = BB = xb+DB[0] xb+DB[0]+DB[1] xb+DB[0]+DB[1]+DB[2] xb+DB[0]+DB[1]+DB[2]+DB[3] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vaddps          %%ymm5, %%ymm0, %%ymm0")            /* ymm0 = T = xv+DV[0] xv+DV[0]+DV[1] xv+DV[0]+DV[1]+DV[2] xv+DV[0]+DV[1]+DV[2]+DV[3] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vmulps          %%ymm2, %%ymm1, %%ymm7")            /* ymm7 = B = BA*BB */
                __ASM_EMIT("vextractf128    $1, %%ymm1, %%xmm4")                /* xmm4 = BA[4] BA[5] BA[6] BA[7] */
                __ASM_EMIT("vextractf128    $1, %%ymm0, %%xmm3")                /* xmm3 = T[4] T[5] T[6] T[7] */
                __ASM_EMIT("vextractf128    $1, %%ymm2, %%xmm5")                /* xmm5 = BB[4] BB[5] BB[6] BB[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm3, %%xmm3, %%xmm3")     /* xmm3 = T[7] T[7] T[7] T[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm4, %%xmm4, %%xmm4")     /* xmm4 = BA[7] BA[7] BA[7] BA[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm5, %%xmm5, %%xmm5")     /* xmm5 = BB[7] BB[7] BB[7] BB[7] */

                __ASM_EMIT32("vmovss        %%xmm3, 0x00(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm4, 0x04(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm5, 0x08(%[ptr])")
                __ASM_EMIT64("vmovss        %%xmm3, 0x00(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm4, 0x04(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm5, 0x08(%[corr])")

                __ASM_EMIT("vsqrtps         %%ymm7, %%ymm6")                    /* ymm6 = sqrtf(B) */
                __ASM_EMIT("vcmpps          $5, %[CORR_CC], %%ymm7, %%ymm1")    /* ymm1 = B >= threshold */
                __ASM_EMIT("vdivps          %%ymm6, %%ymm0, %%ymm0")            /* ymm0 = T/sqrtf(B) */
                __ASM_EMIT32("mov           %[dst], %[ptr]")
                __ASM_EMIT("vandps          %%ymm1, %%ymm0, %%ymm0")            /* ymm0 = (B >= threshold) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add             $0x20, %[a_head]")                  /* ++a_head */
                __ASM_EMIT("add             $0x20, %[b_head]")                  /* ++b_head */
                __ASM_EMIT32("vmovups       %%ymm0, 0x00(%[ptr])")
                __ASM_EMIT64("vmovups       %%ymm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x20, %[a_tail]")                  /* ++a_tail */
                __ASM_EMIT("add             $0x20, %[b_tail]")                  /* ++b_tail */
                __ASM_EMIT32("add           $0x20, %[ptr]")
                __ASM_EMIT64("add           $0x20, %[dst]")
                __ASM_EMIT32("mov           %[ptr], %[dst]")
                __ASM_EMIT32("subl          $8, %[count]")
                __ASM_EMIT64("sub           $8, %[count]")
                __ASM_EMIT("4:")
                /* 4x block */
                __ASM_EMIT32("addl          $4, %[count]")
                __ASM_EMIT64("add           $4, %[count]")
                __ASM_EMIT("jl              6f")
                __ASM_EMIT("vmovups         0x00(%[a_head]), %%xmm0")           /* xmm0 = ah */
                __ASM_EMIT("vmovups         0x00(%[b_head]), %%xmm1")           /* xmm1 = bh */
                __ASM_EMIT("vmovups         0x00(%[a_tail]), %%xmm3")           /* xmm3 = at */
                __ASM_EMIT("vmovups         0x00(%[b_tail]), %%xmm4")           /* xmm4 = bt */
                __ASM_EMIT("vmulps          %%xmm1, %%xmm0, %%xmm2")            /* xmm2 = ah*bh */
                __ASM_EMIT("vmulps          %%xmm0, %%xmm0, %%xmm0")            /* xmm0 = ah*ah */
                __ASM_EMIT("vmulps          %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = bh*bh */
                __ASM_EMIT("vxorps          %%xmm6, %%xmm6, %%xmm6")            /* xmm6 = 0 0 0 0 0 0 0 0 */
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm3, %%xmm2")            /* xmm2 = DV = ah*bh - at*bt */
                __ASM_EMIT("vfnmadd231ps    %%xmm3, %%xmm3, %%xmm0")            /* xmm0 = DA = ah*ah - at*at */
                __ASM_EMIT("vfnmadd231ps    %%xmm4, %%xmm4, %%xmm1")            /* xmm1 = DB = bh*bh - bt*bt */

                __ASM_EMIT("vmovlhps        %%xmm0, %%xmm6, %%xmm3")            /* xmm3 = 0 0 DA[0] DA[1] 0 0 DA[4] DA[5] */
                __ASM_EMIT("vmovlhps        %%xmm1, %%xmm6, %%xmm4")            /* xmm4 = 0 0 DB[0] DB[1] 0 0 DB[4] DB[5] */
                __ASM_EMIT("vmovlhps        %%xmm2, %%xmm6, %%xmm5")            /* xmm5 = 0 0 DV[0] DV[1] 0 0 DV[4] DV[5] */
                __ASM_EMIT("vaddps          %%xmm3, %%xmm0, %%xmm0")            /* xmm0 = DA[0] DA[1] DA[0]+DA[2] DA[1]+DA[3] DA[4] DA[5] DA[4]+DA[6] DA[5]+DA[7] */
                __ASM_EMIT("vaddps          %%xmm4, %%xmm1, %%xmm1")            /* xmm1 = DB[0] DB[1] DB[0]+DB[2] DB[1]+DB[3] DB[4] DB[5] DB[4]+DB[6] DB[5]+DB[7] */
                __ASM_EMIT("vaddps          %%xmm5, %%xmm2, %%xmm2")            /* xmm2 = DV[0] DV[1] DV[0]+DV[2] DV[1]+DV[3] DV[4] DV[5] DV[4]+DV[6] DV[5]+DV[7] */
                __ASM_EMIT("vshufps         $0x99, %%xmm0, %%xmm3, %%xmm3")     /* xmm3 = 0 DA[0] DA[1] DA[0]+DA[2] 0 DA[4] DA[5] DA[4]+DA[6] */
                __ASM_EMIT("vshufps         $0x99, %%xmm1, %%xmm4, %%xmm4")     /* xmm4 = 0 DB[0] DB[1] DB[0]+DB[2] 0 DB[4] DB[5] DB[4]+DB[6] */
                __ASM_EMIT("vshufps         $0x99, %%xmm2, %%xmm5, %%xmm5")     /* xmm5 = 0 DV[0] DV[1] DV[0]+DV[2] 0 DV[4] DV[5] DV[4]+DV[6] */
                __ASM_EMIT("vaddps          %%xmm0, %%xmm3, %%xmm3")            /* xmm3 = DA[0] DA[0]+DA[1] DA[0]+DA[1]+DA[2] DA[0]+DA[1]+DA[2]+DA[3] DA[4] DA[4]+DA[5] DA[4]+DA[5]+DA[6] DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%xmm1, %%xmm4, %%xmm4")            /* xmm4 = DB[0] DB[0]+DB[1] DB[0]+DB[1]+DB[2] DB[0]+DB[1]+DB[2]+DB[3] DB[4] DB[4]+DB[5] DB[4]+DB[5]+DB[6] DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vaddps          %%xmm2, %%xmm5, %%xmm5")            /* xmm5 = DV[0] DV[0]+DV[1] DV[0]+DV[1]+DV[2] DV[0]+DV[1]+DV[2]+DV[3] DV[4] DV[4]+DV[5] DV[4]+DV[5]+DV[6] DV[4]+DV[5]+DV[6]+DV[7] */

                __ASM_EMIT32("mov           %[corr], %[ptr]")
                __ASM_EMIT32("vbroadcastss  0x00(%[ptr]), %%xmm0")
                __ASM_EMIT32("vbroadcastss  0x04(%[ptr]), %%xmm1")
                __ASM_EMIT32("vbroadcastss  0x08(%[ptr]), %%xmm2")
                __ASM_EMIT64("vbroadcastss  0x00(%[corr]), %%xmm0")
                __ASM_EMIT64("vbroadcastss  0x04(%[corr]), %%xmm1")
                __ASM_EMIT64("vbroadcastss  0x08(%[corr]), %%xmm2")

                __ASM_EMIT("vaddps          %%xmm3, %%xmm1, %%xmm1")            /* xmm1 = BA = xa+DA[0] xa+DA[0]+DA[1] xa+DA[0]+DA[1]+DA[2] xa+DA[0]+DA[1]+DA[2]+DA[3] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6] xa+DA[0]+DA[1]+DA[2]+DA[3]+DA[4]+DA[5]+DA[6]+DA[7] */
                __ASM_EMIT("vaddps          %%xmm4, %%xmm2, %%xmm2")            /* xmm2 = BB = xb+DB[0] xb+DB[0]+DB[1] xb+DB[0]+DB[1]+DB[2] xb+DB[0]+DB[1]+DB[2]+DB[3] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6] xb+DB[0]+DB[1]+DB[2]+DB[3]+DB[4]+DB[5]+DB[6]+DB[7] */
                __ASM_EMIT("vaddps          %%xmm5, %%xmm0, %%xmm0")            /* xmm0 = T = xv+DV[0] xv+DV[0]+DV[1] xv+DV[0]+DV[1]+DV[2] xv+DV[0]+DV[1]+DV[2]+DV[3] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6] xv+DV[0]+DV[1]+DV[2]+DV[3]+DV[4]+DV[5]+DV[6]+DV[7] */
                __ASM_EMIT("vmulps          %%xmm2, %%xmm1, %%xmm7")            /* xmm7 = B = BA*BB */
                __ASM_EMIT("vshufps         $0xff, %%xmm0, %%xmm0, %%xmm3")     /* xmm3 = T[7] T[7] T[7] T[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm1, %%xmm1, %%xmm4")     /* xmm4 = BA[7] BA[7] BA[7] BA[7] */
                __ASM_EMIT("vshufps         $0xff, %%xmm2, %%xmm2, %%xmm5")     /* xmm5 = BB[7] BB[7] BB[7] BB[7] */

                __ASM_EMIT32("vmovss        %%xmm3, 0x00(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm4, 0x04(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm5, 0x08(%[ptr])")
                __ASM_EMIT64("vmovss        %%xmm3, 0x00(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm4, 0x04(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm5, 0x08(%[corr])")

                __ASM_EMIT("vsqrtps         %%xmm7, %%xmm6")                    /* xmm6 = sqrtf(B) */
                __ASM_EMIT("vcmpps          $5, %[CORR_CC], %%xmm7, %%xmm1")    /* xmm1 = B >= threshold */
                __ASM_EMIT("vdivps          %%xmm6, %%xmm0, %%xmm0")            /* xmm0 = T/sqrtf(B) */
                __ASM_EMIT32("mov           %[dst], %[ptr]")
                __ASM_EMIT("vandps          %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = (B >= threshold) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add             $0x10, %[a_head]")                  /* ++a_head */
                __ASM_EMIT("add             $0x10, %[b_head]")                  /* ++b_head */
                __ASM_EMIT32("vmovups       %%xmm0, 0x00(%[ptr])")
                __ASM_EMIT64("vmovups       %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x10, %[a_tail]")                  /* ++a_tail */
                __ASM_EMIT("add             $0x10, %[b_tail]")                  /* ++b_tail */
                __ASM_EMIT32("add           $0x10, %[ptr]")
                __ASM_EMIT64("add           $0x10, %[dst]")
                __ASM_EMIT32("mov           %[ptr], %[dst]")
                __ASM_EMIT32("subl          $4, %[count]")
                __ASM_EMIT64("sub           $4, %[count]")
                __ASM_EMIT("6:")

                /* 1x blocks */
                __ASM_EMIT32("addl          $3, %[count]")
                __ASM_EMIT64("add           $3, %[count]")
                __ASM_EMIT("jl              8f")
                __ASM_EMIT("7:")
                __ASM_EMIT("vmovss          0x00(%[a_head]), %%xmm0")           /* xmm0 = ah */
                __ASM_EMIT("vmovss          0x00(%[b_head]), %%xmm1")           /* xmm1 = bh */
                __ASM_EMIT("vmovss          0x00(%[a_tail]), %%xmm3")           /* xmm3 = at */
                __ASM_EMIT("vmovss          0x00(%[b_tail]), %%xmm4")           /* xmm4 = bt */
                __ASM_EMIT("vmulss          %%xmm1, %%xmm0, %%xmm2")            /* xmm2 = ah*bh */
                __ASM_EMIT("vmulss          %%xmm0, %%xmm0, %%xmm0")            /* xmm0 = ah*ah */
                __ASM_EMIT("vmulss          %%xmm1, %%xmm1, %%xmm1")            /* xmm1 = bh*bh */
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm3, %%xmm2")            /* xmm2 = DV = ah*bh - at*bt */
                __ASM_EMIT("vfnmadd231ss    %%xmm3, %%xmm3, %%xmm0")            /* xmm0 = DA = ah*ah - at*at */
                __ASM_EMIT("vfnmadd231ss    %%xmm4, %%xmm4, %%xmm1")            /* xmm1 = DB = bh*bh - bt*bt */

                __ASM_EMIT32("mov           %[corr], %[ptr]")
                __ASM_EMIT32("vmovss        0x00(%[ptr]), %%xmm3")
                __ASM_EMIT32("vmovss        0x04(%[ptr]), %%xmm4")
                __ASM_EMIT32("vmovss        0x08(%[ptr]), %%xmm5")
                __ASM_EMIT64("vmovss        0x00(%[corr]), %%xmm3")
                __ASM_EMIT64("vmovss        0x04(%[corr]), %%xmm4")
                __ASM_EMIT64("vmovss        0x08(%[corr]), %%xmm5")

                __ASM_EMIT("vaddss          %%xmm3, %%xmm2, %%xmm2")            /* xmm2 = T = xv+DV */
                __ASM_EMIT("vaddss          %%xmm4, %%xmm0, %%xmm0")            /* xmm0 = BA = xa+DA */
                __ASM_EMIT("vaddss          %%xmm5, %%xmm1, %%xmm1")            /* xmm1 = BB = xb+DB */
                __ASM_EMIT("vmulss          %%xmm1, %%xmm0, %%xmm7")            /* xmm7 = B = BA*BB */

                __ASM_EMIT32("vmovss        %%xmm2, 0x00(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm0, 0x04(%[ptr])")
                __ASM_EMIT32("vmovss        %%xmm1, 0x08(%[ptr])")
                __ASM_EMIT64("vmovss        %%xmm2, 0x00(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm0, 0x04(%[corr])")
                __ASM_EMIT64("vmovss        %%xmm1, 0x08(%[corr])")

                __ASM_EMIT("vsqrtss         %%xmm7, %%xmm7, %%xmm6")            /* xmm6 = sqrtf(B) */
                __ASM_EMIT("vcmpss          $5, %[CORR_CC], %%xmm7, %%xmm1")    /* xmm1 = B >= threshold */
                __ASM_EMIT("vdivss          %%xmm6, %%xmm2, %%xmm0")            /* xmm0 = T/sqrtf(B) */
                __ASM_EMIT32("mov           %[dst], %[ptr]")
                __ASM_EMIT("vandps          %%xmm1, %%xmm0, %%xmm0")            /* xmm0 = (B >= threshold) ? T/sqrtf(B) : 0 */
                __ASM_EMIT("add             $0x04, %[a_head]")                  /* ++a_head */
                __ASM_EMIT("add             $0x04, %[b_head]")                  /* ++b_head */
                __ASM_EMIT32("vmovss        %%xmm0, 0x00(%[ptr])")
                __ASM_EMIT64("vmovss        %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("add             $0x04, %[a_tail]")                  /* ++a_tail */
                __ASM_EMIT("add             $0x04, %[b_tail]")                  /* ++b_tail */
                __ASM_EMIT32("add           $0x04, %[ptr]")
                __ASM_EMIT64("add           $0x04, %[dst]")
                __ASM_EMIT32("mov           %[ptr], %[dst]")
                __ASM_EMIT32("decl          %[count]")
                __ASM_EMIT64("dec           %[count]")
                __ASM_EMIT("jge             7b")
                __ASM_EMIT("8:")

                : __IF_32(
                    [ptr] "=&r" (ptr),
                    [corr] "+m" (corr), [dst] "+m" (dst),
                    [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                    [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                    [count] "+m" (count)
                  )
                  __IF_64(
                    [dst] "+r" (dst),
                    [a_head] "+r" (a_head), [b_head] "+r" (b_head),
                    [a_tail] "+r" (a_tail), [b_tail] "+r" (b_tail),
                    [count] "+r" (count)
                  )
                : __IF_64( [corr] "r" (corr), )
                  [CORR_CC] "o" (corr_const),
                  [CORR_IDX] "o" (corr_idx),
                  [CORR_KMASK] "o" (corr_kmask)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%k1", "%k2", "%k3"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */





#endif /* PRIVATE_DSP_ARCH_X86_AVX512_CORRELATION_H_ */
