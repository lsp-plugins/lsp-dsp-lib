/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 5 окт. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_COMPRESSOR_H_
#define PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_COMPRESSOR_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void compressor_x2_gain(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float x     = fabsf(src[i]);
                if ((x <= c->k[0].start) && (x <= c->k[1].start))
                {
                    dst[i]      = c->k[0].gain * c->k[1].gain;
                    continue;
                }

                float lx    = logf(x);
                float g1    = (x <= c->k[0].start) ? c->k[0].gain :
                              (x >= c->k[0].end) ? expf(lx * c->k[0].tilt[0] + c->k[0].tilt[1]) :
                              expf((c->k[0].herm[0]*lx + c->k[0].herm[1])*lx + c->k[0].herm[2]);
                float g2    = (x <= c->k[1].start) ? c->k[1].gain :
                              (x >= c->k[1].end) ? expf(lx * c->k[1].tilt[0] + c->k[1].tilt[1]) :
                              expf((c->k[1].herm[0]*lx + c->k[1].herm[1])*lx + c->k[1].herm[2]);

                dst[i]      = g1 * g2;
            }
        }

        void compressor_x2_curve(float *dst, const float *src, const dsp::compressor_x2_t *c, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float x     = fabsf(src[i]);
                if ((x <= c->k[0].start) && (x <= c->k[1].start))
                {
                    dst[i]      = c->k[0].gain * c->k[1].gain * x;
                    continue;
                }

                float lx    = logf(x);
                float g1    = (x <= c->k[0].start) ? c->k[0].gain :
                              (x >= c->k[0].end) ? expf(lx * c->k[0].tilt[0] + c->k[0].tilt[1]) :
                              expf((c->k[0].herm[0]*lx + c->k[0].herm[1])*lx + c->k[0].herm[2]);
                float g2    = (x <= c->k[1].start) ? c->k[1].gain :
                              (x >= c->k[1].end) ? expf(lx * c->k[1].tilt[0] + c->k[1].tilt[1]) :
                              expf((c->k[1].herm[0]*lx + c->k[1].herm[1])*lx + c->k[1].herm[2]);

                dst[i]      = g1 * g2 * x;
            }
        }
    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_DYNAMICS_COMPRESSOR_H_ */
