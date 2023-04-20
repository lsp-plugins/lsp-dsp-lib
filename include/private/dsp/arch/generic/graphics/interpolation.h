/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 31 мар. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_
#define PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void smooth_cubic_linear(float *dst, float start, float stop, size_t count)
        {
            float dy = 2.0f * (stop - start);
            float nx = 1.0f / (count + 1); // Normalizing x

            for (size_t i=0; i<count; ++i)
            {
                float x = i * nx;
                *(dst++) = start + dy * x*x * (1.5f - x);
            }
        }

        void smooth_cubic_log(float *dst, float start, float stop, size_t count)
        {
            float dy = 2.0f * logf(stop/start);
            float nx = 1.0f / (count + 1); // Normalizing x

            for (size_t i=0; i<count; ++i)
            {
                float x = i * nx;
                *(dst++) = start * expf(dy * x*x * (1.5f - x));
            }
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_INTERPOLATION_H_ */
