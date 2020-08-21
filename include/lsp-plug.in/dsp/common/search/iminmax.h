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

#ifndef LSP_PLUG_IN_DSP_COMMON_SEARCH_IMINMAX_H_
#define LSP_PLUG_IN_DSP_COMMON_SEARCH_IMINMAX_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate @ min { src }
 *
 * @param src source vector
 * @param count number of elements
 * @return minimum value index
 */
LSP_DSP_LIB_SYMBOL(size_t, min_index, const float *src, size_t count);

/** Calculate @ max { src }
 *
 * @param src source vector
 * @param count number of elements
 * @return maximum value
 */
LSP_DSP_LIB_SYMBOL(size_t, max_index, const float *src, size_t count);

/** Calculate @ minmax { src }
 *
 * @param src source vector
 * @param count number of elements
 * @param min pointer to store minimum value index
 * @param max pointer to store maximum value index
 */
LSP_DSP_LIB_SYMBOL(void, minmax_index, const float *src, size_t count, size_t *min, size_t *max);

/** Calculate @ max { abs(src) }
 *
 * @param src source
 * @param count number of samples
 * @return index of maximum element
 */
LSP_DSP_LIB_SYMBOL(size_t, abs_max_index, const float *src, size_t count);

/** Calculate @ min { abs(src) }
 *
 * @param src source
 * @param count number of samples
 * @return index of maximum element
 */
LSP_DSP_LIB_SYMBOL(size_t, abs_min_index, const float *src, size_t count);

/** Calculate @ minmax { abs(src) }
 *
 * @param src source vector
 * @param count number of elements
 * @param min pointer to store absolute minimum value index
 * @param max pointer to store absolute maximum value index
 */
LSP_DSP_LIB_SYMBOL(void, abs_minmax_index, const float *src, size_t count, size_t *min, size_t *max);

#endif /* LSP_PLUG_IN_DSP_COMMON_SEARCH_IMINMAX_H_ */
