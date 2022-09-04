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

        constexpr uint32_t pabc_mask32  = __IF_LEBE( 0x00ffffffU, 0xffffff00U );
        constexpr uint64_t pabc_mask64  = __IF_LEBE( 0x00ffffff00ffffffULL, 0xffffff00ffffff00ULL );

        void pabc32_set_alpha(void *dst, const void *src, uint8_t alpha, size_t count)
        {
            uint32_t value32        = __IF_LEBE(uint32_t(alpha) << 24, uint32_t(alpha));

        #ifdef ARCH_64BIT
            uint64_t value64        = (uint64_t(value32) << 32) | value32;
            uint64_t *pdst64        = static_cast<uint64_t *>(dst);
            const uint64_t *psrc64  = static_cast<const uint64_t *>(src);

            // 8x loop
            for ( ; count >= 8; count -= 8)
            {
                uint64_t a          = psrc64[0];
                uint64_t b          = psrc64[1];
                uint64_t c          = psrc64[2];
                uint64_t d          = psrc64[3];

                a                   = (a & pabc_mask64) | value64;
                b                   = (b & pabc_mask64) | value64;
                c                   = (c & pabc_mask64) | value64;
                d                   = (d & pabc_mask64) | value64;

                pdst64[0]           = a;
                pdst64[1]           = b;
                pdst64[2]           = c;
                pdst64[3]           = d;

                psrc64             += 4;
                pdst64             += 4;
            }

            uint32_t *pdst          = reinterpret_cast<uint32_t *>(pdst64);
            const uint32_t *psrc    = reinterpret_cast<const uint32_t *>(psrc64);

            // 4x loop
            if (count >= 4)
            {
                uint32_t a          = psrc[0];
                uint32_t b          = psrc[1];
                uint32_t c          = psrc[2];
                uint32_t d          = psrc[3];

                a                   = (a & pabc_mask32) | value32;
                b                   = (b & pabc_mask32) | value32;
                c                   = (c & pabc_mask32) | value32;
                d                   = (d & pabc_mask32) | value32;

                pdst[0]             = a;
                pdst[1]             = b;
                pdst[2]             = c;
                pdst[3]             = d;

                psrc               += 4;
                pdst               += 4;
                count              -= 4;
            }
        #else
            uint32_t *pdst          = static_cast<uint32_t *>(dst);
            const uint32_t *psrc    = static_cast<const uint32_t *>(src);

            // 4x loop
            for ( ; count >= 4; count -= 4)
            {
                uint32_t a          = psrc[0];
                uint32_t b          = psrc[1];
                uint32_t c          = psrc[2];
                uint32_t d          = psrc[3];

                a                   = (a & pabc_mask32) | value32;
                b                   = (b & pabc_mask32) | value32;
                c                   = (c & pabc_mask32) | value32;
                d                   = (d & pabc_mask32) | value32;

                pdst[0]             = a;
                pdst[1]             = b;
                pdst[2]             = c;
                pdst[3]             = d;

                psrc               += 4;
                pdst               += 4;
            }
        #endif /* ARCH_64_BIT */

            // 2x loop
            if (count >= 2)
            {
                uint32_t a          = psrc[0];
                uint32_t b          = psrc[1];

                a                   = (a & pabc_mask32) | value32;
                b                   = (b & pabc_mask32) | value32;

                pdst[0]             = a;
                pdst[1]             = b;

                psrc               += 2;
                pdst               += 2;
                count              -= 2;
            }

            if (count)
                pdst[0]             = (psrc[0] & pabc_mask32) | value32;
        }

    } /* namespace generic */
} /* namespace lsp */


#endif /* PRIVATE_DSP_ARCH_GENERIC_GRAPHICS_PIXELFMT_H_ */
