/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 нояб. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_HMATH_MSUM_H_
#define PRIVATE_DSP_ARCH_GENERIC_HMATH_MSUM_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float moving_avg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                value      += b[i] - a[i];
                dst[i]      = value * avg;
            }
            return value;
        }

        float moving_pavg(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                value      += b[i] - a[i];
                dst[i]      = lsp_max(value * avg, 0.0f);
            }
            return value;
        }

        float moving_mean_sqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float xb  = b[i];
                const float xa  = a[i];
                value          += xb*xb - xa*xa;
                dst[i]          = value * avg;
            }
            return value;
        }

        float moving_mean_psqr(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float xb  = b[i];
                const float xa  = a[i];
                value          += xb*xb - xa*xa;
                dst[i]          = lsp_max(value * avg, 0.0f);
            }
            return value;
        }

        float moving_rms(float *dst, const float *a, const float *b, float value, float avg, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float xb  = b[i];
                const float xa  = a[i];
                value          += xb*xb - xa*xa;
                dst[i]          = sqrtf(lsp_max(value*avg, 0.0f));
            }
            return value;
        }
    } /* namespace generic */
} /* namespace lsp */


#endif /* INCLUDE_PRIVATE_DSP_ARCH_GENERIC_HMATH_MSUM_H_ */
