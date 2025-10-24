/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void logb1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(dst[i]) * float(M_LOG2E);
        }

        void logb2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(src[i]) * float(M_LOG2E);
        }

        void loge1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(dst[i]);
        }

        void loge2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(src[i]);
        }

        void logd1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::log10f(dst[i]);
        }

        void logd2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::log10f(src[i]);
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_ */
