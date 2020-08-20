/*
 * exp.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_EXP_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_EXP_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Compute dst[i] = exp(dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, exp1, float *dst, size_t count);

/**
 * Compute dst[i] = exp(src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, exp2, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_EXP_H_ */
