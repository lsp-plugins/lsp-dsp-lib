/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 20 сент. 2023 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_SQR_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_SQR_H_

#include <lsp-plug.in/dsp/common/types.h>

/**
 * Compute squares: dst[i] = dst[i]*dst[i]
 * @param dst destination
 * @param count number of elements to process
 */
LSP_DSP_LIB_SYMBOL(void, sqr1, float *dst, size_t count);

/**
 * Compute squares: dst[i] = src[i]*src[i]
 * @param dst destination
 * @param src source
 * @param count number of elements in source
 */
LSP_DSP_LIB_SYMBOL(void, sqr2, float *dst, const float *src, size_t count);



#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_SQR_H_ */
