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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_FILL_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_FILL_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/**
 * Fill unused filters in x2 filter cascades with normal filters:
 * @param h pointer to the head of filter cascade
 * @param t pointer to the tail of filter cascade
 */
LSP_DSP_LIB_SYMBOL(void, fcascade_fill_x2, LSP_DSP_LIB_TYPE(f_cascade_t) *h, LSP_DSP_LIB_TYPE(f_cascade_t) *t);

/**
 * Fill unused filters in x4 filter cascades with normal filters:
 * @param h pointer to the head of filter cascade
 * @param t pointer to the tail of filter cascade
 */
LSP_DSP_LIB_SYMBOL(void, fcascade_fill_x4, LSP_DSP_LIB_TYPE(f_cascade_t) *h, LSP_DSP_LIB_TYPE(f_cascade_t) *t);

/**
 * Fill unused filters in x8 filter cascades with normal filters:
 * @param h pointer to the head of filter cascade
 * @param t pointer to the tail of filter cascade
 */
LSP_DSP_LIB_SYMBOL(void, fcascade_fill_x8, LSP_DSP_LIB_TYPE(f_cascade_t) *h, LSP_DSP_LIB_TYPE(f_cascade_t) *t);

/**
 * Fill unused filters in x16 filter cascades with normal filters:
 * @param h pointer to the head of filter cascade
 * @param t pointer to the tail of filter cascade
 */
LSP_DSP_LIB_SYMBOL(void, fcascade_fill_x16, LSP_DSP_LIB_TYPE(f_cascade_t) *h, LSP_DSP_LIB_TYPE(f_cascade_t) *t);


#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_FILL_H_ */
