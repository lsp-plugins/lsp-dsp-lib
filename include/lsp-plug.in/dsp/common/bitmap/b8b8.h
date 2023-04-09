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

#ifndef LSP_PLUG_IN_DSP_COMMON_BITMAP_B8B8_H_
#define LSP_PLUG_IN_DSP_COMMON_BITMAP_B8B8_H_

#include <lsp-plug.in/dsp/common/bitmap.h>

/**
 * Put the source 8-bit bitmap to destination 8-bit bitmap,
 * the result pixel value will be computed as: result[i] = src[i]
 */
LSP_DSP_LIB_SYMBOL(void, bitmap_put_b8b8, LSP_DSP_LIB_TYPE(bitmap_t) *dst, const LSP_DSP_LIB_TYPE(bitmap_t) *src, ssize_t x, ssize_t y);

/**
 * Put the source 8-bit bitmap to destination 8-bit bitmap,
 * the result pixel value will be computed as: result[i] = saturate(dst[i] + src[i])
 */
LSP_DSP_LIB_SYMBOL(void, bitmap_add_b8b8, LSP_DSP_LIB_TYPE(bitmap_t) *dst, const LSP_DSP_LIB_TYPE(bitmap_t) *src, ssize_t x, ssize_t y);

/**
 * Put the source 8-bit bitmap to destination 8-bit bitmap,
 * the result pixel value will be computed as: result[i] = saturate(dst[i] - src[i])
 */
LSP_DSP_LIB_SYMBOL(void, bitmap_sub_b8b8, LSP_DSP_LIB_TYPE(bitmap_t) *dst, const LSP_DSP_LIB_TYPE(bitmap_t) *src, ssize_t x, ssize_t y);

/**
 * Put the source 8-bit bitmap to destination 8-bit bitmap,
 * the result pixel value will be computed as: result[i] = max(dst[i], src[i])
 */
LSP_DSP_LIB_SYMBOL(void, bitmap_max_b8b8, LSP_DSP_LIB_TYPE(bitmap_t) *dst, const LSP_DSP_LIB_TYPE(bitmap_t) *src, ssize_t x, ssize_t y);

/**
 * Put the source 8-bit bitmap to destination 8-bit bitmap,
 * the result pixel value will be computed as: result[i] = min(dst[i], src[i])
 */
LSP_DSP_LIB_SYMBOL(void, bitmap_min_b8b8, LSP_DSP_LIB_TYPE(bitmap_t) *dst, const LSP_DSP_LIB_TYPE(bitmap_t) *src, ssize_t x, ssize_t y);

#endif /* LSP_PLUG_IN_DSP_COMMON_BITMAP_B8B8_H_ */
