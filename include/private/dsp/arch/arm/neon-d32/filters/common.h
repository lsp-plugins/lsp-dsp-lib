/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 30 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_IMPL */

namespace lsp
{
    namespace neon_d32
    {

        IF_ARCH_ARM(
            static const uint32_t biquad_x4_mask[8] __lsp_aligned16 =
            {
                0xffffffff, 0x00000000, 0x00000000, 0x00000000,
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
            };
        )

        IF_ARCH_ARM(
            static const uint32_t biquad_x8_mask[16] __lsp_aligned16 =
            {
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                0xffffffff, 0xffffffff, 0xffffffff, 0xffffffff,
                0xffffffff, 0x00000000, 0x00000000, 0x00000000,
                0x00000000, 0x00000000, 0x00000000, 0x00000000,
            };
        )

    } /* namespace neon_d32 */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_ARM_NEON_D32_FILTERS_COMMON_H_ */
