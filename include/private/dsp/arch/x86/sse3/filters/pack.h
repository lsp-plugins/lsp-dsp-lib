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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_PACK_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_PACK_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */

namespace lsp
{
    namespace sse3
    {

        #define BIQUAD_PACK_X8_CORE(base) \
            __ASM_EMIT("movups      0x00 + " base "(%[src]), %%xmm0")   /* xmm0 = b0[0] b1[0] b2[0] a1[0] */ \
            __ASM_EMIT("movlps      0x10 + " base "(%[src]), %%xmm1")   /* xmm1 = a2[0] ? ? ? */ \
            __ASM_EMIT("movups      0x20 + " base "(%[src]), %%xmm2")   /* xmm2 = b0[1] b1[1] b2[1] a1[1] */ \
            __ASM_EMIT("movhps      0x30 + " base "(%[src]), %%xmm1")   /* xmm1 = a2[0] ? a2[1] ? */ \
            __ASM_EMIT("movups      0x40 + " base "(%[src]), %%xmm3")   /* xmm3 = b0[2] b1[2] b2[2] a1[2] */ \
            __ASM_EMIT("movlps      0x50 + " base "(%[src]), %%xmm4")   /* xmm4 = a2[2] ? ? ? */ \
            __ASM_EMIT("movups      0x60 + " base "(%[src]), %%xmm5")   /* xmm5 = b0[3] b1[3] b2[3] a1[3] */ \
            __ASM_EMIT("movhps      0x70 + " base "(%[src]), %%xmm4")   /* xmm4 = a2[2] ? a2[3] ? */ \
            __ASM_EMIT("movups      0x80 + " base "(%[src]), %%xmm8") \
            __ASM_EMIT("movlps      0x90 + " base "(%[src]), %%xmm9") \
            __ASM_EMIT("movups      0xa0 + " base "(%[src]), %%xmm10") \
            __ASM_EMIT("movhps      0xb0 + " base "(%[src]), %%xmm9") \
            __ASM_EMIT("movups      0xc0 + " base "(%[src]), %%xmm11") \
            __ASM_EMIT("movlps      0xd0 + " base "(%[src]), %%xmm12") \
            __ASM_EMIT("movups      0xe0 + " base "(%[src]), %%xmm13") \
            __ASM_EMIT("movhps      0xf0 + " base "(%[src]), %%xmm12") \
            \
            __ASM_EMIT("movaps      %%xmm0, %%xmm6")                    /* xmm6 = b0[0] b1[0] b2[0] a1[0] */ \
            __ASM_EMIT("movaps      %%xmm8, %%xmm14") \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                    /* xmm7 = b0[2] b1[2] b2[2] a1[2] */ \
            __ASM_EMIT("movaps      %%xmm11, %%xmm15") \
            __ASM_EMIT("unpcklps    %%xmm2, %%xmm0")                    /* xmm0 = b0[0] b0[1] b1[0] b1[1] */ \
            __ASM_EMIT("unpcklps    %%xmm10, %%xmm8") \
            __ASM_EMIT("unpcklps    %%xmm5, %%xmm3")                    /* xmm3 = b0[2] b0[3] b1[2] b1[3] */ \
            __ASM_EMIT("unpcklps    %%xmm13, %%xmm11") \
            __ASM_EMIT("unpckhps    %%xmm2, %%xmm6")                    /* xmm6 = b2[0] b2[1] a1[0] a1[1] */ \
            __ASM_EMIT("unpckhps    %%xmm10, %%xmm14") \
            __ASM_EMIT("unpckhps    %%xmm5, %%xmm7")                    /* xmm7 = b2[2] b2[3] a1[2] a1[3] */ \
            __ASM_EMIT("unpckhps    %%xmm13, %%xmm15") \
            __ASM_EMIT("shufps      $0x88, %%xmm4, %%xmm1")             /* xmm1 = a2[0] a2[1] a2[2] a2[3] */ \
            __ASM_EMIT("shufps      $0x88, %%xmm12, %%xmm9") \
            \
            __ASM_EMIT("movaps      %%xmm0, %%xmm2")                    /* xmm2 = b0[0] b0[1] b1[0] b1[1] */ \
            __ASM_EMIT("movaps      %%xmm8, %%xmm10") \
            __ASM_EMIT("movaps      %%xmm6, %%xmm5")                    /* xmm5 = b2[0] b2[1] a1[0] a1[1] */ \
            __ASM_EMIT("movaps      %%xmm14, %%xmm13") \
            __ASM_EMIT("movlhps     %%xmm3, %%xmm0")                    /* xmm0 = b0[0] b0[1] b0[2] b0[3] */ \
            __ASM_EMIT("movlhps     %%xmm11, %%xmm8") \
            __ASM_EMIT("movlhps     %%xmm7, %%xmm6")                    /* xmm6 = b2[0] b2[1] b2[2] b2[3] */ \
            __ASM_EMIT("movlhps     %%xmm15, %%xmm14") \
            __ASM_EMIT("movhlps     %%xmm2, %%xmm3")                    /* xmm3 = b1[0] b1[1] b1[2] b1[3] */ \
            __ASM_EMIT("movhlps     %%xmm10, %%xmm11") \
            __ASM_EMIT("movhlps     %%xmm5, %%xmm7")                    /* xmm7 = a1[0] a1[1] a1[2] a1[3] */ \
            __ASM_EMIT("movhlps     %%xmm13, %%xmm15")

        void x64_biquad_pack_x8(dsp::biquad_x8_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_64_ASM(
                BIQUAD_PACK_X8_CORE("0x00")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm8, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm11, 0x30(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x40(%[dst])")
                __ASM_EMIT("movups      %%xmm14, 0x50(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0x60(%[dst])")
                __ASM_EMIT("movups      %%xmm15, 0x70(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x80(%[dst])")
                __ASM_EMIT("movups      %%xmm9, 0x90(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7",
                  "%xmm8", "%xmm9", "%xmm10", "%xmm11",
                  "%xmm12", "%xmm13", "%xmm14", "%xmm15"
            );
        }

        void x64_biquad_pack_x16(dsp::biquad_x16_t *dst, const dsp::biquad_x1_t *src)
        {
            ARCH_X86_64_ASM(
                BIQUAD_PACK_X8_CORE("0x000")
                __ASM_EMIT("movups      %%xmm0, 0x00(%[dst])")
                __ASM_EMIT("movups      %%xmm8, 0x10(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x40(%[dst])")
                __ASM_EMIT("movups      %%xmm11, 0x50(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0x80(%[dst])")
                __ASM_EMIT("movups      %%xmm14, 0x90(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0xc0(%[dst])")
                __ASM_EMIT("movups      %%xmm15, 0xd0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x100(%[dst])")
                __ASM_EMIT("movups      %%xmm9, 0x110(%[dst])")

                BIQUAD_PACK_X8_CORE("0x100")
                __ASM_EMIT("movups      %%xmm0, 0x20(%[dst])")
                __ASM_EMIT("movups      %%xmm8, 0x30(%[dst])")
                __ASM_EMIT("movups      %%xmm3, 0x60(%[dst])")
                __ASM_EMIT("movups      %%xmm11, 0x70(%[dst])")
                __ASM_EMIT("movups      %%xmm6, 0xa0(%[dst])")
                __ASM_EMIT("movups      %%xmm14, 0xb0(%[dst])")
                __ASM_EMIT("movups      %%xmm7, 0xe0(%[dst])")
                __ASM_EMIT("movups      %%xmm15, 0xf0(%[dst])")
                __ASM_EMIT("movups      %%xmm1, 0x120(%[dst])")
                __ASM_EMIT("movups      %%xmm9, 0x130(%[dst])")

                :
                : [dst] "r" (dst), [src] "r" (src)
                : "memory",
                  "%xmm0", "%xmm1", "%xmm2", "%xmm3",
                  "%xmm4", "%xmm5", "%xmm6", "%xmm7"
            );
        }


    } /* namespace sse3 */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_PACK_H_ */
