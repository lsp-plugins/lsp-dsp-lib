/*
 * log.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_
#define LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_

#include <lsp-plug.in/dsp/types.h>

namespace lsp
{
    namespace dsp
    {
        /**
         * Compute binary logarithm: dst[i] = log(2, dst[i])
         * @param dst destination
         * @param count number of elements in destination
         */
        extern void (* logb1)(float *dst, size_t count);

        /**
         * Compute binary logarithm: dst[i] = log(2, src[i])
         * @param dst destination
         * @param src source
         * @param count number of elements in source
         */
        extern void (* logb2)(float *dst, const float *src, size_t count);

        /**
         * Compute natural logarithm: dst[i] = log(E, dst[i])
         * @param dst destination
         * @param count number of elements in destination
         */
        extern void (* loge1)(float *dst, size_t count);

        /**
         * Compute natural logarithm: dst[i] = log(E, src[i])
         * @param dst destination
         * @param src source
         * @param count number of elements in source
         */
        extern void (* loge2)(float *dst, const float *src, size_t count);

        /**
         * Compute decimal logarithm: dst[i] = log(10, dst[i])
         * @param dst destination
         * @param count number of elements in destination
         */
        extern void (* logd1)(float *dst, size_t count);

        /**
         * Compute decimal logarithm: dst[i] = log(10, src[i])
         * @param dst destination
         * @param src source
         * @param count number of elements in source
         */
        extern void (* logd2)(float *dst, const float *src, size_t count);

    }
}

#endif /* LSP_PLUG_IN_DSP_COMMON_PMATH_LOG_H_ */
