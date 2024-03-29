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

#ifndef PRIVATE_DSP_ARCH_GENERIC_COMPLEX_H_
#define PRIVATE_DSP_ARCH_GENERIC_COMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void complex_mul2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float re            = (dst_re[i]) * (src_re[i]) - (dst_im[i]) * (src_im[i]);
                float im            = (dst_re[i]) * (src_im[i]) + (dst_im[i]) * (src_re[i]);
                dst_re[i]           = re;
                dst_im[i]           = im;
            }
        }

        void complex_mul3(float *dst_re, float *dst_im, const float *src1_re, const float *src1_im, const float *src2_re, const float *src2_im, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float re            = (src1_re[i]) * (src2_re[i]) - (src1_im[i]) * (src2_im[i]);
                float im            = (src1_re[i]) * (src2_im[i]) + (src1_im[i]) * (src2_re[i]);
                dst_re[i]           = re;
                dst_im[i]           = im;
            }
        }

        void complex_rcp1(float *dst_re, float *dst_im, size_t count)
        {
            while (count--)
            {
                float re            = *dst_re;
                float im            = *dst_im;
                float mag           = 1.0f / (re * re + im * im);

                *(dst_re++)         = re * mag;
                *(dst_im++)         = -im * mag;
            }
        }

        void complex_rcp2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            while (count--)
            {
                float re            = *(src_re++);
                float im            = *(src_im++);
                float mag           = 1.0f / (re * re + im * im);

                *(dst_re++)         = re * mag;
                *(dst_im++)         = -im * mag;
            }
        }

        void complex_cvt2modarg(float *dst_mod, float *dst_arg, const float *src_re, const float *src_im, size_t count)
        {
            while (count--)
            {
                float r         = *(src_re++);
                float i         = *(src_im++);
                float r2        = r * r;
                float i2        = i * i;

                float m         = sqrtf(r2 + i2);
                float a         = (i != 0.0f) ? 2.0f * atanf((m - r)/ i) :
                                  (r == 0.0f) ? NAN :
                                  (r < 0.0f) ? M_PI : 0.0f;

                *(dst_mod++)    = m;
                *(dst_arg++)    = a;
            }
        }

        void complex_arg(float *dst, const float *re, const float *im, size_t count)
        {
            while (count--)
            {
                float r         = *(re++);
                float i         = *(im++);
                float r2        = r * r;
                float i2        = i * i;

                float m         = sqrtf(r2 + i2);
                float a         = (i != 0.0f) ? 2.0f * atanf((m - r)/ i) :
                                  (r == 0.0f) ? NAN :
                                  (r < 0.0f) ? M_PI : 0.0f;

                *(dst++)        = a;
            }
        }

        void complex_cvt2reim(float *dst_re, float *dst_im, const float *src_mod, const float *src_arg, size_t count)
        {
            while (count--)
            {
                float mod       = *(src_mod++);
                float arg       = *(src_arg++);
                *(dst_re++)     = mod * cosf(arg);
                *(dst_im++)     = mod * sinf(arg);
            }
        }

        void complex_mod(float *dst_mod, const float *src_re, const float *src_im, size_t count)
        {
            while (count--)
            {
                float re        = *(src_re++);
                float im        = *(src_im++);
                *(dst_mod++)    = sqrtf(re*re + im*im);
            }
        }

        void complex_div2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float re        = src_re[i] * dst_re[i] + src_im[i] * dst_im[i];
                float im        = src_re[i] * dst_im[i] - src_im[i] * dst_re[i];
                float n         = 1.0f / (src_re[i] * src_re[i] + src_im[i] * src_im[i]);

                dst_re[i]       = re * n;
                dst_im[i]       = im * n;
            }
        }

        void complex_rdiv2(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float re        = dst_re[i] * src_re[i] + dst_im[i] * src_im[i];
                float im        = dst_re[i] * src_im[i] - dst_im[i] * src_re[i];
                float n         = 1.0f / (dst_re[i] * dst_re[i] + dst_im[i] * dst_im[i]);

                dst_re[i]       = re * n;
                dst_im[i]       = im * n;
            }
        }

        void complex_div3(float *dst_re, float *dst_im, const float *t_re, const float *t_im, const float *b_re, const float *b_im, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float re        = t_re[i] * b_re[i] + t_im[i] * b_im[i];
                float im        = t_im[i] * b_re[i] - t_re[i] * b_im[i];
                float n         = 1.0f / (b_re[i] * b_re[i] + b_im[i] * b_im[i]);

                dst_re[i]       = re * n;
                dst_im[i]       = im * n;
            }
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_COMPLEX_H_ */
