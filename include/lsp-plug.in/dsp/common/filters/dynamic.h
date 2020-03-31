/*
 * dynamic.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_
#define LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_

#include <lsp-plug.in/dsp/types.h>

#include <lsp-plug.in/dsp/common/filters/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Process single dynamic bi-quadratic filter for multiple samples
         *
         * @param dst array of count destination samples to emit
         * @param src array of count source samples to process
         * @param d pointer to filter memory (2 floats)
         * @param count number of samples to process
         * @param f array of count memory-aligned bi-quadratic filters
         */
        extern void (* dyn_biquad_process_x1)(float *dst, const float *src, float *d, size_t count, const biquad_x1_t *f);

        /** Process two dynamic bi-quadratic filters for multiple samples
         *
         * @param dst array of count destination samples to emit
         * @param src array of count source samples to process
         * @param d pointer to filter memory (2 floats)
         * @param count number of samples to process
         * @param f array matrix of (count+1)*2 memory-aligned bi-quadratic filters
         */
        extern void (* dyn_biquad_process_x2)(float *dst, const float *src, float *d, size_t count, const biquad_x2_t *f);

        /** Process four dynamic bi-quadratic filters for multiple samples
         *
         * @param dst array of count destination samples to emit
         * @param src array of count source samples to process
         * @param d pointer to filter memory (8 floats)
         * @param count number of samples to process
         * @param f array matrix of (count+3)*4 memory-aligned bi-quadratic filters
         */
        extern void (* dyn_biquad_process_x4)(float *dst, const float *src, float *d, size_t count, const biquad_x4_t *f);

        /** Process eight dynamic bi-quadratic filters for multiple samples
         *
         * @param dst array of count destination samples to emit
         * @param src array of count source samples to process
         * @param d pointer to filter memory (16 floats)
         * @param count number of samples to process
         * @param f array matrix of (count+7)*8 memory-aligned bi-quadratic filters
         */
        extern void (* dyn_biquad_process_x8)(float *dst, const float *src, float *d, size_t count, const biquad_x8_t *f);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_FILTERS_DYNAMIC_H_ */
