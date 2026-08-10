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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_H_
#define PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
    #define BIQUAD_PACK_X8_CORE(base) \
        __ASM_EMIT("vmovups     0x00 + " base "(%[src]), %%ymm0")       /* ymm0 = b0[0] b1[0] b2[0] a1[0] a2[0] ? ? ? */ \
        __ASM_EMIT("vmovups     0x20 + " base "(%[src]), %%ymm1")       /* ymm1 = b0[1] b1[1] b2[1] a1[1] a2[1] ? ? ? */ \
        __ASM_EMIT("vmovups     0x40 + " base "(%[src]), %%ymm2")       /* ymm2 = b0[2] b1[2] b2[2] a1[2] a2[2] ? ? ? */ \
        __ASM_EMIT("vmovups     0x60 + " base "(%[src]), %%ymm3")       /* ymm3 = b0[3] b1[3] b2[3] a1[3] a2[3] ? ? ? */ \
        __ASM_EMIT("vmovups     0x80 + " base "(%[src]), %%ymm4")       /* ymm0 = b0[4] b1[4] b2[4] a1[4] a2[4] ? ? ? */ \
        __ASM_EMIT("vmovups     0xa0 + " base "(%[src]), %%ymm5")       /* ymm1 = b0[5] b1[5] b2[5] a1[5] a2[5] ? ? ? */ \
        __ASM_EMIT("vmovups     0xc0 + " base "(%[src]), %%ymm6")       /* ymm2 = b0[6] b1[6] b2[6] a1[6] a2[6] ? ? ? */ \
        __ASM_EMIT("vmovups     0xe0 + " base "(%[src]), %%ymm7")       /* ymm3 = b0[7] b1[7] b2[7] a1[7] a2[7] ? ? ? */ \
        \
        __ASM_EMIT("vunpcklps   %%ymm1, %%ymm0, %%ymm8")                /* ymm8 = b0[0] b0[1] b1[0] b1[1] a2[0] a2[1] ? ? */ \
        __ASM_EMIT("vunpcklps   %%ymm3, %%ymm2, %%ymm9")                /* ymm9 = b0[2] b0[3] b1[2] b1[3] a2[2] a2[3] ? ? */ \
        __ASM_EMIT("vunpcklps   %%ymm5, %%ymm4, %%ymm10")               /* ymm10= b0[4] b0[5] b1[4] b1[5] a2[4] a2[5] ? ? */ \
        __ASM_EMIT("vunpcklps   %%ymm7, %%ymm6, %%ymm11")               /* ymm11= b0[6] b0[7] b1[6] b1[7] a2[6] a2[7] ? ? */ \
        __ASM_EMIT("vunpckhps   %%ymm1, %%ymm0, %%ymm0")                /* ymm0 = b2[0] b2[1] a1[0] a1[1] ? ? ? ? */ \
        __ASM_EMIT("vunpckhps   %%ymm3, %%ymm2, %%ymm2")                /* ymm2 = b2[2] b2[3] a1[2] a1[3] ? ? ? ? */ \
        __ASM_EMIT("vunpckhps   %%ymm5, %%ymm4, %%ymm4")                /* ymm4 = b2[4] b2[5] a1[4] a1[5] ? ? ? ? */ \
        __ASM_EMIT("vunpckhps   %%ymm7, %%ymm6, %%ymm6")                /* ymm6 = b2[6] b2[7] a1[6] a1[7] ? ? ? ? */ \
        __ASM_EMIT("vinsertf128 $1, %%xmm4, %%ymm0, %%ymm0")            /* ymm0 = b2[0] b2[1] a1[0] a1[1] b2[4] b2[5] a1[4] a1[5] */ \
        __ASM_EMIT("vinsertf128 $1, %%xmm6, %%ymm2, %%ymm2")            /* ymm2 = b2[2] b2[3] a1[2] a1[3] b2[6] b2[7] a1[6] a1[7] */ \
        \
        __ASM_EMIT("vshufps     $0x44, %%ymm9, %%ymm8, %%ymm1")         /* ymm1 = b0[0] b0[1] b0[2] b0[3] a2[0] a2[1] a2[2] a2[3] */ \
        __ASM_EMIT("vshufps     $0x44, %%ymm11, %%ymm10, %%ymm3")       /* ymm3 = b0[4] b0[5] b0[6] b0[7] a2[4] a2[5] a2[6] a2[7] */ \
        __ASM_EMIT("vshufps     $0x44, %%ymm2, %%ymm0, %%ymm5")         /* ymm5 = b2[0] b2[1] b2[2] b2[3] b2[4] b2[5] b2[6] b2[7] */ \
        __ASM_EMIT("vshufps     $0xee, %%ymm9, %%ymm8, %%ymm8")         /* ymm8 = b1[0] b1[1] b1[2] b1[3] ? ? ? ? */ \
        __ASM_EMIT("vshufps     $0xee, %%ymm11, %%ymm10, %%ymm10")      /* ymm10= b1[4] b1[5] b1[6] b1[7] ? ? ? ? */ \
        __ASM_EMIT("vshufps     $0xee, %%ymm2, %%ymm0, %%ymm0")         /* ymm0 = a1[0] a1[1] a1[2] a1[3] a1[4] a1[5] a1[6] a1[7] */ \
        \
        __ASM_EMIT("vinsertf128 $1, %%xmm3, %%ymm1, %%ymm4")            /* ymm4 = b0[0] b0[1] b0[2] b0[3] b0[4] b0[5] b0[6] b0[7] */ \
        __ASM_EMIT("vinsertf128 $1, %%xmm10, %%ymm8, %%ymm8")           /* ymm8 = b1[0] b1[1] b1[2] b1[3] b1[4] b1[5] b1[6] b1[7] */ \
        __ASM_EMIT("vperm2f128  $0x31, %%ymm3, %%ymm1, %%ymm3")         /* ymm3 = a2[0] a2[1] a2[2] a2[3] b0[4] b0[5] b0[6] b0[7] */

        void x64_biquad_pack_x8(dsp::biquad_x8_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_64_ASM(
                BIQUAD_PACK_X8_CORE("0x000")
                __ASM_EMIT("vmovups     %%ymm4, 0x000(%[dst])")
                __ASM_EMIT("vmovups     %%ymm8, 0x020(%[dst])")
                __ASM_EMIT("vmovups     %%ymm5, 0x040(%[dst])")
                __ASM_EMIT("vmovups     %%ymm0, 0x060(%[dst])")
                __ASM_EMIT("vmovups     %%ymm3, 0x080(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11"
            );
        }

        void x64_biquad_pack_x16(dsp::biquad_x16_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_64_ASM(
                BIQUAD_PACK_X8_CORE("0x000")
                __ASM_EMIT("vmovups     %%ymm4, 0x000(%[dst])")
                __ASM_EMIT("vmovups     %%ymm8, 0x040(%[dst])")
                __ASM_EMIT("vmovups     %%ymm5, 0x080(%[dst])")
                __ASM_EMIT("vmovups     %%ymm0, 0x0c0(%[dst])")
                __ASM_EMIT("vmovups     %%ymm3, 0x100(%[dst])")

                BIQUAD_PACK_X8_CORE("0x100")
                __ASM_EMIT("vmovups     %%ymm4, 0x020(%[dst])")
                __ASM_EMIT("vmovups     %%ymm8, 0x060(%[dst])")
                __ASM_EMIT("vmovups     %%ymm5, 0x0a0(%[dst])")
                __ASM_EMIT("vmovups     %%ymm0, 0x0e0(%[dst])")
                __ASM_EMIT("vmovups     %%ymm3, 0x120(%[dst])")
                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11"
            );
        }

    } /* namespace avx */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_X86_AVX_FILTERS_PACK_H_ */
