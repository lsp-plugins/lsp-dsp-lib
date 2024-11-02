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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_ABS_VV_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_ABS_VV_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void abs1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = fabsf(dst[i]);
        }

        void abs2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = fabsf(src[i]);
        }

        void abs_add2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     += fabsf(src[i]);
        }

        void abs_sub2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     -= fabsf(src[i]);
        }

        void abs_rsub2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     = fabsf(src[i]) - dst[i];
        }

        void abs_mul2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     *= fabsf(src[i]);
        }

        void abs_div2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]     /= fabsf(src[i]);
        }

        void abs_rdiv2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = fabsf(src[i]) / dst[i];
        }

        void abs_max2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = lsp_max(dst[i], fabsf(src[i]));
        }

        void abs_min2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = lsp_min(dst[i], fabsf(src[i]));
        }

        void abs_add3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] + fabsf(src2[i]);
        }

        void abs_sub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] - fabsf(src2[i]);
        }

        void abs_rsub3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = fabsf(src2[i]) - src1[i];
        }

        void abs_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] * fabsf(src2[i]);
        }

        void abs_div3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = src1[i] / fabsf(src2[i]);
        }

        void abs_rdiv3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = fabsf(src2[i]) / src1[i];
        }

        void abs_max3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = lsp_max(src1[i], fabsf(src2[i]));
        }

        void abs_min3(float *dst, const float *src1, const float *src2, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i]      = lsp_min(src1[i], fabsf(src2[i]));
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_ABS_VV_H_ */
