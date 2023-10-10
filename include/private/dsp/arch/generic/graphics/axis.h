/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_AXIS_H_
#define PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_AXIS_H_

namespace lsp
{
    namespace generic
    {
        void axis_apply_lin1(float *x, const float *v, float zero, float norm, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float vec   = v[i];
                x[i]       += norm * (vec + zero);
            }
        }

        void axis_apply_log2(float *x, float *y, const float *v, float zero, float norm_x, float norm_y, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float vec    = v[i];
                if (vec < 0.0f)
                    vec     = -vec;
                if (vec < LSP_DSP_AMPLIFICATION_THRESH)
                    vec     = LSP_DSP_AMPLIFICATION_THRESH;
                float k     = logf(vec * zero);
                x[i]       += norm_x * k;
                y[i]       += norm_y * k;
            }
        }

        void axis_apply_log1(float *x, const float *v, float zero, float norm_x, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float vec    = v[i];
                if (vec < 0.0f)
                    vec     = -vec;
                if (vec < LSP_DSP_AMPLIFICATION_THRESH)
                    vec     = LSP_DSP_AMPLIFICATION_THRESH;
                float k     = logf(vec * zero);
                x[i]       += norm_x * k;
            }
        }

    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_AXIS_H_ */
