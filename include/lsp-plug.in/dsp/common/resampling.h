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

#ifndef LSP_PLUG_IN_DSP_COMMON_RESAMPLING_H_
#define LSP_PLUG_IN_DSP_COMMON_RESAMPLING_H_

#include <lsp-plug.in/dsp/common/types.h>

LSP_DSP_LIB_BEGIN_NAMESPACE

/** Resampling/oversampling funtion type.
 * Remember that destination buffer must be times greater and have additional gap (>=64 samples) at
 * the tail to contain complete convolution tail after resampling.
 * Consider using LSP_DSP_RESAMPLING_RSV_SAMPLES constant fo fullfill buffer requirements
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
typedef void (* LSP_DSP_LIB_TYPE(resampling_function_t))(float *dst, const float *src, size_t count);

LSP_DSP_LIB_END_NAMESPACE

/**
 * The number of additionally reserved samples at the tail of the buffer
 */
#define LSP_DSP_RESAMPLING_RSV_SAMPLES              1024

/** Perform 2x lanczos oversampling (2 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x2, float *dst, const float *src, size_t count);

/** Perform 2x lanczos oversampling (3 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x3, float *dst, const float *src, size_t count);

/** Perform 2x lanczos oversampling (4 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x4, float *dst, const float *src, size_t count);

/** Perform 2x lanczos oversampling (4 lobes for 12-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x12bit, float *dst, const float *src, size_t count);

/** Perform 2x lanczos oversampling (10 lobes for 16-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x16bit, float *dst, const float *src, size_t count);

/** Perform 2x lanczos oversampling (62 lobes for 24-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x24bit, float *dst, const float *src, size_t count);


/** Perform 3x lanczos oversampling (2 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x2, float *dst, const float *src, size_t count);

/** Perform 3x lanczos oversampling (3 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x3, float *dst, const float *src, size_t count);

/** Perform 3x lanczos oversampling (4 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x4, float *dst, const float *src, size_t count);

/** Perform 3x lanczos oversampling (4 lobes for 12-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x12bit, float *dst, const float *src, size_t count);

/** Perform 3x lanczos oversampling (10 lobes for 16-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x16bit, float *dst, const float *src, size_t count);

/** Perform 3x lanczos oversampling (62 lobes for 24-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x24bit, float *dst, const float *src, size_t count);


/** Perform 4x lanczos oversampling (2 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x2, float *dst, const float *src, size_t count);

/** Perform 4x lanczos oversampling (3 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x3, float *dst, const float *src, size_t count);

/** Perform 4x lanczos oversampling (4 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x4, float *dst, const float *src, size_t count);

/** Perform 4x lanczos oversampling (4 lobes for 12-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x12bit, float *dst, const float *src, size_t count);

/** Perform 4x lanczos oversampling (4 lobes for 16-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x16bit, float *dst, const float *src, size_t count);

/** Perform 4x lanczos oversampling (4 lobes for 24-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x24bit, float *dst, const float *src, size_t count);


/** Perform 6x lanczos oversampling (2 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x2, float *dst, const float *src, size_t count);

/** Perform 6x lanczos oversampling (3 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x3, float *dst, const float *src, size_t count);

/** Perform 6x lanczos oversampling (4 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x4, float *dst, const float *src, size_t count);

/** Perform 6x lanczos oversampling (4 lobes for 12-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x12bit, float *dst, const float *src, size_t count);

/** Perform 6x lanczos oversampling (10 lobes for 16-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x16bit, float *dst, const float *src, size_t count);

/** Perform 6x lanczos oversampling (62 lobes for 24-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x24bit, float *dst, const float *src, size_t count);


/** Perform 8x lanczos oversampling (2 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x2, float *dst, const float *src, size_t count);

/** Perform 8x lanczos oversampling (3 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x3, float *dst, const float *src, size_t count);

/** Perform 8x lanczos oversampling (4 lobes), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x4, float *dst, const float *src, size_t count);

/** Perform 8x lanczos oversampling (4 lobes for 12-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x12bit, float *dst, const float *src, size_t count);

/** Perform 8x lanczos oversampling (10 lobes for 16-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x16bit, float *dst, const float *src, size_t count);

/** Perform 8x lanczos oversampling (62 lobes for 24-bit sample precision), destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x24bit, float *dst, const float *src, size_t count);

/** Copy each even sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to produce
 */
LSP_DSP_LIB_SYMBOL(void, downsample_2x, float *dst, const float *src, size_t count);

/** Copy each 3rd sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to produce
 */
LSP_DSP_LIB_SYMBOL(void, downsample_3x, float *dst, const float *src, size_t count);

/** Copy each 4th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to produce
 */
LSP_DSP_LIB_SYMBOL(void, downsample_4x, float *dst, const float *src, size_t count);

/** Copy each 6th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to produce
 */
LSP_DSP_LIB_SYMBOL(void, downsample_6x, float *dst, const float *src, size_t count);

/** Copy each 8th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to produce
 */
LSP_DSP_LIB_SYMBOL(void, downsample_8x, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_RESAMPLING_H_ */
