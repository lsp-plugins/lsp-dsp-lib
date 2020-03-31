/*
 * convolution.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_CONVOLUTION_H_
#define LSP_PLUG_IN_DSP_COMMON_CONVOLUTION_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /**
         * Calculate convolution of source signal and convolution and add to destination buffer
         * @param dst destination buffer to add result of convolution
         * @param src source signal
         * @param conv convolution
         * @param length length of convolution
         * @param count the number of samples in source signal to process
         */
        extern void (* convolve)(float *dst, const float *src, const float *conv, size_t length, size_t count);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_CONVOLUTION_H_ */
