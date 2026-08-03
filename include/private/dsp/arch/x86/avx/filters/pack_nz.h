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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_NZ_H_
#define PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_NZ_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovups     0x00(%[src]), %%xmm0")                  // xmm0 = b0[0] b1[0] b2[0] a1[0]
                __ASM_EMIT("vmovss      0x10(%[src]), %%xmm1")                  // xmm1 = a2[0] 0 0 0
                __ASM_EMIT("vmovups     0x20(%[src]), %%xmm2")                  // xmm2 = b0[1] b1[1] b2[1] a1[1]
                __ASM_EMIT("vmovhps     0x30(%[src]), %%xmm1, %%xmm1")          // xmm1 = a2[0] 0 a2[1] ?
                __ASM_EMIT("vunpckhps   %%xmm2, %%xmm0, %%xmm3")                // xmm3 = b0[0] b0[1] b1[0] b1[1]
                __ASM_EMIT("vunpcklps   %%xmm2, %%xmm0, %%xmm0")                // xmm0 = b0[0] b0[1] b1[0] b1[1]
                __ASM_EMIT("vshufps     $0x58, %%xmm1, %%xmm1, %%xmm1")         // xmm1 = a2[0] a2[1] 0 0
                __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups     %%xmm3, 0x10(%[dst])")
                __ASM_EMIT("vmovups     %%xmm1, 0x20(%[dst])")
                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        void biquad_pack_x4(dsp::biquad_x4_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("vmovups     0x00(%[src]), %%xmm0")                  // xmm0 = b0[0] b1[0] b2[0] a1[0]
                __ASM_EMIT("vmovss      0x10(%[src]), %%xmm1")                  // xmm1 = a2[0] 0 0 0
                __ASM_EMIT("vmovups     0x20(%[src]), %%xmm2")                  // xmm2 = b0[1] b1[1] b2[1] a1[1]
                __ASM_EMIT("vinsertps   $0x10, 0x30(%[src]), %%xmm1, %%xmm1")   // xmm1 = a2[0] a2[1] 0 0
                __ASM_EMIT("vmovups     0x40(%[src]), %%xmm3")                  // xmm3 = b0[2] b1[2] b2[2] a1[2]
                __ASM_EMIT("vmovhps     0x50(%[src]), %%xmm1, %%xmm1")          // xmm1 = a2[0] a2[1] a2[2] ?
                __ASM_EMIT("vmovups     0x60(%[src]), %%xmm4")                  // xmm4 = b0[3] b1[3] b2[3] a1[3]
                __ASM_EMIT("vinsertps   $0x30, 0x70(%[src]), %%xmm1, %%xmm1")   // xmm1 = a2[0] a2[1] a2[2] a2[3]

                __ASM_EMIT("vunpckhps   %%xmm2, %%xmm0, %%xmm5")                // xmm5 = b2[0] b2[1] a1[0] a1[1]
                __ASM_EMIT("vunpckhps   %%xmm4, %%xmm3, %%xmm6")                // xmm6 = b2[2] b2[3] a1[2] a1[3]
                __ASM_EMIT("vunpcklps   %%xmm2, %%xmm0, %%xmm0")                // xmm0 = b0[0] b0[1] b1[0] b1[1]
                __ASM_EMIT("vunpcklps   %%xmm4, %%xmm3, %%xmm3")                // xmm3 = b0[2] b0[3] b1[2] b1[3]
                __ASM_EMIT("vmovhlps    %%xmm0, %%xmm3, %%xmm4")                // xmm4 = b1[0] b1[1] b1[2] b1[3]
                __ASM_EMIT("vmovhlps    %%xmm5, %%xmm6, %%xmm2")                // xmm2 = a1[0] a1[1] a1[2] a1[3]
                __ASM_EMIT("vmovlhps    %%xmm3, %%xmm0, %%xmm0")                // xmm0 = b0[0] b0[1] b0[2] b0[3]
                __ASM_EMIT("vmovlhps    %%xmm6, %%xmm5, %%xmm5")                // xmm5 = b2[0] b2[1] b2[2] b2[3]
                __ASM_EMIT("vmovups     %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("vmovups     %%xmm4, 0x10(%[dst])")
                __ASM_EMIT("vmovups     %%xmm5, 0x20(%[dst])")
                __ASM_EMIT("vmovups     %%xmm2, 0x30(%[dst])")
                __ASM_EMIT("vmovups     %%xmm1, 0x40(%[dst])")
                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6"
            );
        }

    } /* namespace avx */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_NZ_H_ */
