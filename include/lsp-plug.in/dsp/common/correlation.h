/*
 * Copyright (C) 2024 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2024 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-dsp-lib
 * Created on: 7 мар. 2024 г.
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
#ifndef LSP_PLUG_IN_DSP_COMMON_CORRELATION_H_
#define LSP_PLUG_IN_DSP_COMMON_CORRELATION_H_

#include <lsp-plug.in/dsp/common/types.h>

LSP_DSP_LIB_BEGIN_NAMESPACE

#pragma pack(push, 1)
/**
 * DSP context to store and restore machine state
 */
typedef struct LSP_DSP_LIB_TYPE(correlation_t)
{
    float   v;      // the a*b aggregated value
    float   a;      // the a*a aggregated value
    float   b;      // the b*b aggregated value
    float   pad;    // unused value
} LSP_DSP_LIB_TYPE(correlation_t);
#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

/** Compute normalized correlation between two signals
 *
 * @param dst destination buffer to store result
 * @param a the first signal data
 * @param b the second signal data
 * @param value the accumulated data from previous iteration
 * @param tail the offset of the last element relative to the first one in correlation (length of the window minus one)
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, correlation, LSP_DSP_LIB_TYPE(correlation_t) *corr, float *dst, const float *a, const float *b, size_t tail, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_CORRELATION_H_ */
