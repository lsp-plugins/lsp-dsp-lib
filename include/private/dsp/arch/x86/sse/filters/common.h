/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 25 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_SSE_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_X86_SSE_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_X86_SSE_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_SSE_IMPL */

namespace lsp
{
    namespace sse
    {
        IF_ARCH_X86(
            static const uint32_t biquad_mask_const[] __lsp_aligned16 =
            {
                0xffffffff, 0, 0, 0
            };
        );

        #define FILTER_X4_CORE_BASE(A0, A1, A2, B1, B2) \
            /* In: xmm1 = s */ \
            /* Out: xmm1 = s2, xmm2 = p1, xmm3 = p2 */ \
            /* Clobber: xmm4, xmm5 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm2")                            /* xmm2     = s */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm3")                            /* xmm3     = s */ \
            __ASM_EMIT("mulps       " A0 "(%[f]), %%xmm1")                      /* xmm1     = s*a0 */ \
            __ASM_EMIT("mulps       " A1 "(%[f]), %%xmm2")                      /* xmm2     = s*a1 */ \
            __ASM_EMIT("addps       %%xmm6, %%xmm1")                            /* xmm1     = s2 = s*a0+d0 */ \
            __ASM_EMIT("mulps       " A2 "(%[f]), %%xmm3")                      /* xmm3     = s*a2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm4")                            /* xmm4     = s2 */ \
            __ASM_EMIT("movaps      %%xmm1, %%xmm5")                            /* xmm5     = s2 */ \
            __ASM_EMIT("mulps       " B1 "(%[f]), %%xmm4")                      /* xmm4     = s2*b1 */ \
            __ASM_EMIT("mulps       " B2 "(%[f]), %%xmm5")                      /* xmm5     = s2*b2 */ \
            __ASM_EMIT("addps       %%xmm4, %%xmm2")                            /* xmm2     = s*a1 + s2*b1 = p1 */ \
            __ASM_EMIT("addps       %%xmm5, %%xmm3")                            /* xmm3     = s*a2 + s2*b2 = p2 */

        #define FILTER_X4_PRE_MEMSYNC \
            /* In:  xmm2 = p1, xmm3 = p2, xmm6 = d0, xmm7 = d1 */ \
            /* Out: xmm0 = mask, xmm6 = d0', xmm7 = d1' */ \
            /* Clobber: xmm2, xmm3, xmm4, xmm5 */ \
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            /* xmm2     = p1 + d1 */ \
            __ASM_EMIT("movaps      %[MASK], %%xmm0")                           /* xmm0     = MASK */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            /* xmm4     = MASK */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            /* xmm5     = MASK */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            /* xmm2     = (p1 + d1) & MASK */ \
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            /* xmm3     = p2 & MASK */ \
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            /* xmm4     = d0 & ~MASK */ \
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            /* xmm5     = d1 & ~MASK */ \
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            /* xmm4     = (p1 + d1) & MASK | (d0 & ~MASK) */ \
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            /* xmm5     = (p2 & MASK) | (d1 & ~MASK) */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            /* xmm6     = d0' */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            /* xmm7     = d1' */ \

        #define FILTER_X4_MEMSYNC \
            /* In:  xmm2 = p1, xmm3 = p2, xmm6 = d0, xmm7 = d1 */ \
            /* Out: xmm6 = d0', xmm7 = d1' */ \
            /* Clobber: xmm2, xmm4, xmm5 */ \
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            /* xmm2     = p1 + d1 */ \
            __ASM_EMIT("movaps      %%xmm3, %%xmm7")                            /* xmm7     = d1' = p2 */ \
            __ASM_EMIT("movaps      %%xmm2, %%xmm6")                            /* xmm6     = d0' = p1 + d1 */ \

        #define FILTER_X4_POST_MEMSYNC \
            /* In:  xmm0 = mask, xmm2 = p1, xmm3 = p2, xmm6 = d0, xmm7 = d1 */ \
            /* Out: xmm6 = d0', xmm7 = d1' */ \
            /* Clobber: xmm2, xmm3, xmm4, xmm5 */ \
            __ASM_EMIT("addps       %%xmm7, %%xmm2")                            /* xmm2     = p1 + d1 */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm4")                            /* xmm4     = MASK */ \
            __ASM_EMIT("movaps      %%xmm0, %%xmm5")                            /* xmm5     = MASK */ \
            __ASM_EMIT("andps       %%xmm4, %%xmm2")                            /* xmm2     = (p1 + d1) & MASK */ \
            __ASM_EMIT("andps       %%xmm5, %%xmm3")                            /* xmm3     = p2 & MASK */ \
            __ASM_EMIT("andnps      %%xmm6, %%xmm4")                            /* xmm4     = d0 & ~MASK */ \
            __ASM_EMIT("andnps      %%xmm7, %%xmm5")                            /* xmm5     = d1 & ~MASK */ \
            __ASM_EMIT("orps        %%xmm2, %%xmm4")                            /* xmm4     = (p1 + d1) & MASK | (d0 & ~MASK) */ \
            __ASM_EMIT("orps        %%xmm3, %%xmm5")                            /* xmm5     = (p2 & MASK) | (d1 & ~MASK) */ \
            __ASM_EMIT("movaps      %%xmm4, %%xmm6")                            /* xmm6     = d0' */ \
            __ASM_EMIT("movaps      %%xmm5, %%xmm7")                            /* xmm7     = d1' */

        #define FILTER_X4_CORE      FILTER_X4_CORE_BASE("0x00", "0x10", "0x20", "0x30", "0x40")

        #define FILTER_X8P1_CORE    FILTER_X4_CORE_BASE("0x00", "0x20", "0x40", "0x60", "0x80")
        #define FILTER_X8P2_CORE    FILTER_X4_CORE_BASE("0x10", "0x30", "0x50", "0x70", "0x90")

        #define FILTER_X16P1_CORE   FILTER_X4_CORE_BASE("0x000", "0x040", "0x080", "0x0c0", "0x100")
        #define FILTER_X16P2_CORE   FILTER_X4_CORE_BASE("0x010", "0x050", "0x090", "0x0d0", "0x110")
        #define FILTER_X16P3_CORE   FILTER_X4_CORE_BASE("0x020", "0x060", "0x0a0", "0x0e0", "0x120")
        #define FILTER_X16P4_CORE   FILTER_X4_CORE_BASE("0x030", "0x070", "0x0b0", "0x0f0", "0x130")


    } /* namespace sse */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_SSE_FILTERS_COMMON_H_ */
