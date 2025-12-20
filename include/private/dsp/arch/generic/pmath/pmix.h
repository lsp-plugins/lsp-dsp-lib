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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_PMIX_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_PMIX_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void pmix_v1(float *dst, const float *src, const float *k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     += (src[i] - dst[i])*k[i];
        }

        void pmix_v2(float *dst, const float *src1, const float *src2, const float *k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] + (src2[i] - src1[i])*k[i];
        }

        void pmix_k1(float *dst, const float *src, float k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     += (src[i] - dst[i])*k;
        }

        void pmix_k2(float *dst, const float *src1, const float *src2, float k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] + (src2[i] - src1[i])*k;
        }

    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_PMIX_H_ */
