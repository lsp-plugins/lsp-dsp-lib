/*
 * Copyright (C) 2022 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2022 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 6 нояб. 2022 г.
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

#ifndef INCLUDE_PRIVATE_DSP_ARCH_GENERIC_PMATH_NORMALIZE_H_
#define INCLUDE_PRIVATE_DSP_ARCH_GENERIC_PMATH_NORMALIZE_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void abs_normalized(float *dst, const float *src, size_t count)
        {
            // Calculate absolute values
            dsp::abs2(dst, src, count);

            // Find the maximum value
            float max = dsp::max(dst, count);

            // Divide if it is possible
            if (max != 0.0f)
                dsp::mul_k2(dst, 1.0f / max, count);
        }

        void normalize1(float *dst, size_t count)
        {
            // Find minimum and maximum
            float max = dsp::abs_max(dst, count);
            // Normalize OR do nothing
            if (max > 0.0f)
                dsp::mul_k2(dst, 1.0f / max, count);
        }

        void normalize2(float *dst, const float *src, size_t count)
        {
            // Find the absolute maximum
            float max = dsp::abs_max(src, count);
            // Normalize OR copy
            if (max > 0.0f)
                dsp::mul_k3(dst, src, 1.0f / max, count);
            else
                dsp::copy(dst, src, count);
        }
    } /* namespace generic */
} /* namespace lsp */


#endif /* INCLUDE_PRIVATE_DSP_ARCH_GENERIC_PMATH_NORMALIZE_H_ */
