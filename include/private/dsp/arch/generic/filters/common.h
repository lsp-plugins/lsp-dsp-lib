/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 25 июл. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_FILTERS_COMMON_H_
#define PRIVATE_DSP_ARCH_GENERIC_FILTERS_COMMON_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {

        #define GENERIC_BIQUAD_X1_CORE(dst, src, d, count, f, inc) \
            while (count--) \
            { \
                float s     = *(src++); \
                /*   a: a0 a1 a2 0 */ \
                /*   b: b1 b2 0  0 */ \
                float s2    = f->b0*s + d[0]; \
                float p1    = f->b1*s + f->a1*s2; \
                float p2    = f->b2*s + f->a2*s2; \
                \
                /* Shift buffer */ \
                d[0]        = d[1] + p1; \
                d[1]        = p2; \
                \
                /* Store result */ \
                *(dst++)    = s2; \
                inc; \
            }

        #define GENERIC_BIQUAD_X2_CORE(dst, src, d, count, f, inc) \
            float s, r, s2, r2, p1, q1, p2, q2; \
            \
            /*   a: a0 a0 a1 a2 i0 i0 i1 i2 */ \
            /*   b: b1 b2 0  0  j1 j2 0  0 */ \
            \
            /* First filter only */ \
            s           = *(src++); \
            s2          = f->b0[0]*s + d[0]; \
            p1          = f->b1[0]*s + f->a1[0]*s2; \
            p2          = f->b2[0]*s + f->a2[0]*s2; \
            r           = s2; \
            d[0]        = d[2] + p1; \
            d[2]        = p2; \
            inc; \
            \
            /* Both filters */ \
            for (size_t i=1; i<count; ++i) \
            { \
                s           = *(src++); \
                r2          = f->b0[1]*r + d[1]; \
                s2          = f->b0[0]*s + d[0]; \
                \
                q1          = f->b1[1]*r + f->a1[1]*r2; \
                p1          = f->b1[0]*s + f->a1[0]*s2; \
                q2          = f->b2[1]*r + f->a2[1]*r2; \
                p2          = f->b2[0]*s + f->a2[0]*s2; \
                \
                r           = s2; \
                *(dst++)    = r2; \
                \
                /* Shift buffers */ \
                d[1]        = d[3] + q1; \
                d[0]        = d[2] + p1; \
                d[3]        = q2; \
                d[2]        = p2; \
                \
                inc; \
            } \
            \
            /* Second filter only */ \
            r2          = f->b0[1]*r + d[1]; \
            q1          = f->b1[1]*r + f->a1[1]*r2; \
            q2          = f->b2[1]*r + f->a2[1]*r2; \
            *dst        = r2; \
            d[1]        = d[3] + q1; \
            d[3]        = q2;

        #define GENERIC_BIQUAD_X4_CORE(dst, src, d, count, f, dpad, inc) \
            /* Start filters, mask enables the specific filter */ \
            mask = 1; \
            do \
            { \
                /* Push sample */ \
                s[0]        = *(src++); \
                \
                /* Calculate filters by mask and shift buffers */ \
                s2[0]       = f->b0[0]*s[0] + d[0]; \
                p1[0]       = f->b1[0]*s[0] + f->a1[0]*s2[0]; \
                p2[0]       = f->b2[0]*s[0] + f->a2[0]*s2[0]; \
                d[0]        = d[dpad + 0] + p1[0]; \
                d[dpad + 0] = p2[0]; \
                \
                if (mask & 0x2) \
                { \
                    s2[1]       = f->b0[1]*s[1] + d[1]; \
                    p1[1]       = f->b1[1]*s[1] + f->a1[1]*s2[1]; \
                    p2[1]       = f->b2[1]*s[1] + f->a2[1]*s2[1]; \
                    d[1]        = d[dpad + 1] + p1[1]; \
                    d[dpad + 1] = p2[1]; \
                } \
                if (mask & 0x4) \
                { \
                    s2[2]       = f->b0[2]*s[2] + d[2]; \
                    p1[2]       = f->b1[2]*s[2] + f->a1[2]*s2[2]; \
                    p2[2]       = f->b2[2]*s[2] + f->a2[2]*s2[2]; \
                    d[2]        = d[dpad + 2] + p1[2]; \
                    d[dpad + 2] = p2[2]; \
                } \
                inc; \
                \
                /* Shift buffer */ \
                s[3]        = s2[2]; \
                s[2]        = s2[1]; \
                s[1]        = s2[0]; \
                if ((--count) <= 0) \
                    break; \
                mask        = (mask << 1) | 1; \
            } while (mask != 0x0f); \
            \
            /* Process all filters simultaneously, mask = 0x0f */ \
            for ( ; count > 0; --count) \
            { \
                /* Push sample */ \
                s[0]        = *(src++); \
                \
                /* Calculate filters by mask and shift buffers */ \
                s2[0]       = f->b0[0]*s[0] + d[0]; \
                s2[1]       = f->b0[1]*s[1] + d[1]; \
                s2[2]       = f->b0[2]*s[2] + d[2]; \
                s2[3]       = f->b0[3]*s[3] + d[3]; \
                \
                p1[0]       = f->b1[0]*s[0] + f->a1[0]*s2[0]; \
                p1[1]       = f->b1[1]*s[1] + f->a1[1]*s2[1]; \
                p1[2]       = f->b1[2]*s[2] + f->a1[2]*s2[2]; \
                p1[3]       = f->b1[3]*s[3] + f->a1[3]*s2[3]; \
                \
                p2[0]       = f->b2[0]*s[0] + f->a2[0]*s2[0]; \
                p2[1]       = f->b2[1]*s[1] + f->a2[1]*s2[1]; \
                p2[2]       = f->b2[2]*s[2] + f->a2[2]*s2[2]; \
                p2[3]       = f->b2[3]*s[3] + f->a2[3]*s2[3]; \
                \
                d[0]        = d[dpad + 0] + p1[0]; \
                d[1]        = d[dpad + 1] + p1[1]; \
                d[2]        = d[dpad + 2] + p1[2]; \
                d[3]        = d[dpad + 3] + p1[3]; \
                \
                d[dpad + 0] = p2[0]; \
                d[dpad + 1] = p2[1]; \
                d[dpad + 2] = p2[2]; \
                d[dpad + 3] = p2[3]; \
                inc; \
                \
                /* Shift buffer */ \
                *(dst++)    = s2[3]; \
                s[3]        = s2[2]; \
                s[2]        = s2[1]; \
                s[1]        = s2[0]; \
            } \
            \
            /* Finish processing */ \
            mask      <<= 1; \
            do \
            { \
                /* Calculate filters by mask and shift buffers */ \
                if (mask & 0x2) \
                { \
                    s2[1]       = f->b0[1]*s[1] + d[1]; \
                    p1[1]       = f->b1[1]*s[1] + f->a1[1]*s2[1]; \
                    p2[1]       = f->b2[1]*s[1] + f->a2[1]*s2[1]; \
                    d[1]        = d[dpad + 1] + p1[1]; \
                    d[dpad + 1] = p2[1]; \
                } \
                if (mask & 0x4) \
                { \
                    s2[2]       = f->b0[2]*s[2] + d[2]; \
                    p1[2]       = f->b1[2]*s[2] + f->a1[2]*s2[2]; \
                    p2[2]       = f->b2[2]*s[2] + f->a2[2]*s2[2]; \
                    d[2]        = d[dpad + 2] + p1[2]; \
                    d[dpad + 2] = p2[2]; \
                } \
                if (mask & 0x08) \
                { \
                    s2[3]       = f->b0[3]*s[3] + d[3]; \
                    p1[3]       = f->b1[3]*s[3] + f->a1[3]*s2[3]; \
                    p2[3]       = f->b2[3]*s[3] + f->a2[3]*s2[3]; \
                    d[3]        = d[dpad + 3] + p1[3]; \
                    d[dpad + 3] = p2[3]; \
                    *(dst++)    = s2[3]; \
                } \
                inc; \
                \
                /* Shift buffer */ \
                s[3]        = s2[2]; \
                s[2]        = s2[1]; \
                s[1]        = s2[0]; \
                \
                /* Update mask */ \
                mask      <<= 1; \
            } while (mask & 0x0f); \

    } /* namespace lsp */
} /* namespace generic */

#endif /* PRIVATE_DSP_ARCH_GENERIC_FILTERS_COMMON_H_ */
