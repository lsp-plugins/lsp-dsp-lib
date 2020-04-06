/*
 * msmatrix.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_
#define LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Convert stereo signal to mid-side signal
         *
         * @param m mid signal
         * @param s side signal
         * @param l left signal
         * @param r right signal
         * @param count number of samples to process
         */
        extern void (* lr_to_ms)(float *m, float *s, const float *l, const float *r, size_t count);

        /** Convert stereo signal to middle signal
         *
         * @param m mid signal
         * @param l left channel
         * @param r right channel
         * @param count number of samples to process
         */
        extern void (* lr_to_mid)(float *m, const float *l, const float *r, size_t count);

        /** Convert stereo signal to side signal
         *
         * @param s side signal
         * @param l left channel
         * @param r right channel
         * @param count number of samples to process
         */
        extern void (* lr_to_side)(float *s, const float *l, const float *r, size_t count);

        /** Convert mid-side signal to left-right signal
         *
         * @param l left signal
         * @param r right signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_lr)(float *l, float *r, const float *m, const float *s, size_t count);

        /** Convert mid-side signal to left signal
         *
         * @param l left signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_left)(float *l, const float *m, const float *s, size_t count);

        /** Convert mid-side signal to right signal
         *
         * @param r right signal
         * @param m mid signal
         * @param s side signal
         * @param count number of samples to process
         */
        extern void (* ms_to_right)(float *r, const float *m, const float *s, size_t count);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_MSMATRIX_H_ */
