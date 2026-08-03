/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_FILTERS_PACK_H_
#define PRIVATE_DSP_ARCH_GENERIC_FILTERS_PACK_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

#include <private/dsp/arch/generic/filters/common.h>

namespace lsp
{
    namespace generic
    {

        void biquad_pack_x2(dsp::biquad_x2_t *dst, const dsp::biquad_x1_t *src)
        {
            dst->b0[0]  = src[0].b0;
            dst->b0[1]  = src[1].b0;
            dst->b1[0]  = src[0].b1;
            dst->b1[1]  = src[1].b1;
            dst->b2[0]  = src[0].b2;
            dst->b2[1]  = src[1].b2;

            dst->a1[0]  = src[0].a1;
            dst->a1[1]  = src[1].a1;
            dst->a2[0]  = src[0].a2;
            dst->a2[1]  = src[1].a2;

            dst->p[0]   = 0.0f;
            dst->p[1]   = 0.0f;
        }

        void biquad_pack_x4(dsp::biquad_x4_t *dst, const dsp::biquad_x1_t *src)
        {
            dst->b0[0]  = src[0].b0;
            dst->b0[1]  = src[1].b0;
            dst->b0[2]  = src[2].b0;
            dst->b0[3]  = src[3].b0;

            dst->b1[0]  = src[0].b1;
            dst->b1[1]  = src[1].b1;
            dst->b1[2]  = src[2].b1;
            dst->b1[3]  = src[3].b1;

            dst->b2[0]  = src[0].b2;
            dst->b2[1]  = src[1].b2;
            dst->b2[2]  = src[2].b2;
            dst->b2[3]  = src[3].b2;

            dst->a1[0]  = src[0].a1;
            dst->a1[1]  = src[1].a1;
            dst->a1[2]  = src[2].a1;
            dst->a1[3]  = src[3].a1;

            dst->a2[0]  = src[0].a2;
            dst->a2[1]  = src[1].a2;
            dst->a2[2]  = src[2].a2;
            dst->a2[3]  = src[3].a2;
        }

        void biquad_pack_x8(dsp::biquad_x8_t *dst, const dsp::biquad_x1_t *src)
        {
            dst->b0[0]  = src[0].b0;
            dst->b0[1]  = src[1].b0;
            dst->b0[2]  = src[2].b0;
            dst->b0[3]  = src[3].b0;
            dst->b0[4]  = src[4].b0;
            dst->b0[5]  = src[5].b0;
            dst->b0[6]  = src[6].b0;
            dst->b0[7]  = src[7].b0;

            dst->b1[0]  = src[0].b1;
            dst->b1[1]  = src[1].b1;
            dst->b1[2]  = src[2].b1;
            dst->b1[3]  = src[3].b1;
            dst->b1[4]  = src[4].b1;
            dst->b1[5]  = src[5].b1;
            dst->b1[6]  = src[6].b1;
            dst->b1[7]  = src[7].b1;

            dst->b2[0]  = src[0].b2;
            dst->b2[1]  = src[1].b2;
            dst->b2[2]  = src[2].b2;
            dst->b2[3]  = src[3].b2;
            dst->b2[4]  = src[4].b2;
            dst->b2[5]  = src[5].b2;
            dst->b2[6]  = src[6].b2;
            dst->b2[7]  = src[7].b2;

            dst->a1[0]  = src[0].a1;
            dst->a1[1]  = src[1].a1;
            dst->a1[2]  = src[2].a1;
            dst->a1[3]  = src[3].a1;
            dst->a1[4]  = src[4].a1;
            dst->a1[5]  = src[5].a1;
            dst->a1[6]  = src[6].a1;
            dst->a1[7]  = src[7].a1;

            dst->a2[0]  = src[0].a2;
            dst->a2[1]  = src[1].a2;
            dst->a2[2]  = src[2].a2;
            dst->a2[3]  = src[3].a2;
            dst->a2[4]  = src[4].a2;
            dst->a2[5]  = src[5].a2;
            dst->a2[6]  = src[6].a2;
            dst->a2[7]  = src[7].a2;
        }

        void biquad_pack_x16(dsp::biquad_x16_t *dst, const dsp::biquad_x1_t *src)
        {
            dst->b0[0]  = src[0].b0;
            dst->b0[1]  = src[1].b0;
            dst->b0[2]  = src[2].b0;
            dst->b0[3]  = src[3].b0;
            dst->b0[4]  = src[4].b0;
            dst->b0[5]  = src[5].b0;
            dst->b0[6]  = src[6].b0;
            dst->b0[7]  = src[7].b0;
            dst->b0[8]  = src[8].b0;
            dst->b0[9]  = src[9].b0;
            dst->b0[10] = src[10].b0;
            dst->b0[11] = src[11].b0;
            dst->b0[12] = src[12].b0;
            dst->b0[13] = src[13].b0;
            dst->b0[14] = src[14].b0;
            dst->b0[15] = src[15].b0;

            dst->b1[0]  = src[0].b1;
            dst->b1[1]  = src[1].b1;
            dst->b1[2]  = src[2].b1;
            dst->b1[3]  = src[3].b1;
            dst->b1[4]  = src[4].b1;
            dst->b1[5]  = src[5].b1;
            dst->b1[6]  = src[6].b1;
            dst->b1[7]  = src[7].b1;
            dst->b1[8]  = src[8].b1;
            dst->b1[9]  = src[9].b1;
            dst->b1[10] = src[10].b1;
            dst->b1[11] = src[11].b1;
            dst->b1[12] = src[12].b1;
            dst->b1[13] = src[13].b1;
            dst->b1[14] = src[14].b1;
            dst->b1[15] = src[15].b1;

            dst->b2[0]  = src[0].b2;
            dst->b2[1]  = src[1].b2;
            dst->b2[2]  = src[2].b2;
            dst->b2[3]  = src[3].b2;
            dst->b2[4]  = src[4].b2;
            dst->b2[5]  = src[5].b2;
            dst->b2[6]  = src[6].b2;
            dst->b2[7]  = src[7].b2;
            dst->b2[8]  = src[8].b2;
            dst->b2[9]  = src[9].b2;
            dst->b2[10] = src[10].b2;
            dst->b2[11] = src[11].b2;
            dst->b2[12] = src[12].b2;
            dst->b2[13] = src[13].b2;
            dst->b2[14] = src[14].b2;
            dst->b2[15] = src[15].b2;

            dst->a1[0]  = src[0].a1;
            dst->a1[1]  = src[1].a1;
            dst->a1[2]  = src[2].a1;
            dst->a1[3]  = src[3].a1;
            dst->a1[4]  = src[4].a1;
            dst->a1[5]  = src[5].a1;
            dst->a1[6]  = src[6].a1;
            dst->a1[7]  = src[7].a1;
            dst->a1[8]  = src[8].a1;
            dst->a1[9]  = src[9].a1;
            dst->a1[10] = src[10].a1;
            dst->a1[11] = src[11].a1;
            dst->a1[12] = src[12].a1;
            dst->a1[13] = src[13].a1;
            dst->a1[14] = src[14].a1;
            dst->a1[15] = src[15].a1;

            dst->a2[0]  = src[0].a2;
            dst->a2[1]  = src[1].a2;
            dst->a2[2]  = src[2].a2;
            dst->a2[3]  = src[3].a2;
            dst->a2[4]  = src[4].a2;
            dst->a2[5]  = src[5].a2;
            dst->a2[6]  = src[6].a2;
            dst->a2[7]  = src[7].a2;
            dst->a2[8]  = src[8].a2;
            dst->a2[9]  = src[9].a2;
            dst->a2[10] = src[10].a2;
            dst->a2[11] = src[11].a2;
            dst->a2[12] = src[12].a2;
            dst->a2[13] = src[13].a2;
            dst->a2[14] = src[14].a2;
            dst->a2[15] = src[15].a2;
        }

    } /* namespace generic */
} /* namespace lsp */

#endif /* PRIVATE_DSP_ARCH_GENERIC_FILTERS_PACK_H_ */
