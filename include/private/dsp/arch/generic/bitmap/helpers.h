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

#include <lsp-plug.in/common/types.h>

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

        static inline bitmap_part_t bitmap_clip_rect(const bitmap_t *dst, const bitmap_t *src, ssize_t x, ssize_t y)
        {
            bitmap_part_t rect;

            rect.dst_x      = lsp_max(ssize_t(0), x);
            rect.dst_y      = lsp_max(ssize_t(0), y);
            rect.src_x      = rect.dst_x - x;
            rect.src_y      = rect.dst_y - y;
            rect.count_y    = lsp_min(dst->height - rect.dst_y, src->height - rect.src_y);
            rect.count_x    = lsp_min(dst->width  - rect.dst_x, src->width  - rect.src_x);

            return rect;
        }

        static inline uint8_t b8_saturate(int value)
        {
            return (value < 0) ? 0x00 : (value > 0xff) ? 0xff : value;
        }

    } /* namespace generic */
} /* namespaec lsp */




#endif /* PRIVATE_DSP_ARCH_GENERIC_BITMAP_HELPERS_H_ */
