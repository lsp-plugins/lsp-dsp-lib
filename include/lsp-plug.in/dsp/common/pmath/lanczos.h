/*
 * Copyright (C) 2025 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2025 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 апр. 2025 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_LANCZOS_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_LANCZOS_H_

/**
 * Calculate lanczos filter response function with generated argument:
 *
 *   for each x = PI * (k*i - p)
 *
 *            { 1.0                            if fabsf(x) <= 1e-6
 *   dst[i] = { sinc(x) * sinc(x*a)/(a * x^2)  if fabsf(x) < t and  fabsf(x) >= 1e-6
 *            { 0.0                            otherwise
 *
 * @param dst destination vector
 * @param src source vector
 * @param k number of samples per lobe multiplied by PI
 * @param p shift in lobes multiplied by PI
 * @param t the number of lobes multiplied by PI
 * @param a reverse number of lobes
 * @param count number of elements
 */
LSP_DSP_LIB_SYMBOL(void, lanczos1, float *dst, float k, float p, float t, float a, size_t count);



#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_SINCSINC_H_ */
