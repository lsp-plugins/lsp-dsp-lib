/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 26 нояб. 2025 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_CLAMP_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_CLAMP_H_

/**
 * Compute clamped values:
 *   dst[i] = clamp(dst[i], min[i], max[i]) = min(max(dst[i], min[i]), max[i])
 *
 * @param dst destination array
 * @param min the source array with minimum allowed values
 * @param max the source array with maximum allowed values
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, clamp_vv1, float *dst, const float *min, const float *max, size_t count);

/**
 * Compute clamped values:
 *   dst[i] = clamp(src[i], min[i], max[i]) = min(max(src[i], min[i]), max[i])
 *
 * @param dst destination array
 * @param src source array
 * @param min the source array with minimum allowed values
 * @param max the source array with maximum allowed values
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, clamp_vv2, float *dst, const float *src, const float *min, const float *max, size_t count);

/**
 * Compute clamped values:
 *   dst[i] = clamp(dst[i], min, max) = min(max(dst[i], min), max)
 *
 * @param dst destination array
 * @param min the minimum allowed value
 * @param max the maximum allowed value
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, clamp_kk1, float *dst, float min, float max, size_t count);

/**
 * Compute clamped values:
 *   dst[i] = clamp(src[i], min, max) = min(max(src[i], min), max)
 *
 * @param dst destination array to store values
 * @param src source array
 * @param min the minimum allowed value
 * @param max the maximum allowed value
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, clamp_kk2, float *dst, const float *src, float min, float max, size_t count);



#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_CLAMP_H_ */
