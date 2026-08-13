/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 24 июн. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_LERP_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_LERP_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void lerp_vvv(float *dst, const float *a, const float *b, const float *k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a[i] + (b[i] - a[i]) * k[i];
        }

        void lerp_vvk(float *dst, const float *a, const float *b, float k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a[i] + (b[i] - a[i]) * k;
        }

        void lerp_vkv(float *dst, const float *a, float b, const float *k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a[i] + (b - a[i]) * k[i];
        }

        void lerp_vkk(float *dst, const float *a, float b, float k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a[i] + (b - a[i]) * k;
        }

        void lerp_kvv(float *dst, float a, const float *b, const float *k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a + (b[i] - a) * k[i];
        }

        void lerp_kvk(float *dst, float a, const float *b, float k, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]  = a + (b[i] - a) * k;
        }

        void lerp_kkv(float *dst, float a, float b, const float *k, size_t count)
        {
            const float d = b - a;
            for (size_t i=0; i<count; ++i)
                dst[i]  = a + d * k[i];
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_LERP_H_ */
