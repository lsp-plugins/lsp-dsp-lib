/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 окт. 2020 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_PIXELFMT_H_
#define PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_PIXELFMT_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void rgba32_to_bgra32(void *dst, const void *src, size_t count)
        {
            const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
            uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

            for (size_t i=0; i<count; ++i)
            {
                uint32_t c      = s[i];
                d[i]            = ((c&0xff0000) >> 16) | ((c&0xff)<<16) | (c&0xff00ff00);
            }
        }

        void abgr32_to_bgra32(void *dst, const void *src, size_t count)
        {
            const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
            uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

            for (size_t i=0; i<count; ++i)
            {
                uint32_t c      = s[i];
                d[i]            = (c << 24) | (c >> 8);
            }
        }

    #ifdef ARCH_LE
        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
            uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

            for (size_t i=0; i<count; ++i)
            {
                uint32_t c      = s[i];
                d[i]            = 0xff000000 | (c >> 8);
            }
        }
    #else /* ARCH_BE */
        void abgr32_to_bgrff32(void *dst, const void *src, size_t count)
        {
            const uint32_t *s   = reinterpret_cast<const uint32_t *>(src);
            uint32_t *d         = reinterpret_cast<uint32_t *>(dst);

            for (size_t i=0; i<count; ++i)
            {
                uint32_t c      = s[i];
                d[i]            = 0x000000ff | (c << 8);
            }
        }
    #endif

        void rgba32_to_bgra32_ra(void *dst, const void *src, size_t count)
        {
            const uint8_t *s   = reinterpret_cast<const uint8_t *>(src);
            uint8_t *d         = reinterpret_cast<uint8_t *>(dst);
            uint32_t    r, g, b, a, k;

            for (size_t i=0; i<count; ++i)
            {
                a       = 0xff - s[3];
                k       = 0x10101 * d[3]; // k = (0x1000000 * a) / 0xff
                r       = (s[0] * k) >> 24;
                g       = (s[1] * k) >> 24;
                b       = (s[2] * k) >> 24;

                d[0]    = uint8_t(b);
                d[1]    = uint8_t(g);
                d[2]    = uint8_t(r);
                d[3]    = uint8_t(a);

                s      += 4;
                d      += 4;
            }
        }

        void rgba_to_bgra32(void *dst, const float *src, size_t count)
        {
            uint8_t *p = reinterpret_cast<uint8_t *>(dst);
            for (; count > 0; count--, p += 4, src +=4 )
            {
                float A     = 255.0f - src[3] * 255.0f;
                float R     = src[0] * A;
                float G     = src[1] * A;
                float B     = src[2] * A;

                // Saturate
                if (A < 0.0f)
                    A           = 0.0f;
                else if (A > 255.0f)
                    A           = 255.0f;
                if (R < 0.0f)
                    R           = 0.0f;
                else if (R > 255.0f)
                    R           = 255.0f;
                if (G < 0.0f)
                    G           = 0.0f;
                else if (G > 255.0f)
                    G           = 255.0f;
                if (B < 0.0f)
                    B           = 0.0f;
                else if (B > 255.0f)
                    B           = 255.0f;

                // Store result
                p[0]        = B;
                p[1]        = G;
                p[2]        = R;
                p[3]        = A;
            }
        }
    }
}


#endif /* PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_PIXELFMT_H_ */
