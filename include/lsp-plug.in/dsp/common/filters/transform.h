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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

//---------------------------------------------------------------------------------------
// Bilinear transformation of dynamic filters
//---------------------------------------------------------------------------------------
/** Perform bilinear transformation of one filter bank
 *
 * @param bf memory-aligned target transformed biquad x1 filters
 * @param bc memory-aligned source analog bilinear filter cascades
 * @param kf frequency shift coefficient
 * @param count number of cascades  to process
 */
LSP_DSP_LIB_SYMBOL(void, bilinear_transform_x1, LSP_DSP_LIB_TYPE(biquad_x1_t) *bf, const LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, size_t count);

/** Perform bilinear transformation of two filter banks
 *
 * @param bf memory-aligned target transformed biquad x2 filters
 * @param bc memory-aligned source analog bilinear filter cascades matrix
 * @param kf frequency shift coefficient
 * @param count number of matrix rows to process
 */
LSP_DSP_LIB_SYMBOL(void, bilinear_transform_x2, LSP_DSP_LIB_TYPE(biquad_x2_t) *bf, const LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, size_t count);

/** Perform bilinear transformation of four filter banks
 *
 * @param bf memory-aligned target transformed biquad x4 filters
 * @param bc memory-aligned source analog bilinear filter cascades matrix
 * @param kf frequency shift coefficient
 * @param count number of matrix rows to process
 */
LSP_DSP_LIB_SYMBOL(void, bilinear_transform_x4, LSP_DSP_LIB_TYPE(biquad_x4_t) *bf, const LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, size_t count);

/** Perform bilinear transformation of eight filter banks
 *
 * @param bf memory-aligned target transformed biquad x8 filters
 * @param bc memory-aligned source analog bilinear filter cascades matrix
 * @param kf frequency shift coefficient
 * @param count number of matrix rows to process
 */
LSP_DSP_LIB_SYMBOL(void, bilinear_transform_x8, LSP_DSP_LIB_TYPE(biquad_x8_t) *bf, const LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, size_t count);

//---------------------------------------------------------------------------------------
// Matched Z transformation of dynamic filters
//---------------------------------------------------------------------------------------
/** Perform matched Z transformation of one filter bank,
 * function modifies contents of the bc memory chunk
 *
 * @param bf memory-aligned target transformed biquad x1 filters
 * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
 * @param kf frequency shift coefficient (filter frequency)
 * @param td tranformation coefficient (2 * pi / sample rate)
 * @param count number of cascades  to process
 */
LSP_DSP_LIB_SYMBOL(void, matched_transform_x1, LSP_DSP_LIB_TYPE(biquad_x1_t) *bf, LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, float td, size_t count);

/** Perform matched Z transformation of two filter banks,
 * function modifies contents of the bc memory chunk
 *
 * @param bf memory-aligned target transformed biquad x2 filters
 * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
 * @param kf frequency shift coefficient (filter frequency)
 * @param td tranformation coefficient (2 * pi / sample rate)
 * @param count number of cascades  to process
 */
LSP_DSP_LIB_SYMBOL(void, matched_transform_x2, LSP_DSP_LIB_TYPE(biquad_x2_t) *bf, LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, float td, size_t count);

/** Perform matched Z transformation of four filter banks,
 * function modifies contents of the bc memory chunk
 *
 * @param bf memory-aligned target transformed biquad x4 filters
 * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
 * @param kf frequency shift coefficient (filter frequency)
 * @param td tranformation coefficient (2 * pi / sample rate)
 * @param count number of cascades  to process
 */
LSP_DSP_LIB_SYMBOL(void, matched_transform_x4, LSP_DSP_LIB_TYPE(biquad_x4_t) *bf, LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, float td, size_t count);

/** Perform matched Z transformation of eight filter banks,
 * function modifies contents of the bc memory chunk
 *
 * @param bf memory-aligned target transformed biquad x8 filters
 * @param bc memory-aligned source analog bilinear filter cascades, modified after execution
 * @param kf frequency shift coefficient (filter frequency)
 * @param td tranformation coefficient (2 * pi / sample rate)
 * @param count number of cascades  to process
 */
LSP_DSP_LIB_SYMBOL(void, matched_transform_x8, LSP_DSP_LIB_TYPE(biquad_x8_t) *bf, LSP_DSP_LIB_TYPE(f_cascade_t) *bc, float kf, float td, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFORM_H_ */
