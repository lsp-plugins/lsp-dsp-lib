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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_FILTERS_PACK_H_
#define PRIVATE_DSP_ARCH_X86_SSE_FILTERS_PACK_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {

        void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                __ASM_EMIT("movups      0x00(%[src]), %%xmm0")  // xmm0 = b0[0] b1[0] b2[0] a1[0]
                __ASM_EMIT("movss       0x10(%[src]), %%xmm1")  // xmm1 = a2[0] 0 0 0
                __ASM_EMIT("movups      0x20(%[src]), %%xmm2")  // xmm2 = b0[1] b1[1] b2[1] a1[1]
                __ASM_EMIT("movaps      %%xmm0, %%xmm3")        // xmm3 = b0[0] b1[0] b2[0] a1[0]
                __ASM_EMIT("movhps      0x30(%[src]), %%xmm1")  // xmm1 = a2[0] 0 a2[1] ?
                __ASM_EMIT("unpcklps    %%xmm2, %%xmm0")        // xmm0 = b0[0] b0[1] b1[0] b1[1]
                __ASM_EMIT("shufps      $0x58, %%xmm1, %%xmm1") // xmm1 = a2[0] a2[1] 0 0
                __ASM_EMIT("unpckhps    %%xmm2, %%xmm3")        // xmm3 = b2[0] b2[1] a1[0] a1[2]
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x20(%[dst])")
                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3"
            );
        }

        #define BIQUAD_PACK_X4_CORE(base) \
            __ASM_EMIT("movups      0x00 + " base "(%[src]), %%xmm0")   /* xmm0 = b0[0] b1[0] b2[0] a1[0] */ \
            __ASM_EMIT("movlps      0x10 + " base "(%[src]), %%xmm1")   /* xmm1 = a2[0] ? ? ? */ \
            __ASM_EMIT("movups      0x20 + " base "(%[src]), %%xmm2")   /* xmm2 = b0[1] b1[1] b2[1] a1[1] */ \
            __ASM_EMIT("movhps      0x30 + " base "(%[src]), %%xmm1")   /* xmm1 = a2[0] ? a2[1] ? */ \
            __ASM_EMIT("movups      0x40 + " base "(%[src]), %%xmm3")   /* xmm3 = b0[2] b1[2] b2[2] a1[2] */ \
            __ASM_EMIT("movlps      0x50 + " base "(%[src]), %%xmm4")   /* xmm4 = a2[2] ? ? ? */ \
            __ASM_EMIT("movups      0x60 + " base "(%[src]), %%xmm5")   /* xmm5 = b0[3] b1[3] b2[3] a1[3] */ \
            __ASM_EMIT("movhps      0x70 + " base "(%[src]), %%xmm4")   /* xmm4 = a2[2] ? a2[3] ? */ \
            \
            __ASM_EMIT("movaps      %%xmm0, %%xmm6")                    /* xmm6 = b0[0] b1[0] b2[0] a1[0] */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                    /* xmm7 = b0[2] b1[2] b2[2] a1[2] */ \
            __ASM_EMIT("unpcklps    %%xmm2, %%xmm0")                    /* xmm0 = b0[0] b0[1] b1[0] b1[1] */ \
            __ASM_EMIT("unpcklps    %%xmm5, %%xmm3")                    /* xmm3 = b0[2] b0[3] b1[2] b1[3] */ \
            __ASM_EMIT("unpckhps    %%xmm2, %%xmm6")                    /* xmm6 = b2[0] b2[1] a1[0] a1[1] */ \
            __ASM_EMIT("unpckhps    %%xmm5, %%xmm7")                    /* xmm7 = b2[2] b2[3] a1[2] a1[3] */ \
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm1")             /* xmm1 = a2[0] a2[1] a2[2] a2[3] */ \
            \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = b0[0] b0[1] b1[0] b1[1] */ \
            __ASM_EMIT("movaps      %%xmm6, %%xmm5")                    /* xmm5 = b2[0] b2[1] a1[0] a1[1] */ \
            __ASM_EMIT("movlhps     %%xmm3, %%xmm0")                    /* xmm0 = b0[0] b0[1] b0[2] b0[3] */ \
            __ASM_EMIT("movlhps     %%xmm7, %%xmm6")                    /* xmm6 = b2[0] b2[1] b2[2] b2[3] */ \
            __ASM_EMIT("movhlps     %%xmm2, %%xmm3")                    /* xmm3 = b1[0] b1[1] b1[2] b1[3] */ \
            __ASM_EMIT("movhlps     %%xmm5, %%xmm7")                    /* xmm7 = a1[0] a1[1] a1[2] a1[3] */

        void biquad_pack_x4(dsp::biquad_x4_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                BIQUAD_PACK_X4_CORE("0x00")

                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x30(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x40(%[dst])")
                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void biquad_pack_x8(dsp::biquad_x8_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                BIQUAD_PACK_X4_CORE("0x00")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x40(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x60(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x80(%[dst])")

                BIQUAD_PACK_X4_CORE("0x80")
                __ASM_EMIT("movups      %%xmm0, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x30(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x50(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x70(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x90(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }

        void biquad_pack_x16(dsp::biquad_x16_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_ASM(
                BIQUAD_PACK_X4_CORE("0x000")
                __ASM_EMIT("movups      %%xmm0, 0x000(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x040(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x080(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x0c0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x100(%[dst])")

                BIQUAD_PACK_X4_CORE("0x080")
                __ASM_EMIT("movups      %%xmm0, 0x010(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x050(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x090(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x0d0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x110(%[dst])")

                BIQUAD_PACK_X4_CORE("0x100")
                __ASM_EMIT("movups      %%xmm0, 0x020(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x060(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x0a0(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x0e0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x120(%[dst])")

                BIQUAD_PACK_X4_CORE("0x180")
                __ASM_EMIT("movups      %%xmm0, 0x030(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x070(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x0b0(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x0f0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x130(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_FILTERS_PACK_H_ */
