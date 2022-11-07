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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_MINMAX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_MINMAX_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Compute minimum values between two arrays:
 *   dst[i] = min(dst[i], src[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pmin2, float *dst, const float *src, size_t count);

/**
 * Compute minimum values between two arrays with disregarded sign:
 *   dst[i] = (abs(dst[i]) < abs(src[i])) ? dst[i] : src[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, psmin2, float *dst, const float *src, size_t count);

/**
 * Compute absolute minimum values between two arrays:
 *   dst[i] = (abs(dst[i]) < abs(src[i])) ? abs(dst[i]) : abs(src[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pamin2, float *dst, const float *src, size_t count);

/**
 * Compute maximum values between two arrays:
 *   dst[i] = max(dst[i], src[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pmax2, float *dst, const float *src, size_t count);

/**
 * Compute maximum values between two arrays with disregarded sign:
 *   dst[i] = (abs(dst[i]) < abs(src[i])) ? src[i] : dst[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, psmax2, float *dst, const float *src, size_t count);

/**
 * Compute absolute maximum values between two arrays:
 *   dst[i] = (abs(dst[i]) < abs(src[i])) ? abs(src[i]) : abs(dst[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pamax2, float *dst, const float *src, size_t count);

/**
 * Compute minimum values between two arrays:
 *   dst[i] = min(a[i], b[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pmin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Compute minimum values between two arrays with disregarded sign:
 *   dst[i] = (abs(a[i]) < abs(b[i])) ? a[i] : b[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, psmin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Compute absolute minimum values between two arrays:
 *   dst[i] = (abs(a[i]) < abs(b[i])) ? abs(a[i]) : abs(b[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pamin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Compute maximum values between two arrays:
 *   dst[i] = max(a[i], b[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pmax3, float *dst, const float *a, const float *b, size_t count);

/**
 * Compute maximum values between two arrays with disregarded sign:
 *   dst[i] = (abs(a[i]) < abs(b[i])) ? b[i] : a[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, psmax3, float *dst, const float *a, const float *b, size_t count);

/**
 * Compute absolute minimum values between two arrays:
 *   dst[i] = (abs(a[i]) < abs(b[i])) ? abs(b[i]) : abs(a[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, pamax3, float *dst, const float *a, const float *b, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_MINMAX_H_ */
