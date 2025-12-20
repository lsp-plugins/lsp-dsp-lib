/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 нояб. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_CLAMP_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_CLAMP_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void clamp_vv1(float *dst, const float *min, const float *max, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s         = dst[i];
                if (s < min[i])
                    s               = min[i];
                else if (s > max[i])
                    s               = max[i];
                dst[i]          = s;
            }
        }

        void clamp_vv2(float *dst, const float *src, const float *min, const float *max, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s         = src[i];
                if (s < min[i])
                    s               = min[i];
                else if (s > max[i])
                    s               = max[i];
                dst[i]          = s;
            }
        }

        void clamp_kk1(float *dst, float min, float max, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s         = dst[i];
                if (s < min)
                    s               = min;
                else if (s > max)
                    s               = max;
                dst[i]          = s;
            }
        }

        void clamp_kk2(float *dst, const float *src, float min, float max, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s         = src[i];
                if (s < min)
                    s               = min;
                else if (s > max)
                    s               = max;
                dst[i]          = s;
            }
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_CLAMP_H_ */
