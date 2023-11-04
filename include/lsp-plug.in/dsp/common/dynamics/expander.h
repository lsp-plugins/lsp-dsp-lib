/*
 * Copyright (C) 2023 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2023 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 1 нояб. 2023 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_DYNAMICS_EXPANDER_H_
#define LSP_PLUG_IN_DSP_COMMON_DYNAMICS_EXPANDER_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/dynamics/types.h>

LSP_DSP_LIB_SYMBOL(void, uexpander_x1_gain, float *dst, const float *src, const LSP_DSP_LIB_TYPE(expander_knee_t) *c, size_t count);
LSP_DSP_LIB_SYMBOL(void, dexpander_x1_gain, float *dst, const float *src, const LSP_DSP_LIB_TYPE(expander_knee_t) *c, size_t count);

LSP_DSP_LIB_SYMBOL(void, uexpander_x1_curve, float *dst, const float *src, const LSP_DSP_LIB_TYPE(expander_knee_t) *c, size_t count);
LSP_DSP_LIB_SYMBOL(void, dexpander_x1_curve, float *dst, const float *src, const LSP_DSP_LIB_TYPE(expander_knee_t) *c, size_t count);


#endif /* LSP_PLUG_IN_DSP_COMMON_DYNAMICS_EXPANDER_H_ */
