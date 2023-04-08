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

#ifndef LSP_PLUG_IN_DSP_COMMON_BITMAP_H_
#define LSP_PLUG_IN_DSP_COMMON_BITMAP_H_

#include <lsp-plug.in/dsp/common/types.h>

#ifdef __cplusplus
namespace lsp
{
    namespace dsp
    {
#endif /* __cplusplus */

        typedef struct LSP_DSP_LIB_TYPE(bitmap_t)
        {
            ssize_t     width;
            ssize_t     height;
            ssize_t     stride;
            uint8_t    *data;
        } LSP_DSP_LIB_TYPE(bitmap_t);

#ifdef __cplusplus
    } /* namespace dsp */
} /* namespace lsp */
#endif /* __cplusplus */

#include <lsp-plug.in/dsp/common/bitmap/b1b8.h>
#include <lsp-plug.in/dsp/common/bitmap/b2b8.h>
#include <lsp-plug.in/dsp/common/bitmap/b4b8.h>
#include <lsp-plug.in/dsp/common/bitmap/b8b8.h>

#endif /* LSP_PLUG_IN_DSP_COMMON_BITMAP_H_ */
