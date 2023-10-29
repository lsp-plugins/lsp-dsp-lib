/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 19 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_GATE_H_
#define PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_GATE_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void gate_x1_gain(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float x     = fabsf(src[i]);
                if (x <= c->start)
                    x           = c->gain_start;
                else if (x >= c->end)
                    x           = c->gain_end;
                else
                {
                    float lx    = logf(x);
                    x           = expf(((c->herm[0]*lx + c->herm[1])*lx + c->herm[2])*lx + c->herm[3]);
                }
                dst[i]      = x;
            }
        }

        void gate_x1_curve(float *dst, const float *src, const dsp::gate_knee_t *c, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float x     = fabsf(src[i]);
                if (x <= c->start)
                    x          *= c->gain_start;
                else if (x >= c->end)
                    x          *= c->gain_end;
                else
                {
                    float lx    = logf(x);
                    x          *= expf(((c->herm[0]*lx + c->herm[1])*lx + c->herm[2])*lx + c->herm[3]);
                }
                dst[i]      = x;
            }
        }
    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_GATE_H_ */
