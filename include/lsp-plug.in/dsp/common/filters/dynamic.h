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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/** Process single dynamic bi-quadratic filter for multiple samples
 *
 * @param dst array of count destination samples to emit
 * @param src array of count source samples to process
 * @param d pointer to filter memory (2 floats)
 * @param count number of samples to process
 * @param f array of count memory-aligned bi-quadratic filters
 */
LSP_DSP_LIB_SYMBOL(void, dyn_biquad_process_x1, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x1_t) *f);

/** Process two dynamic bi-quadratic filters for multiple samples
 *
 * @param dst array of count destination samples to emit
 * @param src array of count source samples to process
 * @param d pointer to filter memory (2 floats)
 * @param count number of samples to process
 * @param f array matrix of (count+1)*2 memory-aligned bi-quadratic filters
 */
LSP_DSP_LIB_SYMBOL(void, dyn_biquad_process_x2, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x2_t) *f);

/** Process four dynamic bi-quadratic filters for multiple samples
 *
 * @param dst array of count destination samples to emit
 * @param src array of count source samples to process
 * @param d pointer to filter memory (8 floats)
 * @param count number of samples to process
 * @param f array matrix of (count+3)*4 memory-aligned bi-quadratic filters
 */
LSP_DSP_LIB_SYMBOL(void, dyn_biquad_process_x4, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x4_t) *f);

/** Process eight dynamic bi-quadratic filters for multiple samples
 *
 * @param dst array of count destination samples to emit
 * @param src array of count source samples to process
 * @param d pointer to filter memory (16 floats)
 * @param count number of samples to process
 * @param f array matrix of (count+7)*8 memory-aligned bi-quadratic filters
 */
LSP_DSP_LIB_SYMBOL(void, dyn_biquad_process_x8, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x8_t) *f);

#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_ */
