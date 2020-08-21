/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_DSP_COMMON_COPY_H_
#define LSP_PLUG_IN_DSP_COMMON_COPY_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Copy data: dst[i] = src[i]
 *
 * @param dst destination pointer
 * @param src source pointer
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, copy, float *dst, const float *src, size_t count);

/** Move data: dst[i] = src[i]
 *
 * @param dst destination pointer
 * @param src source pointer
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, move, float *dst, const float *src, size_t count);

/** Fill data: dst[i] = value
 *
 * @param dst destination pointer
 * @param value filling value
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fill, float *dst, float value, size_t count);

/** Fill data with zeros: dst[i] = 0.0f
 *
 * @param dst destination pointer
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fill_zero, float *dst, size_t count);

/** Fill data with ones: dst[i] = 1.0f
 *
 * @param dst destination pointer
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fill_one, float *dst, size_t count);

/** Fill data with negative ones: dst[i] = -1.0f
 *
 * @param dst destination pointer
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fill_minus_one, float *dst, size_t count);

/** Reverse the order of samples: dst[i] <=> dst[count - i - 1]
 *
 * @param dst the buffer to reverse
 * @param count number of samples in buffer
 */
LSP_DSP_LIB_SYMBOL(void, reverse1, float *dst, size_t count);

/** Reverse the order of samples: dst[i] <=> src[count - i - 1]
 *
 * @param dst destination buffer to reverse
 * @param src source buffer to reverse
 * @param count number of samples in buffer
 */
LSP_DSP_LIB_SYMBOL(void, reverse2, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_COPY_H_ */
