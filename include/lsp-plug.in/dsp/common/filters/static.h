/*
 * static.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_

#include <lsp-plug.in/dsp/common/types.h>
#include <lsp-plug.in/dsp/common/filters/types.h>

/** Process single bi-quadratic filter for multiple samples
 *
 * @param dst destination samples
 * @param src source samples
 * @param count number of samples to process
 * @param f bi-quadratic filter structure
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x1, float *dst, const float *src, size_t count, LSP_DSP_LIB_TYPE(biquad_t) *f);

/** Process two bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param count number of samples to process
 * @param f bi-quadratic filter structure
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x2, float *dst, const float *src, size_t count, LSP_DSP_LIB_TYPE(biquad_t) *f);

/** Process four bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param count number of samples to process
 * @param f bi-quadratic filter structure
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x4, float *dst, const float *src, size_t count, LSP_DSP_LIB_TYPE(biquad_t) *f);

/** Process eight bi-quadratic filters for multiple samples simultaneously
 *
 * @param dst destination samples
 * @param src source samples
 * @param count number of samples to process
 * @param f bi-quadratic filter structure
 */
LSP_DSP_LIB_SYMBOL(void, biquad_process_x8, float *dst, const float *src, size_t count, LSP_DSP_LIB_TYPE(biquad_t) *f);

#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_STATIC_H_ */
