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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Compute binary logarithm: dst[i] = log(2, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, logb1, float *dst, size_t count);

/**
 * Compute binary logarithm: dst[i] = log(2, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, logb2, float *dst, const float *src, size_t count);

/**
 * Compute natural logarithm: dst[i] = log(E, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, loge1, float *dst, size_t count);

/**
 * Compute natural logarithm: dst[i] = log(E, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, loge2, float *dst, const float *src, size_t count);

/**
 * Compute decimal logarithm: dst[i] = log(10, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, logd1, float *dst, size_t count);

/**
 * Compute decimal logarithm: dst[i] = log(10, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, logd2, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_ */
