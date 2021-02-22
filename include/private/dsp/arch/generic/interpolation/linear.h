/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 13 дек. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_INTERPOLATION_LINEAR_H_
#define PRIVATE_DSP_ARCH_GENERIC_INTERPOLATION_LINEAR_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void lin_inter_set(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]      = (x * dy + y0);
        }

        void lin_inter_mul2(float *dst, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]     *= (x * dy + y0);
        }

        void lin_inter_mul3(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]      = src[i] * (x * dy + y0);
        }

        void lin_inter_fmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]     += src[i]*(x * dy + y0);
        }

        void lin_inter_frmadd2(float *dst, const float *src, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]     = dst[i]*(x * dy + y0) + src[i];
        }

        void lin_inter_fmadd3(float *dst, const float *src1, const float *src2, int32_t x0, float y0, int32_t x1, float y1, int32_t x, uint32_t n)
        {
            float dy    = (y1 - y0) / float(x1 - x0);
            x          -= x0;
            for (size_t i=0; i<n; ++i, ++x)
                dst[i]  = src1[i]*(x * dy + y0) + src2[i];
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_GENERIC_INTERPOLATION_LINEAR_H_ */
