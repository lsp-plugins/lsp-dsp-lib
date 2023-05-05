/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 21 апр. 2023 г.
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

#ifndef PRIVATE_UTEST_DSP_BITMAP_UTIL_H_
#define PRIVATE_UTEST_DSP_BITMAP_UTIL_H_

#include <lsp-plug.in/dsp/dsp.h>

namespace lsp
{
    namespace test
    {
        typedef struct bitmap_pattern_t
        {
            ssize_t x;
            ssize_t y;
            uint8_t fill;
            const char *data;
        } bitmap_pattern_t;

        /**
         * Rasterize bitmap
         * @param bitmap bitmap to rasterize
         * @return rasterized bitmap image, should be free()'d after use
         */
        char *rasterize(const dsp::bitmap_t *bitmap);

    } /* namespace test */
} /* namespace lsp */


#endif /* PRIVATE_UTEST_DSP_BITMAP_UTIL_H_ */
