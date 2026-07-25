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

#ifndef PRIVATE_DSP_ARCH_GENERIC_FILTERS_DYNAMIC_H_
#define PRIVATE_DSP_ARCH_GENERIC_FILTERS_DYNAMIC_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <private/dsp/arch/generic/filters/common.h>

namespace lsp
{
    namespace generic
    {
        void dyn_biquad_process_x1(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f)
        {
            GENERIC_BIQUAD_X1_CORE(dst, src, d, count, f, ++f);
        }

        void dyn_biquad_process_x2(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f)
        {
            if (count <= 0)
                return;

            GENERIC_BIQUAD_X2_CORE(dst, src, d, count, f, ++f);
        }

        void dyn_biquad_process_x4(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f)
        {
            if (count <= 0)
                return;

            size_t mask;
            float s[4], s2[4], p1[4], p2[4];
            s[0]        = 0.0f;
            s[1]        = 0.0f;
            s[2]        = 0.0f;
            s[3]        = 0.0f;
            s2[0]       = 0.0f;
            s2[1]       = 0.0f;
            s2[2]       = 0.0f;
            s2[3]       = 0.0f;

            GENERIC_BIQUAD_X4_CORE(dst, src, d, count, f, 0x04, ++f);
        }

        void dyn_biquad_process_x8(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f)
        {
            // This code already works worse than biquad_process_x4
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
            const biquad_x8_t *bq = f;
            size_t i        = count;
            GENERIC_BIQUAD_X4_CORE(dp, src, d, i, bq, 8, ++bq);
            d              += 4;

            // Step 2
            bq              = reinterpret_cast<const biquad_x8_t *>(&f[4].b0[4]);
            dp              = dst;
            i               = count;
            GENERIC_BIQUAD_X4_CORE(dp, dst, d, i, bq, 8, ++bq);
        }
    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_FILTERS_DYNAMIC_H_ */
