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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Add constant value to the data
 * @param dst destination
 * @param k constant value to add
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, add_k2, float *dst, float k, size_t count);

/**
 * Subtract constant value from the data
 * @param dst destination
 * @param k constant value to subtract
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, sub_k2, float *dst, float k, size_t count);

/**
 * Subtract data from constant value
 * @param dst destination
 * @param k constant value to subtract
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rsub_k2, float *dst, float k, size_t count);

/**
 * Divide data by constant value
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, div_k2, float *dst, float k, size_t count);

/**
 * Divide constant value by data
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rdiv_k2, float *dst, float k, size_t count);

/**
 * Compute remainder of dst/k: dst = dst - k * int(dst/k)
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, mod_k2, float *dst, float k, size_t count);

/**
 * Compute remainder of k/dst: dst = k - dst * int(k/dst)
 * @param dst destination/divisor
 * @param k constant value to use as divident
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rmod_k2, float *dst, float k, size_t count);

/** Scale: dst[i] = dst[i] * k
 *
 * @param dst destination
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, mul_k2, float *dst, float k, size_t count);

/**
 * Add constant value to the data
 * @param dst destination
 * @param src source
 * @param k constant value to add
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, add_k3, float *dst, const float *src, float k, size_t count);

/**
 * Subtract constant value from the data
 * @param dst destination
 * @param src source
 * @param k constant value to subtract
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, sub_k3, float *dst, const float *src, float k, size_t count);

/**
 * Subtract value data from constant value
 * @param dst destination
 * @param src source
 * @param k constant value to subtract
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rsub_k3, float *dst, const float *src, float k, size_t count);

/**
 * Divide data by constant value
 * @param dst destination
 * @param src source
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, div_k3, float *dst, const float *src, float k, size_t count);

/**
 * Divide constant value by data
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rdiv_k3, float *dst, const float *src, float k, size_t count);

/**
 * Compute remainder of src/k: dst = src - k * int(src/k)
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, mod_k3, float *dst, const float *src, float k, size_t count);

/**
 * Compute remainder of k/src: dst = k - src * int(k/src)
 * @param dst destination
 * @param k constant value to use as divisor
 * @param count number of elements of destination to modify
 */
LSP_DSP_LIB_SYMBOL(void, rmod_k3, float *dst, const float *src, float k, size_t count);

/** Scale: dst[i] = src[i] * k
 *
 * @param dst destination
 * @param src source
 * @param k multiplier
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, mul_k3, float *dst, const float *src, float k, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_OP_KX_H_ */
