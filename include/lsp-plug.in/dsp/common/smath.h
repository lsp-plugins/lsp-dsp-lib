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

#ifndef LSP_PLUG_IN_DSP_COMMON_SMATH_H_
#define LSP_PLUG_IN_DSP_COMMON_SMATH_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Power of floating-point value by integer constant
 *
 * @param x value to power
 * @param deg the power degree
 * @return result of x^deg calculation
 */
LSP_DSP_LIB_SYMBOL(float, ipowf, float x, int deg);

/** Calculate the integer root of value
 *
 * @param x the value to calculate
 * @param deg the root degree, should be positive
 * @return the deg'th root of x
 */
LSP_DSP_LIB_SYMBOL(float, irootf, float x, int deg);

#endif /* LSP_PLUG_IN_DSP_COMMON_SMATH_H_ */
