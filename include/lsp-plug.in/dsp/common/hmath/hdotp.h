/*
 * hdotp.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_HMATH_HDOTP_H_
#define LSP_PLUG_IN_DSP_COMMON_HMATH_HDOTP_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate dot product: sum {from 0 to count-1} (a[i] * b[i])
 *
 * @param a first vector
 * @param b second vector
 * @param count number of elements
 * @return scalar multiplication
 */
LSP_DSP_LIB_SYMBOL(float, h_dotp, const float *a, const float *b, size_t count);

/** Calculate dot product of squares: sum {from 0 to count-1} (sqr(a[i]) * sqr(b[i]))
 *
 * @param a first vector
 * @param b second vector
 * @param count number of elements
 * @return scalar multiplication
 */
LSP_DSP_LIB_SYMBOL(float, h_sqr_dotp, const float *a, const float *b, size_t count);

/** Calculate dot product of absolute values: sum {from 0 to count-1} (abs(a[i]) * abs(b[i]))
 *
 * @param a first vector
 * @param b second vector
 * @param count number of elements
 * @return scalar multiplication
 */
LSP_DSP_LIB_SYMBOL(float, h_abs_dotp, const float *a, const float *b, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_HMATH_HDOTP_H_ */
