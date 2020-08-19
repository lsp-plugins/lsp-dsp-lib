/*
 * smath.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_SMATH_H_
#define LSP_PLUG_IN_DSP_COMMON_SMATH_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Power of floating-point value by integer constant
 *
 * @param x value to power
 * @param deg the power degree
 * @return result of x^deg calculation
 */
LSP_DSP_LIB_SYMBOL(float , ipowf, float x, int deg);

/** Calculate the integer root of value
 *
 * @param x the value to calculate
 * @param deg the root degree, should be positive
 * @return the deg'th root of x
 */
LSP_DSP_LIB_SYMBOL(float , irootf, float x, int deg);

#endif /* LSP_PLUG_IN_DSP_COMMON_SMATH_H_ */
