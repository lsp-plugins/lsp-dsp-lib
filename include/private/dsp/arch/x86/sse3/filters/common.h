/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE3_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE3_IMPL */


namespace lsp
{
    namespace sse3
    {
        IF_ARCH_X86_64(
            static const uint32_t biquad_mask_const[] __lsp_aligned16 =
            {
                0xffffffff, 0, 0, 0
            };
        );

        #define X64_FILTER_X8_CORE(B0, B1, B2, A1, A2) \
            /* In: xmm1 = s, xmm9 = r */ \
            /* Out: xmm2 = p1, xmm3 = p2, xmm10 = q1, xmm11 = q2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            /* xmm2     = s */ \
            __ASM_EMIT("movaps      %%xmm9, %%xmm10")                           /* xmm10    = r */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            /* xmm3     = s */ \
            __ASM_EMIT("movaps      %%xmm9, %%xmm11")                           /* xmm11    = r */ \
            __ASM_EMIT("mulps       0x00 + " B0 "(%[f]), %%xmm1")               /* xmm1     = s*b0 */ \
            __ASM_EMIT("mulps       0x10 + " B0 "(%[f]), %%xmm9")               /* xmm9     = r*j0 */ \
            __ASM_EMIT("mulps       0x00 + " B1 "(%[f]), %%xmm2")               /* xmm2     = s*b1 */ \
            __ASM_EMIT("mulps       0x10 + " B1 "(%[f]), %%xmm10")              /* xmm10    = r*j1 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            /* xmm1     = s*b0+d0 = s2 */ \
            __ASM_EMIT("addps       %%xmm14, %%xmm9")                           /* xmm9     = r*j0+d1 = r2 */ \
            __ASM_EMIT("mulps       0x00 + " B2 "(%[f]), %%xmm3")               /* xmm3     = s*b2 */ \
            __ASM_EMIT("mulps       0x10 + " B2 "(%[f]), %%xmm11")              /* xmm11    = r*j2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            /* xmm4     = s2 */ \
            __ASM_EMIT("movaps      %%xmm9, %%xmm12")                           /* xmm12    = r2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            /* xmm5     = s2 */ \
            __ASM_EMIT("movaps      %%xmm9, %%xmm13")                           /* xmm13    = r2 */ \
            __ASM_EMIT("mulps       0x00 + " A1 "(%[f]), %%xmm4")               /* xmm4     = s2*a1 */ \
            __ASM_EMIT("mulps       0x10 + " A1 "(%[f]), %%xmm12")              /* xmm12    = r2*i1 */ \
            __ASM_EMIT("mulps       0x00 + " A2 "(%[f]), %%xmm5")               /* xmm5     = s2*a2 */ \
            __ASM_EMIT("mulps       0x10 + " A2 "(%[f]), %%xmm13")              /* xmm13    = r2*i2 */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            /* xmm2     = s*b1 + s2*a1 = p1 */ \
            __ASM_EMIT("addps       %%xmm12, %%xmm10")                          /* xmm10    = r*j1 + r2*i1 = q1 */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            /* xmm3     = s*b2 + s2*a2 = p2 */ \
            __ASM_EMIT("addps       %%xmm13, %%xmm11")                          /* xmm11    = r*j2 + r2*i2 = q2 */

        #define X64_FILTER_X8_MASK_MEMSYNC \
            /* In:  xmm0 = MASK_LO, xmm8 = MASK_HI, */ \
            /*      xmm2 = p1, xmm3 = p2, xmm10=q1, xmm11=q2, */ \
            /*      xmm6 = d0, xmm7 = d2, xmm14=d1, xmm15=d3 */ \
            /* Out: xmm6 = d0', xmm7 = d2', xmm14=d1', xmm15=d3' */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            /* xmm4     = MASK_LO */ \
            __ASM_EMIT("movaps      %%xmm8, %%xmm12")                           /* xmm12    = MASK_HI */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            /* xmm5     = MASK_LO */ \
            __ASM_EMIT("movaps      %%xmm8, %%xmm13")                           /* xmm13    = MASK_HI */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            /* xmm2     = (p1 + d2) & MASK_LO */ \
            __ASM_EMIT("andps       %%xmm12, %%xmm10")                          /* xmm10    = (q1 + d3) & MASK_HI */ \
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            /* xmm3     = p2 & MASK_LO */ \
            __ASM_EMIT("andps       %%xmm13, %%xmm11")                          /* xmm11    = q2 & MASK_HI */ \
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            /* xmm4     = d0 & ~MASK_LO */ \
            __ASM_EMIT("andnps      %%xmm14, %%xmm12")                          /* xmm12    = d1 & ~MASK_HI */ \
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            /* xmm5     = d2 & ~MASK_LO */ \
            __ASM_EMIT("andnps      %%xmm15, %%xmm13")                          /* xmm13    = d3 & ~MASK_HI */ \
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            /* xmm4     = (p1 + d2) & MASK_LO | (d0 & ~MASK_LO) */ \
            __ASM_EMIT("orps        %%xmm10, %%xmm12")                          /* xmm12    = (q1 + d3) & MASK_HI | (d1 & ~MASK_HI) */ \
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            /* xmm5     = (p2 & MASK_LO) | (d2 & ~MASK_LO) */ \
            __ASM_EMIT("orps        %%xmm11, %%xmm13")                          /* xmm13    = (q2 & MASK_HI) | (d3 & ~MASK_HI) */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            /* xmm6     = d0' */ \
            __ASM_EMIT("movaps      %%xmm12, %%xmm14")                          /* xmm14    = d1' */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            /* xmm7     = d2' */ \
            __ASM_EMIT("movaps      %%xmm13, %%xmm15")                          /* xmm15    = d3' */

        #define X64_FILTER_X8_NOMASK_MEMSYNC \
            /* In:  xmm0 = MASK_LO, xmm8 = MASK_HI, */ \
            /*      xmm2 = p1, xmm3 = p2, xmm10=q1, xmm11=q2, */ \
            /*      xmm6 = d0, xmm7 = d2, xmm14=d1, xmm15=d3 */ \
            /* Out: xmm6 = d0', xmm7 = d2', xmm14=d1', xmm15=d3' */ \
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            /* xmm2     = p1 + d2 */ \
            __ASM_EMIT("addps       %%xmm15, %%xmm10")                          /* xmm10    = q1 + d3 */ \
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            /* xmm6     = d0' */ \
            __ASM_EMIT("movaps      %%xmm10, %%xmm14")                          /* xmm14    = d1' */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            /* xmm7     = d2' */ \
            __ASM_EMIT("movaps      %%xmm11, %%xmm15")                          /* xmm15    = d3' */


        #define X64_FILTER_X8       X64_FILTER_X8_CORE("0x00", "0x20", "0x40", "0x60", "0x80")
        #define X64_FILTER_X16P1    X64_FILTER_X8_CORE("0x00", "0x40", "0x80", "0xc0", "0x100")
        #define X64_FILTER_X16P2    X64_FILTER_X8_CORE("0x20", "0x60", "0xa0", "0xe0", "0x120")
    } /* namespace sse3 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE3_FILTERS_COMMON_H_ */
