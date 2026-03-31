/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 мар. 2026 г.
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


#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_MS_MINMAX_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_MS_MINMAX_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void ms_pmin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (m < s) ? m : s;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_psmin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (fabsf(m) < fabsf(s)) ? m : s;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pamin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float am  = fabsf(m);
                const float as  = fabsf(s);
                const float out = (am < as) ? am : as;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pmax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (m < s) ? s : m;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_psmax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (fabsf(m) < fabsf(s)) ? s : m;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pamax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = dst[i];
                const float r   = src[i];
                const float m   = l + r;
                const float s   = l - r;
                const float am  = fabsf(m);
                const float as  = fabsf(s);
                const float out = (am < as) ? as : am;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (m < s) ? m : s;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (fabsf(m) < fabsf(s)) ? m : s;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float am  = fabsf(m);
                const float as  = fabsf(s);
                const float out = (am < as) ? am : as;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (m < s) ? s : m;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float out = (fabsf(m) < fabsf(s)) ? s : m;

                dst[i]          = out * 0.5f;
            }
        }

        void ms_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float l   = a[i];
                const float r   = b[i];
                const float m   = l + r;
                const float s   = l - r;
                const float am  = fabsf(m);
                const float as  = fabsf(s);
                const float out = (am < as) ? as : am;

                dst[i]          = out * 0.5f;
            }
        }


        void lr_pmin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (l < r) ? l : r;
            }
        }

        void lr_psmin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (fabsf(l) < fabsf(r)) ? l : r;
            }
        }

        void lr_pamin2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;
                const float al  = fabsf(l);
                const float ar  = fabsf(r);

                dst[i]          = (al < ar) ? al : ar;
            }
        }

        void lr_pmax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (l < r) ? r : l;
            }
        }

        void lr_psmax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (fabsf(l) < fabsf(r)) ? r : l;
            }
        }

        void lr_pamax2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = dst[i];
                const float s   = src[i];
                const float l   = m + s;
                const float r   = m - s;
                const float al  = fabsf(l);
                const float ar  = fabsf(r);

                dst[i]          = (al < ar) ? ar : al;
            }
        }

        void lr_pmin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (l < r) ? l : r;
            }
        }

        void lr_psmin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (fabsf(l) < fabsf(r)) ? l : r;
            }
        }

        void lr_pamin3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;
                const float al  = fabsf(l);
                const float ar  = fabsf(r);

                dst[i]          = (al < ar) ? al : ar;
            }
        }

        void lr_pmax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (l < r) ? r : l;
            }
        }

        void lr_psmax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;

                dst[i]          = (fabsf(l) < fabsf(r)) ? r : l;
            }
        }

        void lr_pamax3(float *dst, const float *a, const float *b, size_t count)
        {
            for (size_t i=0; i<count; ++i)
            {
                const float m   = a[i];
                const float s   = b[i];
                const float l   = m + s;
                const float r   = m - s;
                const float al  = fabsf(l);
                const float ar  = fabsf(r);

                dst[i]          = (al < ar) ? ar : al;
            }
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_MS_MINMAX_H_ */
