/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 10 нояб. 2024 г.
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

#ifndef LSP_PLUG_IN_DSP_COMMON_PAN_H_
#define LSP_PLUG_IN_DSP_COMMON_PAN_H_

#include <lsp-plug.in/dsp/common/types.h>

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)

/**
 * Definition for the panorama calulation function (parallel form)
 *
 * @param dst destination buffer to store value
 * @param l left channel data
 * @param r right channel data
 * @param dfl default value if it is not possible to compute panorama
 * @param count number of samples to process
 */
typedef void (* LSP_DSP_LIB_TYPE(depan_t))(float *dst, const float *l, const float *r, float dfl, size_t count);

#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

/**
 * Calculate the linear pan law panorama position between left and right channels (parallel form):
 *   pan = abs(R) / (abs(L) + abs(R))
 *
 * @param dst destination buffer to store value
 * @param l left channel data
 * @param r right channel data
 * @param dfl default value if it is not possible to compute panorama
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, depan_lin, float *dst, const float *l, const float *r, float dfl, size_t count);

/**
 * Calculate the equal power pan law (quadratic) panorama position between left and right channels (parallel form):
 *   pan = R^2 / (L^2 + R^2)
 *
 * @param dst destination buffer to store value
 * @param l left channel data
 * @param r right channel data
 * @param dfl default value if it is not possible to compute panorama
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, depan_eqpow, float *dst, const float *l, const float *r, float dfl, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_PAN_H_ */
