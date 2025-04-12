/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 апр. 2025 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_COS_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_COS_H_

/**
 * Calculate cosine function: dst[i] = cos(dst[i])
 *
 * @param dst destination vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, cosf1, float *dst, size_t count);

/**
 * Calculate cosine function: dst[i] = cos(src[i])
 *
 * @param dst destination vector
 * @param src source vector
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, cosf2, float *dst, const float *src, size_t count);

/**
 * Calculate cosine function with generated argument: dst[i] = cos(k*i + p)
 *
 * @param dst destination vector
 * @param k phase step
 * @param p initial phase
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, cosf_kp1, float *dst, float k, float p, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_COS_H_ */
