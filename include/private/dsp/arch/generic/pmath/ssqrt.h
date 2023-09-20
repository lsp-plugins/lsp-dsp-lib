/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 сент. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_SQRT_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_SQRT_H_

namespace lsp
{
    namespace generic
    {
        void ssqrt1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s     = dst[i];
                dst[i]      = (s > 0.0f) ? sqrt(s) : 0.0f;
            }
        }

        void ssqrt2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float s     = src[i];
                dst[i]      = (s > 0.0f) ? sqrt(s) : 0.0f;
            }
        }

    } /* namespace generic */
} /* namespace lsp */




#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_SQRT_H_ */
