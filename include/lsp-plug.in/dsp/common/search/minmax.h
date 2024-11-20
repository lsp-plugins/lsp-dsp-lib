/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_
#define LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate min { src }
 *
 * @param src source vector
 * @param count number of elements
 * @return minimum value
 */
LSP_DSP_LIB_SYMBOL(float, min, const float *src, size_t count);

/** Calculate max { src }
 *
 * @param src source vector
 * @param count number of elements
 * @return maximum value
 */
LSP_DSP_LIB_SYMBOL(float, max, const float *src, size_t count);

/** Get absolute maximum: result = max { abs(src[i]) }
 *
 * @param src source array
 * @param count number of elements
 * @return result
 */
LSP_DSP_LIB_SYMBOL(float, abs_max, const float *src, size_t count);

/** Get maximum ignoring sign: result = src[i] : abs(src[i]) -> max
 *
 * @param src source array
 * @param count number of elements
 * @return result
 */
LSP_DSP_LIB_SYMBOL(float, sign_max, const float *src, size_t count);

/** Get absolute minimum: result = min { abs(src[i]) }
 *
 * @param src source array
 * @param count number of elements
 * @return result
 */
LSP_DSP_LIB_SYMBOL(float, abs_min, const float *src, size_t count);

/** Get maximum ignoring sign: result = src[i] : abs(src[i]) -> min
 *
 * @param src source array
 * @param count number of elements
 * @return result
 */
LSP_DSP_LIB_SYMBOL(float, sign_min, const float *src, size_t count);

/** Calculate min { src }, max { src }
 *
 * @param src source vector
 * @param min pointer to store minimum value
 * @param max pointer to store maximum value
 * @param count number of elements
 * @return maximum value
 */
LSP_DSP_LIB_SYMBOL(void, minmax, const float *src, size_t count, float *min, float *max);

/** Calculate min { abs(src) }, max { abs(src) }
 *
 * @param src source vector
 * @param min pointer to store minimum value
 * @param max pointer to store maximum value
 * @param count number of elements
 * @return maximum value
 */
LSP_DSP_LIB_SYMBOL(void, abs_minmax, const float *src, size_t count, float *min, float *max);

/** Calculate:
 *    min = src[i] : abs(src[i]) -> min,
 *    max = src[i] : abs(src[i]) -> max
 *
 * @param src source vector
 * @param min pointer to store minimum value
 * @param max pointer to store maximum value
 * @param count number of elements
 * @return maximum value
 */
LSP_DSP_LIB_SYMBOL(void, sign_minmax, const float *src, size_t count, float *min, float *max);

#endif /* LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_ */
