/*
 * Copyright (C) 2026 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2026 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 2 авг. 2026 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_PACK_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_PACK_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/**
 * Pack array of two biquad x1 filters into a single biquad_x2 filter
 * @param dst 16-byte aligned destination filter
 * @param src 16-byte aligned source filter
 */
LSP_DSP_LIB_SYMBOL(void, biquad_pack_x2, LSP_DSP_LIB_TYPE(biquad_x2_t) *dst, const LSP_DSP_LIB_TYPE(biquad_x1_t) *src);

/**
 * Pack array of four biquad x1 filters into a single biquad_x4 filter
 * @param dst 16-byte aligned destination filter
 * @param src 16-byte aligned source filter
 */
LSP_DSP_LIB_SYMBOL(void, biquad_pack_x4, LSP_DSP_LIB_TYPE(biquad_x4_t) *dst, const LSP_DSP_LIB_TYPE(biquad_x1_t) *src);

/**
 * Pack array of eight biquad x1 filters into a single biquad_x8 filter
 * @param dst 32-byte aligned destination filter
 * @param src 16-byte aligned source filter
 */
LSP_DSP_LIB_SYMBOL(void, biquad_pack_x8, LSP_DSP_LIB_TYPE(biquad_x8_t) *dst, const LSP_DSP_LIB_TYPE(biquad_x1_t) *src);

/**
 * Pack array of sixteen biquad x1 filters into a single biquad_x16 filter
 * @param dst 64-byte aligned destination filter
 * @param src 16-byte aligned source filter
 */
LSP_DSP_LIB_SYMBOL(void, biquad_pack_x16, LSP_DSP_LIB_TYPE(biquad_x16_t) *dst, const LSP_DSP_LIB_TYPE(biquad_x1_t) *src);


#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_PACK_H_ */
