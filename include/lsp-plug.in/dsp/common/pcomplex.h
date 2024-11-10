/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PCOMPLEX_H_
#define LSP_PLUG_IN_DSP_COMMON_PCOMPLEX_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate packed complex multiplication: dst = src1 * src2
 *
 * @param dst destination to store complex numbers
 * @param src1 source 1
 * @param src2 source 2
 * @param count number of multiplications
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_mul3, float *dst, const float *src1, const float *src2, size_t count);

/** Calculate packed complex multiplication: dst = dst * src
 *
 * @param dst destination and source to store complex numbers
 * @param src source
 * @param count number of multiplications
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_mul2, float *dst, const float *src, size_t count);

/**
 * Divide complex numbers:
 *      dst = dst / src
 *
 * @param dst destination, source top
 * @param src bottom
 * @param count number of divisions
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_div2, float *dst, const float *src, size_t count);

/**
 * Divide complex numbers:
 *      dst = src / dst
 *
 * @param dst destination, source bottom
 * @param src top
 * @param count number of divisions
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_rdiv2, float *dst, const float *src, size_t count);

/**
 * Divide complex numbers:
 *      dst = t / b
 *
 * @param dst destination
 * @param t top
 * @param b bottom
 * @param count number of divisions
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_div3, float *dst, const float *t, const float *b, size_t count);

/** Calculate packed complex reciprocal:
 *   dst[i].{re,im} = 1 / (dst[i].re + j * dst[i].im)
 *
 * @param dst source, destination to store complex numbers
 * @param count number of multiplications
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_rcp1, float *dst, size_t count);

/** Calculate packed complex reciprocal:
 *   dst[i].{re,im} = 1 / (src[i].re + j * src[i].im)
 *
 * @param dst destination to store complex numbers
 * @param src source
 * @param count number of multiplications
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_rcp2, float *dst, const float *src, size_t count);

/** Fill output array with same complex numbers
 *
 * @param dst target array to fill
 * @param re real part of complex number
 * @param im imaginary part of complex number
 * @param count number of elements to fill
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_fill_ri, float *dst, float re, float im, size_t count);

/** Convert real to packed complex:
 *  dst[i].re = src[i]
 *  dst[i].im = 0
 *
 * @param dst destination packed complex data
 * @param src source real data
 * @param count number of items to convert
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c, float *dst, const float *src, size_t count);

/** Convert packed complex to real:
 *  dst[i] = src[i].re
 *
 * @param dst destination real data
 * @param src source packed complex data
 * @param count number of items to convert
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r, float *dst, const float *src, size_t count);

/** Get module for complex numbers: mod = sqrt(re*re + im*im)
 *
 * @param dst_mod array to sore module
 * @param src packed complex number data
 * @param count count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_mod, float *dst_mod, const float *src, size_t count);

/** Convert packed complex number to polar form
 *
 * @param mod module of the complex number
 * @param arg argument of the complex number in range of [-PI, +PI]
 * @param src packed complex number data
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_modarg, float *mod, float *arg, const float *src, size_t count);

/** Get argument for complex numbers in range of [-PI; +PI]
 *
 * @param dst array to sore argument
 * @param src packed complex number data
 * @param count count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_arg, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] + src[i].re
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_add2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] - src[i].re
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_sub2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = src[i].re - dst[i]
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_rsub2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] * src[i].re
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_mul2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] / src[i].re
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_div2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = src[i].re / dst[i]
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_c2r_rdiv2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] + pcomplex{src[i], 0}
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_add2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] - pcomplex{src[i], 0}
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_sub2, float *dst, const float *src, size_t count);

/**
* Calculate: dst[i] = pcomplex{src[i], 0} - dst[i]
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_rsub2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] * pcomplex{src[i], 0}
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_mul2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = dst[i] / pcomplex{src[i], 0}
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_div2, float *dst, const float *src, size_t count);

/**
 * Calculate: dst[i] = pcomplex{src[i], 0} / dst[i]
 * @param dst destination real number array
 * @param src source packed complex number array
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_r2c_rdiv2, float *dst, const float *src, size_t count);

/** Compute complex correlation between two sources and store to the result array
 *
 * @param dst_corr array to store normalized correlation
 * @param src1 set of complex numbers
 * @param src2 set of complex numbers
 * @param count count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, pcomplex_corr, float *dst_corr, const float *src1, const float *src2, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PCOMPLEX_H_ */
