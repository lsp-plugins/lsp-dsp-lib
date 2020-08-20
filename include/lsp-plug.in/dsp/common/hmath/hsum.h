/*
 * hsum.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_HMATH_HSUM_H_
#define LSP_PLUG_IN_DSP_COMMON_HMATH_HSUM_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate horizontal sum: result = sum (i) from 0 to count-1 src[i]
 *
 * @param src vector to summarize
 * @param count number of elements
 * @return status of operation
 */
LSP_DSP_LIB_SYMBOL(float, h_sum, const float *src, size_t count);

/** Calculate horizontal sum: result = sum (i) from 0 to count-1 sqr(src[i])
 *
 * @param src vector to summarize
 * @param count number of elements
 * @return status of operation
 */
LSP_DSP_LIB_SYMBOL(float, h_sqr_sum, const float *src, size_t count);

/** Calculate horizontal sum of absolute values: result = sum (i) from 0 to count-1 abs(src[i])
 *
 * @param src vector to summarize
 * @param count number of elements
 * @return status of operation
 */
LSP_DSP_LIB_SYMBOL(float, h_abs_sum, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_HMATH_HSUM_H_ */
