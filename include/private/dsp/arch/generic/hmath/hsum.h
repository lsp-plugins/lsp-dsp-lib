/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_HMATH_HSUM_H_
#define PRIVATE_DSP_ARCH_GENERIC_HMATH_HSUM_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float h_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            for (size_t i=0; i<count; ++i)
                result         += src[i];
            return result;
        }

        float h_sqr_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            for (size_t i=0; i<count; ++i)
            {
                float tmp       = src[i];
                result         += tmp * tmp;
            }
            return result;
        }

        float h_abs_sum(const float *src, size_t count)
        {
            float result    = 0.0f;
            for (size_t i=0; i<count; ++i)
                result         += fabsf(src[i]);
            return result;
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_HMATH_HSUM_H_ */
