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

#ifndef PRIVATE_DSP_ARCH_GENERIC_SEARCH_H_
#define PRIVATE_DSP_ARCH_GENERIC_SEARCH_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        float min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float min = src[0];
            for (size_t i=1; i<count; ++i)
            {
                float tmp = src[i];
                if (tmp < min)
                    min = tmp;
            }
            return min;
        }

        float max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float max = src[0];
            for (size_t i=1; i<count; ++i)
            {
                float tmp = src[i];
                if (tmp > max)
                    max = tmp;
            }
            return max;
        }

        float abs_min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float min = fabsf(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float tmp = fabsf(src[i]);
                if (tmp < min)
                    min     = tmp;
            }
            return min;
        }

        float abs_max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float max = fabsf(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float tmp = fabsf(src[i]);
                if (tmp > max)
                    max     = tmp;
            }
            return max;
        }

        float sign_min(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float vmin  = src[0];
            float amin  = fabsf(vmin);
            for (size_t i=1; i<count; ++i)
            {
                float vtmp  = src[i];
                float atmp  = fabsf(vtmp);
                if (atmp < amin)
                {
                    vmin        = vtmp;
                    amin        = atmp;
                }
            }
            return vmin;
        }

        float sign_max(const float *src, size_t count)
        {
            if (count == 0)
                return 0.0f;

            float vmax  = src[0];
            float amax  = fabsf(vmax);
            for (size_t i=1; i<count; ++i)
            {
                float vtmp  = src[i];
                float atmp  = fabsf(vtmp);
                if (atmp > amax)
                {
                    vmax        = vtmp;
                    amax        = atmp;
                }
            }
            return vmax;
        }

        void minmax(const float *src, size_t count, float *min, float *max)
        {
            if (count == 0)
            {
                *min    = 0.0f;
                *max    = 0.0f;
                return;
            }

            float a_min = src[0], a_max = src[0];
            for (size_t i=1; i<count; ++i)
            {
                float tmp   = src[i];
                if (tmp < a_min)
                    a_min       = tmp;
                if (tmp > a_max)
                    a_max       = tmp;
            }
            *min    = a_min;
            *max    = a_max;
        }

        void abs_minmax(const float *src, size_t count, float *min, float *max)
        {
            if (count == 0)
            {
                *min    = 0.0f;
                *max    = 0.0f;
                return;
            }

            float a_min = fabsf(src[0]), a_max = fabsf(src[0]);

            for (size_t i=1; i<count; ++i)
            {
                float tmp   = fabsf(src[i]);
                if (tmp < a_min)
                    a_min       = tmp;
                if (tmp > a_max)
                    a_max       = tmp;
            }
            *min    = a_min;
            *max    = a_max;
        }

        void sign_minmax(const float *src, size_t count, float *min, float *max)
        {
            if (count == 0)
            {
                *min    = 0.0f;
                *max    = 0.0f;
                return;
            }

            float vmin = src[0], vmax = src[0];
            float amin = fabsf(vmin), amax = fabsf(vmax);

            for (size_t i=1; i<count; ++i)
            {
                float vtmp  = src[i];
                float atmp  = fabsf(vtmp);

                if (atmp < amin)
                {
                    vmin        = vtmp;
                    amin        = atmp;
                }
                if (vtmp > amax)
                {
                    vmax        = vtmp;
                    amax        = atmp;
                }
            }
            *min    = vmin;
            *max    = vmax;
        }

        size_t min_index(const float *src, size_t count)
        {
            if (count <= 0)
                return 0;

            size_t index = 0;
            float vmin = src[0];
            for (size_t i=1; i<count; ++i)
            {
                float v = src[i];
                if (v < vmin)
                {
                    index   = i;
                    vmin    = v;
                }
            }
            return index;
        }

        size_t max_index(const float *src, size_t count)
        {
            if (count <= 0)
                return 0;

            size_t index = 0;
            float vmax = src[0];
            for (size_t i=1; i<count; ++i)
            {
                float v = src[i];
                if (v > vmax)
                {
                    index   = i;
                    vmax    = v;
                }
            }
            return index;
        }

        size_t abs_min_index(const float *src, size_t count)
        {
            if (count <= 0)
                return 0;

            size_t index = 0;
            float s = fabsf(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float d = fabsf(src[i]);
                if (d < s)
                {
                    index   = i;
                    s       = d;
                }
            }
            return index;
        }

        size_t abs_max_index(const float *src, size_t count)
        {
            if (count <= 0)
                return 0;

            size_t index = 0;
            float s = fabsf(src[0]);
            for (size_t i=1; i<count; ++i)
            {
                float d = fabsf(src[i]);
                if (d > s)
                {
                    index   = i;
                    s       = d;
                }
            }
            return index;
        }

        void minmax_index(const float *src, size_t count, size_t *min, size_t *max)
        {
            size_t imin = 0, imax = 0;
            if (count > 0)
            {
                float vmin = src[0];
                float vmax = vmin;

                for (size_t i=1; i<count; ++i)
                {
                    float v = src[i];
                    if (v < vmin)
                    {
                        imin    = i;
                        vmin    = v;
                    }
                    if (v > vmax)
                    {
                        imax    = i;
                        vmax    = v;
                    }
                }
            }

            *min = imin;
            *max = imax;
        }

        void abs_minmax_index(const float *src, size_t count, size_t *min, size_t *max)
        {
            size_t imin = 0, imax = 0;
            if (count > 0)
            {
                float vmin = fabsf(src[0]);
                float vmax = vmin;

                for (size_t i=1; i<count; ++i)
                {
                    float v = fabsf(src[i]);
                    if (v < vmin)
                    {
                        imin    = i;
                        vmin    = v;
                    }
                    if (v > vmax)
                    {
                        imax    = i;
                        vmax    = v;
                    }
                }
            }

            *min = imin;
            *max = imax;
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_SEARCH_H_ */
