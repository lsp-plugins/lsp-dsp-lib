/*
 * minmax.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_
#define LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_

#include <lsp-plug.in/dsp/common/types.h>

namespace lsp
{
    namespace dsp
    {
        /** Calculate min { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return minimum value
         */
        LSP_DSP_LIB_IMPORT
        float (* min)(const float *src, size_t count);

        /** Calculate max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        LSP_DSP_LIB_IMPORT
        float (* max)(const float *src, size_t count);

        /** Get absolute maximum: result = max { abs(src[i]) }
         *
         * @param src source array
         * @param count number of elements
         * @return result
         */
        LSP_DSP_LIB_IMPORT
        float (* abs_max)(const float *src, size_t count);

        /** Get absolute minimum: result = min { abs(src[i]) }
         *
         * @param src source array
         * @param count number of elements
         * @return result
         */
        LSP_DSP_LIB_IMPORT
        float (* abs_min)(const float *src, size_t count);

        /** Calculate min { src }, max { src }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        LSP_DSP_LIB_IMPORT
        void (* minmax)(const float *src, size_t count, float *min, float *max);

        /** Calculate min { abs(src) }, max { abs(src) }
         *
         * @param src source vector
         * @param count number of elements
         * @return maximum value
         */
        LSP_DSP_LIB_IMPORT
        void (* abs_minmax)(const float *src, size_t count, float *min, float *max);
    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_SEARCH_MINMAX_H_ */
