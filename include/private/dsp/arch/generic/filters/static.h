/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_FILTERS_STATIC_H_
#define PRIVATE_DSP_ARCH_GENERIC_FILTERS_STATIC_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <private/dsp/arch/generic/filters/common.h>

namespace lsp
{
    namespace generic
    {
        void biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
        {
            GENERIC_BIQUAD_X1_CORE(dst, src, d, count, f, );
        }

        void biquad_process_x2(float *dst, const float *src, float * d, size_t count, const biquad_x2_t *f)
        {
            if (count <= 0)
                return;

            GENERIC_BIQUAD_X2_CORE(dst, src, d, count, f, );
        }

        void biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
        {
            if (count <= 0)
                return;

            size_t mask;
            float s[4], s2[4], p1[4], p2[4];
            s[0]            = 0.0f;
            s[1]            = 0.0f;
            s[2]            = 0.0f;
            s[3]            = 0.0f;
            s2[0]           = 0.0f;
            s2[1]           = 0.0f;
            s2[2]           = 0.0f;
            s2[3]           = 0.0f;

            GENERIC_BIQUAD_X4_CORE(dst, src, d, count, f, 4, );
        }

        void biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
        {
            // This code already works badly instead of biquad_process_x4
            if (count <= 0)
                return;

            size_t mask;
            float s[4], s2[4], p1[4], p2[4];
            s[0]            = 0.0f;
            s[1]            = 0.0f;
            s[2]            = 0.0f;
            s[3]            = 0.0f;
            s2[0]           = 0.0f;
            s2[1]           = 0.0f;
            s2[2]           = 0.0f;
            s2[3]           = 0.0f;

            // Step 1
            float *dp       = dst;
            size_t i        = count;
            GENERIC_BIQUAD_X4_CORE(dp, src, d, i, f, 8, );
            d              += 4;

            // Step 2
            const biquad_x8_t *bq = reinterpret_cast<const biquad_x8_t *>(&f->b0[4]);
            dp              = dst;
            i               = count;
            GENERIC_BIQUAD_X4_CORE(dp, dst, d, i, bq, 8, );
        }

        void biquad_process_x16(float *dst, const float *src, float *d, size_t count, const biquad_x16_t *f)
        {
            // This code already works badly instead of biquad_process_x4
            if (count <= 0)
                return;

            size_t mask;
            float s[4], s2[4], p1[4], p2[4];
            s[0]            = 0.0f;
            s[1]            = 0.0f;
            s[2]            = 0.0f;
            s[3]            = 0.0f;
            s2[0]           = 0.0f;
            s2[1]           = 0.0f;
            s2[2]           = 0.0f;
            s2[3]           = 0.0f;

            // Step 1
            float *dp       = dst;
            size_t i        = count;
            GENERIC_BIQUAD_X4_CORE(dp, src, d, i, f, 16, );
            d              += 4;

            // Step 2
            const biquad_x16_t *bq   = reinterpret_cast<const biquad_x16_t *>(&f->b0[4]);
            const float *sp = dst;
            dp              = dst;
            i               = count;
            GENERIC_BIQUAD_X4_CORE(dp, sp, d, i, bq, 16, );
            d              += 4;

            // Step 3
            bq              = reinterpret_cast<const biquad_x16_t *>(&f->b0[8]);
            sp              = dst;
            dp              = dst;
            i               = count;
            GENERIC_BIQUAD_X4_CORE(dp, sp, d, i, bq, 16, );
            d              += 4;

            // Step 4
            bq              = reinterpret_cast<const biquad_x16_t *>(&f->b0[12]);
            dp              = dst;
            i               = count;
            GENERIC_BIQUAD_X4_CORE(dp, dst, d, i, bq, 16, );
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_FILTERS_STATIC_H_ */
