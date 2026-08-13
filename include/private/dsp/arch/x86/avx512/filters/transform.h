/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_TRANSFORM_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_TRANSFORM_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/filters/common.h>

namespace lsp
{
    namespace avx512
    {

        IF_ARCH_X86_64(
            static const uint32_t bilinear_transform_const[] __lsp_aligned64 = {
                0x00, 0x08, 0x01, 0x09, 0x02, 0x0a, 0x03, 0x0b, 0x10, 0x18, 0x11, 0x19, 0x12, 0x1a, 0x13, 0x1b,
                0x14, 0x1c, 0x15, 0x1d, 0x16, 0x1e, 0x17, 0x1f, 0x04, 0x0c, 0x05, 0x0d, 0x06, 0x0e, 0x07, 0x0f,

                LSP_DSP_VEC16(0x3f800000),               // 1.0f
            };
        )

        void x64_bilinear_transform_x16(dsp::biquad_x16_t *bf, const dsp::f_cascade_t *bc, float kf, size_t count)
        {
            ARCH_X86_64_ASM(
                __ASM_EMIT("test            %[count], %[count]")
                __ASM_EMIT("jz              100f")

                // Prepare values
                __ASM_EMIT("vbroadcastss    %[kf], %%zmm0")                     // zmm0  = kf
                __ASM_EMIT("vmovaps         0x00(%[MASK]), %%zmm14")            // zmm14 = mask1
                __ASM_EMIT("vmovaps         0x40(%[MASK]), %%zmm15")            // zmm15 = mask1
                __ASM_EMIT("vmulps          %%zmm0, %%zmm0, %%zmm1")            // zmm1  = kf*kf = kf2

                // Do a loop
                __ASM_EMIT("1:")
                __ASM_EMIT("vmovaps         0x000(%[bc]), %%zmm2")              /* zmm2  = t0[0 ] t1[0 ] t2[0 ] ?      b0[0 ] b1[0 ] b2[0 ] ?      t0[1 ] t1[1 ] t2[1 ] ?      b0[1 ] b1[1 ] b2[1 ] ?      */
                __ASM_EMIT("vmovaps         0x040(%[bc]), %%zmm3")              /* zmm3  = t0[2 ] t1[2 ] t2[2 ] ?      b0[2 ] b1[2 ] b2[2 ] ?      t0[3 ] t1[3 ] t2[3 ] ?      b0[3 ] b1[3 ] b2[3 ] ?      */
                __ASM_EMIT("vmovaps         0x080(%[bc]), %%zmm4")              /* zmm4  = t0[4 ] t1[4 ] t2[4 ] ?      b0[4 ] b1[4 ] b2[4 ] ?      t0[5 ] t1[5 ] t2[5 ] ?      b0[5 ] b1[5 ] b2[5 ] ?      */
                __ASM_EMIT("vmovaps         0x0c0(%[bc]), %%zmm5")              /* zmm5  = t0[6 ] t1[6 ] t2[6 ] ?      b0[6 ] b1[6 ] b2[6 ] ?      t0[7 ] t1[7 ] t2[7 ] ?      b0[7 ] b1[7 ] b2[7 ] ?      */
                __ASM_EMIT("vmovaps         0x100(%[bc]), %%zmm6")              /* zmm6  = t0[8 ] t1[8 ] t2[8 ] ?      b0[8 ] b1[8 ] b2[8 ] ?      t0[9 ] t1[9 ] t2[9 ] ?      b0[9 ] b1[9 ] b2[9 ] ?      */
                __ASM_EMIT("vmovaps         0x140(%[bc]), %%zmm7")              /* zmm7  = t0[10] t1[10] t2[10] ?      b0[10] b1[10] b2[10] ?      t0[11] t1[11] t2[11] ?      b0[11] b1[11] b2[11] ?      */
                __ASM_EMIT("vmovaps         0x180(%[bc]), %%zmm8")              /* zmm8  = t0[12] t1[12] t2[12] ?      b0[12] b1[12] b2[12] ?      t0[13] t1[13] t2[13] ?      b0[13] b1[13] b2[13] ?      */
                __ASM_EMIT("vmovaps         0x1c0(%[bc]), %%zmm9")              /* zmm9  = t0[14] t1[14] t2[14] ?      b0[14] b1[14] b2[14] ?      t0[15] t1[15] t2[15] ?      b0[15] b1[15] b2[15] ?      */
                /* Transpose, step 1 */
                __ASM_EMIT("vunpcklps       %%zmm3, %%zmm2, %%zmm10")           /* zmm10 = t0[0 ] t0[2 ] t1[0 ] t1[2 ] b0[0 ] b0[2 ] b1[0 ] b1[2 ] t0[1 ] t0[3 ] t1[1 ] t1[3 ] b0[1 ] b0[3 ] b1[1 ] b1[3 ] */
                __ASM_EMIT("vunpcklps       %%zmm5, %%zmm4, %%zmm11")           /* zmm11 = t0[4 ] t0[6 ] t1[4 ] t1[6 ] b0[4 ] b0[6 ] b1[4 ] b1[6 ] t0[5 ] t0[7 ] t1[5 ] t1[7 ] b0[5 ] b0[7 ] b1[5 ] b1[7 ] */
                __ASM_EMIT("vunpcklps       %%zmm7, %%zmm6, %%zmm12")           /* zmm12 = t0[8 ] t0[10] t1[8 ] t1[10] b0[8 ] b0[10] b1[8 ] b1[10] t0[9 ] t0[11] t1[9 ] t1[11] b0[9 ] b0[11] b1[9 ] b1[11] */
                __ASM_EMIT("vunpcklps       %%zmm9, %%zmm8, %%zmm13")           /* zmm13 = t0[12] t0[14] t1[12] t1[14] b0[12] b0[14] b1[12] b1[14] t0[13] t0[15] t1[13] t1[15] b0[13] b0[15] b1[13] b1[15] */
                __ASM_EMIT("vunpckhps       %%zmm3, %%zmm2, %%zmm2")            /* zmm2  = t2[0 ] t2[2 ] ?      ?      b2[0 ] b2[2 ] ?      ?      t2[1 ] t2[3 ] ?      ?      b2[1 ] b2[3 ] ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm5, %%zmm4, %%zmm4")            /* zmm4  = t2[4 ] t2[6 ] ?      ?      b2[4 ] b2[6 ] ?      ?      t2[5 ] t2[7 ] ?      ?      b2[5 ] b0[7 ] ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm7, %%zmm6, %%zmm6")            /* zmm6  = t2[8 ] t2[10] ?      ?      b2[8 ] b2[10] ?      ?      t2[9 ] t2[11] ?      ?      b2[9 ] b0[11] ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm9, %%zmm8, %%zmm8")            /* zmm8  = t2[12] t2[14] ?      ?      b2[12] b2[14] ?      ?      t2[13] t2[15] ?      ?      b2[13] b0[15] ?      ?      */
                /* Transpose, step 2 */ \
                __ASM_EMIT("vshufps         $0x44, %%zmm11, %%zmm10, %%zmm3")   /* zmm3  = t0[0 ] t0[2 ] t0[4 ] t0[6 ] b0[0 ] b0[2 ] b0[4 ] b0[6 ] t0[1 ] t0[3 ] t0[5 ] t0[7 ] b0[1 ] b0[3 ] b0[5 ] b0[7 ] */
                __ASM_EMIT("vshufps         $0x44, %%zmm8,  %%zmm6,  %%zmm9")   /* zmm9  = t2[8 ] t2[10] t2[12] t2[14] b2[8 ] b2[10] b2[12] b2[14] t2[9 ] t2[11] t2[13] t2[15] b2[9 ] b0[11] b2[13] b0[15] */
                __ASM_EMIT("vshufps         $0x44, %%zmm4,  %%zmm2,  %%zmm2")   /* zmm2  = t2[0 ] t2[2 ] t2[4 ] t2[6 ] b2[0 ] b2[2 ] b2[4 ] b2[6 ] t2[1 ] t2[3 ] t2[5 ] t2[7 ] b2[1 ] b2[3 ] b2[5 ] b0[7 ] */
                __ASM_EMIT("vshufps         $0xee, %%zmm11, %%zmm10, %%zmm4")   /* zmm4  = t1[0 ] t1[2 ] t1[4 ] t1[6 ] b1[0 ] b1[2 ] b1[4 ] b1[6 ] t1[1 ] t1[3 ] t1[5 ] t1[7 ] b1[1 ] b1[3 ] b1[5 ] b1[7 ] */
                __ASM_EMIT("vshufps         $0x44, %%zmm13, %%zmm12, %%zmm8")   /* zmm8  = t0[8 ] t0[10] t0[12] t0[14] b0[8 ] b0[10] b0[12] b0[14] t0[9 ] t0[11] t0[13] t0[15] b0[9 ] b0[11] b0[13] b0[15] */
                __ASM_EMIT("vshufps         $0xee, %%zmm13, %%zmm12, %%zmm10")  /* zmm10 = t1[8 ] t1[10] t1[12] t1[14] b1[8 ] b1[10] b1[12] b1[14] t1[9 ] t1[11] t1[13] t1[15] b1[9 ] b1[11] b1[13] b1[15] */
                /* Transpose, step 3 */
                __ASM_EMIT("vmovaps         %%zmm9, %%zmm6")                    /* zmm6  = t2[8 ] t2[10] t2[12] t2[14] b2[8 ] b2[10] b2[12] b2[14] t2[9 ] t2[11] t2[13] t2[15] b2[9 ] b0[11] b2[13] b0[15] */
                __ASM_EMIT("vmovaps         %%zmm10, %%zmm7")                   /* zmm7  = t1[8 ] t1[10] t1[12] t1[14] b1[8 ] b1[10] b1[12] b1[14] t1[9 ] t1[11] t1[13] t1[15] b1[9 ] b1[11] b1[13] b1[15] */
                __ASM_EMIT("vmovaps         %%zmm8, %%zmm5")                    /* zmm5  = t0[8 ] t0[10] t0[12] t0[14] b0[8 ] b0[10] b0[12] b0[14] t0[9 ] t0[11] t0[13] t0[15] b0[9 ] b0[11] b0[13] b0[15] */
                __ASM_EMIT("vpermt2ps       %%zmm2, %%zmm15, %%zmm6")           /* zmm6  = b2[0 ] b2[1 ] b2[2 ] b2[3 ] b2[4 ] b2[5 ] b2[6 ] b2[7 ] b2[8 ] b2[9 ] b2[10] b2[11] b2[12] b2[13] b2[14] b2[15] */
                __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm15, %%zmm7")           /* zmm7  = b1[0 ] b1[1 ] b1[2 ] b1[3 ] b1[4 ] b1[5 ] b1[6 ] b1[7 ] b1[8 ] b1[9 ] b1[10] b1[11] b1[12] b1[13] b1[14] b1[15] */
                __ASM_EMIT("vpermt2ps       %%zmm3, %%zmm15, %%zmm5")           /* zmm5  = b0[0 ] b0[1 ] b0[2 ] b0[3 ] b0[4 ] b0[5 ] b0[6 ] b0[7 ] b0[8 ] b0[9 ] b0[10] b0[11] b0[12] b0[13] b0[14] b0[15] */
                __ASM_EMIT("vpermt2ps       %%zmm9, %%zmm14, %%zmm2")           /* zmm2  = t2[0 ] t2[1 ] t2[2 ] t2[3 ] t2[4 ] t2[5 ] t2[6 ] t2[7 ] t2[8 ] t2[9 ] t2[10] t2[11] t2[12] t2[13] t2[14] t2[15] */
                __ASM_EMIT("vpermt2ps       %%zmm10, %%zmm14, %%zmm4")          /* zmm4  = t1[0 ] t1[1 ] t1[2 ] t1[3 ] t1[4 ] t1[5 ] t1[6 ] t1[7 ] t1[8 ] t1[9 ] t1[10] t1[11] t1[12] t1[13] t1[14] t1[15] */
                __ASM_EMIT("vpermt2ps       %%zmm8, %%zmm14, %%zmm3")           /* zmm3  = t0[0 ] t0[1 ] t0[2 ] t0[3 ] t0[4 ] t0[5 ] t0[6 ] t0[7 ] t0[8 ] t0[9 ] t0[10] t0[11] t0[12] t0[13] t0[14] t0[15] */
                /* Now we are ready to perform calculations of top and bottom part */
                /* zmm2 = t2, zmm3 = t0 = T0, zmm4 = t1 */
                /* zmm5 = b0 = B0, zmm6 = b2, zmm7 = b1 */
                __ASM_EMIT("vmovaps         0x80(%[MASK]), %%zmm12")            /* zmm12 = 1 */
                __ASM_EMIT("vmulps          %%zmm0, %%zmm4, %%zmm4")            /* zmm4  = T1 = t1 * kf */
                __ASM_EMIT("vmulps          %%zmm1, %%zmm2, %%zmm2")            /* zmm2  = T2 = t2 * kf2 */
                __ASM_EMIT("vmulps          %%zmm0, %%zmm7, %%zmm7")            /* zmm7  = B1 = b1 * kf */
                __ASM_EMIT("vmulps          %%zmm1, %%zmm6, %%zmm6")            /* zmm6  = B2 = b2 * kf2 */
                __ASM_EMIT("vaddps          %%zmm2, %%zmm3, %%zmm10")           /* zmm10 = T0 + T2 */
                __ASM_EMIT("vsubps          %%zmm5, %%zmm6, %%zmm8")            /* zmm8  = B2 - B0 */
                __ASM_EMIT("vsubps          %%zmm2, %%zmm3, %%zmm3")            /* zmm3  = T0 - T2 */
                __ASM_EMIT("vaddps          %%zmm5, %%zmm6, %%zmm5")            /* zmm5  = B2 + B0 */
                __ASM_EMIT("vaddps          %%zmm4, %%zmm10, %%zmm11")          /* zmm11 = T0 + T1 + T2 */
                __ASM_EMIT("vaddps          %%zmm8, %%zmm8, %%zmm8")            /* zmm8  = 2 * (B2 - B0) */
                __ASM_EMIT("vaddps          %%zmm3, %%zmm3, %%zmm3")            /* zmm3  = 2 * (T0 - T2) */
                __ASM_EMIT("vaddps          %%zmm5, %%zmm7, %%zmm9")            /* zmm9  = B0 + B1 + B2 */
                __ASM_EMIT("vsubps          %%zmm4, %%zmm10, %%zmm4")           /* zmm4  = T0 - T1 + T2 */
                __ASM_EMIT("vdivps          %%zmm9, %%zmm12, %%zmm9")           /* zmm9  = N = 1 / (B0 + B1 + B2) */
                __ASM_EMIT("vsubps          %%zmm5, %%zmm7, %%zmm5")            /* zmm5  = B1 - B2 - B0 */
                __ASM_EMIT("vmulps          %%zmm9, %%zmm11, %%zmm11")          /* zmm11 = B0 = (T0 + T1 + T2) * N */
                __ASM_EMIT("vmulps          %%zmm9, %%zmm3, %%zmm3")            /* zmm3  = B1 = 2 * (T0 - T2) * N */
                __ASM_EMIT("vmulps          %%zmm9, %%zmm4, %%zmm4")            /* zmm4  = B2 = (T0 - T1 + T2) * N */
                __ASM_EMIT("vmulps          %%zmm9, %%zmm8, %%zmm8")            /* zmm8  = A0 = 2 * (B2 - B0) * N */
                __ASM_EMIT("vmulps          %%zmm9, %%zmm5, %%zmm5")            /* zmm5  = A1 = (B1 - B2 - B0) * N */
                /* Store values */
                __ASM_EMIT("vmovaps         %%zmm11, 0x000(%[bf])")
                __ASM_EMIT("vmovaps         %%zmm3,  0x040(%[bf])")
                __ASM_EMIT("vmovaps         %%zmm4,  0x080(%[bf])")
                __ASM_EMIT("vmovaps         %%zmm8,  0x0c0(%[bf])")
                __ASM_EMIT("vmovaps         %%zmm5,  0x100(%[bf])")
                // Repeat cycle
                __ASM_EMIT("add             $0x200, %[bc]")
                __ASM_EMIT("add             $0x140,  %[bf]")
                __ASM_EMIT("dec             %[count]")
                __ASM_EMIT("jnz             1b")

                __ASM_EMIT("100:")
                : [count] "+r" (count), [bc] "+r" (bc), [bf] "+r" (bf)
                : [MASK] "r" (bilinear_transform_const),
                  [kf] "m" (kf)
                : "cc", "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_TRANSFORM_H_ */
