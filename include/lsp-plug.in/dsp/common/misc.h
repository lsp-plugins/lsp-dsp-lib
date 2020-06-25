/*
 * misc.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_MISC_H_
#define LSP_PLUG_IN_DSP_COMMON_MISC_H_

#include <lsp-plug.in/dsp/common/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Calculate absolute normalized values: dst[i] = abs(src[i]) / max { abs(src) }
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        LSP_DSP_LIB_IMPORT
        void (* abs_normalized)(float *dst, const float *src, size_t count);

        /** Calculate normalized values: dst[i] = src[i] / (max { abs(src) })
         *
         * @param dst destination vector
         * @param src source vector
         * @param count number of elements
         */
        LSP_DSP_LIB_IMPORT
        void (* normalize)(float *dst, const float *src, size_t count);
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_MISC_H_ */
