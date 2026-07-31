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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/** Process single bi-quadratic filter for multiple samples
 *
 * @param dst destination samples
 * @param src source samples
 * @param d filter memory, should be 4-byte aligned
 * @param count number of samples to process
 * @param f bi-quadratic filter structure, should be 4-byte aligned
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x1, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x1_t) *f);

/** Process two bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param d filter memory, should be 8-byte aligned
 * @param count number of samples to process
 * @param f bi-quadratic filter structure, should be 16-byte aligned
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x2, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x2_t) *f);

/** Process four bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param d filter memory, should be 16-byte aligned
 * @param count number of samples to process
 * @param f bi-quadratic filter structure, should be 16-byte aligned
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x4, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x4_t) *f);

/** Process eight bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param d filter memory, should be 32-byte aligned
 * @param count number of samples to process
 * @param f bi-quadratic filter structure, should be 32-byte aligned
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x8, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x8_t) *f);

/** Process sixteen bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param d filter memory, should be 64-byte aligned
 * @param count number of samples to process
 * @param f bi-quadratic filter structure, should be 64-byte aligned
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x16, float *dst, const float *src, float *d, size_t count, const LSP_DSP_LIB_TYPE(biquad_x16_t) *f);


#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_ */
