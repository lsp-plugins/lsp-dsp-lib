/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 апр. 2025 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_LANCZOS_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_LANCZOS_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void lanczos1(float *dst, float k, float p, float t, float a, size_t count)
        {
            for (size_t j=0; j<count; ++j)
            {
                const float x1  = float(j)*k - p;
                const float ax  = fabsf(x1);

                if (ax < t)
                {
                    const float x2  = x1 * a;
                    dst[j]          = (ax >= 1e-4f) ? (sinf(x1) * sinf(x2)) / (x1 * x2) : 1.0f;
                }
                else
                    dst[j]      = 0.0f;
            }
        }

    } /* namespace generic */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_LANCZOS_H_ */
