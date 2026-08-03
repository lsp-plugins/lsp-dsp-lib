/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_PACK_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_PACK_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

#include <private/dsp/arch/x86/avx512/filters/common.h>

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86_64(
            static const uint32_t biquad_pack_const[] __lsp_aligned64 = {
                0x00, 0x01, 0x02, 0x03, 0x10, 0x11, 0x12, 0x13, 0x08, 0x09, 0x0a, 0x0b, 0x18, 0x19, 0x1a, 0x1b,
                0x00, 0x08, 0x01, 0x09, 0x02, 0x0a, 0x03, 0x0b, 0x10, 0x18, 0x11, 0x19, 0x12, 0x1a, 0x13, 0x1b,
                0x04, 0x0c, 0x05, 0x0d, 0x06, 0x0e, 0x07, 0x0f, 0x14, 0x1c, 0x15, 0x1d, 0x16, 0x1e, 0x17, 0x1f,
                0x00, 0x08, 0x01, 0x09, 0x02, 0x0a, 0x03, 0x0b, 0x04, 0x0c, 0x05, 0x0d, 0x06, 0x0e, 0x07, 0x0f
            };
        )

        void x64_biquad_pack_x16(dsp::biquad_x16_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovups         0x000(%[src]), %%zmm0")                 /* zmm0 = b0[0 ] b1[0 ] b2[0 ] a1[0 ] a2[0 ] ?      ?      ?      b0[1 ] b1[1 ] b2[1 ] a1[1 ] a2[1 ] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x040(%[src]), %%zmm1")                 /* zmm1 = b0[2 ] b1[2 ] b2[2 ] a1[2 ] a2[2 ] ?      ?      ?      b0[3 ] b1[3 ] b2[3 ] a1[3 ] a2[3 ] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x080(%[src]), %%zmm2")                 /* zmm2 = b0[4 ] b1[4 ] b2[4 ] a1[4 ] a2[4 ] ?      ?      ?      b0[5 ] b1[5 ] b2[5 ] a1[5 ] a2[5 ] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x0c0(%[src]), %%zmm3")                 /* zmm3 = b0[6 ] b1[6 ] b2[6 ] a1[6 ] a2[6 ] ?      ?      ?      b0[7 ] b1[7 ] b2[7 ] a1[7 ] a2[7 ] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x100(%[src]), %%zmm4")                 /* zmm4 = b0[8 ] b1[8 ] b2[8 ] a1[8 ] a2[8 ] ?      ?      ?      b0[9 ] b1[9 ] b2[9 ] a1[9 ] a2[9 ] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x140(%[src]), %%zmm5")                 /* zmm5 = b0[10] b1[10] b2[10] a1[10] a2[10] ?      ?      ?      b0[11] b1[11] b2[11] a1[11] a2[11] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x180(%[src]), %%zmm6")                 /* zmm6 = b0[12] b1[12] b2[12] a1[12] a2[12] ?      ?      ?      b0[13] b1[13] b2[13] a1[13] a2[13] ?      ?      ?      */
                __ASM_EMIT("vmovups         0x1c0(%[src]), %%zmm7")                 /* zmm7 = b0[14] b1[14] b2[14] a1[14] a2[14] ?      ?      ?      b0[15] b1[15] b2[15] a1[15] a2[15] ?      ?      ?      */

                __ASM_EMIT("vunpckhps       %%zmm1, %%zmm0, %%zmm8")                /* zmm8 = b2[0 ] b2[2 ] a1[0 ] a1[2 ] ?      ?      ?      ?      b2[1 ] b2[3 ] a1[1 ] a1[3 ] ?      ?      ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm3, %%zmm2, %%zmm9")                /* zmm9 = b2[4 ] b2[6 ] a1[4 ] a1[6 ] ?      ?      ?      ?      b2[5 ] b2[7 ] a1[5 ] a1[7 ] ?      ?      ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm5, %%zmm4, %%zmm10")               /* zmm10= b2[8 ] b2[10] a1[8 ] a1[10] ?      ?      ?      ?      b2[9 ] b2[11] a1[9 ] a1[11] ?      ?      ?      ?      */
                __ASM_EMIT("vunpckhps       %%zmm7, %%zmm6, %%zmm11")               /* ymm11= b2[12] b2[14] a1[12] a1[14] ?      ?      ?      ?      b2[13] b2[15] a1[13] a1[15] ?      ?      ?      ?      */
                __ASM_EMIT("vmovaps         0x00(%[CC]), %%zmm12")
                __ASM_EMIT("vmovaps         0x40(%[CC]), %%zmm13")
                __ASM_EMIT("vmovaps         0x80(%[CC]), %%zmm14")
                __ASM_EMIT("vmovaps         0xc0(%[CC]), %%zmm15")
                __ASM_EMIT("vunpcklps       %%zmm1, %%zmm0, %%zmm0")                /* zmm0 = b0[0 ] b0[2 ] b1[0 ] b1[2 ] a2[0 ] a2[2 ] ?      ?      b0[1 ] b0[3 ] b1[1 ] b1[3 ] a2[1 ] a2[3 ] ?      ?      */
                __ASM_EMIT("vunpcklps       %%zmm3, %%zmm2, %%zmm2")                /* zmm2 = b0[4 ] b0[6 ] b1[4 ] b1[6 ] a2[4 ] a2[6 ] ?      ?      b0[5 ] b0[7 ] b1[5 ] b1[7 ] a2[5 ] a2[7 ] ?      ?      */
                __ASM_EMIT("vunpcklps       %%zmm5, %%zmm4, %%zmm4")                /* zmm4 = b0[8 ] b0[10] b1[8 ] b1[10] a2[8 ] a2[10] ?      ?      b0[9 ] b0[11] b1[9 ] b1[11] a2[9 ] a2[11] ?      ?      */
                __ASM_EMIT("vunpcklps       %%zmm7, %%zmm6, %%zmm6")                /* ymm6 = b0[12] b0[14] b1[12] b1[14] a2[12] a2[14] ?      ?      b0[13] b0[15] b1[13] b1[15] a2[13] a2[15] ?      ?      */
                __ASM_EMIT("vpermt2ps       %%zmm10, %%zmm12, %%zmm8")              /* zmm8 = b2[0 ] b2[2 ] a1[0 ] a1[2 ] b2[8 ] b2[10] a1[8 ] a1[10] b2[1 ] b2[3 ] a1[1 ] a1[3 ] b2[9 ] b2[11] a1[9 ] a1[11] */
                __ASM_EMIT("vpermt2ps       %%zmm11, %%zmm12, %%zmm9")              /* zmm9 = b2[4 ] b2[6 ] a1[4 ] a1[6 ] b2[12] b2[14] a1[12] a1[14] b2[5 ] b2[7 ] a1[5 ] a1[7 ] b2[13] b2[15] a1[13] a1[15] */

                __ASM_EMIT("vshufps         $0xee, %%zmm2, %%zmm0, %%zmm1")         /* zmm1 = b1[0 ] b1[2 ] b1[4 ] b1[6 ] ?      ?      ?      ?      b1[1 ] b1[3 ] b1[5 ] b1[7 ] ?      ?      ?      ?      */
                __ASM_EMIT("vshufps         $0xee, %%zmm6, %%zmm4, %%zmm3")         /* zmm3 = b1[8 ] b1[10] b1[12] b1[14] ?      ?      ?      ?      b1[9 ] b1[11] b1[13] b1[15] ?      ?      ?      ?      */
                __ASM_EMIT("vshufps         $0xee, %%zmm9, %%zmm8, %%zmm5")         /* zmm5 = a1[0 ] a1[2 ] a1[4 ] a1[6 ] a1[8 ] a1[10] a1[12] a1[14] a1[1 ] a1[3 ] a1[5 ] a1[7 ] a1[9 ] a1[11] a1[13] a1[15] */
                __ASM_EMIT("vshufps         $0x44, %%zmm2, %%zmm0, %%zmm2")         /* zmm2 = b0[0 ] b0[2 ] b0[4 ] b0[6 ] a2[0 ] a2[2 ] a2[4 ] a2[6 ] b0[1 ] b0[3 ] b0[5 ] b0[7 ] a2[1 ] a2[3 ] a2[5 ] a2[7 ] */
                __ASM_EMIT("vshufps         $0x44, %%zmm6, %%zmm4, %%zmm4")         /* zmm4 = b0[8 ] b0[10] b0[12] b0[14] a2[8 ] a2[10] a2[12] a2[14] b0[9 ] b0[11] b0[13] b0[15] a2[9 ] a2[11] a2[13] a2[15] */
                __ASM_EMIT("vshufps         $0x44, %%zmm9, %%zmm8, %%zmm8")         /* zmm8 = b2[0 ] b2[2 ] b2[4 ] b2[6 ] b2[8 ] b2[10] b2[12] b2[14] b2[1 ] b2[3 ] b2[5 ] b2[7 ] b2[9 ] b2[11] b2[13] b2[15] */

                __ASM_EMIT("vmovaps         %%zmm2, %%zmm10")
                __ASM_EMIT("vpermt2ps       %%zmm3, %%zmm13, %%zmm1")               /* zmm1 = b1[0..15] */
                __ASM_EMIT("vpermps         %%zmm5, %%zmm15, %%zmm5")               /* zmm5 = a1[0..15] */
                __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm13, %%zmm10")              /* zmm10= b0[0..15] */
                __ASM_EMIT("vpermps         %%zmm8, %%zmm15, %%zmm8")               /* zmm8 = b2[0..15] */
                __ASM_EMIT("vpermt2ps       %%zmm4, %%zmm14, %%zmm2")               /* zmm2 = a2[0..15] */

                __ASM_EMIT("vmovups         %%zmm10, 0x000(%[dst])")
                __ASM_EMIT("vmovups         %%zmm1, 0x040(%[dst])")
                __ASM_EMIT("vmovups         %%zmm8, 0x080(%[dst])")
                __ASM_EMIT("vmovups         %%zmm5, 0x0c0(%[dst])")
                __ASM_EMIT("vmovups         %%zmm2, 0x100(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src), [CC] "r" (biquad_pack_const)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

    } /* namespace avx512 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_PACK_H_ */
