/*
 * log.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Compute binary logarithm: dst[i] = log(2, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, logb1, float *dst, size_t count);

/**
 * Compute binary logarithm: dst[i] = log(2, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, logb2, float *dst, const float *src, size_t count);

/**
 * Compute natural logarithm: dst[i] = log(E, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, loge1, float *dst, size_t count);

/**
 * Compute natural logarithm: dst[i] = log(E, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, loge2, float *dst, const float *src, size_t count);

/**
 * Compute decimal logarithm: dst[i] = log(10, dst[i])
 * @param dst destination
 * @param count number of elements in destination
 */
LSP_DSP_LIB_SYMBOL(void, logd1, float *dst, size_t count);

/**
 * Compute decimal logarithm: dst[i] = log(10, src[i])
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, logd2, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_ */
