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
 * Object to store correlation state.
 *
 * The correlation is computed using the following formula:
 *
 *                   sum(a[i] * b[i])
 * corr = ---------------------------------------
 *         sqrt(sum(a[i]*a[i]) * sum(b[i]*b[i]))
 *
 * where i is in range of 0 to count-1.
 *
 */
typedef struct LSP_DSP_LIB_TYPE(correlation_t)
{
    float   v;      // the aggregated value of sum(a*b)
    float   a;      // the aggregated value of sum(a*a)
    float   b;      // the aggregated value of sum(b*b)
} LSP_DSP_LIB_TYPE(correlation_t);

#pragma pack(pop)

LSP_DSP_LIB_END_NAMESPACE

/**
 * Compute the initial intermediate values of correlation between two signals,
 * the function can be called multiple times, so the value of corr structure
 * should be cleared before first call.
 *
 * @param corr the object to initialize with intermediate results
 * @param a the pointer to the first signal buffer
 * @param b the pointer to the second signal buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, corr_init,
    LSP_DSP_LIB_TYPE(correlation_t) *corr,
    const float *a, const float *b,
    size_t count);

/**
 * Compute incremental value of normalized correlation between two signals
 *
 * @param corr the object that holds intermediate results
 * @param dst destination buffer to store result
 * @param a_head the pointer to the head of the first signal buffer
 * @param b_head the pointer to the head of the second signal buffer
 * @param a_tail the pointer to the tail of the first signal buffer
 * @param b_tail the pointer to the tail of the second signal buffer
 * @param head the offset of the head element relative to the first one in correlation (length of the window minus one)
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, corr_incr,
    LSP_DSP_LIB_TYPE(correlation_t) *corr,
    float *dst,
    const float *a_head, const float *b_head,
    const float *a_tail, const float *b_tail,
    size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_CORRELATION_H_ */
