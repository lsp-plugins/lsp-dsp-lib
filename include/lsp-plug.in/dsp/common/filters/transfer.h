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

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFER_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFER_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/**
 * Compute filter transfer function, computes complex dst = H(f)
 * @param re destination to store transfer function (real value)
 * @param im destination to store transfer function (imaginary value)
 * @param c filter cascade
 * @param freq normalized frequency array
 * @param count size of frequency array
 */
LSP_DSP_LIB_SYMBOL(void, filter_transfer_calc_ri, float *re, float *im, const LSP_DSP_LIB_TYPE(f_cascade_t) *c, const float *freq, size_t count);

/**
 * Apply filter transfer function, computes complex dst = dst * H(f)
 * @param re destination to apply transfer function (real value)
 * @param im destination to apply transfer function (imaginary value)
 * @param c filter cascade
 * @param freq normalized frequency array
 * @param count size of frequency array
 */
LSP_DSP_LIB_SYMBOL(void, filter_transfer_apply_ri, float *re, float *im, const LSP_DSP_LIB_TYPE(f_cascade_t) *c, const float *freq, size_t count);

/**
 * Compute filter transfer function, computes complex dst = H(f)
 * @param dst destination to store transfer function (packed complex value)
 * @param c filter cascade
 * @param freq normalized frequency array
 * @param count size of frequency array
 */
LSP_DSP_LIB_SYMBOL(void, filter_transfer_calc_pc, float *dst, const LSP_DSP_LIB_TYPE(f_cascade_t) *c, const float *freq, size_t count);

/**
 * Apply filter transfer function, computes complex dst = dst * H(f)
 * @param dst destination to apply transfer function (packed complex value)
 * @param c filter cascade
 * @param freq normalized frequency array
 * @param count size of frequency array
 */
LSP_DSP_LIB_SYMBOL(void, filter_transfer_apply_pc, float *dst, const LSP_DSP_LIB_TYPE(f_cascade_t) *c, const float *freq, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_TRANSFER_H_ */
