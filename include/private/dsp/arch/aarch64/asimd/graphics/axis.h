/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 9 сент. 2022 г.
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

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_
#define PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_

#ifndef PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_IMPL */

namespace lsp
{
    namespace asimd
    {
        IF_ARCH_AARCH64(
            static const uint32_t LOG_IARGS[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(0x007fffff),           // X_MANT
                LSP_DSP_VEC4(0x0000007f)            // X_MMASK
            };

            static const float LOG_FARGS[] __lsp_aligned16 =
            {
                LSP_DSP_VEC4(1e-8),                  // X_AMP
                LSP_DSP_VEC4(0.5f),                  // X_HALF
                LSP_DSP_VEC4(M_SQRT1_2),             // SQRT1_2
                LSP_DSP_VEC4(1.0f),                  // ONE
                LSP_DSP_VEC4(7.0376836292E-2),       // C0
                LSP_DSP_VEC4(-1.1514610310E-1),      // C1
                LSP_DSP_VEC4(1.1676998740E-1),       // C2
                LSP_DSP_VEC4(-1.2420140846E-1),      // C3
                LSP_DSP_VEC4(+1.4249322787E-1),      // C4
                LSP_DSP_VEC4(-1.6668057665E-1),      // C5
                LSP_DSP_VEC4(+2.0000714765E-1),      // C6
                LSP_DSP_VEC4(-2.4999993993E-1),      // C7
                LSP_DSP_VEC4(+3.3333331174E-1),      // C8
                LSP_DSP_VEC4(-0.5f),                 // C9
                LSP_DSP_VEC4(M_LN2 - 2.12194440e-4)  // LXE + LN2
            };
        )


    } /* namespace asimd */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_AARCH64_ASIMD_GRAPHICS_AXIS_H_ */
