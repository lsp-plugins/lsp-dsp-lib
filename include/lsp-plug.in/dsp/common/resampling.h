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

#ifdef __cplusplus
namespace lsp
{
    namespace dsp
    {
#endif /* __cplusplus */

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

#ifdef __cplusplus
    }
}
#endif /* __cplusplus */

/**
 * The number of additionally reserved samples at the tail of the buffer
 */
#define LSP_DSP_RESAMPLING_RSV_SAMPLES              64

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x2, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x3, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*2 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_2x4, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x2, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x3, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*3 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_3x4, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x2, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x3, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*4 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_4x4, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x2, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x3, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*6 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_6x4, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x2, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x3, float *dst, const float *src, size_t count);

/** Perform lanczos resampling, destination buffer must be cleared and contain only
 * resampling tail from previous resampling
 *
 * @param dst destination buffer of count*8 samples + LSP_DSP_RESAMPLING_RSV_SAMPLES samples for convolution tail
 * @param src source buffer of count samples
 * @param count number of samples
 */
LSP_DSP_LIB_SYMBOL(void, lanczos_resample_8x4, float *dst, const float *src, size_t count);

/** Copy each even sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, downsample_2x, float *dst, const float *src, size_t count);

/** Copy each 3rd sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, downsample_3x, float *dst, const float *src, size_t count);

/** Copy each 4th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, downsample_4x, float *dst, const float *src, size_t count);

/** Copy each 6th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, downsample_6x, float *dst, const float *src, size_t count);

/** Copy each 8th sample to output buffer
 *
 * @param dst destination buffer
 * @param src source buffer
 * @param count number of samples to process
 */
LSP_DSP_LIB_SYMBOL(void, downsample_8x, float *dst, const float *src, size_t count);

#endif /* LSP_PLUG_IN_DSP_COMMON_RESAMPLING_H_ */
