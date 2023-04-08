/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 8 апр. 2023 г.
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

#ifndef PRIVATE_DSP_ARCH_GENERIC_BITMAP_HELPERS_H_
#define PRIVATE_DSP_ARCH_GENERIC_BITMAP_HELPERS_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        typedef struct bitmap_part_t
        {
            ssize_t     src_x;
            ssize_t     src_y;
            ssize_t     dst_x;
            ssize_t     dst_y;
            ssize_t     count_x;
            ssize_t     count_y;
        } bitmap_part_t;

        inline bool test_bitmap_put(bitmap_part_t *dst, const bitmap_t *a, const bitmap_t *b, ssize_t x, ssize_t y)
        {
            if ((x >= a->width) || (y >= a->height))
                return false;
            if ((x <= -b->width) || (y <= -b->height))
                return false;

            if (x >= 0)
            {
                dst->src_x      = 0;
                dst->dst_x      = x;
            }
            else
            {
                dst->src_x      = -x;
                dst->dst_x      = 0;
            }

            if (y >= 0)
            {
                dst->src_y      = 0;
                dst->dst_y      = y;
            }
            else
            {
                dst->src_x      = -y;
                dst->dst_x      = 0;
            }

            dst->count_x    = lsp_min(dst->src_x + b->width,  dst->dst_x + a->width ) - dst->src_x;
            dst->count_y    = lsp_min(dst->src_y + b->height, dst->dst_y + a->height) - dst->src_y;

            return true;
        }

    } /* namespace generic */
} /* namespaec lsp */




#endif /* PRIVATE_DSP_ARCH_GENERIC_BITMAP_HELPERS_H_ */
