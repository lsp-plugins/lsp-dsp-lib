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

#include <lsp-plug.in/stdlib/stdlib.h>
#include <private/utest/dsp/bitmap/util.h>

namespace lsp
{
    namespace test
    {
        static const char hex_chars[] = "0123456789abcdef";

        /**
         * Rasterize bitmap
         * @param bitmap bitmap to rasterize
         * @return rasterized bitmap image, should be free()'d after use
         */
        char *rasterize(const dsp::bitmap_t *bitmap)
        {
            size_t to_alloc = (bitmap->width * 2 + 1) * bitmap->height + 1;

            char *buf = reinterpret_cast<char *>(malloc(to_alloc));
            if (buf == NULL)
                return buf;

            char *p = buf;
            for (ssize_t y=0; y<bitmap->height; ++y)
            {
                const uint8_t *s = &bitmap->data[y * bitmap->stride];
                for (ssize_t x=0; x<bitmap->width; ++x)
                {
                    uint8_t b   = s[x];
                    *(p++)      = hex_chars[b >> 4];
                    *(p++)      = hex_chars[b & 0x0f];
                }
                *(p++)      = '\n';
            }
            *(p++)      = '\0';

            return buf;
        }

    } /* namespace test */
} /* namespace lsp */


