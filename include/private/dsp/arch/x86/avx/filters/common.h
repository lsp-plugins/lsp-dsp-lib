/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 27 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_X86_AVX_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_X86_AVX_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX_IMPL */

namespace lsp
{
    namespace avx
    {
        IF_ARCH_X86(
            static const uint32_t biquad_mask_const[8] __lsp_aligned32 =
            {
                0xffffffff, 0, 0, 0, 0, 0, 0, 0
            };
        )

        #define FILTER_X8_CORE_FMA3(B0, B1, B2, A1, A2) \
            __ASM_EMIT("vmulps              " B1 "(%[f]), %%ymm1, %%ymm2")                      /* ymm2     = s*b1 */ \
            __ASM_EMIT("vmulps              " B2 "(%[f]), %%ymm1, %%ymm3")                      /* ymm3     = s*b2 */ \
            __ASM_EMIT("vfmadd132ps         " B0 "(%[f]), %%ymm6, %%ymm1")                      /* ymm1     = s*b0+d0 = s2 */ \
            __ASM_EMIT("vfmadd231ps         " A1 "(%[f]), %%ymm1, %%ymm2")                      /* ymm2     = s*b1 + s2*a1 = p1 */ \
            __ASM_EMIT("vfmadd231ps         " A2 "(%[f]), %%ymm1, %%ymm3")                      /* ymm3     = s*b2 + s2*a2 = p2 */

        #define X64_FILTER_X16_CORE_FMA3(B0, B1, B2, A1, A2) \
            __ASM_EMIT("vmulps              0x00 + " B1 "(%[f]), %%ymm1, %%ymm2")               /* ymm2     = s*b1 */ \
            __ASM_EMIT("vmulps              0x20 + " B1 "(%[f]), %%ymm9, %%ymm10") \
            __ASM_EMIT("vmulps              0x00 + " B2 "(%[f]), %%ymm1, %%ymm3")               /* ymm3     = s*b2 */ \
            __ASM_EMIT("vmulps              0x20 + " B2 "(%[f]), %%ymm9, %%ymm11") \
            __ASM_EMIT("vfmadd132ps         0x00 + " B0 "(%[f]), %%ymm6, %%ymm1")               /* ymm1     = s*b0+d0 = s2 */ \
            __ASM_EMIT("vfmadd132ps         0x20 + " B0 "(%[f]), %%ymm14, %%ymm9") \
            __ASM_EMIT("vfmadd231ps         0x00 + " A1 "(%[f]), %%ymm1, %%ymm2")               /* ymm2     = s*b1 + s2*a1 = p1 */ \
            __ASM_EMIT("vfmadd231ps         0x20 + " A1 "(%[f]), %%ymm9, %%ymm10") \
            __ASM_EMIT("vfmadd231ps         0x00 + " A2 "(%[f]), %%ymm1, %%ymm3")               /* ymm3     = s*b2 + s2*a2 = p2 */ \
            __ASM_EMIT("vfmadd231ps         0x20 + " A2 "(%[f]), %%ymm9, %%ymm11") \

        #define FILTER_X8_FMA3          FILTER_X8_CORE_FMA3("0x00", "0x20", "0x40", "0x60", "0x80")
        #define FILTER_X16P1_FMA3       FILTER_X8_CORE_FMA3("0x00", "0x40", "0x80", "0xc0", "0x100")
        #define FILTER_X16P2_FMA3       FILTER_X8_CORE_FMA3("0x20", "0x60", "0xa0", "0xe0", "0x120")

        #define X64_FILTER_FMA3         X64_FILTER_X16_CORE_FMA3("0x00", "0x40", "0x80", "0xc0", "0x100")

    } /* namespace avx */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX_FILTERS_COMMON_H_ */
