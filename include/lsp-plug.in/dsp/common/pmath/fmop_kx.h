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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate dst[i] = dst[i] + src[i] * k
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmadd_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = dst[i] - src[i] * k
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmsub_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = src[i] * k - dst[i]
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrsub_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = dst[i] * src[i] * k
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmmul_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = dst[i] / (src[i] * k)
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmdiv_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = (src[i] * k) / dst[i]
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrdiv_k3, float *dst, const float *src, float k, size_t count);

/** Calculate remainder: dst[i] = dst[i] - (src[i]*k)*int(dst[i]/(src[i]*k))
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmmod_k3, float *dst, const float *src, float k, size_t count);

/** Calculate reverse remainder: dst[i] = src[i] - (dst[i]*k)*int(src[i]/(dst[i]*k))
 *
 * @param dst destination array
 * @param src source array
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrmod_k3, float *dst, const float *src, float k, size_t count);

/** Calculate dst[i] = src1[i] + src2[i] * k
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmadd_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate dst[i] = src1[i] - src2[i] * k
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmsub_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate dst[i] = src2[i] * k - src1[i]
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrsub_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate dst[i] = src1[i] * src2[i] * k
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmmul_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate dst[i] = src1[i] / (src2[i] * k)
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmdiv_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate dst[i] = (src2[i] * k) / src1[i]
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrdiv_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate remainder: dst[i] = src1[i] - (src2[i]*k)*int(src1[i]/(src2[i]*k))
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmmod_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

/** Calculate reverse remainder: dst[i] = src2[i]*k - src1[i]*int((src2[i]*k)/src1[i])
 *
 * @param dst destination array
 * @param src1 source array 1
 * @param src2 source array 2
 * @param k multiplier for elements of array 2
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, fmrmod_k4, float *dst, const float *src1, const float *src2, float k, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_FMOP_KX_H_ */
