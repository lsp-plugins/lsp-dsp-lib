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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_PMIX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_PMIX_H_

/**
 * Compute mixed value:
 *   dst[i] = dst[i] + (src[i] - dst[i]) * k[i];
 *
 * @param dst destination array
 * @param src source array
 * @param k array with mix coefficient [0..1.0]
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pmix_v1, float *dst, const float *src, const float *k, size_t count);

/**
 * Compute mixed value:
 *   dst[i] = src1[i] + (src2[i] - src1[i]) * k[i];
 *
 * @param dst destination array
 * @param src1 first source array
 * @param src2 second source array
 * @param k array with mix coefficient [0..1.0]
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pmix_v2, float *dst, const float *src1, const float *src2, const float *k, size_t count);

/**
 * Compute mixed value:
 *   dst[i] = dst[i] + (src[i] - dst[i]) * k;
 *
 * @param dst destination array
 * @param src source array
 * @param k mix coefficient [0..1.0]
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pmix_k1, float *dst, const float *src, float k, size_t count);

/**
 * Compute mixed value:
 *   dst[i] = src1[i] + (src2[i] - src1[i]) * k;
 *
 * @param dst destination array
 * @param src1 first source array
 * @param src2 second source array
 * @param k mix coefficient [0..1.0]
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pmix_k2, float *dst, const float *src1, const float *src2, float k, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_PMIX_H_ */
