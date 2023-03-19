/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 февр. 2023 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_LRAMP_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_LRAMP_H_


/** Calculate linear ramping envelope: dst[i] = v1 + ((v2-v1)*i) / count
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_set1, float *dst, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = dst[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp1, float *dst, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = src[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp2, float *dst, const float *src, float v1, float v2, size_t count);


/** Apply linear ramping envelope: dst[i] = dst[i] + src[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_add2, float *dst, const float *src, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = dst[i] - src[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_sub2, float *dst, const float *src, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = src[i] * (v1 + ((v2-v1)*i) / count) - dst[i]
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_rsub2, float *dst, const float *src, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = dst[i] * src[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_mul2, float *dst, const float *src, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = dst[i] / (src[i] * (v1 + ((v2-v1)*i) / count))
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_div2, float *dst, const float *src, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = (src[i] * (v1 + ((v2-v1)*i) / count)) / dst[i]
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_rdiv2, float *dst, const float *src, float v1, float v2, size_t count);


/** Apply linear ramping envelope: dst[i] = a[i] + b[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_add3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = a[i] - b[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_sub3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = b[i] * (v1 + ((v2-v1)*i) / count) - a[i]
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_rsub3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = a[i] * b[i] * (v1 + ((v2-v1)*i) / count)
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_mul3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = a[i] / (b[i] * (v1 + ((v2-v1)*i) / count))
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_div3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

/** Apply linear ramping envelope: dst[i] = (b[i] * (v1 + ((v2-v1)*i) / count)) / a[i]
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lramp_rdiv3, float *dst, const float *a, const float *b, float v1, float v2, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_LRAMP_H_ */
