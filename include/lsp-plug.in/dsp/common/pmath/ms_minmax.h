/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 28 мар. 2026 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_MS_MINMAX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_MS_MINMAX_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Transform signal to Mid/Side and compute minimum values between mid/side signal of two arrays:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = min(mid[i], side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pmin2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute minimum values between two arrays with disregarded sign:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? mid[i] : side[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_psmin2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute absolute minimum values between two arrays:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? abs(mid[i]) : abs(side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pamin2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute maximum values between two arrays:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = max(mid[i], side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pmax2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute maximum values between two arrays with disregarded sign:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? mid[i] : side[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_psmax2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute absolute maximum values between two arrays:
 *   mid[i] = (dst[i] + src[i]) * 0.5
 *   side[i]= (dst[i] - src[i]) * 0.5
 *   dst[i] = (abs(dst[i]) < abs(src[i])) ? abs(src[i]) : abs(dst[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pamax2, float *dst, const float *src, size_t count);

/**
 * Transform signal to Mid/Side and compute minimum values between two arrays:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = min(mid[i], side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pmin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Transform signal to Mid/Side and compute minimum values between two arrays with disregarded sign:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? mid[i] : side[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_psmin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Transform signal to Mid/Side and compute absolute minimum values between two arrays:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? abs(mid[i]) : abs(side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pamin3, float *dst, const float *a, const float *b, size_t count);

/**
 * Transform signal to Mid/Side and compute maximum values between two arrays:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = max(mid[i], side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pmax3, float *dst, const float *a, const float *b, size_t count);

/**
 * Transform signal to Mid/Side and compute maximum values between two arrays with disregarded sign:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? mid[i] : side[i]
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_psmax3, float *dst, const float *a, const float *b, size_t count);

/**
 * Transform signal to Mid/Side and compute absolute minimum values between two arrays:
 *   mid[i] = (a[i] + b[i]) * 0.5
 *   side[i]= (a[i] - b[i]) * 0.5
 *   dst[i] = (abs(mid[i]) < abs(side[i])) ? abs(mid[i]) : abs(side[i])
 * @param dst destination array
 * @param src source array
 * @param count number of elements in each array
 */
LSP_DSP_LIB_SYMBOL(void, ms_pamax3, float *dst, const float *a, const float *b, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_MS_MINMAX_H_ */
