/*
 * log.h
 *
 *  Created on: 31 мар. 2020 г.
 *      Author: sadko
 */

#ifndef PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_
#define PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_

#ifndef PRIVATE_DSP_ARCH_GENERIC_IMPL
    #error "This header should not be included directly"
#endif /* PRIVATE_DSP_ARCH_GENERIC_IMPL */

namespace lsp
{
    namespace generic
    {
        void logb1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(dst[i]) * M_LOG2E;
        }

        void logb2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(src[i]) * M_LOG2E;
        }

        void loge1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(dst[i]);
        }

        void loge2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::logf(src[i]);
        }

        void logd1(float *dst, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::log10f(dst[i]);
        }

        void logd2(float *dst, const float *src, size_t count)
        {
            for (size_t i=0; i<count; ++i)
                dst[i] = ::log10f(src[i]);
        }
    }
}

#endif /* PRIVATE_DSP_ARCH_GENERIC_PMATH_LOG_H_ */
