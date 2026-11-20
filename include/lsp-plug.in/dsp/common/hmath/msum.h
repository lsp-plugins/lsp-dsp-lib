/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 нояб. 2026 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_HMATH_MSUM_H_
#define LSP_PLUG_IN_DSP_COMMON_HMATH_MSUM_H_

#include <lsp-plug.in/dsp/common/types.h>

/** Calculate moving average:
 *   value  = value + b[i] - a[i];
 *   dst[i] = value * avg;
 *
 * @param dst destination to store elements of the moving sum
 * @param a the tail of the moving sum (values to subtract)
 * @param b the head of the moving sum (values to add)
 * @param value current accumulator value of the moving sum
 * @param avg averaging coefficient
 * @param count number of elements to process
 * @return the new accumulator value of the moving sum (equals to last processed element)
 */
LSP_DSP_LIB_SYMBOL(float, moving_avg, float *dst, const float *a, const float *b, float value, float avg, size_t count);

/** Calculate moving mean square:
 *   value  = value + b[i]*b[i] - a[i]*a[i];
 *   dst[i] = value * avg;
 *
 * @param dst destination to store elements of the moving sum
 * @param a the tail of the moving sum (values to subtract)
 * @param b the head of the moving sum (values to add)
 * @param value current accumulator value of the moving sum
 * @param avg averaging coefficient
 * @param count number of elements to process
 * @return the new accumulator value of the moving sum (equals to last processed element)
 */
LSP_DSP_LIB_SYMBOL(float, moving_mean_sqr, float *dst, const float *a, const float *b, float value, float avg, size_t count);

/** Calculate moving root mean square:
 *   value  = value + b[i]*b[i] - a[i]*a[i];
 *   dst[i] = sqrtf(max(value * avg, 0.0f));
 *
 * Because moving sum can introduce some rounding errors, safe square root form is used to avoid NaNs.
 *
 * @param dst destination to store elements of the moving sum
 * @param a the tail of the moving sum (values to subtract)
 * @param b the head of the moving sum (values to add)
 * @param value current accumulator value of the moving sum
 * @param avg averaging coefficient
 * @param count number of elements to process
 * @return the new accumulator value of the moving sum (equals to last processed element)
 */
LSP_DSP_LIB_SYMBOL(float, moving_rms, float *dst, const float *a, const float *b, float value, float avg, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_HMATH_MSUM_H_ */
