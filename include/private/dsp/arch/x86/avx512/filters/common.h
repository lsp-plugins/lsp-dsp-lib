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

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_X86_AVX512_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_X86_AVX512_IMPL */

namespace lsp
{
    namespace avx512
    {
        IF_ARCH_X86(
            static const uint32_t biquad_mask_const[16] __lsp_aligned64 =
            {
                15, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14
            };
        )

        #define FILTER_X16_CORE(B0, B1, B2, A1, A2) \
            __ASM_EMIT("vmulps              " B1 "(%[f]), %%zmm1, %%zmm2")                      /* zmm2     = s*b1 */ \
            __ASM_EMIT("vmulps              " B2 "(%[f]), %%zmm1, %%zmm3")                      /* zmm3     = s*b2 */ \
            __ASM_EMIT("vfmadd132ps         " B0 "(%[f]), %%zmm6, %%zmm1")                      /* zmm1     = s*b0+d0 = s2 */ \
            __ASM_EMIT("vfmadd231ps         " A1 "(%[f]), %%zmm1, %%zmm2")                      /* zmm2     = s*b1 + s2*a1 = p1 */ \
            __ASM_EMIT("vfmadd231ps         " A2 "(%[f]), %%zmm1, %%zmm3")                      /* zmm3     = s*b2 + s2*a2 = p2 */

        #define FILTER_X16          FILTER_X16_CORE("0x00", "0x40", "0x80", "0xc0", "0x100")
    } /* namespace avx512 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_X86_AVX512_FILTERS_COMMON_H_ */
