/*
 * smath.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_SMATH_H_
#define LSP_PLUG_IN_DSP_COMMON_SMATH_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Power of floating-point value by integer constant
         *
         * @param x value to power
         * @param deg the power degree
         * @return result of x^deg calculation
         */
        extern float (* ipowf)(float x, int deg);

        /** Calculate the integer root of value
         *
         * @param x the value to calculate
         * @param deg the root degree, should be positive
         * @return the deg'th root of x
         */
        extern float (* irootf)(float x, int deg);
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_SMATH_H_ */
