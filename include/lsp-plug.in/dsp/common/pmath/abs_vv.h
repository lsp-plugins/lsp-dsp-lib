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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_ABS_VV_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_ABS_VV_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate absolute values: dst[i] = abs(dst[i])
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs1, float *dst, size_t count);

/** Calculate absolute values: dst[i] = abs(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = dst[i] + abs(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_add2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = dst[i] - abs(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_sub2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = abs(src[i]) - dst[i]
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_rsub2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = dst[i] * abs(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_mul2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = dst[i] / abs(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_div2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = abs(src[i]) / dst[i]
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_rdiv2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = max(abs(src[i]), dst[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_max2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = min(abs(src[i]), dst[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_min2, float *dst, const float *src, size_t count);

/** Calculate absolute values: dst[i] = src1[i] + abs(src2[i])
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_add3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = src1[i] - abs(src2[i])
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_sub3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = abs(src2[i]) - src1[i]
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_rsub3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = src1[i] * abs(src2[i])
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_mul3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = src1[i] / abs(src2[i])
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_div3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = abs(src2[i]) / src1[i]
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_rdiv3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = max(src1[i], abs(src2[i]))
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_max3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate absolute values: dst[i] = min(src1[i], abs(src2[i]))
 *
 * @param dst destination vector
 * @param src1 source vector 1
 * @param src2 source vector 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, abs_min3, float *dst, const float *src1, const float *src2, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_ABS_VV_H_ */
