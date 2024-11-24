/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_PCOMPLEX_H_
#define PRIVATE_DSP_ARCH_GENERIC_PCOMPLEX_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void pcomplex_mul2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                // Use temporaries to prevent dst_re and dst_im to be the same to one of the sources
                float res_re        = (dst[0]) * (src[0]) - (dst[1]) * (src[1]);
                float res_im        = (dst[0]) * (src[1]) + (dst[1]) * (src[0]);

                // Store values
                dst[0]              = res_re;
                dst[1]              = res_im;

                // Update pointers
                src                += 2;
                dst                += 2;
            }
        }

        void pcomplex_mul3(float *dst, const float *src1, const float *src2, size_t count)
        {
            while (count--)
            {
                // Use temporaries to prevent dst_re and dst_im to be the same to one of the sources
                float res_re        = (src1[0]) * (src2[0]) - (src1[1]) * (src2[1]);
                float res_im        = (src1[0]) * (src2[1]) + (src1[1]) * (src2[0]);

                // Store values
                dst[0]              = res_re;
                dst[1]              = res_im;

                // Update pointers
                src1               += 2;
                src2               += 2;
                dst                += 2;
            }
        }

        void pcomplex_rcp1(float *dst, size_t count)
        {
            while (count--)
            {
                float re            = dst[0];
                float im            = dst[1];
                float mag           = 1.0f / (re * re + im * im);

                *(dst++)            = re * mag;
                *(dst++)            = -im * mag;
            }
        }

        void pcomplex_rcp2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float re            = *(src++);
                float im            = *(src++);
                float mag           = 1.0f / (re * re + im * im);

                *(dst++)            = re * mag;
                *(dst++)            = -im * mag;
            }
        }

        void pcomplex_r2c(float *dst, const float *src, size_t count)
        {
            if (dst == src)
            {
                // Do backward copy
                dst        += (count-1) << 1;
                src        += (count-1);
                while (count--)
                {
                    dst[0]      = *(src--);
                    dst[1]      = 0.0f;
                    dst        -= 2;
                }
            }
            else
            {
                // Do forward copy
                while (count--)
                {
                    dst[0]      = *(src++);
                    dst[1]      = 0.0f;
                    dst        += 2;
                }
            }
        }

        void pcomplex_fill_ri(float *dst, float re, float im, size_t count)
        {
            while (count --)
            {
                dst[0]      = re;
                dst[1]      = im;
                dst        += 2;
            }
        }

        void pcomplex_c2r(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)    = *src;
                src        += 2;
            }
        }

        void pcomplex_c2r_add2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)   += *src;
                src        += 2;
            }
        }

        void pcomplex_c2r_sub2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)   -= *src;
                src        += 2;
            }
        }

        void pcomplex_c2r_rsub2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *dst        = *src - *dst;
                src        += 2;
                dst        ++;
            }
        }

        void pcomplex_c2r_mul2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)   *= *src;
                src        += 2;
            }
        }

        void pcomplex_c2r_div2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *(dst++)   /= *src;
                src        += 2;
            }
        }

        void pcomplex_c2r_rdiv2(float *dst, const float *src, size_t count)
        {
            while (count --)
            {
                *dst        = *src / *dst;
                src        += 2;
                dst        ++;
            }
        }

        void pcomplex_mod(float *dst_mod, const float *src, size_t count)
        {
            while (count--)
            {
                float re        = src[0];
                float im        = src[1];
                *(dst_mod++)    = sqrtf(re*re + im*im);
                src            += 2;
            }
        }

        void pcomplex_arg(float *dst, const float *src, size_t count)
        {
            for (; count > 0; --count, src += 2)
            {
                float re        = src[0];
                float im        = src[1];
                float re2       = re * re;
                float im2       = im * im;

                float mod       = sqrtf(re2 + im2);
                float arg       = (im != 0.0f) ? 2.0f * atanf((mod - re)/ im) :
                                  (re == 0.0f) ? NAN :
                                  (re < 0.0f) ? M_PI : 0.0f;

                *(dst++)        = arg;
            }
        }

        void pcomplex_modarg(float *mod, float *arg, const float *src, size_t count)
        {
            for (; count > 0; --count, src += 2)
            {
                float r         = src[0];
                float i         = src[1];
                float r2        = r * r;
                float i2        = i * i;

                float m         = sqrtf(r2 + i2);
                float a         = (i != 0.0f) ? 2.0f * atanf((m - r)/ i) :
                                  (r == 0.0f) ? NAN :
                                  (r < 0.0f) ? M_PI : 0.0f;

                *(mod++)        = m;
                *(arg++)        = a;
            }
        }

        void pcomplex_div2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float re        = src[0] * dst[0] + src[1] * dst[1];
                float im        = src[0] * dst[1] - src[1] * dst[0];
                float n         = 1.0f / (src[0] * src[0] + src[1] * src[1]);

                dst[0]          = re * n;
                dst[1]          = im * n;
                src            += 2;
                dst            += 2;
            }
        }

        void pcomplex_rdiv2(float *dst, const float *src, size_t count)
        {
            while (count--)
            {
                float re        = dst[0] * src[0] + dst[1] * src[1];
                float im        = dst[0] * src[1] - dst[1] * src[0];
                float n         = 1.0f / (dst[0] * dst[0] + dst[1] * dst[1]);

                dst[0]          = re * n;
                dst[1]          = im * n;
                src            += 2;
                dst            += 2;
            }
        }

        void pcomplex_div3(float *dst, const float *t, const float *b, size_t count)
        {
            while (count--)
            {
                float re        = t[0] * b[0] + t[1] * b[1];
                float im        = t[1] * b[0] - t[0] * b[1];
                float n         = 1.0f / (b[0] * b[0] + b[1] * b[1]);

                dst[0]          = re * n;
                dst[1]          = im * n;
                t              += 2;
                b              += 2;
                dst            += 2;
            }
        }

        void pcomplex_r2c_add2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                dst[0]     += src[i];
                dst        += 2;
            }
        }

        void pcomplex_r2c_sub2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                dst[0]     -= src[i];
                dst        += 2;
            }
        }

        void pcomplex_r2c_rsub2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                dst[0]      = src[i] - dst[0];
                dst[1]      = -dst[1];
                dst        += 2;
            }
        }

        void pcomplex_r2c_mul2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                dst[0]     *= src[i];
                dst[1]     *= src[i];
                dst        += 2;
            }
        }

        void pcomplex_r2c_div2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                dst[0]     /= src[i];
                dst[1]     /= src[i];
                dst        += 2;
            }
        }

        void pcomplex_r2c_rdiv2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                float k     = src[i] / (dst[0] * dst[0] + dst[1] * dst[1]);

                dst[0]      = k * dst[0];
                dst[1]      = -k * dst[1];
                dst        += 2;
            }
        }

        void pcomplex_corr(float *dst_corr, const float *src1, const float *src2, size_t count)
        {
            /*
             * Complex correlation can be defined as:
             *   corr = re(norm(S1) * conj(norm(S2)))
             *
             * if S1 = a + j*b, S2 = c + j*d, then:
             *
             *   corr = re((a + j*b) * (c - j*d)) / (sqrtf(a^2 + b^2) * sqrtf(c^2 + d^2))
             *
             * After simplifications:
             *
             *   corr = (a*c + b*d) / sqrtf((a^2 + b^2)*(c^2 + d^2))
             *
             * In programming terms:
             *
             *   den  = (a*a + b*b)*(c*c + d*d)
             *   nom  = a*c + b*d
             *   corr = (den > threshold) ? nom / sqrt(den) : 0.0
             */

            while (count--)
            {
                const float a   = src1[0];
                const float b   = src1[1];
                const float c   = src2[0];
                const float d   = src2[1];

                const float den = (a*a + b*b)*(c*c + d*d);
                const float nom = a*c + b*d;

                *dst_corr       = (den >= 1e-36f) ? nom / sqrtf(den) : 0.0f;

                src1           += 2;
                src2           += 2;
                ++dst_corr;
            }
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PCOMPLEX_H_ */
