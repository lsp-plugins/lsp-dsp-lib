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

#ifndef LSP_PLUG_IN_DSP_COMMON_FASTCONV_H_
#define LSP_PLUG_IN_DSP_COMMON_FASTCONV_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Parse input real data to fast convolution data
 *
 * @param dst destination buffer of 2^(rank+1) floats
 * @param src source real data of 2^(rank-1) floats
 * @param rank the convolution rank
 */
LSP_DSP_LIB_SYMBOL(void, fastconv_parse, float *dst, const float *src, size_t rank);

/** Parse input real data to fast convolution data,
 *  convolve with another convolution data,
 *  restore data to real data and add to output buffer
 *
 * @param dst target real data of 2^rank floats to store convolved data
 * @param tmp temporary buffer of 2^(rank+1) floats to store intermediate data
 * @param c fast convolution data of 2^(rank+1) floats to apply to the buffer
 * @param src source real data of 2^(rank-1) floats
 * @param rank the convolution rank
 */
LSP_DSP_LIB_SYMBOL(void, fastconv_parse_apply, float *dst, float *tmp, const float *c, const float *src, size_t rank);

/** Restore convolution to real data,
 * modifies the source fast convolution data
 *
 * @param dst destination real data of 2^rank floats
 * @param src source fast convolution data of 2^(rank+1) floats
 * @param rank the convolution rank
 */
LSP_DSP_LIB_SYMBOL(void, fastconv_restore, float *dst, float *src, size_t rank);

/** Convolve two convolutions and restore data to real data
 * and add to output buffer
 *
 * @param dst target real data of 2^rank floats to apply convolved data
 * @param tmp temporary buffer of 2^(rank+1) floats to store intermediate data
 * @param c1 fast convolution data of 2^(rank+1) floats
 * @param c2 fast convolution data of 2^(rank+1) floats
 * @param rank the convolution rank
 */
LSP_DSP_LIB_SYMBOL(void, fastconv_apply, float *dst, float *tmp, const float *c1, const float *c2, size_t rank);

#endif /* LSP_PLUG_IN_DSP_COMMON_FASTCONV_H_ */
