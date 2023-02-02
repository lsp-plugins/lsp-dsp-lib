/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 февр. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_LRAMP_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_LRAMP_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void lramp_set1(float *dst, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fill(dst, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = v1 + delta * i;
        }

        void lramp1(float *dst, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::mul_k2(dst, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = dst[i] * (v1 + delta * i);
        }

        void lramp2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::mul_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = src[i] * (v1 + delta * i);
        }

        void lramp_add2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmadd_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = dst[i] + src[i] * (v1 + delta * i);
        }

        void lramp_sub2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmsub_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = dst[i] - src[i] * (v1 + delta * i);
        }

        void lramp_rsub2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrsub_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = src[i] * (v1 + delta * i) - dst[i];
        }

        void lramp_mul2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmmul_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = dst[i] * src[i] * (v1 + delta * i);
        }

        void lramp_div2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmdiv_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = dst[i] / (src[i] * (v1 + delta * i));
        }

        void lramp_rdiv2(float *dst, const float *src, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrdiv_k3(dst, src, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = (src[i] * (v1 + delta * i)) / dst[i];
        }

        void lramp_add3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmadd_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = a[i] + b[i] * (v1 + delta * i);
        }

        void lramp_sub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmsub_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = a[i] - b[i] * (v1 + delta * i);
        }

        void lramp_rsub3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrsub_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = b[i] * (v1 + delta * i) - a[i];
        }

        void lramp_mul3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmmul_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = a[i] * b[i] * (v1 + delta * i);
        }

        void lramp_div3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmdiv_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = a[i] / (b[i] * (v1 + delta * i));
        }

        void lramp_rdiv3(float *dst, const float *a, const float *b, float v1, float v2, size_t count)
        {
            float delta = v2 - v1;
            if (delta == 0.0f)
            {
                dsp::fmrdiv_k4(dst, a, b, v1, count);
                return;
            }
            if (count == 0)
                return;

            delta /= count;
            for (size_t i=0; i<count; ++i)
                dst[i] = (b[i] * (v1 + delta * i)) / a[i];
        }

    } /* namespace generic */
} /* namespace lsp */



#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_LRAMP_H_ */
